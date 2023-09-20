/*
    msgbox.h
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

#ifndef _AKUI_MESSAGEBOX_H_
#define _AKUI_MESSAGEBOX_H_

#include "window.h"
#include "form.h"
#include "formdesc.h"
#include "button.h"

namespace akui {

#define MB_OK                1
#define MB_CANCEL            2
#define MB_OK_CANCEL        3
#define MB_YES                4
#define MB_NO                8
#define MB_YES_NO            12

#define ID_OK                1
#define ID_CANCEL            0
#define ID_YES                1
#define ID_NO                0


class cMessageBox : public cForm
{
public:
    friend u32 messageBox( cWindow * parent, const std::string & title, const std::string & msg, u32 style );

    cMessageBox( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & title, const std::string & msg, u32 style );

    ~cMessageBox();

public:

    void draw();

    bool process( const cMessage & msg );

    cWindow& loadAppearance(const std::string& aFileName );

    //u32 doModal();

    //u32 msgRet() { return _msgRet; }

protected:

    void onOK();

    void onCANCEL();

    void onYES() { onOK(); }

    void onNO() { onCANCEL(); }

    bool processKeyMessage( const cKeyMessage & msg );

    bool processTouchMessage( const cTouchMessage & msg );

    std::string _msg;

    cPoint _textPoision;

    u32 _style;

    cButton * _buttonOK;
    cButton * _buttonCANCEL;
    cButton * _buttonYES;
    cButton * _buttonNO;
    cFormDesc _renderDesc;
};

u32 messageBox( cWindow * parent, const std::string & title, const std::string & msg, u32 style );

} // namespace akui

#endif//_MESSAGEBOX_H_
