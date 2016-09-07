#ifndef _FLASH_H
#define _FLASH_H

#include "../main.h"

 #define CONFIG_BLOCK_SIZE 512
unsigned char ReadROM1(unsigned int Addr);
void WriteROM(unsigned long Addr, unsigned char *Buf, unsigned char Kol);
void EraseROM(unsigned long Addr);

void ReadProgramWord(unsigned long,  unsigned long*); // 3 байта + 1 фантомный

void ErasePage(unsigned long) ;
void WriteProgramWord(unsigned long , unsigned long* );

void ReadConfigMemory(unsigned long, unsigned char *);
void WriteConfigMemory(unsigned long);

void WriteLatch         (unsigned long , unsigned long *);
void WriteProgramBlock  (unsigned long , unsigned char * );
void WriteProgramPage   (unsigned long , unsigned char *);

#endif