# Performance comparison

The following is the performance benchmark result running for 1M times to render 3 lines (1 line completely overwrite another one).

You can look at full result at `analysis.txt` for `line3()` function, and `analysis2.txt` for `line7()` function.

## `line3()`

```
Each sample counts as 0.01 seconds.
  %   cumulative   self              self     total           
 time   seconds   seconds    calls  ms/call  ms/call  name    
 52.56      1.03     1.03  3000003     0.00     0.00  line3(sr::Vec2i, sr::Vec2i, sr::FrameBuffer&, unsigned int)
 23.47      1.49     0.46 204000204     0.00     0.00  sr::FrameBuffer::set(int, int, unsigned int)
 14.29      1.77     0.28 204000205     0.00     0.00  std::vector<unsigned int, std::allocator<unsigned int> >::operator[](unsigned long)
  4.59      1.86     0.09        1    90.02   110.02  sr::FrameBuffer::FrameBuffer(int, int)
  2.04      1.90     0.04  4000004     0.00     0.00  std::enable_if<std::__and_<std::__not_<std::__is_tuple_like<int> >, std::is_move_constructible<int>, std::is_move_assignable<int> >::value, void>::type std::swap<int>(int&, int&)
  1.02      1.92     0.02        1    20.00    20.00  std::vector<unsigned int, std::allocator<unsigned int> >::resize(unsigned long, unsigned int const&)
  1.02      1.94     0.02                             main
  0.51      1.95     0.01 12000012     0.00     0.00  std::remove_reference<int&>::type&& std::move<int&>(int&)
  0.26      1.96     0.01  3000003     0.00     0.00  sr::makeColorARGB(unsigned char, unsigned char, unsigned char)
  0.26      1.96     0.01                             frame_dummy
  0.00      1.96     0.00  6000006     0.00     0.00  sr::Vec2i::Vec2i(int, int)
```

## `line7()`

```
Each sample counts as 0.01 seconds.
  %   cumulative   self              self     total           
 time   seconds   seconds    calls  ms/call  ms/call  name    
 39.52      0.49     0.49 204000000     0.00     0.00  sr::FrameBuffer::set(int, int, unsigned int)
 34.68      0.92     0.43  3000000     0.00     0.00  line7(sr::Vec2i, sr::Vec2i, sr::FrameBuffer&, unsigned int)
 18.96      1.16     0.24 204000001     0.00     0.00  std::vector<unsigned int, std::allocator<unsigned int> >::operator[](unsigned long)
  3.23      1.20     0.04        1    40.01    40.01  std::vector<unsigned int, std::allocator<unsigned int> >::resize(unsigned long, unsigned int const&)
  1.61      1.22     0.02  3000000     0.00     0.00  sr::makeColorARGB(unsigned char, unsigned char, unsigned char)
  1.21      1.23     0.02 12000000     0.00     0.00  std::remove_reference<int&>::type&& std::move<int&>(int&)
  0.81      1.24     0.01                             main
  0.00      1.24     0.00  6000000     0.00     0.00  sr::Vec2i::Vec2i(int, int)
  0.00      1.24     0.00  4000000     0.00     0.00  std::enable_if<std::__and_<std::__not_<std::__is_tuple_like<int> >, std::is_move_constructible<int>, std::is_move_assignable<int> >::value, void>::type std::swap<int>(int&, int&)
```

Tested on Ubuntu 18.04 (5.0.0-37-generic #40~18.04.1-Ubuntu SMP) 8GB RAM, Intel(R) Core(TM) i5-3210M CPU @ 2.50GHz

# Build

## Build for Profiling

Use addtional of `-ggdb -pg -O0` when compile to be profiled (sampling) with `gprof` tool which is used to generated above result.

The step in sampling with `gprof` is as follows

1. Compile source file with additional flags as mentioned
2. Run the program
3. Generate sampling report via `gprof line gmon.out > analysis.txt`

These steps repeat 2 times for `line3()` and `line4()`.
Note that in `Makefile`, it is normal compile flags used, not for sampling with `gprof`. So you have to modify yourself.

