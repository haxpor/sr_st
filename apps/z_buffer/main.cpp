#include "SR_Common.h"
#include <algorithm>
#include <limits>

#define FB_WIDTH 512
#define FB_HEIGHT 512

static sr::Color32i white = sr::makeColor32i(255, 255, 255);
static sr::Color32i green = sr::makeColor32i(0, 255, 0);
static sr::Color32i red = sr::makeColor32i(255, 0, 0);
static sr::Vec3f sLightDirection = sr::Vec3f(0.0f, 0.0f, 1.0f);

/// modified version of sr::triangle but with added of input zbuffer
/// `tDepths` is array of float of depth for t0, t1, and t2 respectively.
void triangle(sr::Vec2i t0, sr::Vec2i t1, sr::Vec2i t2, float tDepths[3], sr::FrameBuffer& fb, float zBuffer[], sr::Color32i color)
{
    const int maxW = fb.getWidth() - 1;
    const int maxH = fb.getHeight() - 1;

    // find the bounding box for the triangle
    sr::Vec2i bbMin(maxW, maxH);
    sr::Vec2i bbMax(0, 0);

    // find minimum x/y
    bbMin.x = std::max(0, std::min(bbMin.x, t0.x));
    bbMin.x = std::max(0, std::min(bbMin.x, t1.x));
    bbMin.x = std::max(0, std::min(bbMin.x, t2.x));

    bbMin.y = std::max(0, std::min(bbMin.y, t0.y));
    bbMin.y = std::max(0, std::min(bbMin.y, t1.y));
    bbMin.y = std::max(0, std::min(bbMin.y, t2.y));

    // find maximum x/y
    bbMax.x = std::min(maxW, std::max(bbMax.x, t0.x));
    bbMax.x = std::min(maxW, std::max(bbMax.x, t1.x));
    bbMax.x = std::min(maxW, std::max(bbMax.x, t2.x));

    bbMax.y = std::min(maxH, std::max(bbMax.y, t0.y));
    bbMax.y = std::min(maxH, std::max(bbMax.y, t1.y));
    bbMax.y = std::min(maxH, std::max(bbMax.y, t2.y));

    sr::Vec2i p;
    for (p.x = bbMin.x; p.x<=bbMax.x; ++p.x)
    {
        for (p.y = bbMin.y; p.y<=bbMax.y; ++p.y)
        {
            sr::Vec3f bcScreen = sr::MathUtil::barycentric(t0, t1, t2, p);
            if (bcScreen.x < 0.0f || bcScreen.y < 0.0f || bcScreen.z < 0.0f ||
                bcScreen.x > 1.0f || bcScreen.y > 1.0f || bcScreen.z > 1.0f)
                continue;

            // compute z-depth value (note floating-point) for this particular pixel on this triangle
            // surface
            float z = bcScreen.x*tDepths[0] + bcScreen.y*tDepths[1] + bcScreen.z*tDepths[2];
            // z-buffer testing
            if (zBuffer[static_cast<int>(p.x + p.y*fb.getWidth())] < z)
            {
                zBuffer[static_cast<int>(p.x + p.y*fb.getWidth())] = z;
                fb.set(p.x, p.y, color.packed);
            }
        }
    } 
}

int main()
{
    sr::MathUtil::init();
    sr::FrameBuffer fb(FB_WIDTH, FB_HEIGHT);
    sr::ObjData headModel;
    if (!sr::ObjLoader::loadObjFile("../../res/objs/african_head.obj", headModel))
    {
        LOGE("Failed to load african_head.obj\n");
        return 1;
    }

    float* zBuffer = new float[FB_WIDTH * FB_HEIGHT];
    // initialize zbuffer with maximum value
    for (int i=0; i<FB_WIDTH*FB_HEIGHT; ++i)
        zBuffer[i] = std::numeric_limits<int>::min();

    // do flat shading on model's triangles
    const auto& modelFaces = headModel.faces;
    const auto& modelVertices = headModel.vertices;
    const int kNumModelFaces = modelFaces.size();

    for (int i=0; i<kNumModelFaces; ++i)
    {
        auto& face = modelFaces[i];

        sr::Vec2i screenCoords[3];
        sr::Vec3f worldCoords[3];
        float tDepths[3];

        // convert from world coordinate to screen coordinate
        for (int j=0; j<3; ++j)
        {
            sr::Vec3f worldCoord = modelVertices[face[j]];

            // integer type is important here for correct rendering output without black hole
            screenCoords[j] = sr::Vec2i(static_cast<int>((worldCoord.x + 1.0f) * FB_WIDTH/2.0f + 0.5f), static_cast<int>((worldCoord.y + 1.0f) * FB_HEIGHT/2.0f + 0.5f));
            tDepths[j] = worldCoord.z;
            worldCoords[j] = worldCoord;
        }

        // assume model is exported with right-hand rule (counter-clockwise) triangle
        // compute normal vector this face
        sr::Vec3f faceNormal = sr::cross(worldCoords[1] - worldCoords[0], worldCoords[2] - worldCoords[0]);
        // normalize normal vector
        faceNormal.normalize();

        // check lighting effect on this face using dot product
        float intensity = sr::dot(faceNormal, sLightDirection);
        if (intensity > 0.0f)
        {
            float applyIntensity = intensity * 255;

            triangle(screenCoords[0], screenCoords[1], screenCoords[2], tDepths, fb,
                zBuffer,
                sr::Color32i(applyIntensity, applyIntensity, applyIntensity));
        }
    }

    delete zBuffer;
    zBuffer = nullptr;

    sr::TGAImage::write24("out.tga", fb);
    return 0;
}
