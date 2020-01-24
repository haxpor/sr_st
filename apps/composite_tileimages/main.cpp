/**
 * Composite multiple tile image of raw vector of pixels together into (already) allocated space
 * of resultant image.
 *
 * Compositing should not iterate through one by one pixel again, but efficiently place input tile images
 * into correct position.
 *
 * This simulates rendering multiple of circles which will have some overlapped pixels rendered on
 * all tile buffers, then finally combine them altogether.
 *
 * Performance Test:
 * Tested on Intel(R) Core(TM) i5-3210M CPU @ 2.50GHz, 2 cores, 2 threads each, Ubuntu 18.04 with
 * optimized compilation flags as seen in Makefile.
 *
 * It took around ~65 ms (so around 15 fps) for rectangular shape, but for circle, this increases to
 * ~78 ms. This shows impact from branching within tight loop (see branch prediction for more information)
 * 
 * The tasks to complete are as follows
 *  - sorting all circles according to its z value
 *  - distribute works across multiple threads (4 threads)
 *  - render works from all threads
 *  - combine the works from all threads
 * *doesn't include time in writing out to .tga file.
 *
 * Serial implementation took ~105 ms which is doubled from multithreading implementation. Seems
 * reasonable. If machine has more core, it would be performing better.
 */
#include "SR_Common.h"
#include <vector>
#include <cmath>
#include <thread>
#include <cstring>

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

    friend std::ostream& operator<<(std::ostream& os, const Region& r)
    {
        os << "x0: " << r.x0 << ", y0: " << r.y0 << ", x1: " << r.x1 << ", y1: " << r.y1 << ", width: "
           << r.width << ", height: " << r.height;
        return os;
    }
};

/// aligned to cache line size, reduce by ~10ms
struct __attribute__((aligned(64))) Circle
{
    int x;
    int y;
    int radius;
    int z;
    sr::Color32i color;
    char padded[44];            

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
    //
    // Note: + 1 to not make pixel at the border increment index number which will lead to crash later
    static const float segmentX = std::floor(SIZE / nTiles1D) + 1;
    static const float segmentY = std::floor(SIZE / nTiles1D) + 1;     // although supported only squared screen size, we leave this line here for future addition, but
                                                // actually no need to compute height for each tile again as we already had width
    return TileIndex(static_cast<int>(std::floor(x / segmentX)), static_cast<int>(std::floor(y / segmentY)));
}

/// Distribute the works then add into target Tile. It internally specifically check to add works for
/// such Tile.
///
/// \param circles Input array of Circles to render
/// \param distributedWorks Target distributed works specifically for Tile
/// \param tile Tile to get the works for this distribution
/// \param nTiles1D Number of tiles along 1 dimension.
///
void distributeWorksForTile(const std::vector<Circle>& circles, std::vector<Circle>& distributedWorks, Tile& tile)
{
    Region region = tile.region;

    for (const Circle& e : circles)
    {
        // check intersection between Circle and Region of Tile
        const int circleX = e.x - e.radius;
        const int circleY = e.y - e.radius;
        const int circleSize = 2 * e.radius;

        if (circleX + circleSize > region.x0 &&
            circleY + circleSize > region.y0 &&
            region.x0 + region.width > circleX &&
            region.y0 + region.height > circleY)
        {
            distributedWorks.push_back(e);
        }
    }
}


///  Rasterize input Circle
///
/// \param c Input Circle to rasterize
/// \param tile Target tile to render onto
/// \param lineSize LineSize which is width of each tile
void rasterize(const Circle& c, Tile& tile, const int lineSize)
{
    const int tileOffsetX = tile.region.x0;
    const int tileOffsetY = tile.region.y0;

    const int centerX = c.x;
    const int centerY = c.y;
    const int radius = c.radius;
    const int squaredRadius = radius * radius;

    const int startX = centerX - radius;
    const int startY = centerY - radius;

    const int endX = centerX + radius;
    const int endY = centerY + radius;

    const Region region = tile.region;
    
    for (int y=startY; y<=endY; ++y)
    {
        for (int x=startX; x<=endX; ++x)
        {
            if (x < region.x0 || x > region.x1)
                continue;
            if (y < region.y0 || y > region.y1)
                continue;

            const int dx = x - centerX;
            const int dy = y - centerY;
            if (dx*dx + dy*dy > squaredRadius)
                continue;

            // take into account region offset of this particular tile
            // as tile doesn't have full final framebuffer size
            const int fbIndex = (x-tileOffsetX) + (y-tileOffsetY)*lineSize;
            
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
void combineWork(const Tile& tile, sr::FrameBuffer& fb)
{
    unsigned int* fbPtr = fb.getFrameBuffer();
    
    const int dstStartX = tile.region.x0;
    const int dstStartY = tile.region.y0;
    const int dstWidth = fb.getWidth();

    const int kTileSize = tile.region.width;

    // copy row by row from source to destination
    for (int y=0; y<kTileSize; ++y)
    {
        const int dstIndex = dstStartX + (dstStartY+y)*dstWidth;
        const int srcIndex = y*kTileSize;
        std::memcpy(&fbPtr[dstIndex], &tile.color[srcIndex], sizeof(decltype(tile.color)::value_type) * kTileSize);
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
    generateCircles(circles, 5000);

    sr::Profile::start();
    sortCirclesFarToNear(circles);

    std::thread ts[AVAILABLE_NUM_THREADS];

    // distribute works
    // Each thread check and collect elements that will appear in its own frame.
    // This reduces workload compared to serial implementation that we have to have 4 checks in tight loop
    // but now we can distribute focusly on 1 check for each thread but each thread has to process
    // an entire array of circles.
    for (int i=0; i<AVAILABLE_NUM_THREADS; ++i)
        ts[i] = std::thread([numTiles1D, i](){
            distributeWorksForTile(circles, distributedWorks[i], tiles[i]);
        });
    for (int i=0; i<AVAILABLE_NUM_THREADS; ++i)
        ts[i].join();

#define SHOW_STATS 0
#if SHOW_STATS == 1
    for (int j=0; j<numTiles1D; ++j)
    {
        for (int i=0; i<numTiles1D; ++i)
        {
            const int works = distributedWorks[i + j*numTiles1D].size();
            LOG("distributedWorks[%d][%d] = %lu works [%.3f%%]\n", i, j, works, static_cast<float>(works) / circles.size()); 
        }
    }
    LOG("total works of %d\n", circles.size());
#endif

    // render works
    for (int i=0; i<AVAILABLE_NUM_THREADS; ++i)
        ts[i] = std::thread([lineSize](int i){
            renderWork(distributedWorks[i], tiles[i], lineSize);
        }, i);
    for (int i=0; i<AVAILABLE_NUM_THREADS; ++i)
        ts[i].join();

    // combine works
    for (int i=0; i<AVAILABLE_NUM_THREADS; ++i)
        ts[i] = std::thread([&fb](int i){
            combineWork(tiles[i], fb);           
        }, i);
    for (int i=0; i<AVAILABLE_NUM_THREADS; ++i)
        ts[i].join();
    sr::Profile::endAndPrint();

    sr::TGAImage::write24("out.tga", fb, true);
    return 0;
}
