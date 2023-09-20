/*
Copyright (c) 2009-2011, yellow wood goblin
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the yellow wood goblin nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL YELLOW WOOD GOBLIN BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __ELM_H__
#define __ELM_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int ELM_Mount(void);
void ELM_Unmount(void);
int ELM_ClusterSizeFromHandle(int fildes,uint32_t* size);
int ELM_SectorsPerClusterFromHandle(int fildes,uint32_t* per);
int ELM_ClusterSizeFromDisk(int disk,uint32_t* size);
int ELM_ClustersFromDisk(int disk,uint32_t* clusters);
int ELM_FreeClustersFromDisk(int disk,uint32_t* clusters);
int ELM_SectorsFromDisk(int disk,uint32_t* sectors);
uint32_t ELM_GetFAT(int fildes,uint32_t cluster,uint32_t* sector);
int ELM_DirEntry(int fildes,uint64_t* entry);

void NandFast(void);
void NandFlush(void);

#ifdef __cplusplus
}
#endif

#define MAX_FILENAME_LENGTH 768 // 256 UCS-2 characters encoded into UTF-8 can use up to 768 UTF-8 chars

#endif
