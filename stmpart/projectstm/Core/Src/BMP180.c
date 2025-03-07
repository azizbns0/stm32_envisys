/**
  ******************************************************************************

  BMP180 LIBRARY for STM32 using I2C
  Author:   ControllersTech
  Updated:  26/07/2020

  ******************************************************************************
  Copyright (C) 2017 ControllersTech.com

  This is a free software under the GNU license, you can redistribute it and/or modify it under the terms
  of the GNU General Public License version 3 as published by the Free Software Foundation.
  This software library is shared with public for educational purposes, without WARRANTY and Author is not liable for any damages caused directly
  or indirectly by this software, read more about this on the GNU General Public License.

  ******************************************************************************
*/



#include "stm32f4xx_hal.h"
#include "math.h"

extern I2C_HandleTypeDef hi2c1;
#define BMP180_I2C &hi2c1

#define BMP180_ADDRESS 0xEE


// Defines according to the datsheet
short AC1 = 0;
short AC2 = 0;
short AC3 = 0;
unsigned short AC4 = 0;
unsigned short AC5 = 0;
unsigned short AC6 = 0;
short B1 = 0;
short B2 = 0;
short MB = 0;
short MC = 0;
short MD = 0;

/********************/
long UT = 0;
short oss = 0;
long UP = 0;
long X1 = 0;
long X2 = 0;
long X3 = 0;
long B3 = 0;
long B5 = 0;
unsigned long B4 = 0;
long B6 = 0;
unsigned long B7 = 0;

/*******************/
long Press = 0;
long Temp = 0;
/*************/
uint8_t dma_rx_buffer[22]; // Buffer for calibration data
uint8_t dma_tx_buffer[1];  // Buffer for write commands
volatile uint8_t dma_read_complete = 0; // Flag to indicate DMA read completion
volatile uint8_t dma_write_complete = 0; // Flag to indicate DMA write completion

#define atmPress 101325 //Pa



void read_calliberation_data(void) {
    uint16_t Callib_Start = 0xAA;
    dma_read_complete = 0; // Reset flag
    HAL_I2C_Mem_Read_DMA(BMP180_I2C, BMP180_ADDRESS, Callib_Start, I2C_MEMADD_SIZE_8BIT, dma_rx_buffer, 22);
    while (!dma_read_complete); // Wait for DMA completion
    AC1 = ((dma_rx_buffer[0] << 8) | dma_rx_buffer[1]);
    AC2 = ((dma_rx_buffer[2] << 8) | dma_rx_buffer[3]);
    AC3 = ((dma_rx_buffer[4] << 8) | dma_rx_buffer[5]);
    AC4 = ((dma_rx_buffer[6] << 8) | dma_rx_buffer[7]);
    AC5 = ((dma_rx_buffer[8] << 8) | dma_rx_buffer[9]);
    AC6 = ((dma_rx_buffer[10] << 8) | dma_rx_buffer[11]);
    B1 = ((dma_rx_buffer[12] << 8) | dma_rx_buffer[13]);
    B2 = ((dma_rx_buffer[14] << 8) | dma_rx_buffer[15]);
    MB = ((dma_rx_buffer[16] << 8) | dma_rx_buffer[17]);
    MC = ((dma_rx_buffer[18] << 8) | dma_rx_buffer[19]);
    MD = ((dma_rx_buffer[20] << 8) | dma_rx_buffer[21]);
}


// Get uncompensated Temp
uint16_t Get_UTemp(void) {
    uint8_t datatowrite = 0x2E;
    uint8_t Temp_RAW[2] = {0};
    dma_write_complete = 0; // Reset flag
    HAL_I2C_Mem_Write_DMA(BMP180_I2C, BMP180_ADDRESS, 0xF4, I2C_MEMADD_SIZE_8BIT, &datatowrite, 1);
    while (!dma_write_complete); // Wait for DMA completion
    HAL_Delay(5); // Wait 4.5 ms
    dma_read_complete = 0; // Reset flag
    HAL_I2C_Mem_Read_DMA(BMP180_I2C, BMP180_ADDRESS, 0xF6, I2C_MEMADD_SIZE_8BIT, Temp_RAW, 2);
    while (!dma_read_complete); // Wait for DMA completion
    return ((Temp_RAW[0] << 8) + Temp_RAW[1]);
}

float BMP180_GetTemp (void)
{
	UT = Get_UTemp();
	X1 = ((UT-AC6) * (AC5/(pow(2,15))));
	X2 = ((MC*(pow(2,11))) / (X1+MD));
	B5 = X1+X2;
	Temp = (B5+8)/(pow(2,4));
	return Temp/10.0;
}

// Get uncompensated Pressure
uint32_t Get_UPress(int oss) {
    uint8_t datatowrite = 0x34 + (oss << 6);
    uint8_t Press_RAW[3] = {0};
    dma_write_complete = 0; // Reset flag
    HAL_I2C_Mem_Write_DMA(BMP180_I2C, BMP180_ADDRESS, 0xF4, I2C_MEMADD_SIZE_8BIT, &datatowrite, 1);
    while (!dma_write_complete); // Wait for DMA completion
    switch (oss) {
        case (0): HAL_Delay(5); break;
        case (1): HAL_Delay(8); break;
        case (2): HAL_Delay(14); break;
        case (3): HAL_Delay(26); break;
    }
    dma_read_complete = 0; // Reset flag
    HAL_I2C_Mem_Read_DMA(BMP180_I2C, BMP180_ADDRESS, 0xF6, I2C_MEMADD_SIZE_8BIT, Press_RAW, 3);
    while (!dma_read_complete); // Wait for DMA completion
    return (((Press_RAW[0] << 16) + (Press_RAW[1] << 8) + Press_RAW[2]) >> (8 - oss));
}


float BMP180_GetPress (int oss)
{
	UP = Get_UPress(oss);
	X1 = ((UT-AC6) * (AC5/(pow(2,15))));
	X2 = ((MC*(pow(2,11))) / (X1+MD));
	B5 = X1+X2;
	B6 = B5-4000;
	X1 = (B2 * (B6*B6/(pow(2,12))))/(pow(2,11));
	X2 = AC2*B6/(pow(2,11));
	X3 = X1+X2;
	B3 = (((AC1*4+X3)<<oss)+2)/4;
	X1 = AC3*B6/pow(2,13);
	X2 = (B1 * (B6*B6/(pow(2,12))))/(pow(2,16));
	X3 = ((X1+X2)+2)/pow(2,2);
	B4 = AC4*(unsigned long)(X3+32768)/(pow(2,15));
	B7 = ((unsigned long)UP-B3)*(50000>>oss);
	if (B7<0x80000000) Press = (B7*2)/B4;
	else Press = (B7/B4)*2;
	X1 = (Press/(pow(2,8)))*(Press/(pow(2,8)));
	X1 = (X1*3038)/(pow(2,16));
	X2 = (-7357*Press)/(pow(2,16));
	Press = Press + (X1+X2+3791)/(pow(2,4));

	return Press;
}


float BMP180_GetAlt (int oss)
{
	BMP180_GetPress (oss);
	return 44330*(1-(pow((Press/(float)atmPress), 0.19029495718)));
}

void BMP180_Start (void)
{
	read_calliberation_data();
}
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance == I2C1) {
        dma_read_complete = 1; // Set flag for read completion
    }
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance == I2C1) {
        dma_write_complete = 1; // Set flag for write completion
    }
}

