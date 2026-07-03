#ifndef VIRA_UTILS_H
#define VIRA_UTILS_H
#include <Arduino.h>
static inline uint8_t flipByte(uint8_t b){
uint8_t r = 0;
for(int i=0;i<8;i++){
if(b & (1<<i)) r |= (1<<(7-i));
}return r;}
#endif