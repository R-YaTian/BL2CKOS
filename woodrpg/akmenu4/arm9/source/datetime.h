/*
    datetime.h
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

#ifndef _DATETIME_H_
#define _DATETIME_H_

#include <nds.h>
#include <time.h>
#include "singleton.h"
#include "../../share/stringtool.h"

class cDateTime
{
  private:
    tm iTimeParts;
  private:
    void FillTimeParts(void);
  public:
    cDateTime() {}
    ~cDateTime() {}
  public:
    static const char * weekdayStrings[];
    u16 year(void);
    u8 month(void);
    u8 day(void);
    u8 weekday(void);

    u8 hours(void);
    u8 minutes(void);
    u8 seconds(void);

    std::string getDateString(void);
    std::string getTimeString(void);
    std::string getTimeStampString(void);
};

typedef t_singleton< cDateTime > dateTime_s;
inline cDateTime & datetime() { return dateTime_s::instance(); }

#endif//_DATETIME_H_
