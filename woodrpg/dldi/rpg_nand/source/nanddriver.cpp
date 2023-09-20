/*
    nanddriver.cpp
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

#include "nanddriver.h"
#include "nandprotocol.h"
#include "rpgprotocol.h"
#include <cstring>
#include <cstdio>
#include <malloc.h>
#include "dbgtool.h"

#define NAND_DRIVER_VERSION 0x03

#define cBLK_USE       0x8000         // bit15=1=use else free
#define cBLK_CFG       0x4000         // Config Block or other cases
#define cBLK_uMSK      (cBLK_USE|cBLK_CFG)    // both bits are cleared = Block Free
#define cBLK_aMSK      (0x03ff|cBLK_INIT)   // bit0-10 block address mask: valid 0-1023


#define cBLK_BAD       0x4000         // Bad Block
#define cBLK_INIT      0x0400         // block init

#define cBLK_ECC       0x2000         // check for repeat ECC error

#define BYTES_PER_BLOCK   (2048 * 64)
#define BLOCKS_PER_ZONE   1024
#define LOGIC_BLKS_PER_ZONE 960
#define TOTAL_BLOCKS    BLOCKS_PER_ZONE * TOTAL_ZONES
#define SUB_PAGE_SIZE   512
static u8 TOTAL_ZONES   = 8;


// Addressing method
/*
    1st Byte    CA07, CA06, CA05, CA04, CA03, CA02, CA01, CA00
                 A07,  A06,  A05,  A04,  A03,  A02,  A01,  A00
    2nd Byte     LOW,  LOW,  LOW,  LOW, CA11, CA10, CA09, CA08
                 LOW,  LOW,  LOW,  LOW,  LOW,  A10,  A09,  A08
    3rd Byte    BA07, BA06, PA05, PA04, PA03, PA02, PA01, PA00
                 A18,  A17,  A16,  A15,  A14,  A13,  A12,  A11
    4th Byte    BA15, BA14, BA13, BA12, BA11, BA10, BA09, BA08
                 A26,  A25,  A24,  A23,  A22,  A21,  A20,  A19
    5th Byte     LOW,  LOW,  LOW, BA20, BA19, BA18, BA17, BA16
                 LOW,  LOW,  LOW,  A31,  A30,  A29,  A28,  A27

    CA10 - CA00  <-->  A10 - A00    |   11 bits
    PA05 - PA00  <-->  A16 - A11    | +  6 bits
    BA20 - BA06  <-->  A31 - A17    | + 15 bits
                                    ------------
                                    |   32 bits

    CAx = column address
    PAx = page address
    BAx = block address
*/

//      Redundant structure
//    BYTE   data_status  = 0xff = good, else bad block
//    BYTE   boot_loader_status = 0xff = free block else used block(maybe a cfg block)
//    BYTE   ECC[6]
//    WORD   Address1, Address2
//    BYTE   Reserved[4] = 0xff


static ALIGN(4) u16 rpgNANDLUTable[8192];//[TOTAL_BLOCKS]; // ���������� 1G byte ��Nand���Ҷ�����Ҫ�л�����
static u16 rpgNANDLUTableBlocks = 0;
static u16 lastFreeBlock = 0;
static u8 lastZone = 0xff;
static bool srcIsFree = false;
static ALIGN(4) u8 cfgPage[528];
static bool eccError = false;
static bool ecc2bitError = false;
static ALIGN(4) u8 ndGlobalRWBuffer[2112];

// 2k page nand sub page address: 0=0x000  1=0x210, 2=0x420, 3=0x630, 4=0x830
// 4 ���� 2k page �� redundant data
//static const u16 subPageAddr[5] = { 0x000, 0x210, 0x420, 0x630, 0x830 };
// ���ǣ�����ָ���ʱ��ֻ�� 200h ����ĵ�ַ��ioRpg���Զ����� 210h ����

bool ndInitNAND()
{
  //rpgNANDLUTable = NULL;
  //ndGlobalRWBuffer = (u8 *)malloc(2112);
  lastFreeBlock = 0;
  lastZone = 0xff;
  srcIsFree = false;
  ioRpgSetDeviceStatus( 0x01, 0, 0, 0, false );
  ioRpgSetMapTableAddress( MTN_NAND_OFFSET1, 0 );

  // read cfg page
  // 53 4d 54 44 4d 47 20 00 0e 00 01 06 0d (14) 03 02
  ioRpgReadNand( 0x00000000, cfgPage, 512 );
  if (cfgPage[0] != 'S' ||
    cfgPage[1] != 'M' ||
    cfgPage[2] != 'T' ||
    cfgPage[3] != 'D' ||
    cfgPage[4] != 'M' ||
    cfgPage[5] != 'G')  {
    return false;
  }

  dbg_printf("cfg page mem %08x\n", (u32)cfgPage );
  for( u32 i = 0; i < 16; ++i ) {
    dbg_printf("%02x", cfgPage[i] );
  }

  // read zones count
  TOTAL_ZONES = 1 << (cfgPage[0x0c] - 10);
  dbg_printf("total zones %d\n", TOTAL_ZONES);
  ndBuildLUT();
  return true;
}

void ndBuildLUT()
{
  rpgNANDLUTableBlocks = TOTAL_BLOCKS;
//  if( rpgNANDLUTable != NULL )
//    free( rpgNANDLUTable );
//  rpgNANDLUTable = (u16 *)malloc(TOTAL_BLOCKS * sizeof(u16));

  dbg_printf("build table\n");
  for( u16 i = 0; i < TOTAL_BLOCKS; ++i )
    rpgNANDLUTable[i] = cBLK_INIT;

  ALIGN(4) u8 redundData[16];            // redundant ���ݵĴ�С��16��ȡǰ12�����4���ֽ���ff
  //memset( redundData, 0, 16 );
  for( u16 z = 0; z < TOTAL_ZONES; ++z ) { // ����8��zone
    dbg_printf("zz %d ", z );
    for( u16 b = 0; b < BLOCKS_PER_ZONE; ++b ) { // ÿ��zone 128 ��block

      u16 * pTable = &rpgNANDLUTable[z*BLOCKS_PER_ZONE+b];
      // ���page����ÿ��block�ĵ�һ��page(2112) �ĵ�ַ
      u32 pageAddr = (z * BLOCKS_PER_ZONE + b) * 64 * 2048;

      // ��ȡ��2Kpage��redundant data
      npRead2KRedundant( pageAddr, redundData );

      // ȡ��һ��������redundant���ݣ�(byte 0-15)
      // ���ÿ��Ƿ��� good block
      bool isGoodBlock = (0xff == redundData[0]) && (0xff == redundData[1]);
      if( !isGoodBlock ) { // ���ͷ�����ֽڲ�ȫ�� FF
        *pTable |= cBLK_CFG;  // ��ô���� special block or bad block���ڱ������ϱ��
      }
      else { // ��good block
        u16 logicBlock = 0;
        ((u8 *)&logicBlock)[1] = redundData[8]; // 8�Ƕ�Ӧ���߼����ŵĵĵ�1���ֽڣ���8λ��

        // �ټ��ÿ��Ƿ��� free block
        if( 0xff != ((u8 *)&logicBlock)[1] ) { // ��8�ֽڲ�ΪFF˵���ÿ鱻ʹ�ù�(�ղ�����Ŀ�ȫ��FF)
          ((u8 *)&logicBlock)[0] = redundData[9]; // 9�Ƕ�Ӧ���߼����ŵĵĵ�2���ֽ�(��8λ)
          if( (((u8 *)&logicBlock)[1] == redundData[10]) // check if both addresses are matched
            && (((u8 *)&logicBlock)[0] == redundData[11]) ) { // 10/11 ��8/9 �� copy
              // �� logicBlock ������߼����ڱ����״̬ȡ����(��ס���� z ���zoneƫ��)
              u16 * pLogicBlock = &rpgNANDLUTable[z*BLOCKS_PER_ZONE+logicBlock];
              u16 blockStatus = *pLogicBlock;
              if( blockStatus & cBLK_INIT ) {         // ������߼�����ӳ����ﻹ�ǳ�ʼ״̬��û�б�����һ������飩
                *pLogicBlock = ( blockStatus & cBLK_uMSK ) | b; // ��ô�ͰѸ�������ֵ b ����������ҲҪ����������Ϣ
                *pTable |= cBLK_USE;            // ���[z*BLOCKS_PER_ZONE+b]���������Ѿ���ӳ�䵽ĳ���߼�����
                                      // ������� b ��0-1023��ֵ�����֮����ֱ����
                                      // Ҫ�Ⱥ� zone �����������
              }
              else {
                // ������߼����Ѿ������������飬
                // ��˵��������������redundant��Ϣ����ͬ�����߼��飬
                // ����������ģ�Ҫɾ�����������������ǲ���������pc������
                //nand_blk_erase(gPhyAdd);
                //*(xbyte*)0xe100 = 0xee;
              }
          } //else ��� 8,9 �� 10,11 ��ȣ���һ���Ļ�
        }
        //else �����8�ֽ���FF��ֱ��˵����������� Free Block
        //ʲôҲ���ܣ����� *pTable == cBLK_INIT(0x0400) ���״̬��֮��search��ʱ��
        //��cBLK_uMSK���������Ȼ��0��˵����free��
      }
    }
  }
  dbg_printf("LUT:\n");
  for( u8 i = 0; i < 16; ++i ) {
    dbg_printf("%04x", rpgNANDLUTable[i] );
  }
}

namespace ndwr {
  static u32 oldPhyAddr = 0;
  static u32 phyAddr = 0;
  static u8 page = 0;
  static u32 oldBlockAddr = 0;
  static u32 blockAddr = 0;
  static u8 remainPageCount = 0;
  static u16 logicBlkForPageWrite = 0xFFFF;
  static u32 lastAddr = 0xFFFFFFFF;
}

u32 ndLog2Phy( u32 logicAddress, u32 * oldPhyAddress )
{
  // �߼���ַ��9λȡ����Ȼ��� TOTAL_ZONES * 128 * 1024 * LOGIC_BLKS_PER_ZONE ��ģ����
  logicAddress &= 0xFFFFFE00;
  logicAddress %= TOTAL_ZONES * 128 * 1024 * LOGIC_BLKS_PER_ZONE;

  // ��� logic ��ַ���ڵ� zone
  u32 zone = (logicAddress / BYTES_PER_BLOCK) / LOGIC_BLKS_PER_ZONE;
  // ��� logic ��ַ���� zone ��logic block
  u16 logicBlock = (logicAddress / BYTES_PER_BLOCK) % LOGIC_BLKS_PER_ZONE;
  ndwr::logicBlkForPageWrite = logicBlock;
  // ��� logic ��ַβ���ϵ�ҳ��ַ
  u32 pageAddress = logicAddress & (BYTES_PER_BLOCK - 1);
  // ��� logic block ��Ӧ������block
  u16 * pPhyBlock = &rpgNANDLUTable[zone * BLOCKS_PER_ZONE + logicBlock];
  u16 phyBlock = (*pPhyBlock) & cBLK_aMSK;

  u32 phyAddress = 0xFFFFFE00;  // �����ַ���ڴ�ž���ת����������ַ
  //oldPhyAddress = 0xFFFFFE00; // �����ַ��д����ʱ��ת����ľ������ַ��
  //                                // Ҫ�������ڿ�����ݿ������µ������ַ���ڿ��У�
  //                                // Ȼ��������ڿ����

  bool isRead = (NULL == oldPhyAddress); // ��������oldPhyAddress��������˵����д����
  if( isRead ) { // ��ȡ����ֻ��Ҫ�򵥵Ļ�������ַ
    phyAddress = (zone * BLOCKS_PER_ZONE + phyBlock) * BYTES_PER_BLOCK + pageAddress;
  } else { // д�����Ҫ����free block��Ȼ��д���µĿ飬��ʵ���Զ�дƽ��
    //
    u16 freeBlock = ndSearchFreeBlock( zone, false );
    if( phyBlock & cBLK_INIT )  // ��� logicBlock ��û�б�����һ����Ӧ�������(phyBlock & cBLK_INIT)
      phyBlock = freeBlock; // ��ô�Ͱ������ָ��ղ��ҵ����Ǹ�freeBlock

    /* ���´��뱻 ndWritePages() �еĲ��ִ������
    //bNeedErase = (freeBlock != phyBlock);  // ����ҳ�����freeBlock���������phyBlock��ͬһ����
                        // ��д���Ժ�Ͳ�Ҫ�����������һ���������Ļ��ʻ���Ӧ�ñȽ�С�����Ƿ����˾��Ǵ�BUG

    */
    u16 * p = &rpgNANDLUTable[zone*BLOCKS_PER_ZONE+phyBlock];
    srcIsFree = !(*p & cBLK_uMSK);  // ���phyBlock��û�б��������ţ�˵��src�鱾�����һ���¿�
                    // ��¼��logicBlkForPageWrite�еı��
                    // �Ժ�������������ʱ��ͱ����õ�

    *p &= (~cBLK_USE);      // �� phyBlock ��Ӧ�ı������ݸĳɡ�δ��ӳ�䵽�߼��顱

    // �ղ��ҳ�����freeBlock���Ϊ USED��
    // ���phyBlock�Ѿ�����freeBlock����ô�������д���Ľ�����������ֱ������
        rpgNANDLUTable[zone*BLOCKS_PER_ZONE+freeBlock] |= cBLK_USE;
    // ���� LBA ���������Ǹ�phyBlock��ֵ�滻Ϊ�µ�freeBLock�����ֵ
        *pPhyBlock = (*pPhyBlock & cBLK_uMSK) | freeBlock;

    // ���������ַ�ǣ����µ�freeBlockΪ�����ĵ�ַ
    phyAddress = (zone * BLOCKS_PER_ZONE + freeBlock) * BYTES_PER_BLOCK + pageAddress;

    // ֮ǰ�������ַ�ǣ��Ա�freeblock�滻�����Ǹ�ԭ����blockΪ�����ĵ�ַ
    *oldPhyAddress = (zone * BLOCKS_PER_ZONE + phyBlock) * BYTES_PER_BLOCK + pageAddress;

    // ndLog2Phy �ĵ����߸��� oldPhyAddress ���ڵĿ����������������� phyAddress ���ڿ��У�
    // ������ oldPhyAddress ��������
  }
  return phyAddress;
}

u16 ndSearchFreeBlock( u8 zone, bool markFreeBlkToCfg )
{
  if( lastZone != zone ) {
    lastFreeBlock = 0;
    lastZone = zone;
  }

  u16 i = lastFreeBlock + 1; // i ��ʾ����block���������Ҫ
  if( i >= 1024 )
    i = 0;
  while( true )
  {
    if( !(rpgNANDLUTable[zone * BLOCKS_PER_ZONE + i] & cBLK_uMSK) ) {
      lastFreeBlock = i;
      rpgNANDLUTable[zone * BLOCKS_PER_ZONE + i] |= (markFreeBlkToCfg ? cBLK_CFG : 0);
      return i;
    }
    ++i;
    if( i >= 1024 )
      i = 0;
  }
  dbg_printf("libfat return no free block\n");
  return cBLK_INIT;
}

static u32 lastPhyAddr = 0xFFFFFFFF;
void ndReadPages( u32 addr, u32 pageCount, u8 * buffer )
{
  eccError = false;
  if( ndwr::remainPageCount > 0 ) {
    dbg_printf("ndFinishPartialCopy() called from ndReadPages()\n");
    ndFinishPartialCopy();
  }

  ALIGN(4) u8 readEcc[6];
  ALIGN(4) u8 nandEcc[6];
  u32 eccCode = 0;
  u8 * pReadEcc = (u8 *)(&eccCode);

  u32 phyAddr = ndLog2Phy( addr, NULL );
  for( u32 i = 0; i < pageCount; ++i ) {
    npReadPage528( phyAddr, ndGlobalRWBuffer );
    lastPhyAddr = phyAddr;
    ioRpgMemCopy( buffer, ndGlobalRWBuffer, 512 );
    //DC_FlushRange( buffer, 512 );
    //dmaCopyWords( 3, ndGlobalRWBuffer, buffer, 512 );

    // check ecc during dma copy
    ///////////////////////// ECC //////////////////////
    // generated ecc
    eccCode = npMakeECC256( (u16 *)ndGlobalRWBuffer );
    readEcc[0] = pReadEcc[0];
    readEcc[1] = pReadEcc[1];
    readEcc[2] = pReadEcc[2];
    eccCode = npMakeECC256( (u16 *)(ndGlobalRWBuffer + 256) );
    readEcc[3] = pReadEcc[0];
    readEcc[4] = pReadEcc[1];
    readEcc[5] = pReadEcc[2];

    ////////////////////////////////////////////////
    // stored ecc from nand
    nandEcc[0] = ndGlobalRWBuffer[512+2];
    nandEcc[1] = ndGlobalRWBuffer[512+3];
    nandEcc[2] = ndGlobalRWBuffer[512+4];
    nandEcc[3] = ndGlobalRWBuffer[512+5];
    nandEcc[4] = ndGlobalRWBuffer[512+6];
    nandEcc[5] = ndGlobalRWBuffer[512+7];

    if( !npCheckEcc( nandEcc, readEcc ) ) {
      dbg_printf("ndReadPages ecc error\n at addr %08x/%08x\n", addr, phyAddr );
      eccError = true;
    }

    //while( dmaBusy(3) ) {}

    // if ecc error occured, abandon this reading operation immediately,
    // caller should call ndCheckError() after every ndReadPages()
    // and try ndReadPages() again if ndCheckError() returned false
    if( eccError ) {
      return;
    }
    ///////////////////////// ECC END //////////////////////

    addr += 512;
    phyAddr += 512;
    buffer += 512;
    if( !(phyAddr & 0x1FFFF) ) {
      phyAddr = ndLog2Phy( addr, NULL );
    }
  }
}

// if this driver is used in DLDI, the homebrews may not call ndFinishPartialCopy() at the right time
// so we always call ndFinishPartialCopy() before write anything, any address and any length
void ndWritePagesSafe( u32 addr, u32 pageCount, const u8 * data )
{
  ndWritePages( addr, pageCount, data );
  ndFinishPartialCopy();
}

void ndWritePages( u32 addr, u32 pageCount, const u8 * data )
{
  if( ndwr::remainPageCount > 0 && ( addr != ndwr::lastAddr + 512 ) ) {
    dbg_printf("ndFinishPartialCopy() called from ndWritePages()\n");
    ndFinishPartialCopy();
  }
  //dbg_printf("ndWritePages() %08x %d %08x\n", addr, pageCount, data );

  if( 0 == ndwr::remainPageCount )
  {
    ndwr::oldPhyAddr = 0xFFFFFFE0;

    ndwr::logicBlkForPageWrite = cBLK_INIT;

    //////
    ndwr::phyAddr = ndLog2Phy( addr, &ndwr::oldPhyAddr );
    ndwr::page = (ndwr::oldPhyAddr & 0x1FFFF) / 512;
    ndwr::oldBlockAddr = ndwr::oldPhyAddr & (~0x1FFFF);
    ndwr::blockAddr = ndwr::phyAddr & (~0x1FFFF);
    //////

    // ���page���Ǵ�0��ʼ������ oldPhyAddr ��ǰ��飨0 to page-1�����¿�
    if( ndwr::page > 0 )
      ndNandMove( ndwr::oldBlockAddr, ndwr::blockAddr, ndwr::page );

    ndwr::remainPageCount = 64 * 4 - ndwr::page;
  }

  u8 subPageCount = 4 - (ndwr::phyAddr & 0x7ff) / 512;
  if( 0 == subPageCount ) {
    dbg_printf("subpagecount 0 at %08x\n", ndwr::phyAddr );
    wait_press_b();
  }

  if( subPageCount > pageCount )
    subPageCount = pageCount;
  u8 writeSubPages = subPageCount;

  u8 * pWriteBuffer = ndGlobalRWBuffer + 528 * (4 - subPageCount);
  // д�� addr ָ��������
  for( u32 i = 0; i < pageCount; ++i ) {
    //dbg_printf("subPageCount %d\n", subPageCount );
    npWritePage512( ndwr::phyAddr, data, ndwr::logicBlkForPageWrite, pWriteBuffer );

    ndwr::lastAddr = addr;
    addr += 512;
    ndwr::phyAddr += 512;
    data += 512;
    --ndwr::remainPageCount;
    --subPageCount;
    pWriteBuffer += 528;

    if( 0 == subPageCount ) {// flush
      ioRpgWriteNand( ndwr::phyAddr - writeSubPages * 512, pWriteBuffer - writeSubPages * 528, writeSubPages * 528 );
      subPageCount = 4;
      if( subPageCount > pageCount - i - 1 )
        subPageCount = pageCount - i - 1;
      writeSubPages = subPageCount;
      pWriteBuffer = ndGlobalRWBuffer;
    }

    if( i < pageCount - 1) {
      if( !(ndwr::phyAddr & 0x1FFFF) ) { // cross block, at here remainPageCount should be 0
        if( ndwr::oldBlockAddr != ndwr::blockAddr )
          npEraseBlock( ndwr::oldBlockAddr );
        ndwr::phyAddr = ndLog2Phy( addr, &ndwr::oldPhyAddr ); // ���¼��������ַ
        ndwr::page = (ndwr::oldPhyAddr & 0x1FFFF) / 512;
        ndwr::oldBlockAddr = ndwr::oldPhyAddr & (~0x1FFFF);
        ndwr::blockAddr = ndwr::phyAddr & (~0x1FFFF);
        ndwr::remainPageCount = (u8)(64 * 4);                 // ����պû�ҳ�ͽ����ˣ�����Ͳ�Ҫ����
      }
    }
  }

  // bug �����ڴ˺����� nds ��Ļ�ϣ���� 1520000 �� 1539000 Ҳ���ǰ׵Ļ���ȷʵ����������
  if( 0 == ndwr::remainPageCount ) {
    dbg_printf("ndWritePages() remain 0, ERASE old block ");
    dbg_printf("new%08x old%08x\n", ndwr::phyAddr, ndwr::oldPhyAddr );
    if( ndwr::oldBlockAddr != ndwr::blockAddr )
      npEraseBlock( ndwr::oldBlockAddr );
  } else {
    //dbg_printf("ndWritePages() remain %d\n", ndwr::remainPageCount );
  }
}

void ndFinishPartialCopy()
{
  // ���� oldPhyAddr + pageCount �ĺ��鵽�¿�
  if( 0 == ndwr::remainPageCount ) {
    if( ndwr::oldBlockAddr != ndwr::blockAddr )
      dbg_printf("note: dont mistaken erase\n");
    return;
  }

  //dbg_printf("ndwr::remainPageCount %d\n", ndwr::remainPageCount );

  ndNandMove( ndwr::oldBlockAddr + (64 * 4 - ndwr::remainPageCount) * 512,
      ndwr::blockAddr + (64 * 4 - ndwr::remainPageCount) * 512,
      ndwr::remainPageCount & 0xFF );

  // ��� oldPhyAddr != phyAddr
  // ��ô�Ͳ��� oldPhyAddr
  // �����Ȼ��ͬһ��block���Ͳ�������
  if( ndwr::oldBlockAddr != ndwr::blockAddr )
    npEraseBlock( ndwr::oldBlockAddr );

  ndwr::remainPageCount = 0;

  dbg_printf("FPC: (%08x to %08x)\n", ndwr::oldBlockAddr, ndwr::blockAddr );
}

// 512 Ϊ��λ
void ndCopyPages( u32 srcAddr, u32 destAddr, u8 pageCount )
{
  ALIGN(4) u8 readEcc[6];
  ALIGN(4) u8 nandEcc[6];
  u32 eccCode = 0;
  u8 * pReadEcc = (u8 *)(&eccCode);
  //for( u8 i = 0; i < pageCount; ++i )
  u32 count = pageCount;
  do
  {
    count = --pageCount;
    npReadPage528( srcAddr, ndGlobalRWBuffer );
    eccCode = npMakeECC256( (u16 *)ndGlobalRWBuffer );
    readEcc[0] = pReadEcc[0];
    readEcc[1] = pReadEcc[1];
    readEcc[2] = pReadEcc[2];
    eccCode = npMakeECC256( (u16 *)(ndGlobalRWBuffer + 256) );
    readEcc[3] = pReadEcc[0];
    readEcc[4] = pReadEcc[1];
    readEcc[5] = pReadEcc[2];

    ////////////////////////////////////////////////
    nandEcc[0] = ndGlobalRWBuffer[512+2];
    nandEcc[1] = ndGlobalRWBuffer[512+3];
    nandEcc[2] = ndGlobalRWBuffer[512+4];
    nandEcc[3] = ndGlobalRWBuffer[512+5];
    nandEcc[4] = ndGlobalRWBuffer[512+6];
    nandEcc[5] = ndGlobalRWBuffer[512+7];

    //// do ecc correct
    bool recomputeECC = false;
    if( !npCheckEcc( nandEcc, readEcc ) ) {
      /*debug*/{
      /*debug*/dbg_printf("ndCopyPages ecc error\n at addr %08x to %08x\n", srcAddr, destAddr );
      /*debug*/for( u8 i = 0; i < 6; ++i ) {
      /*debug*/ dbg_printf("%02x", nandEcc[i] );
      /*debug*/}
      /*debug*/dbg_printf("/");
      /*debug*/for( u8 i = 0; i < 6; ++i ) {
      /*debug*/ dbg_printf("%02x", readEcc[i] );
      /*debug*/}
      /*debug*/dbg_printf("/");
      /*debug*/
      /*debug*/npReadPage528( srcAddr, ndGlobalRWBuffer );
      /*debug*/eccCode = npMakeECC256( (u16 *)ndGlobalRWBuffer );
      /*debug*/readEcc[0] = pReadEcc[0];
      /*debug*/readEcc[1] = pReadEcc[1];
      /*debug*/readEcc[2] = pReadEcc[2];
      /*debug*/eccCode = npMakeECC256( (u16 *)(ndGlobalRWBuffer + 256) );
      /*debug*/readEcc[3] = pReadEcc[0];
      /*debug*/readEcc[4] = pReadEcc[1];
      /*debug*/readEcc[5] = pReadEcc[2];
      /*debug*/for( u8 i = 0; i < 6; ++i ) {
      /*debug*/ dbg_printf("%02x", readEcc[i] );
      /*debug*/}
      /*debug*/dbg_printf("\n");
      /*debug*/}
      // �������¿飬�ɿ�erase�����¿��ڱ���ȡ���ɿ��id��Ŀǰ��ʱֻ����һ��
      if( !npEccCorrectData( ndGlobalRWBuffer, nandEcc, readEcc ) ) {
        ecc2bitError = true;
        /*debug*/{
        /*debug*/dbg_printf("ndCopyPages FATAL ERROR\n at a:%08x to %08x\n", srcAddr, destAddr );
        /*debug*/for( u8 i = 0; i < 32; ++i ) {
        /*debug*/ dbg_printf("%02x", ndGlobalRWBuffer[i] );
        /*debug*/}
        /*debug*/dbg_printf("\n");
        /*debug*/for( u8 i = 0; i < 16; ++i ) {
        /*debug*/ dbg_printf("%02x", ndGlobalRWBuffer[512+i] );
        /*debug*/}
        /*debug*/}
        // recompute ecc and write the data with error to avoid user lose too much data
        // max 256 byte data lost per error
        recomputeECC = true;
        npWritePage512( destAddr, ndGlobalRWBuffer, ndwr::logicBlkForPageWrite, NULL );
        dbg_printf( "2bit err, recompute and write\n" );
        wait_press_b();
      } else {
        dbg_printf( "1bit err, corrected\n" );
      }
    }
    if( !recomputeECC )
      npWritePage528( destAddr, ndGlobalRWBuffer, ndwr::logicBlkForPageWrite );

    srcAddr += 512;
    destAddr += 512;
  } while( count );
}

void ndNandMove( u32 srcAddr, u32 destAddr, u8 pageCount )
{
  // �����ַ���Ǵ� block start ��ʼ
  // ��ôҪ���������ǵڼ���2k page�ĵڼ���512 sub page��Ȼ���� ndCopyPages �����
  //

  u8 subPageCount = (2048 - (srcAddr & 0x7FF)) / 512;
  subPageCount &= 3;

  if( subPageCount ) {
    ndCopyPages( srcAddr, destAddr, subPageCount );
    pageCount -= subPageCount;
    srcAddr += subPageCount * 512;
    destAddr += subPageCount * 512;
  }

  u8 _2kPageCount = pageCount / 4;
  // !(srcAddr & 0x1ffff) ��ʾ block ����ʼ��ַ
  // �������ʼ��ַ��ʼ�������п���src��freeblock��Ҫ���
  // ���src����� freeblock����ôҪ����ndCopyPages������ͷ��4��512page����д��logicblk���
  if( !(srcAddr & 0x1ffff) && _2kPageCount && srcIsFree ) {
    ndCopyPages( srcAddr, destAddr, 4 );
    pageCount -= 4;
    --_2kPageCount;
    srcAddr += 2048;
    destAddr += 2048;
    srcIsFree = false;
  }

  for( u8 i = 0; i < _2kPageCount; ++i )
  {
    //if( supportInteralCopy() )
    //do internal copy
    //else

    //ioRpgPageCopyNandToNand( srcAddr, destAddr );
    ndCopyPages( srcAddr, destAddr, 4 );

    srcAddr += 2048;
    destAddr += 2048;
  }

  // ���copy�������� 2k page ֮�󣬻�ʣ�� 512 page��ҲҪ�� ndCopyPages �����
  u8 remainSubPageCount = (pageCount & 3);
  if( remainSubPageCount )
    ndCopyPages( srcAddr, destAddr, remainSubPageCount );
}

bool ndCheckError()
{
  if( !eccError )
    return true;

  ecc2bitError = false;
  ndReplaceBlock( lastPhyAddr );
  if( ecc2bitError )
    return false;

  return true;
}

void ndReplaceBlock( u32 oldBlockAddress )
{
  // ����������鷴�����γ��� ecc ������ôӦ�ñ����Ϊ����
  oldBlockAddress &= (~0x1ffff);

  u16 block = (oldBlockAddress >> 17);
    u16 * p = &rpgNANDLUTable[block];
    bool eccRetry = (*p & cBLK_ECC);
    *p |= cBLK_ECC;


  u32 zone = (oldBlockAddress / BYTES_PER_BLOCK) / BLOCKS_PER_ZONE;
  u16 freeBlock = ndSearchFreeBlock( zone, false );
  u32 newBlockAddress = (zone * BLOCKS_PER_ZONE + freeBlock) << 17;

  ecc2bitError = false;
  dbg_printf("ndCopyPages called from ndReplaceBlock()\n");
  ndCopyPages( oldBlockAddress, newBlockAddress, (u8)256/* or 0, they are same*/ );
  dbg_printf( "BLOCK REPLACED %08x to %08x\n", oldBlockAddress, newBlockAddress );

  // repeat ecc error or 2 bit ecc error, should mark src block bad
  if( eccRetry || ecc2bitError ) {
    dbg_printf("eccRetry=%d, ecc2bitError=%d", eccRetry, ecc2bitError );
    // mark src block bad
    //u32 * f0data = (u32 *)malloc(528);
    u32 f0data[528/4];
    for( size_t i = 0; i < 528 / 4; ++i ) {
      f0data[i] = 0x0f0f0f0f;
    }
    ioRpgWriteNand( oldBlockAddress + 512 * 3, f0data, 528 );
    //free(f0data);
    wait_press_b();
  } else {
    npEraseBlock( oldBlockAddress );
  }

  // rebuild LUT after black replacement
  ndBuildLUT();
}
