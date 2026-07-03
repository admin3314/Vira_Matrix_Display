#include "Vira_Matrix.h"
ViraMatrix::ViraMatrix(uint8_t dataPin,
uint8_t clkPin,
uint8_t latchPin,
uint8_t width,
uint8_t height){
_dataPin = dataPin;
_clkPin = clkPin;
_latchPin = latchPin;
_width = width;
_height = height;}
void ViraMatrix::begin(){
pinMode(_dataPin, OUTPUT);
pinMode(_clkPin, OUTPUT);
pinMode(_latchPin, OUTPUT);
clear();}
void ViraMatrix::clear(){memset(frame, 0, sizeof(frame));}
void ViraMatrix::setFrame(uint8_t *buf){memcpy(frame,buf,8);}
void ViraMatrix::setPixel(uint8_t x, uint8_t y, bool on){
if(x >= _width || y >= _height) return;
uint8_t bit = 7 - x;
if(on)
frame[y] |= (1 << bit);
else
frame[y] &= ~(1 << bit);}
void ViraMatrix::shiftOutByte(uint8_t data){
for(int i=7;i>=0;i--){
digitalWrite(_dataPin, (data >> i) & 1);
digitalWrite(_clkPin, HIGH);
digitalWrite(_clkPin, LOW);}}
void ViraMatrix::show(uint8_t row, uint8_t col){
digitalWrite(_latchPin, LOW);
shiftOutByte(col);
shiftOutByte(row);
digitalWrite(_latchPin, HIGH);}
void ViraMatrix::refresh(){
for(int r=0;r<8;r++){
uint8_t row=(1<<r);
uint8_t col = (uint8_t)(~frame[r]);
show(row,col);
delayMicroseconds(500);}}