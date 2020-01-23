#include "SR_Common.h"
#include <algorithm>

#define FB_WIDTH 512
#define FB_HEIGHT 512

static sr::Color32i white = sr::makeColor32i(255, 255, 255);
static sr::Color32i green = sr::makeColor32i(0, 255, 0);
static sr::Color32i red = sr::makeColor32i(255, 0, 0);

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

    LOG("african_head.obj model:\n");
    LOG("  number of vertices: %d\n", headModel.vertices.size());
    LOG("  number of faces: %d\n", headModel.faces.size());

    // do flat shading on model's triangles
    const auto& modelFaces = headModel.faces;
    const auto& modelVertices = headModel.vertices;
    const int kNumModelFaces = modelFaces.size();

    for (int i=0; i<kNumModelFaces; ++i)
    {
        auto& face = modelFaces[i];
        sr::Vec2i screenCoords[3];

        // convert from world coordinate to screen coordinate
        for (int j=0; j<3; ++j)
        {
            sr::Vec3f worldCoord = modelVertices[face[j]];
            screenCoords[j] = sr::Vec2i((worldCoord.x + 1.0f) * FB_WIDTH/2.0f, (worldCoord.y + 1.0f) * FB_HEIGHT/2.0f);
        }
        sr::triangle(screenCoords[0], screenCoords[1], screenCoords[2], fb,
                sr::Color32i(sr::MathUtil::randInt(255), sr::MathUtil::randInt(255), sr::MathUtil::randInt(255)));
    }

    sr::TGAImage::write24("out.tga", fb);
    return 0;
}
