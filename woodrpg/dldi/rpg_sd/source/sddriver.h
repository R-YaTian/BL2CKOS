/*
    sddriver.h
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

#include <nds.h>


#ifdef __cplusplus
extern "C" {
#endif

bool sddInitSD();

void sddReadBlocks( u32 addr, u32 blockCount, void * buffer );

void sddWriteBlocks( u32 addr, u32 blockCount, const void * buffer );

void sddGetSDInfo( u8 info[8] );

bool isSDHC();

#ifdef __cplusplus
}
#endif
