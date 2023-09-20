/*
    animation.cpp
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

#include "animation.h"

cAnimation::cAnimation()
{
    _visible = false;
}

cAnimation::~cAnimation()
{

}

void cAnimation::show()
{
    _visible = true;
}

void cAnimation::hide()
{
    _visible = false;
}


cAnimationManager::cAnimationManager()
{

}

cAnimationManager::~cAnimationManager()
{

}

void cAnimationManager::update()
{
    std::list< cAnimation * >::iterator itr = _animations.begin();
    while( itr != _animations.end() )
    {
        (*itr)->update();
        ++itr;
    }
}

void cAnimationManager::addAnimation( cAnimation * animation )
{
    _animations.push_back( animation );
}

void cAnimationManager::removeAnimation( cAnimation * animation )
{
    _animations.remove( animation );
}
