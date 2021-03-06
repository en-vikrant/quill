/*
    Copyright (c) 2019, reMarkable AS <technology@remarkable.no>
    Copyright (c) 2019, Gunnar Sletta <gunnar@crimson.no>
    All rights reserved.


    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

    1. Redistributions of source code must retain the above copyright notice,
       this list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.

    3. Neither the name of the copyright holder nor the names of its
       contributors may be used to endorse or promote products derived from
       this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
    CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
    PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
    NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "quill.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "rasterbuffer.h"
#include "solidcolorfill.h"

using namespace Quill;
using namespace std;

struct Consumer
{
    typedef Varying2D Varyings;

    RasterBuffer buffer;

    void operator()(int x, int y, int length, Varying2D v, Varying2D dx);
};


void Consumer::operator()(int x, int y, int length, Varying2D v, Varying2D dx)
{
    unsigned int *dst = buffer.scanline(y) + x;

    for (int i=0; i<length; ++i) {
        int r = int(255 * v.x);
        int b = int(255 * v.y);
        dst[i] += 0xff000000 | (r) | (b << 16);
        dst[i] += 0x00003f00;
        v = v + dx;
    }
}


int main(int argc, char **argv)
{
    LerpRaster<Consumer> raster;

    RasterBuffer *buffer = &raster.fill.buffer;
    buffer->allocate(100, 100);
    buffer->fill(0xff000000);

    raster(Triangle(Vertex(10, 10), Vertex(90, 50), Vertex(40, 90)), Varying2D(1, 1), Varying2D(1, 0), Varying2D(0, 1));
    raster(Triangle(Vertex(10, 10), Vertex(90, 50), Vertex(95,  5)), Varying2D(1, 1), Varying2D(1, 0), Varying2D(0, 0));
    raster(Triangle(Vertex(10, 10), Vertex( 8, 92), Vertex(40, 90)), Varying2D(1, 1), Varying2D(0, 0), Varying2D(0, 1));
    raster(Triangle(Vertex(93, 93), Vertex(90, 50), Vertex(40, 90)), Varying2D(0, 0), Varying2D(1, 0), Varying2D(0, 1));

    stbi_write_png("triangle.png",
                   buffer->width,
                   buffer->height,
                   4,
                   buffer->data,
                   buffer->width * sizeof(unsigned int));

    std::cout << "wrote 'triangle.png'..." << std::endl;
}
