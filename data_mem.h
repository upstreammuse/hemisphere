#ifndef DATA_MEM_H
#define DATA_MEM_H

#include <stdint.h>

uint8_t read_data_mem(uint16_t addr);
void write_data_mem(uint16_t addr, uint8_t data);

#endif
