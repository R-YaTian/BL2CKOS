/*
    animation.h
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

#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include "ui.h"

class cAnimation
{

public:

    cAnimation();

    virtual ~cAnimation();

public:

    virtual void update() = 0;

    void show();

    void hide();

    bool visible() { return _visible; }

protected:

    akui::cPoint _position;
    akui::cSize _size;
    bool _visible;

};

class cAnimationManager
{

public:

    cAnimationManager();

    ~cAnimationManager();

public:

    void update();

    void addAnimation( cAnimation * animation );

    void removeAnimation( cAnimation * animation );

protected:

    std::list< cAnimation * > _animations;

};

typedef t_singleton< cAnimationManager > animationManager_s;
inline cAnimationManager & animationManager() { return animationManager_s::instance(); }


#endif//_ANIMATION_H_
