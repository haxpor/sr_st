#include "SR_Common.h"
#include <algorithm>
#include <cstring>

#define FB_WIDTH 800
#define FB_HEIGHT 500

// high resolution height is used for clearer to see the output
// actually just 1 is needed for all FrameBuffers as the final color buffer will occupy only 1 pixel in height
// just to visualize stuff.
#define HIGH_RESO_HEIGHT 16

static sr::Color32i white = sr::makeColor32i(255, 255, 255);
static sr::Color32i green = sr::makeColor32i(0, 255, 0);
static sr::Color32i red = sr::makeColor32i(255, 0, 0);
static sr::Color32i blue = sr::makeColor32i(0, 0, 255);

// this core code is copied from Graphics.cpp's line() function but added in for yBuffer accepting of parameter
// and internal checking before setting pixel value.
void line(sr::Vec2i start, sr::Vec2i end, sr::FrameBuffer& fb, sr::Color32i color, int yBuffer[])
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
        {
            // convert into color range to compare and output at the end
            int yCvt = static_cast<int>(std::lround(y*1.0f/FB_HEIGHT * 255));
            if (yBuffer[x] < yCvt)     // kinda reverse to z-buffer as now we think in term of 2d with origin at the top-left corner
            {
                yBuffer[x] = yCvt;
                fb.set(y, 0, color.packed);     // just 1 pixel in height to visualize stuff
            }
        }
        else
        {
            int yCvt = static_cast<int>(std::lround(y*1.0f/FB_HEIGHT * 255));
            if (yBuffer[x] < yCvt)
            {
                yBuffer[x] = yCvt;
                fb.set(x, 0, color.packed);     // just 1 pixel in height to visualize stuff
            }
        }

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
    sr::FrameBuffer fb(FB_WIDTH, HIGH_RESO_HEIGHT);    // we will duplicate first row into other rows
                                    // this is for ease in visualize the output for high resolution screen

    int yBuffer[FB_WIDTH];   // represent yBuffer
    for (int i=0; i<FB_WIDTH; ++i)
        yBuffer[i] = -1;        // set as -1 will produce full white color in final buffer output for yBuffer

    line(sr::Vec2i(20, 34), sr::Vec2i(744, 400), fb, red, yBuffer);
    line(sr::Vec2i(120, 434), sr::Vec2i(444, 400), fb, green, yBuffer);
    line(sr::Vec2i(330, 463), sr::Vec2i(594, 200), fb, blue, yBuffer);

    // fill the less of the rows in framebuffers
    // by copying the first rows into the less of rows
    sr::FrameBuffer::const_pointer fbBeginning = fb.getFrameBuffer();

    // slightly special care needed for yBufferFB as we need to set its first row first
    sr::FrameBuffer yBufferFB(FB_WIDTH, HIGH_RESO_HEIGHT);
    sr::FrameBuffer::const_pointer yBufferFBBeginning = yBufferFB.getFrameBuffer();

    for (int i=0; i<yBufferFB.getWidth(); ++i)
        yBufferFB[i] = sr::makeColorPacked(yBuffer[i]);

    for (int i=1; i<HIGH_RESO_HEIGHT; ++i)
    {
        std::memcpy(fb.getFrameBuffer() + i*fb.getWidth(), fbBeginning, fb.getWidth() * sizeof(sr::FrameBuffer::type));       
        std::memcpy(yBufferFB.getFrameBuffer() + i*yBufferFB.getWidth(), yBufferFBBeginning, yBufferFB.getWidth() * sizeof(sr::FrameBuffer::type));
    }

    sr::TGAImage::write24("out.tga", fb);
    sr::TGAImage::write24("yBuffer.tga", yBufferFB);
    return 0;
}
