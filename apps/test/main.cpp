#include "SR_Common.h"
#include <vector>

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
    
    return 0;
}
