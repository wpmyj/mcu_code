/*********************************************************
*�ļ����ƣ�AT24C04.h
*ժ    Ҫ��AT24C04�����ļ�
*
*��ǰ�汾��1.0
*��    �ߣ�����
*��ʼ���ڣ�2012��5��17��
*������ڣ�
*�޸� ��л��
**********************************************************/

#ifndef __AT24C04_H
#define __AT24C04_H
 
#include "stm32f1xx_hal.h"
#include "tim.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
  
// I/O�ڶ���
#define SCL_H()       HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
#define SCL_L()       HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);

#define SDA_H()       HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
#define SDA_L()       HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);

#define SDA_PORT      GPIOD
#define SDA_PIN       GPIO_PIN_12

#define SDA_IN()       set_sda_dir(GPIO_MODE_INPUT)       //SDA�ĳ�����ģʽ
#define SDA_OUT()      set_sda_dir(GPIO_MODE_OUTPUT_PP)     //SDA������ģʽ

#define SDA_VAL()       HAL_GPIO_ReadPin(SDA_PORT,SDA_PIN)

#define deviceaddress          0xA0                  //AT24C02���豸��ַ

#define AT24C04delayus(x)     TIM_delay_us(x)
#define AT24C04delayms(x)     HAL_Delay(x)

#define WriteUInt8(x,y)        Write_1Byte(x,y)                        //д��1byte����
#define ReadUInt8(x)           Read_1Byte_Randomaddress(x)             //��1byte����


void set_sda_dir(uint32_t mode);
unsigned char AT24C04TEST(unsigned char testadd);
unsigned char WriteUInt16(unsigned int num,unsigned char dataaddr);
unsigned int ReadUInt16(unsigned char addr);
unsigned char WriteSInt32(long num,unsigned char dataaddr);
signed long ReadSInt32(unsigned char addr);
unsigned char Write_1Byte(unsigned char wdata,unsigned char dataaddress);                        
unsigned char Write_NByte(unsigned char * outbuf,unsigned char n,unsigned char dataaddress);
unsigned char Read_1Byte_currentaddress(void);
unsigned char Read_NByte_currentaddress(unsigned char * readbuf,unsigned char n);
unsigned char Read_1Byte_Randomaddress(unsigned char dataaddress);
unsigned char Read_NByte_Randomaddress(unsigned char * readbuf,unsigned char n,unsigned char dataaddress);

#endif /* #ifndef __AT24C04_H */