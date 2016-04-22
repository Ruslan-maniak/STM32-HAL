//******************************************************************************
// ��� �����    :  'GPIO.c'
// ���������    :  ������� ������ �����-������ (F1)
// �����        :  �������� �.�.
// ��������     :  
// ����         :  25.07.2015
//******************************************************************************

#include "GPIO.h"

//==============================================================================
//                      ��������� � �������
//==============================================================================
    
    // ����������� ������
    #define A           GPIOA,RCC_APB2Periph_GPIOA
    #define B           GPIOB,RCC_APB2Periph_GPIOB
    #define C           GPIOC,RCC_APB2Periph_GPIOC
    #define D           GPIOD,RCC_APB2Periph_GPIOD
    #define E           GPIOE,RCC_APB2Periph_GPIOE
    #define F           GPIOF,RCC_APB2Periph_GPIOF
    #define G           GPIOG,RCC_APB2Periph_GPIOG
    #define H           GPIOH,RCC_APB2Periph_GPIOH

    // ����������� ��������
    #define LOW         GPIO_Speed_2MHz
    #define MEDIUM      GPIO_Speed_10MHz
    #define HIGH        GPIO_Speed_50MHz

    // ����������� ����������� �������
    #define OUT_PP      GPIO_Mode_Out_PP
    #define OUT_OD      GPIO_Mode_Out_OD
    #define IN_PU       GPIO_Mode_IPU
    #define IN_PD       GPIO_Mode_IPD
    #define IN_FLOAT    GPIO_Mode_IN_FLOATING
    #define IN_ADC      GPIO_Mode_AIN

    // ����������� �������������� �������
    #define USART_TX    GPIO_Mode_AF_PP
    #define USART_RX    IN_FLOAT
    #define CAN_TX      GPIO_Mode_AF_PP
    #define CAN_RX      IN_PU
    #define SPI_SCK     GPIO_Mode_AF_PP
    #define SPI_MOSI    GPIO_Mode_AF_PP
    #define SPI_MISO    GPIO_Mode_AF_PP   

//==============================================================================
//                      ���������� ����������
//==============================================================================

    // ������ ���
    sGpio gpioTemp1 = {A, 11, HIGH, OUT_PP};

    // ������ ���
    sGpio gpioTemp2 = {B, 5, HIGH, IN_FLOAT};    
        
//==============================================================================
//              ������������� ����
//------------------------------------------------------------------------------
//  gpio    - ���
//==============================================================================

void GPIOInit (sGpio* gpio)
{
    if(gpio)
    {
        // ������������ ������������� �����
        RCC_APB2PeriphClockCmd(gpio->rcc, ENABLE);  
        
        // ���������� JTAG-����������� �� ����� B3 � A15 ���� ��� ������������
        if((gpio->port == GPIOB && (1 << gpio->pin) == GPIO_Pin_3) ||
           (gpio->port == GPIOA && (1 << gpio->pin) == GPIO_Pin_15))
        {
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
            GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
        }
        
        // ���������������� ����
        GPIO_InitTypeDef definitions =
        {
            .GPIO_Pin   = (1 << gpio->pin), 
            .GPIO_Speed = gpio->speed,
            .GPIO_Mode  = gpio->mode,   
        };        
        GPIO_Init(gpio->port, &definitions);
        
        // ����� ���� ���� �� ��������
        if(gpio->remap)
        {
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
            GPIO_PinRemapConfig(gpio->remap, ENABLE);
        } 
    }
}

//==============================================================================
//              ��������� �� ���� �������
//------------------------------------------------------------------------------
//  gpio    - ���
//==============================================================================

void GPIOSet (sGpio* gpio)
{
    if(gpio)
        GPIO_SetBits(gpio->port, (1 << gpio->pin));
}

//==============================================================================
//              ��������� �� ���� ����
//------------------------------------------------------------------------------
//  gpio    - ���
//==============================================================================

void GPIOReset (sGpio* gpio)
{
    if(gpio)
        GPIO_ResetBits(gpio->port, (1 << gpio->pin));
}

//==============================================================================
//              ������������ ��������� ����
//------------------------------------------------------------------------------
//  gpio    - ���
//==============================================================================

void GPIOToggle (sGpio* gpio)
{
    if(gpio)
        GPIOWrite(!GPIOReadOut(gpio), gpio);
}

//==============================================================================
//              ��������� �� ���� ��������
//------------------------------------------------------------------------------
//  value   - �������� (0..1)
//  gpio    - ���
//==============================================================================

void GPIOWrite (uint8_t value, sGpio* gpio)
{
    if(gpio)
        GPIO_WriteBit(gpio->port, (1 << gpio->pin), (BitAction)value);
}

//==============================================================================
//              ������ �������� ����� ����
//------------------------------------------------------------------------------
//  gpio    - ���
//  return  - �������� �� ����� ����
//==============================================================================

uint8_t GPIOReadIn (sGpio* gpio)
{
    if(gpio)
        return GPIO_ReadInputDataBit(gpio->port, (1 << gpio->pin));
    return 0;
}

//==============================================================================
//              ������ �������� ������ ����
//------------------------------------------------------------------------------
//  gpio    - ���
//  return  - �������� �� ������ ����
//==============================================================================

uint8_t GPIOReadOut (sGpio* gpio)
{
    if(gpio)
        return GPIO_ReadOutputDataBit(gpio->port, (1 << gpio->pin));
    return 0;
}

