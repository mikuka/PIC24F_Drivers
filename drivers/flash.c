#include "flash.h"

#define ERASE_PAGE          0x4042
#define PROGRAM_ROW         0x4001
#define PROGRAM_WORD        0x4003


#define BLOCK_SIZE_IN_INSTRUCTION  64


unsigned short offset;
unsigned short high_byte;
unsigned short low_byte;

unsigned short high_word;
unsigned short low_word;

void ReadProgramWord(unsigned long addr, unsigned long *program_word) { // 3 байта + 1 фантомный
    TBLPAG = *((unsigned short*) (&addr) + 1);
    offset = (unsigned short) (addr);
    NVMCON = PROGRAM_WORD;
    asm("tblrdh.w [%1], %0" : "=r"(high_word) : "r"(offset));
    asm("tblrdl.w [%1], %0" : "=r"(low_word) : "r"(offset));
    *(((unsigned short*) (program_word)) + 1) = high_word;
    *((unsigned short*) (program_word)) = low_word;
    return;
}

unsigned char ReadProgramByte(unsigned int addr) {
    unsigned int addrOffset;
    unsigned char VarByte1;
    unsigned char VarByte2;
    unsigned char VarByte3;
    unsigned char VB;
    TBLPAG = *((unsigned short*) (&addr) + 1);
    addrOffset = (unsigned short) (addr);
    NVMCON = PROGRAM_WORD;
    asm("tblrdl.b [%1], %0" : "=r"(VarByte1) : "r"(addrOffset)); //Read low byte
    asm("tblrdl.b [%1], %0" : "=r"(VarByte2) : "r"(addrOffset + 1)); //Read middle byte
    asm("tblrdh.b [%1], %0" : "=r"(VarByte3) : "r"(addrOffset)); //Read high byte
    VB = VarByte1 | VarByte2 | VarByte3;
    return VB;
}

void WriteProgramWord(unsigned long addr, unsigned long *program_word) {
    unsigned long address = addr;

    NVMCON = PROGRAM_WORD;
    offset = *(unsigned int*) (&address );
    TBLPAG = *(((unsigned int*) (&address )) + 1);
    low_byte = *((unsigned int*) (program_word));
    high_byte = *(((unsigned int*) (program_word)) + 1);

    asm("push  W0");
    asm("push  W1");
    asm("MOV     _offset, W1");
    asm("MOV     _low_byte, W0");
    asm("TBLWTL W0,[W1]");
    asm("MOV     _high_byte, W0");
    asm("TBLWTH W0,[W1++]");
    asm("MOV     W1, _offset");
    asm("pop   W1");
    asm("pop   W0");

    CORCONbits.IPL3 = 1;
    NVMKEY = 0x55;
    NVMKEY = 0xAA;
    NVMCONbits.WR = 1;

    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");

    while (NVMCONbits.WR) {
    };
    CORCONbits.IPL3 = 0;
}

void WriteLatch(unsigned long addr, unsigned long *program_word) {
    TBLPAG = *(((unsigned int*) (&addr)) + 1);
    offset = *(unsigned int*) (&addr);
    low_byte = *((unsigned int*) (program_word));
    high_byte = *(((unsigned int*) (program_word)) + 1);

    __builtin_tblwtl(offset, low_byte);
    __builtin_tblwth(offset, high_byte);
}

void WriteProgramBlock(unsigned long addr, unsigned char *buffer) { // запись блока из 64 инструкций
    unsigned int i, j;
    unsigned long program_word;
    j = 0;

    for (i = 0; i < BLOCK_SIZE_IN_INSTRUCTION; i++) {
        program_word = 0;
        *(((unsigned char *) (&program_word)) + 0) = buffer[j];
        j++;
        *(((unsigned char *) (&program_word)) + 1) = buffer[j];
        j++;
        WriteLatch(addr, &program_word);
        addr += 2;
    }
    NVMCON = PROGRAM_ROW;
    _IPL = 7;
    __builtin_write_NVM();
    while (NVMCONbits.WR == 1) {
    };
    _IPL = 0;
}

void WriteProgramPage(unsigned long addr, unsigned char *buffer) { // запись 512 инструкций
    unsigned short i, j;
    unsigned char *buffer_temp;
    for (i = 0; i < 8; i++) {
        WriteProgramBlock(addr + i * 0x80, &buffer[i * 128]);
    }
}

void WriteProgramRow(unsigned long addr, unsigned long *program_word) {
    unsigned long address = addr;
    NVMCON = PROGRAM_ROW;
    offset = *(unsigned short*) (&address );
    TBLPAG = *(((unsigned short*) (&address )) + 1);
    low_byte = *((unsigned short*) (program_word));
    high_byte = *(((unsigned short*) (program_word)) + 1);

    asm("push  W0");
    asm("push  W1");
    asm("MOV     _offset, W1");
    asm("MOV     _low_byte, W0");
    asm("TBLWTL W0,[W1]");
    asm("MOV     _high_byte, W0");
    asm("TBLWTH W0,[W1++]");
    asm("MOV     W1, _offset");
    asm("pop   W1");
    asm("pop   W0");

    CORCONbits.IPL3 = 1;
    NVMKEY = 0x55;
    NVMKEY = 0xAA;
    NVMCONbits.WR = 1;
    asm("nop");
    asm("nop");
    while (NVMCONbits.WR) {
    };
    CORCONbits.IPL3 = 0;
}

void ErasePage(unsigned long addr) {
    unsigned short temp = TBLPAG;
    
    unsigned long address = addr;

    NVMCON = ERASE_PAGE;
    TBLPAG = *((unsigned short*) (&address) + 1);
    offset = *((unsigned short*) (&address));
    __builtin_tblwtl(offset,offset );
    __builtin_write_NVM();
    while (NVMCONbits.WR){
    };
    TBLPAG = temp;   
}

void ReadConfigMemory(unsigned long block_addr, unsigned char *buffer) {
    unsigned short i, j;
    unsigned long temp_program_word;
    j = 0;
    for (i = 0; i < 0x400; i += 2) { //
        ReadProgramWord(block_addr, &temp_program_word);
        *buffer = *((unsigned char*) (&temp_program_word));
        buffer++;
        *buffer = *((unsigned char*) (&temp_program_word) + 1);
        buffer++;
        block_addr += 2;
    }
}

void WriteConfigMemory(unsigned long block_addr) {
    unsigned short i, j;
    unsigned long temp_program_word;
    j = 0;
    for (i = 0; i < CONFIG_BLOCK_SIZE; i += 2) { //
        *(((unsigned char *) (&temp_program_word)) + 0) = config_buffer[j];
        j++;
        *(((unsigned char *) (&temp_program_word)) + 1) = config_buffer[j];
        j++;
        *(((unsigned char *) (&temp_program_word)) + 2) = 0;
        WriteProgramWord(block_addr, (unsigned long *) &temp_program_word);
        block_addr += 2;
    }
}