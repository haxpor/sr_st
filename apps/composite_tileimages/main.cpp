/**
 * Composite multiple tile image of raw vector of pixels together into (already) allocated space
 * of resultant image.
 *
 * Compositing should not iterate through one by one pixel again, but efficiently place input tile images
 * into correct position.
 *
 * This simulates rendering multiple of circles which will have some overlapped pixels rendered on
 * all tile buffers, then finally combine them altogether.
 */
#include "SR_Common.h"
#include <vector>
#include <cmath>
#include <thread>

/// Screen size. For this implementation supports only squared size.
#define SIZE 1024

/// Pixel format is 32-bit little-endian ARGB
static sr::Color32i white = sr::makeColor32i(255, 255, 255);
static sr::Color32i green = sr::makeColor32i(0, 255, 0);
static sr::Color32i red = sr::makeColor32i(255, 0, 0);

struct Region
{
    int x0;
    int y0;
    int x1;
    int y1;
    int width;
    int height;

    Region()
        : x0(0)
        , y0(0)
        , x1(0)
        , y1(0)
        , width(0)
        , height(0)
    {
    }

    Region(int x_, int y_, int width_, int height_)
        : x0(x_)
        , y0(y_)
        , x1(x_ + width_)
        , y1(y_ + height_)
        , width(width_)
        , height(height_)
    {
    }
};

struct Circle
{
    int x;
    int y;
    int radius;
    int z;
    sr::Color32i color;

    Circle(int x_, int y_, int radius_, int z_, sr::Color32i color_)
        : x(x_)
        , y(y_)
        , radius(radius_)
        , z(z_)
        , color(color_)
    {
    }
};
std::vector<Circle> circles;

struct TileIndex
{
    int x;
    int y;

    TileIndex(int x_, int y_)
        : x(x_)
        , y(y_)
    {
    }
};

struct Tile
{
    std::vector<unsigned int> color;
    Region region;
    int xIndex;
    int yIndex;
};

// Assume available number of threads. This included physical and logical cores.
// Baseline of most machine should have 2 cores with 2 threads each, then total in 4.
#define AVAILABLE_NUM_THREADS 4

Tile tiles[AVAILABLE_NUM_THREADS];

// Distributed works for all cores. Chosen to duplicates Circle element instead of using indexing
// to increase performance in cache accessing.
std::vector<Circle> distributedWorks[AVAILABLE_NUM_THREADS];

/// Generate random circles then add into input circles
/// Input `numCircles` for number of circles to be generated.
///
/// \param circles std::vector of Circle, this will be updated in-place.
/// \param numCircles Total number of circles to generate.
///
void generateCircles(std::vector<Circle>& circles, const int numCircles)
{
    for (int i=0; i<numCircles; ++i)
    {
        circles.emplace_back(Circle(
                    sr::MathUtil::randInt2(0, SIZE),
                    sr::MathUtil::randInt2(0, SIZE),
                    sr::MathUtil::randInt2(50, 60),
                    sr::MathUtil::randInt2(1,30),
                    sr::makeColor32i(sr::MathUtil::randInt(255), sr::MathUtil::randInt(255), sr::MathUtil::randInt(255)))
                );
    }
}

/// We don't have z-buffer in this case, so we use painter algorithm sorting from far to near.
/// This is a compare function used with std::sort
void sortCirclesFarToNear(std::vector<Circle>& circles)
{
    std::sort(circles.begin(), circles.end(), [](const Circle& a, const Circle& b) -> bool {
            return a.z > b.z;
            });
}

/// Compute number of tiles from number of threads.
/// Number of tiles will be symmetric, and it might use all available cores.
///
/// \param nThreads Number of available threads
/// \param outNumTiles1D Number of tiles along 1 dimension. This will be filled.
/// \param outNumTiles Number of total tiles. This will be filled.
///
void computeNumTilesFromNumThreads(int nThreads, int& outNumTiles1D, int& outNumTiles)
{
    const int numTiles1D = std::floor(std::sqrt(nThreads));
    outNumTiles1D = numTiles1D;
    outNumTiles = numTiles1D * numTiles1D;
}

/// Assume tile is squared.
/// Return lineSize
///
/// \param tiles Tiles as pre-declared with maximum of AVAILABLE_NUM_THREADS as hinted. These will be prepared to be allocated for total number of pixels to support in rendering for each tile.
/// \param nTiles1D Total number of tiles along 1 dimension
/// \return Line size of each tile which is width of each tile.
int preAllocateSpaceForTiles(Tile tiles[AVAILABLE_NUM_THREADS], const int nTiles1D)
{
    const int kNumSize1D = std::floor(SIZE / nTiles1D);
    const int kPreAllocatedSize = kNumSize1D * kNumSize1D;
    for (int j=0; j<nTiles1D; ++j)
    {
        for (int i=0; i<nTiles1D; ++i)
        {
            const int tileIndex = i + j*nTiles1D;
            tiles[tileIndex].xIndex = i;
            tiles[tileIndex].yIndex = j;
            tiles[tileIndex].color.resize(kPreAllocatedSize, 0xFF000000);     // initially set to opaque black color
            tiles[tileIndex].region = Region(kNumSize1D * i, kNumSize1D * j, kNumSize1D, kNumSize1D);
        }
    }

    return kNumSize1D;
}

/// Determine tile-index from input position in screen-space for total number of tiles in 1 dimension
/// This is for efficient in computation, you should cache num tiles along 1 dimension externally to this call.
/// This assume that total tiles is squared, as `nTiles1D` is used for both axes of x, and y.
///
/// \param x X position in screen space
/// \param y Y position in screen space
/// \param nTiles1D Number of tiles along 1 dimension
/// \return Newly constructed TileIndex representing tile index for input position in screen space.
///
TileIndex determineTileIndex(int x, int y, int nTiles1D)
{
    // TODO: Return the list of tile index such Circle lives in. It could be at most 3 adjacent tiles
    // for one Circle can live in according to its radius.
    const float segmentX = SIZE / nTiles1D;
    const float segmentY = SIZE / nTiles1D;     // although supported only squared screen size, we leave this line here for future addition, but
                                                // actually no need to compute height for each tile again as we already had width
    return TileIndex(static_cast<int>(std::floor(x / segmentX)), static_cast<int>(std::floor(y / segmentY)));
}

/// Distribute all rendering works into array of Circles.
///
/// \param circles Input array of Circles to render
/// \param distributedWorks Input pre-declared array of distributed works to hold the result from distributed works. Number of arrays must be exactly AVAILABLE_NUM_THREADS as hinted.
/// \param nTiles1D Number of tiles along 1 dimension.
///
void distributeWorks(const std::vector<Circle>& circles, std::vector<Circle> distributedWorks[AVAILABLE_NUM_THREADS], const int nTiles1D)
{
    for (const Circle& e : circles)
    {
        TileIndex ti = determineTileIndex(e.x, e.y, nTiles1D);
        int dWorkIndex = ti.x + ti.y * nTiles1D;     // convert from 2D into 1D indexing for distributed works vector
        distributedWorks[dWorkIndex].push_back(e);
    }
}

///  Rasterize input Circle
///
/// \param c Input Circle to rasterize
/// \param tile Target tile to render onto
/// \param lineSize LineSize which is width of each tile
void rasterize(const Circle& c, Tile& tile, const int lineSize)
{
    const int startX = c.x - c.radius;
    const int startY = c.y - c.radius;

    const int endX = startX + 2*c.radius;
    const int endY = startY + 2*c.radius;

    const Region region = tile.region;
    
    for (int y=startY; y<=endY; ++y)
    {
        for (int x=startX; x<=endX; ++x)
        {
            if (x < region.x0 || x >= region.x1)
                continue;
            if (y < region.y0 || y >= region.y1)
                continue;
            const int fbIndex = x + y*lineSize;
            tile.color[fbIndex] = c.color.packed;
        }
    }
}

/// Perform render work from input `works` which consists of Circles to render, output resultant frameBuffer into `tile`.
///
/// \param works Array of Circle to render
/// \param tile Target framebuffer to render onto
/// \param lineSize Width of a single tile
void renderWork(const std::vector<Circle>& works, Tile& tile, const int lineSize)
{
    for (const Circle& c: works)
    {
        rasterize(c, tile, lineSize);
    }
}

/// Combine work from input tile then output into target framebuffer
void combineWork(const Tile& tile, sr::FrameBuffer& fb, const int workIndex)
{
    std::cout << "combine work from addr:" << reinterpret_cast<uintptr_t>(&tile) << std::endl;
    unsigned int* fbPtr = fb.getFrameBuffer();

    const int kTileSize = tile.region.width;
    
    const int srcStartX = tile.region.x0;
    const int srcStartY = tile.region.y0;
    const int srcWidth = fb.getWidth();

    for (int y=0, srcY=srcStartY; y<kTileSize; ++y, ++srcY)
    {
        for (int x=0, srcX=srcStartX; x<kTileSize; ++x, ++srcX)
        {
            const int srcIndex = x + y*kTileSize;
            const int dstIndex = srcX + srcY*srcWidth;
            fbPtr[dstIndex] = tile.color[srcIndex];
        }
    }
}

int main()
{
    sr::MathUtil::init();
    sr::FrameBuffer fb(1024, 1024);

    int numTiles;
    int numTiles1D;
    computeNumTilesFromNumThreads(AVAILABLE_NUM_THREADS, numTiles1D, numTiles);
    const int lineSize = preAllocateSpaceForTiles(tiles, numTiles1D);
    generateCircles(circles, 20);

    sr::Profile::start();
    sortCirclesFarToNear(circles);
    distributeWorks(circles, distributedWorks, numTiles1D);

    //for (int j=0; j<numTiles1D; ++j)
    //{
    //    for (int i=0; i<numTiles1D; ++i)
    //    {
    //        std::cout << "distributedWorks[" << j << "][" << i << "] = " << distributedWorks[i + j*2].size() << " works" << std::endl;
    //    }
    //}

    std::thread ts[AVAILABLE_NUM_THREADS];

    // render works
    for (int i=0; i<AVAILABLE_NUM_THREADS; ++i)
        ts[i] = std::thread([&lineSize](int i){
            renderWork(distributedWorks[i], tiles[i], lineSize);
        }, i);
    for (int i=0; i<AVAILABLE_NUM_THREADS; ++i)
        ts[i].join();

    // combine works
    for (int i=0; i<AVAILABLE_NUM_THREADS; ++i)
        ts[i] = std::thread([&fb](int i){
            combineWork(tiles[i], fb, i);           
        }, i);
    for (int i=0; i<AVAILABLE_NUM_THREADS; ++i)
        ts[i].join();
    sr::Profile::endAndPrint();

    sr::TGAImage::write24("out.tga", fb);
    return 0;
}
