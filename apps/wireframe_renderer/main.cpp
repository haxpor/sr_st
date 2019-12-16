///
/// Load .obj file then render it as wireframe using Bresenham's Line Algorithm
///
#include "SR_Common.h"
#include <cmath>
#include <algorithm>

/// Bresenham line drawing algorithm.
/// This covers all cases (all octants).
/// This function is grabbped from line/ program's line7() function.
static void line(sr::Vec2i start, sr::Vec2i end, sr::FrameBuffer& fb, unsigned int color)
{
    bool steep = false;
    if (std::abs(start.x - end.x) < std::abs(start.y - end.y))          // choose range that has more width
    {
        std::swap(start.x, start.y);
        std::swap(end.x, end.y);
        steep = true;
    }
    if (start.x > end.x)
    {
        std::swap(start.x, end.x);
        std::swap(start.y, end.y);
    }
    const int dx = end.x - start.x;
    const int dy = end.y - start.y;
    const int D = std::abs(2*dy);       // base difference constant on no changing of y, thus it's still at the same level
    int error = 0;
    const int errorIncrement = -2*dx;   // now dx is on the right side, and 2 is multiplied as initially we multiplied by 2 so the less should be the same

    int y = start.y;
    const int yStep = end.y > start.y ? 1 : -1;

    for (int x = start.x; x <= end.x; ++x)
    {
        if (steep)
            fb.set(y, x, color);
        else
            fb.set(x, y, color);

        error += D;
        if (error > dx)                 // dx switched to the left side of the equation thus it's now positive
        {
            y += yStep;
            error += errorIncrement;
        }
    }
}

int main()
{
    sr::FrameBuffer fb(1024, 1024);

    sr::ObjData modelData;
    if (!sr::ObjLoader::loadObjFile("../../res/objs/dragon.obj", modelData))
    {
        LOGE("Cannot load dragon.obj file\n");
        exit(1);
    }

    const std::vector<sr::Vec3f>& vertices = modelData.vertices;
    const std::vector<std::vector<unsigned int>>& faces = modelData.faces;
    const int facesCount = faces.size();
    const float kScale = 0.13f;

    std::cout << "dragon.obj: " << modelData.vertices.size() << ", " << modelData.faces.size() << std::endl;

    sr::Profile::start();
    for (int i=0; i<facesCount; ++i)
    {
        const std::vector<unsigned int>& fIndicesList = faces[i];

        for (int j=0; j<3; ++j)
        {
            sr::Vec3f p1 = vertices[fIndicesList[j]];
            sr::Vec3f p2 = vertices[fIndicesList[(j+1)%3]];

            sr::Vec2i cvtP1 = sr::Vec2i(
                    (p1.x * kScale + 1.0f) * fb.getWidth()*0.5f,
                    (p1.y * kScale + 1.0f) * fb.getHeight()*0.5f - 300.0f);
            sr::Vec2i cvtP2 = sr::Vec2i(
                    (p2.x * kScale + 1.0f) * fb.getWidth()*0.5f,
                    (p2.y * kScale + 1.0f) * fb.getHeight()*0.5f - 300.0f);

            line(cvtP1, cvtP2, fb, 0xFFFFFFFF);
        }
    }
    sr::Profile::endAndPrint();
    sr::TGAImage::write24("out.tga", fb.getFrameBuffer(), fb.getWidth(), fb.getHeight());
    return 0;
}
