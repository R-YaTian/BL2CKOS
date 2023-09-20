/*
    bmp15.h
    Copyright (C) 2007 Acekard, www.acekard.com
    Copyright (C) 2007-2009 somebody
    Copyright (C) 2009 yellow wood goblin

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _BMP15_H_
#define _BMP15_H_

#include <nds.h>
#include <memory>
#include <string>

class cBMP15
{
    friend cBMP15 createBMP15( u32 width, u32 height );
    friend cBMP15 createBMP15FromFile( const std::string & filename );
    friend cBMP15 createBMP15FromMem( void * mem );
    //friend void destroyBMP15( cBMP15 * bmp );

public:

    explicit cBMP15();
    explicit cBMP15( u32 width, u32 height );
    ~cBMP15();

public:

    u32 width() const { return _width; }        // width is memory bitmap's width

    u32 height() const { return _height; }    // height is memory bitmap's height

    u32 pitch() const { return _pitch; }        // pitch returns bytes per line

    u32 * buffer() { return _buffer; }

    const u32 * buffer() const { return _buffer; }

    bool valid() const { return NULL != _buffer; }

protected:

    u32 _width;

    u32 _height;

    u32 _pitch;

    u32 * _buffer;    // 按 32 位地址对齐，可以在 bitblt 的时候加快速度
};

cBMP15 createBMP15( u32 width, u32 height );
cBMP15 createBMP15FromFile( const std::string & filename );
cBMP15 createBMP15FromMem( void * mem );
//void destroyBMP15( cBMP15 * bmp );
//void destroyBMP15ByFilename( const char * filename );


#endif//_BMP15_H_
