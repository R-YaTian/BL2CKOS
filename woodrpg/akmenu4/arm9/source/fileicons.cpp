/*
    fileicons.cpp
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

#include "fileicons.h"
#include "systemfilenames.h"
#include "ui/binaryfind.h"
#include "icons.h"
#include "globalsettings.h"
#include <sys/dir.h>
#include <elm.h>

cFileIconItem::cFileIconItem(const std::string& aFolderName,const std::string& aFileName):_loaded(false),_foldername(aFolderName),_filename(aFileName)
{
}

cBMP15& cFileIconItem::Icon(void)
{
  Load();
  return _icon;
}

void cFileIconItem::Load(void)
{
  if(!_loaded)
  {
    _icon=createBMP15FromFile(_foldername+_filename+".bmp");
    _loaded=true;
  }
}

static bool Comp(const cFileIconItem& item1,const cFileIconItem& item2)
{
  return strcasecmp(item1.FileName().c_str(),item2.FileName().c_str())<0;
}

bool cFileIconItem::operator<(const cFileIconItem& aValue)const
{
  return Comp(*this,aValue);
}

cFileIcons::cFileIcons()
{
  cIconPaths* paths=new cIconPaths;
  LoadFolder(*paths,SFN_UI_ICONS_DIRECTORY);
  LoadFolder(*paths,SFN_ICONS_DIRECTORY);
  for(cIconPaths::const_iterator it=paths->begin();it!=paths->end();++it)
  {
    _icons.push_back(*it);
  }
  delete paths;
}

void cFileIcons::LoadFolder(cIconPaths& aPaths,const std::string& aFolder)
{
  DIR_ITER* dir=diropen(aFolder.c_str());
  if(NULL!=dir)
  {
    struct stat st;
    char longFilename[MAX_FILENAME_LENGTH];
    while(dirnext(dir,longFilename,&st)==0)
    {
      if((st.st_mode&S_IFDIR)==0)
      {
        size_t len=strlen(longFilename);
        if(len>4)
        {
          char* extName=longFilename+len-4;
          if(strcasecmp(extName,".bmp")==0)
          {
            *extName=0;
            aPaths.insert(cFileIconItem(aFolder,longFilename));
          }
        }
      }
    }
    dirclose(dir);
  }
}

s32 cFileIcons::Icon(const std::string& aValue)
{
  if(!_icons.size()) return -1;
  std::vector<cFileIconItem>::iterator result=akui::binary_find(_icons.begin(),_icons.end(),cFileIconItem("",aValue),Comp);
  if(result==_icons.end())
  {
    return -1;
  }
  cBMP15& image=(*result).Icon();
  return ((image.valid()&&image.width()==32&&image.height()==32)?(result-_icons.begin()):-1);
}

void cFileIcons::Draw(s32 idx,u8 x,u8 y,GRAPHICS_ENGINE engine)
{
  gdi().maskBlt(_icons[idx].Icon().buffer(),x,y,32,32,engine);
}

void cFileIcons::DrawMem(s32 idx,void* mem)
{
  cIcons::maskBlt((const u16*)_icons[idx].Icon().buffer(),(u16*)mem);
}
