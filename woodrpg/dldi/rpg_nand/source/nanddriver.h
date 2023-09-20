/*
    nanddriver.h
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

#ifndef _NANDDRIVER_H_
#define _NANDDRIVER_H_

#include <nds.h>

#ifdef __cplusplus
extern "C" {
#endif

bool ndInitNAND();

// �����߼�/�����ӳ���
void ndBuildLUT();

// �������߼���ַ������ֵ�������ַ
// oldPhyAddress ����Ϊ NULL�����ʾ��д��������Ҫ���� oldPhyAddress ��������
// ʹ�����߿��Բ��� �����Ϊ FREE �� oldPhyAddress ���ڵ� block
// logicBlock [out] �������� oldPhyAddress ���ڿ���ʵ��freeblock��ʱ�򣬴����
// ת���õ��� logicBlock ��ţ��Ա��Ժ�д���ʱ����Ը���freeblock����һ�����

u32 ndLog2Phy( u32 logicAddress, u32 * oldPhyAddress );

// ���������ָ���� zone ��Ѱ�� freeblock������ֵ��Χ 0 - 1023
u16 ndSearchFreeBlock( u8 zone, bool markFreeBlkToCfg );

// �����ݣ�addr ��ֵ ĩ 9 λ����0, addr �� LBA
void ndReadPages( u32 addr, u32 pageCount, u8 * buffer );

// д���ݣ�addr ��ֵ ĩ 9 λ����0, addr �� LBA
void ndWritePages( u32 addr, u32 pageCount, const u8 * data );

// д���ݣ�addr ��ֵ ĩ 9 λ����0, addr �� LBA��д����������δ��ɵ�block
void ndWritePagesSafe( u32 addr, u32 pageCount, const u8 * data );

// ִ�� nand ���ڲ� Page Copy ������һ��������д���ʱ�򣬰�old block�����ݿ�����free block
// pageCount ��ָ�ж��ٸ�512 page
// startSubPage ��ָ�ӵ�һ��2k page���ĸ�512k page��ʼ
// ���ڶ����� block ֮�俽����������� 64 �� page
// srcAddr �� destAddr Ӧ�ö���
void ndNandMove( u32 srcAddr, u32 destAddr, u8 pageCount );

// �� �ڲ� PageCopy �޷�ʹ�õ�ʱ���ֶ�����src��ַ��ʼ 528 Ϊ��λ�� page ��dest��ַ
// pageCount ��ָ�ж��ٸ�512 page
// startSubPage ��ָ�ӵ�һ��2k page���ĸ�512k page��ʼ
// ���ڶ����� block ֮�俽����������� 64 ��2kpage�������512Ҳ�պ�256��
void ndCopyPages( u32 srcAddr, u32 destAddr, u8 pageCount );

// �� �ڲ� PageCopy �޷�ʹ�õ�ʱ���ֶ�����src��ַ��ʼ 528 Ϊ��λ�� page ��dest��ַ
// pageCount ��ָ�ж��ٸ�512 page
// startSubPage ��ָ�ӵ�һ��2k page���ĸ�512k page��ʼ
// ���ڶ����� block ֮�俽����������� 64 ��2kpage�������512Ҳ�պ�256��
void ndCopyPages2K( u32 srcAddr, u32 destAddr, u8 pageCount );

// �����β������ǽ������ϴε����������Write������������β�������Write����(����Read)��
// ��ô��ִ��ndFinishPartialCopy�����ѸղŰ��Ƶ��Ǹ�block�ĺ�벿�����ݿ������µ�block��
void ndFinishPartialCopy();

/*
������
*/
void ndReplaceBlock( u32 oldBlockAddr );

// ����д�������Ƿ��� ecc ����
// ��������� ecc ����˵���ɿ���ܲ��ȶ���
// ��ô��ndReplaceBlock()�Ѿɿ鿽�����¿飬�ɿ���Ϊ���飬������������д����� 2 bit ecc error
// ��ô���¼����Ǹ�page��ecc���Ѵ��Ŵ��������д���¿飬���ⶪʧ��������
// ����ֵ�� 0  no error
//          1  1 bit ecc error
//          2  2 bit ecc error
bool ndCheckError();
/*
���������
*/

#ifdef __cplusplus
}
#endif

#endif//_NANDDRIVER_H_
