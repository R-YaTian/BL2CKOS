/*
    globalsettings.cpp
    Copyright (C) 2007 Acekard, www.acekard.com
    Copyright (C) 2007-2009 somebody
    Copyright (C) 2009 yellow wood goblin
    Copyright (C) 2015 BL2CK

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

#include "globalsettings.h"
#include "inifile.h"
#include "systemfilenames.h"
#include "fontfactory.h"
#include <string.h>
#include <sys/stat.h>

cGlobalSettings::cGlobalSettings()
{
    fontHeight=12;
#if defined(_STORAGE_r4)
    brightness=1;
#endif
    language=1;
    langDirectory="English";
    uiName="zelda";
    startupFolder="...";
    fileListType=0;
    romTrim=0;
    showHiddenFiles=false;
    enterLastDirWhenBoot=true;
    scrollSpeed = EScrollFast;
    showGbaRoms=true;
    viewMode=EViewInternal;
    gbaSleepHack=false;
    gbaAutoSave=false;
    Animation=true;
    cheats=false;
    softreset=true;
    dma=true;
    sdsave=true;
    cheatDB=false;
    slot2mode=ESlot2Ask;
    saveExt=false;
    safeMode=false;
    show12hrClock=false;
    autorunWithLastRom=false;
    homebrewreset=false;
}

void cGlobalSettings::loadSettings()
{
    std::string temp;
    CIniFile ini( SFN_GLOBAL_SETTINGS );
    fontHeight = ini.GetInt( "system", "fontHeight", fontHeight );
    langDirectory = ini.GetString( "system", "langDirectory", langDirectory );
    uiName = ini.GetString( "system", "uiName", uiName );
    startupFolder = ini.GetString( "system", "startupFolder", startupFolder );
    if('/'!=startupFolder[startupFolder.length()-1]) startupFolder+='/';
    fileListType = ini.GetInt( "system", "fileListType", fileListType );
    romTrim = ini.GetInt( "system", "romTrim", romTrim );
    showHiddenFiles = ini.GetInt( "system", "showHiddenFiles", showHiddenFiles );
    enterLastDirWhenBoot = ini.GetInt( "system", "enterLastDirWhenBoot", enterLastDirWhenBoot );
    gbaSleepHack=ini.GetInt("system","gbaSleepHack",gbaSleepHack);
    gbaAutoSave=ini.GetInt("system","gbaAutoSave",gbaAutoSave);
    Animation=ini.GetInt("system","Animation",Animation);
    cheats=ini.GetInt("system","cheats",cheats);
    softreset=ini.GetInt("system","softreset",softreset);
    dma=ini.GetInt("system","dma",dma);
    sdsave=ini.GetInt("system","sdsave",sdsave);
    safeMode=ini.GetInt("system","safemode",safeMode);
    show12hrClock=ini.GetInt("system","Show12hrClock",show12hrClock);
    autorunWithLastRom=ini.GetInt("system","autorunWithLastRom",autorunWithLastRom);
    homebrewreset=ini.GetInt("system","homebrewreset",homebrewreset);

    temp = ini.GetString( "system", "scrollSpeed", "fast" );
    scrollSpeed = (temp=="slow")?EScrollSlow:((temp=="medium")?EScrollMedium:EScrollFast);

    temp = ini.GetString( "system", "viewMode", "icon" );
    viewMode = (temp=="list")?EViewList:((temp=="icon")?EViewIcon:EViewInternal);

    temp = ini.GetString( "system", "slot2mode", "ask" );
    slot2mode = (temp=="gba")?ESlot2Gba:((temp=="nds")?ESlot2Nds:ESlot2Ask);

    temp = ini.GetString( "system", "saveext", ".NDS.SAV" );
    saveExt = (temp==".SAV");

    struct stat st;
    if(0==stat(SFN_CHEATS,&st)) cheatDB=true;

#if defined(_STORAGE_r4)
    CIniFile iniBacklight(SFN_BACKLIGHT);
    brightness=iniBacklight.GetInt("brightness","brightness",brightness);
    setBrightness(brightness);
#endif
    updateSafeMode();
}

void cGlobalSettings::saveSettings()
{
    // the commented code means those parameters are not allow to change in menu
    CIniFile ini( SFN_GLOBAL_SETTINGS );
    //ini.SetInt( "system", "fontHeight", fontHeight );
    ini.SetString( "system", "uiName", uiName );
    ini.SetString( "system", "langDirectory", langDirectory );
    ini.SetInt( "system", "fileListType", fileListType );
    ini.SetInt( "system", "romTrim", romTrim );
    ini.SetInt( "system", "showHiddenFiles", showHiddenFiles );
    ini.SetInt("system","gbaSleepHack",gbaSleepHack);
    ini.SetInt("system","gbaAutoSave",gbaAutoSave);
    ini.SetInt("system","Animation",Animation);
    ini.SetInt("system","cheats",cheats);
    ini.SetInt("system","softreset",softreset);
    ini.SetInt("system","dma",dma);
    ini.SetInt("system","sdsave",sdsave);
    ini.SetInt("system","safemode",safeMode);
    ini.SetInt("system","Show12hrClock",show12hrClock);
    ini.SetInt("system","homebrewreset",homebrewreset);

    ini.SetString( "system", "scrollSpeed", (scrollSpeed==EScrollSlow)?"slow":((scrollSpeed==EScrollMedium)?"medium":"fast") );
    ini.SetString( "system", "viewMode", (viewMode==EViewList)?"list":((viewMode==EViewIcon)?"icon":"internal") );
    ini.SetString( "system", "slot2mode", (slot2mode==ESlot2Gba)?"gba":((slot2mode==ESlot2Nds)?"nds":"ask") );
    ini.SetString( "system", "saveext", saveExt?".SAV":".NDS.SAV" );

    ini.SaveIniFile( SFN_GLOBAL_SETTINGS );
    updateSafeMode();
}

void cGlobalSettings::updateSafeMode(void)
{
  if(safeMode)
  {
    fileListType=0;
    showHiddenFiles=false;
    viewMode=EViewInternal;
  }
}

u32 cGlobalSettings::CopyBufferSize(void)
{
  if(font().FontRAM()<300*1024) return 1024*1024;
  return 512*1024;
}

void cGlobalSettings::nextBrightness(void)
{
#if defined(_STORAGE_rpg) || defined(_STORAGE_ak2i) || defined(_STORAGE_r4idsn)
  fifoSendValue32(FIFO_USER_01,MENU_MSG_BRIGHTNESS_NEXT);
#elif defined(_STORAGE_r4)
  fifoSendValue32(FIFO_USER_01,MENU_MSG_BRIGHTNESS_GET);
  while(!fifoCheckValue32(FIFO_USER_01));
  u32 currentLevel=fifoGetValue32(FIFO_USER_01);
  brightness=(currentLevel+1)&3;

  setBrightness(brightness);
  CIniFile ini(SFN_BACKLIGHT);
  ini.SetInt("brightness","brightness",brightness);
  ini.SaveIniFile(SFN_BACKLIGHT);
#endif
}

#if defined(_STORAGE_r4)
void cGlobalSettings::setBrightness(u32 level)
{
  fifoSendValue32(FIFO_USER_01,MENU_MSG_BRIGHTNESS_SET0+(brightness&3));
}
#endif
