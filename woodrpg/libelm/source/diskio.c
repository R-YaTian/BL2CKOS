/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/

#include <elm_bool.h>
#include <diskio.h>
#include <stdio.h>
#include <limits.h>
#include <time.h>
#ifdef USE_DLDI
#include <string.h>
#include <nds/arm9/dldi.h>
#else
#include <string.h>
int ioWoodInit(unsigned char aDrive);
int ioWoodStatus(unsigned char aDrive);
int ioWoodRead(unsigned char aDrive,unsigned char* aBuffer,unsigned long aSector,unsigned char aCount);
int ioWoodWrite(unsigned char aDrive,const unsigned char* aBuffer,unsigned long aSector,unsigned char aCount);
void ioWoodSync(unsigned char aDrive);
#endif
/*-----------------------------------------------------------------------*/
/* Correspondence between physical drive number and physical drive.      */

#define NAND 0
#define SD   1

/*-----------------------------------------------------------------------*/
/* CACHE                                                                 */

#define SECTOR_SIZE 512
#define CACHE_SIZE 8

typedef struct _CACHE_
{
  BYTE data[SECTOR_SIZE];
  BOOL valid;
  DWORD sector;
  DWORD stamp;
  DWORD drive;
} CACHE;

static CACHE _cache[CACHE_SIZE];
static DWORD accessCounter=0;

static inline DWORD stamp(void) {return ++accessCounter;}

static BOOL read_from_cache(DWORD drv,DWORD sector,BYTE *buff)
{
  BOOL res=FALSE; size_t ii;
  for(ii=0;ii<CACHE_SIZE;++ii)
  {
    if(_cache[ii].valid&&_cache[ii].drive==drv&&_cache[ii].sector==sector)
    {
      _cache[ii].stamp=stamp();
      memcpy(buff,_cache[ii].data,SECTOR_SIZE);
      res=TRUE;
      break;
    }
  }
  return res;
}

static void add_to_cache(DWORD drv,DWORD sector,BYTE *buff)
{
  size_t ii;
  int oldest_item=-1,free_item=-1;
  DWORD oldest_stamp=UINT_MAX;
  for(ii=0;ii<CACHE_SIZE;++ii)
  {
    if(!_cache[ii].valid)
    {
      free_item=ii;
      break;
    }
    else
    {
      if(_cache[ii].stamp<oldest_stamp)
      {
        oldest_item=ii;
        oldest_stamp=_cache[ii].stamp;
      }
    }
  }
  if(free_item<0) free_item=oldest_item;
  if(free_item<0) return; //ALGORITHM ERROR
  memcpy(_cache[free_item].data,buff,SECTOR_SIZE);
  _cache[free_item].valid=TRUE;
  _cache[free_item].sector=sector;
  _cache[free_item].stamp=stamp();
  _cache[free_item].drive=drv;
}

static void invalidate_cache(DWORD drv,DWORD sector,BYTE count)
{
  size_t ii;
  DWORD top=sector+count;
  for(ii=0;ii<CACHE_SIZE;++ii)
  {
    if(_cache[ii].valid&&_cache[ii].drive==drv&&_cache[ii].sector>=sector&&_cache[ii].sector<top)
    {
      _cache[ii].valid=FALSE;
    }
  }
}

/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */

#ifdef USE_DLDI
static const DISC_INTERFACE* disk=NULL;
#endif

DSTATUS disk_initialize(BYTE drv)
{
  switch(drv)
  {
    case NAND:
#ifdef USE_DLDI
      if(!disk)
      {
        disk=dldiGetInternal();
        if(!disk->startup()) break;
      }
      return disk->isInserted()?0:STA_NOINIT;
#else
    case SD:
      return ioWoodInit(drv)?0:STA_NOINIT;
#endif
  }
  return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */

DSTATUS disk_status (
  BYTE drv    /* Physical drive nmuber (0..) */
)
{
  switch(drv)
  {
    case NAND:
#ifdef USE_DLDI
      return disk->isInserted()?0:STA_NOINIT;
#else
    case SD:
      return ioWoodStatus(drv)?0:STA_NOINIT;
#endif
  }
  return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

DRESULT disk_read_internal (
  BYTE drv,   /* Physical drive nmuber (0..) */
  BYTE *buff,   /* Data buffer to store read data */
  DWORD sector, /* Sector address (LBA) */
  BYTE count    /* Number of sectors to read (1..255) */
)
{
  switch(drv)
  {
    case NAND:
#ifdef USE_DLDI
      return disk->readSectors(sector,count,buff)?RES_OK:RES_ERROR;
#else
    case SD:
      return ioWoodRead(drv,buff,sector,count)?RES_OK:RES_ERROR;
#endif
  }
  return RES_PARERR;
}

DRESULT disk_read (
  BYTE drv,   /* Physical drive nmuber (0..) */
  BYTE *buff,   /* Data buffer to store read data */
  DWORD sector, /* Sector address (LBA) */
  BYTE count    /* Number of sectors to read (1..255) */
)
{
  DRESULT res=RES_PARERR;
  switch(drv)
  {
    case NAND:
    case SD:
      if(count==1&&read_from_cache(drv,sector,buff)) return RES_OK;
      if(count!=1) invalidate_cache(drv,sector,count);
      res=disk_read_internal(drv,buff,sector,count);
      if(res==RES_OK&&count==1) add_to_cache(drv,sector,buff);
      break;
  }
  return res;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */

#if _READONLY == 0
DRESULT disk_write (
  BYTE drv,     /* Physical drive nmuber (0..) */
  const BYTE *buff, /* Data to be written */
  DWORD sector,   /* Sector address (LBA) */
  BYTE count      /* Number of sectors to write (1..255) */
)
{
  switch(drv)
  {
    case NAND:
#ifndef USE_DLDI
    case SD:
#endif
      invalidate_cache(drv,sector,count);
#ifdef USE_DLDI
      return disk->writeSectors(sector,count,buff)?RES_OK:RES_ERROR;
#else
      return ioWoodWrite(drv,buff,sector,count)?RES_OK:RES_ERROR;
#endif
  }
  return RES_PARERR;
}
#endif /* _READONLY */



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */

DRESULT disk_ioctl (
  BYTE drv,   /* Physical drive nmuber (0..) */
  BYTE ctrl,    /* Control code */
  void *buff    /* Buffer to send/receive control data */
)
{
#ifdef USE_DLDI
  if(drv==NAND&&ctrl==CTRL_SYNC)
  {
    return disk->clearStatus()?RES_OK:RES_ERROR;
  }
  return RES_PARERR;
#else
  if(ctrl==CTRL_SYNC)
  {
    ioWoodSync(drv);
  }
  return RES_OK;
#endif
}

#define MAX_HOUR 23
#define MAX_MINUTE 59
#define MAX_SECOND 59

#define MAX_MONTH 11
#define MIN_MONTH 0
#define MAX_DAY 31
#define MIN_DAY 1

DWORD get_fattime()
{
  struct tm timeParts;
  time_t epochTime;

  if (time(&epochTime) == (time_t)-1) {
    return 0;
  }
  localtime_r(&epochTime, &timeParts);

  // Check that the values are all in range.
  // If they are not, return 0 (no timestamp)
  if ((timeParts.tm_hour < 0) || (timeParts.tm_hour > MAX_HOUR))  return 0;
  if ((timeParts.tm_min < 0) || (timeParts.tm_min > MAX_MINUTE)) return 0;
  if ((timeParts.tm_sec < 0) || (timeParts.tm_sec > MAX_SECOND)) return 0;
  if ((timeParts.tm_mon < MIN_MONTH) || (timeParts.tm_mon > MAX_MONTH)) return 0;
  if ((timeParts.tm_mday < MIN_DAY) || (timeParts.tm_mday > MAX_DAY)) return 0;

  return (
    (((timeParts.tm_year - 80) & 0x7F) << 25) | // Adjust for MS-FAT base year (1980 vs 1900 for tm_year)
    (((timeParts.tm_mon + 1) & 0xF) << 21) |
    ((timeParts.tm_mday & 0x1F) << 16) |
    ((timeParts.tm_hour & 0x1F) << 11) |
    ((timeParts.tm_min & 0x3F) << 5) |
    ((timeParts.tm_sec >> 1) & 0x1F)
  );
}
