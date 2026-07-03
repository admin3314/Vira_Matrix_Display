#ifndef VIRA_MATRIX_H
#define VIRA_MATRIX_H
#include <Arduino.h>
#pragma once
#include "Vira_Emoji.h"
#include "Vira_Font5x7.h"
#include "Vira_Utils.h"
class ViraMatrix {
public:
ViraMatrix(uint8_t dataPin,
uint8_t clkPin,
uint8_t latchPin,
uint8_t width = 8,
uint8_t height = 8);
void begin();
void refresh();
void clear();
void setFrame(uint8_t *buf);
void setPixel(uint8_t x, uint8_t y, bool on); 
private:
uint8_t _dataPin;
uint8_t _clkPin;
uint8_t _latchPin;
uint8_t _width;
uint8_t _height;
uint8_t frame[8];
void show(uint8_t row, uint8_t col);
void shiftOutByte(uint8_t data);};
#endif