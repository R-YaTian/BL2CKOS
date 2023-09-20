/*
    popmenu.h
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

#ifndef _POPMENU_H_
#define _POPMENU_H_

#include <nds.h>
#include <vector>
#include "point.h"
#include "window.h"
#include "form.h"
#include "sigslot.h"

namespace akui {

class cPopMenu : public cWindow
{
public:

    cPopMenu( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text );

    ~cPopMenu();

public:

    void draw();

    bool process( const cMessage & msg );

    // ����ѡ�е���
    void popup();

    void addItem( size_t index, const std::string & itemText );

    void removeItem( size_t index );

    size_t itemCount();

    void clearItem();

    Signal1< s16 > itemClicked;

protected:

    void onShow();

    bool processKeyMessage( const cKeyMessage & msg );

    bool processTouchMessage( const cTouchMessage & msg );

    size_t itemBelowPoint( const cPoint & pt );

    void drawItems();

    s16 barWidth(void);

    cPoint _itemTopLeftPoint;

    std::vector< std::string > _items;

    s16 _selectedItemIndex;

    s16 _itemHeight;
    s16 _itemWidth;
    s16 _barLeft;

    COLOR _textColor;
    COLOR _textHighLightColor;
    COLOR _barColor;

    cBitmapDesc * _renderDesc;
    bool _skipTouch;
};

}
#endif//_POPMENU_H_
