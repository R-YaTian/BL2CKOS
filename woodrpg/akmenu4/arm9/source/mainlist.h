/*
    mainlist.h
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

#ifndef _MAINLIST_H_
#define _MAINLIST_H_

#include <nds.h>
#include "listview.h"
#include "sigslot.h"
#include "keymessage.h"
#include "touchmessage.h"
#include "dsrom.h"
#include "zoomingicon.h"

#if defined(_STORAGE_rpg)
#define SD_ROOT_0 "fat1:"
#else
#define SD_ROOT_0 "fat0:"
#endif
#define SD_ROOT SD_ROOT_0"/"

// 显示游戏列表，文件列表等等
class cMainList : public akui::cListView
{
public:

    enum VIEW_MODE
    {
        VM_LIST = 0,
        VM_ICON,
        VM_INTERNAL
    };

    enum //COLUMN_LIST
    {
        ICON_COLUMN = 0,
        SHOWNAME_COLUMN = 1,
        INTERNALNAME_COLUMN = 2,
        REALNAME_COLUMN = 3,
        SAVETYPE_COLUMN = 4,
        FILESIZE_COLUMN = 5
    };

public:

    cMainList( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text );

    ~cMainList();

public:

    int init();

    bool enterDir( const std::string & dirName );

    void backParentDir();

    void refresh();

    std::string getCurrentDir();

    bool getRomInfo( u32 rowIndex, DSRomInfo & info ) const;

    void setRomInfo( u32 rowIndex, const DSRomInfo & info );

    void setViewMode( VIEW_MODE mode );

    std::string getSelectedFullPath();

    std::string getSelectedShowName();

    VIEW_MODE getViewMode() { return _viewMode; }

    void arrangeIcons();

    akui::Signal1< u32 > selectedRowHeadClicked;

    akui::Signal0 directoryChanged;

    akui::Signal1< bool& > animateIcons;

public:

    bool IsFavorites(void);

    void SwitchShowAllFiles(void);

    const std::vector<std::string>* Saves(void);

protected:

    void draw();

    void drawIcons();                        // 直接画家算法画 icons

    enum {
        POSITION = 0,
        CONTENT = 1
    };

    void updateActiveIcon( bool updateContent );  // 更新活动图标的坐标等等

    void updateInternalNames(void);

protected:

    void onSelectedRowClicked( u32 index );

    void onSelectChanged( u32 index );

    void onScrolled( u32 index );

protected:

    VIEW_MODE _viewMode;

    std::string _currentDir;

    std::vector< std::string > _extnameFilter;

    std::vector< DSRomInfo > _romInfoList;

    cZoomingIcon _activeIcon;

    float _activeIconScale;

    bool _showAllFiles;

    std::vector< std::string > _saves;

protected:
    u32 _topCount;
    u32 _topuSD;
    u32 _topSlot2;
    u32 _topFavorites;
public:
    u32 Slot2(void) {return _topSlot2;}
};


#endif//_MAINLIST_H_
