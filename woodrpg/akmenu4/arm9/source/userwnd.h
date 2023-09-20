/*
    userwnd.h
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

#ifndef _USERWINDOW_H_
#define _USERWINDOW_H_

#include "window.h"
#include "bmp15.h"
#include "datetime.h"
#include "singleton.h"
#include "point.h"


class cUserWindow : public akui::cWindow
{
public:

    cUserWindow();

    ~cUserWindow() {}

public:


    void draw();

    akui::cWindow & loadAppearance(const std::string& aFileName ){return *this;}

protected:

    void init();

    cBMP15 _userPicture;
    int _px;
    int _py;
    std::string _userText;
    int _tx;
    int _ty;
    int _tw;
    int _th;
    u16 _userTextColor;
    std::string _userName;
    int _ux;
    int _uy;
    u16 _userNameColor;
    bool _showUserName;
    bool _showCustomText;
    bool _showCustomPic;

};

typedef t_singleton< cUserWindow > UserWindow_s;
inline cUserWindow & userWindow() { return UserWindow_s::instance(); }


#endif//_UserWindow_H_
