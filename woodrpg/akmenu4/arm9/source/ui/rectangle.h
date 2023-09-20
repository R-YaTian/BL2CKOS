/*
    rectangle.h
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

#ifndef _AKUI_RECTANGLE_H_
#define _AKUI_RECTANGLE_H_

#include "point.h"

namespace akui {

class cRect
{
    // ---------------------------------------------------------------------------------------------
    //  Xstruction
    // ---------------------------------------------------------------------------------------------
public:
    //! default contructor - both points at (0, 0)
    cRect() {}

    //! construction via two points
    cRect(const cPoint& p1, const cPoint& p2);

    //! construction from components of the two corner points
    cRect(int x1, int y1, int x2, int y2);


    // ---------------------------------------------------------------------------------------------
    //  Public Interface
    // ---------------------------------------------------------------------------------------------
public:
    //! returns the position (i.e. lowest value corner) of the rectangle
    cPoint position() const { return position_; }

    //! returns half the rectangle's size
    cSize halfSize() const { return cPoint( size_.x >> 1, size_.y >> 1 ); }

    //! the center point
    cPoint centerPoint() const;

    //! is the passed in point within the rectangle's horizontal edges
    bool isAboveAndBelow(const cPoint& p) const;

    //! is the passed in point within the rectangle's vertical edges
    bool isLeftAndRightOf(const cPoint& p) const;

    //! is the passed in point within the rectangle's edges
    bool surrounds(const cPoint& p) const;

    //! the smallest x coord
    int minX() const { return position_.x; }

    //! the smallest y coord
    int minY() const { return position_.y; }

    //! the largest x coord
    int maxX() const { return position_.x + size_.x; }

    //! the largest y coord
    int maxY() const { return position_.y + size_.y; }

    //! the top right corner point
    cPoint topRight() const;

    //! the bottom left corner point
    cPoint bottomLeft() const;

    //! the top left corner point
    cPoint topLeft() const;

    //! the bottom right corner point
    cPoint bottomRight() const;

    //! returns the current size
    cPoint size() const { return size_; }

    //! move to the passed in point
    cRect& setPosition(const cPoint& p)
    {
        position_ = p;
        return *this;
    }

    //! resize to the passed in size
    cRect& setSize(const cSize& s)
    {
        size_ = s;
        return *this;
    }


    //! translate through the passed in offset
    cRect& translateBy(const cPoint& p);

    //! \brief expands the rectangle in all directions by the passed in amount
    //! \note Accepts negative values
    cRect& expandBy(int amount);

    //! \brief expands the rectangle horizontally by the passed in amount
    //! \note Accepts negative values
    cRect& expandWidthBy(int amount);

    //! \brief expands the rectangle horizontally by the passed in amount
    //! \note Accepts negative values
    cRect& expandHeightBy(int amount);

    //! the height of the rectangle
    int height() const;

    //! the width of the rectangle
    int width() const;

    //! Snaps the size and position of the rectangle to the nearest whole number positions
    cRect& snap();


    // ---------------------------------------------------------------------------------------------
    //  Operators
    // ---------------------------------------------------------------------------------------------
public:
    //! equality
    bool operator == (const cRect& rect) const;

    //! inequality
    bool operator != (const cRect& rect) const;



    // ---------------------------------------------------------------------------------------------
    //  Implementation
    // ----------------------------------------------------------------------------------------------
private:
    //! lowest value corner of the rectangle
    cPoint position_;

    //! extents of the rectangle
    cPoint size_;

};

////! output operator
//std::ostream& operator << (std::ostream& os, const cRect& rect);


    // *************************************************************************************************
} // namespace Core

#endif//_AKUI_RECTANGLE_H_
