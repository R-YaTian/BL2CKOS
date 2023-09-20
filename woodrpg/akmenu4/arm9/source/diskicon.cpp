/*
    diskicon.cpp
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

#include "diskicon.h"
#include "bmp15.h"
#include "inifile.h"
#include "systemfilenames.h"
#include "../../share/memtool.h"
#include "../../share/timetool.h"
#include "globalsettings.h"

using namespace akui;



cDiskIcon::cDiskIcon() : cWindow( NULL, "diskicon" )
{
    _size = cSize( 0, 0 );
    _position = cPoint( 0, 0 );
    _engine = GE_MAIN;
    _icon.init( 1 );
    _icon.setPosition( 226, 174 );
    _icon.setPriority( 3 );
    _icon.setBufferOffset( 16 );
    _icon.show();

    fillMemory( _icon.buffer(), 32 * 32 * 2, 0x00000000 );
}

void cDiskIcon::draw()
{
    // do nothing
}

cWindow & cDiskIcon::loadAppearance(const std::string& aFileName )
{

    CIniFile ini( SFN_UI_SETTINGS );

    u16 x = ini.GetInt( "disk icon", "x", 238 );
    u16 y = ini.GetInt( "disk icon", "y", 172 );
    _icon.setPosition( x, y );

    cBMP15 icon = createBMP15FromFile( aFileName );

    u32 pitch = icon.pitch()>>1;
    for( u8 i = 0; i < icon.height(); ++i ) {
        for( u8 j = 0; j < icon.width(); ++j ) {
            ((u16 *)_icon.buffer())[i*32+j] = ((u16 *)icon.buffer())[i * pitch + j];
        }
    }
    dbg_printf("cDiskIcon::loadAppearance ok %d\n", icon.valid() );
    return *this;
}

void cDiskIcon::blink( void )
{
    if( _icon.visible() )
        _icon.hide();
    else
        _icon.show();
}

void cDiskIcon::turnOn()
{
    _icon.show();
}

void cDiskIcon::turnOff()
{
    _icon.hide();
}
