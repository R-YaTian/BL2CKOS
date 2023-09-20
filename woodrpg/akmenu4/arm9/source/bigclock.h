/*
    bigclock.h
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

#ifndef _BIGCLOCK_H_
#define _BIGCLOCK_H_

#include "window.h"
#include "bmp15.h"
#include "datetime.h"
#include "singleton.h"


//class cClockNumber : public
class cBigClock : public akui::cWindow
{
public:

    cBigClock();

    ~cBigClock() {}

public:

    void init();

    void draw();

    void blinkColon();

    akui::cWindow & loadAppearance(const std::string& aFileName );

protected:

    void drawNumber( u8 id, u8 number );

    void drawColon();

    cBMP15 _numbers;
    cBMP15 _colon;

    bool _show;
    bool _colonShow;
    bool _ampmShow;
    akui::cPoint _ampmPosition;
    COLOR _ampmColor;
};


typedef t_singleton< cBigClock > bigClock_s;
inline cBigClock & bigClock() { return bigClock_s::instance(); }


#endif//_BIGCLOCK_H_
