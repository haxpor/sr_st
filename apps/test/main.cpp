#include "SR_Common.h"
#include <vector>
#include <cassert>

int main()
{
    // logging
    LOG("Log normal\n");
    LOG_YELLOW("Log - Yellow\n");
    LOGE("Log error\n");
    LOGE_BLUE("Log error - Blue\n");
    LOGA("Log (thread-safe)\n");
    LOGA_CYAN("Log (thread-safe) - Magenta\n");

    // MathUtil
    sr::MathUtil::init(10);
    std::cout << sr::MathUtil::randInt(10) << std::endl;
    std::cout << sr::MathUtil::randInt2(10, 20) << std::endl;
    std::cout << "MathUtil::randInt(25): " << sr::MathUtil::randInt(25) << std::endl;
    std::cout << "MathUtil::randInt2(50, 100): " << sr::MathUtil::randInt2(50, 100) << std::endl;
    std::cout << "MathUtil::randFloat(): " << sr::MathUtil::randFloat() << std::endl;
    std::cout << "MathUtil::randFloat(15.0f): " << sr::MathUtil::randFloat(15.0f) << std::endl;
    std::cout << "MathUtil::randFloat2(15.0f, 25.0f): " << sr::MathUtil::randFloat2(15.0f, 25.0f) << std::endl;

    // Graphics Util
    const unsigned int redColor = sr::makeColorARGB(255, 255, 0, 0);
    const unsigned int redColorAlt = sr::makeColorARGB(255, 0, 0);
    static_assert(redColor == redColorAlt, "Two resultant colors must be equal");
    static_assert(((redColor >> 16) & 0xFF) == 0xFF, "Red component in color must be 0xFF");

    // Vec2f
    sr::Vec2f vec2f = sr::Vec2f(500.0f, 1000.0f);
    std::cout << vec2f.x << ", " << vec2f.u << ", " << vec2f.s << std::endl;
    assert(vec2f.x == vec2f.u && "x should equal to u");        // at least on gcc, <number1> == <number2> == <number3> return false
    assert(vec2f.x == vec2f.s && "x should equal to s");
    assert(vec2f.y == vec2f.v && "y should equal to v");
    assert(vec2f.y == vec2f.t && "y should equal to t");

    // Color32i
    sr::Color32i colori = sr::Color32i(255, 0, 255, 255);
    std::cout << colori.packed << std::endl;
    assert(colori.a == 255 && "alpha component should be equal to 255");
    assert(colori.r == 255 && "red comonent value should be equal to 255");
    assert(colori.b == 255 && "blue component value should be equal to 255");
    assert(colori.packed == 0xFFFF00FF && "packed value should be equal to final set value");

    // Color32f
    sr::Color32f colorf = sr::Color32f(1.0f, 0.0f, 1.0f, 1.0f);
    assert(colorf.a == 1.0f && "alpha component should be equal to 1.0f");
    assert(colorf.r == 1.0f && "red comonent value should be equal to 1.0f");
    assert(colorf.b == 1.0f && "blue component value should be equal to 1.0f");

    // TGAImage
    std::vector<unsigned int> frameBuffer;
    frameBuffer.resize(256 * 256);      // for 256 x 256 image
    // make a 10x10 red rectangle in the framebuffer
    const int centerX = 256/2;
    const int centerY = 256/2;
    for (int i=centerX-5; i<centerX+5; ++i)
    {
        for (int j=centerY-5; j<centerY+5; ++j)
        {
            frameBuffer[i + j*256] = 0xFF0000;
        }
    } 

    // + Profiler
    std::cout << "Write .tga image to file\n";
    sr::Profile::start();
    sr::TGAImage::write24("out.tga", &frameBuffer[0], 256, 256);
    sr::Profile::endAndPrint();

    // ObjLoader
    std::cout << "Load dragon.obj\n";
    sr::Profile::start();
    sr::ObjData objdata;
    sr::ObjLoader::loadObjFile("../../res/objs/dragon.obj", objdata);
    sr::Profile::endAndPrint();

    std::cout << "Total vertices: " << objdata.vertices.size() << std::endl;
    std::cout << "Total faces: " << objdata.faces.size() << std::endl;
    
    return 0;
}
