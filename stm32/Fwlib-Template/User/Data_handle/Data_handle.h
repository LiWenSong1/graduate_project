#ifndef __DATA_HANDLE_H
#define __DATA_HANDLE_H

#include "stm32f10x.h"

uint8_t get_data(const char* data_name,char* recv,uint8_t recv_len);
#endif