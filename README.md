# sr_st

Software Renderer Studying repo.

Target support is `gcc` first then later will add support for `MSVC`.
For now, the former support is there.

Various testbeds are inside `apps`.
In each directory, it's self-contained applications using Makefile to build.
Just go there, and hit `make`.

# Common

Inside `common/` directory, it's common code consisting of the following systems

* `FrameBuffer` - act as holder for pixels before writing into image file
* `GraphicsUtil` - utility graphics functions
* `Logger` - logging utlity to standard output, or standard error output
* `MathUtil` - math related utility functions i.e. random integer or floating-point number
* `ObjLoader` - `.obj` file loader
* `Profile` - profiler measuring executable time of function or code conveniently
* `TGAImage` - `.tga` image writter
* `Types` - supports essential math structure i.e. `Vec2i` for integer, `Vec2f` for floating-point type, etc

# Plan

Each program in its separate directory i.e. `wireframe_renderer/` represents validation and testbed
for one or more of software renderer's technique. Whenever it matures, it will be added into the common
code inside `common/` gradually.

# License

MIT, Wasin Thonkaew
It will be awesome if you benefit from using any part of this project. Please let me know, it will
make me happy.
