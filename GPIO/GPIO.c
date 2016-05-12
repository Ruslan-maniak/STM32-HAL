//******************************************************************************
// Имя файла    :  'GPIO.c'
// заголовок    :  Драйвер портов ввода-вывода (F1)
// Автор        :  Маньянов Р.Р.
// Контакты     :  
// Дата         :  25.07.2015
//******************************************************************************

#include "GPIO.h"

//==============================================================================
//                      Константы и макросы
//==============================================================================
    
    // кол-во векторов внешних прерываний
    #define EXTI_VECTORS_COUNT  7

    // определение портов
    #define A           GPIOA,RCC_APB2Periph_GPIOA,GPIO_PortSourceGPIOA
    #define B           GPIOB,RCC_APB2Periph_GPIOB,GPIO_PortSourceGPIOB
    #define C           GPIOC,RCC_APB2Periph_GPIOC,GPIO_PortSourceGPIOC
    #define D           GPIOD,RCC_APB2Periph_GPIOD,GPIO_PortSourceGPIOD
    #define E           GPIOE,RCC_APB2Periph_GPIOE,GPIO_PortSourceGPIOE
    #define F           GPIOF,RCC_APB2Periph_GPIOF,GPIO_PortSourceGPIOF
    #define G           GPIOG,RCC_APB2Periph_GPIOG,GPIO_PortSourceGPIOG
    #define H           GPIOH,RCC_APB2Periph_GPIOH,GPIO_PortSourceGPIOH

    // определение скорости
    #define LOW         GPIO_Speed_2MHz
    #define MEDIUM      GPIO_Speed_10MHz
    #define HIGH        GPIO_Speed_50MHz

    // определение стандартных функций
    #define OUT_PP      GPIO_Mode_Out_PP
    #define OUT_OD      GPIO_Mode_Out_OD
    #define IN_PU       GPIO_Mode_IPU
    #define IN_PD       GPIO_Mode_IPD
    #define IN_FLOAT    GPIO_Mode_IN_FLOATING
    #define IN_ADC      GPIO_Mode_AIN

    // определение альтернативных функций
    #define USART_TX    GPIO_Mode_AF_PP
    #define USART_RX    IN_FLOAT
    #define CAN_TX      GPIO_Mode_AF_PP
    #define CAN_RX      IN_PU
    #define SPI_SCK     GPIO_Mode_AF_PP
    #define SPI_MOSI    GPIO_Mode_AF_PP
    #define SPI_MISO    GPIO_Mode_AF_PP   

//==============================================================================
//                      Глобальные переменные
//==============================================================================

    // пин кнопки
    sGpio gpioKey = {C, 12, HIGH, IN_PU};

    // пин лампочки
    sGpio gpioLed = {B, 7, HIGH, OUT_PP};
    
    // внешние прерывания
    sExtiHandler  extiHandlers[EXTI_VECTORS_COUNT];
    
//==============================================================================
//              Инициализация пина
//------------------------------------------------------------------------------
//  gpio    - пин
//==============================================================================

void GPIOInit (sGpio* gpio)
{
    if(gpio)
    {
        // тактирование используемого порта
        RCC_APB2PeriphClockCmd(gpio->rcc, ENABLE);  
        
        // отключение JTAG-функционала от пинов B3 и A15 если они используются
        if((gpio->port == GPIOB && (1 << gpio->pin) == GPIO_Pin_3) ||
           (gpio->port == GPIOA && (1 << gpio->pin) == GPIO_Pin_15))
        {
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
            GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
        }
        
        if((gpio->port == GPIOA && (1 << gpio->pin) == GPIO_Pin_13) ||
           (gpio->port == GPIOA && (1 << gpio->pin) == GPIO_Pin_14))
        {
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
            GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
        }
        
        // конфигурирование пина
        GPIO_InitTypeDef definitions =
        {
            .GPIO_Pin   = (1 << gpio->pin), 
            .GPIO_Speed = gpio->speed,
            .GPIO_Mode  = gpio->mode,   
        };        
        GPIO_Init(gpio->port, &definitions);
        
        // ремап пина если он назначен
        if(gpio->remap)
        {
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
            GPIO_PinRemapConfig(gpio->remap, ENABLE);
        } 
    }
}

//==============================================================================
//              Установка на пине единицы
//------------------------------------------------------------------------------
//  gpio    - пин
//==============================================================================

void GPIOSet (sGpio* gpio)
{
    if(gpio)
        GPIO_SetBits(gpio->port, (1 << gpio->pin));
}

//==============================================================================
//              Установка на пине нуля
//------------------------------------------------------------------------------
//  gpio    - пин
//==============================================================================

void GPIOReset (sGpio* gpio)
{
    if(gpio)
        GPIO_ResetBits(gpio->port, (1 << gpio->pin));
}

//==============================================================================
//              Переключение состояния пина
//------------------------------------------------------------------------------
//  gpio    - пин
//==============================================================================

void GPIOToggle (sGpio* gpio)
{
    if(gpio)
        GPIOWrite(!GPIOReadOut(gpio), gpio);
}

//==============================================================================
//              Установка на пине значения
//------------------------------------------------------------------------------
//  value   - значение (0,1)
//  gpio    - пин
//==============================================================================

void GPIOWrite (uint8_t value, sGpio* gpio)
{
    if(gpio)
        GPIO_WriteBit(gpio->port, (1 << gpio->pin), (BitAction)value);
}

//==============================================================================
//              Чтение значения входа пина
//------------------------------------------------------------------------------
//  gpio    - пин
//  return  - значение на входе пина
//==============================================================================

uint8_t GPIOReadIn (sGpio* gpio)
{
    if(gpio)
        return GPIO_ReadInputDataBit(gpio->port, (1 << gpio->pin));
    return 0;
}

//==============================================================================
//              Чтение значения выхода пина
//------------------------------------------------------------------------------
//  gpio    - пин
//  return  - значение на выходе пина
//==============================================================================

uint8_t GPIOReadOut (sGpio* gpio)
{
    if(gpio)
        return GPIO_ReadOutputDataBit(gpio->port, (1 << gpio->pin));
    return 0;
}

//==============================================================================
//              Конфигурация внешнего прерывания
//------------------------------------------------------------------------------
//  triger      - тип тригера
//  priority    - приоритет прерывания (0..15)
//  fuction     - внешний обработчик
//  object      - объект использующий прерывание
//  gpio        - пин
//==============================================================================

void GPIOExtiConfig (eGpioTriger triger, uint8_t priority, void (*fuction)(void*), void (*object), sGpio* gpio)
{    
    if(gpio)
    {
        NVIC_InitTypeDef   NVIC_InitStructure; 
               
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
        GPIO_EXTILineConfig(gpio->portSource, gpio->pin);
        
        uint8_t handlerNumber;
        if(gpio->pin >= 10)
        {
            handlerNumber = 6;
            NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;                
        }
        else if(gpio->pin >= 5)
        {
            handlerNumber = 5;
            NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
        }
        else
        {
            handlerNumber = gpio->pin;  
            NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn + gpio->pin;
        }
        
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = priority;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);   
        
        extiHandlers[handlerNumber].gpio = gpio;
        extiHandlers[handlerNumber].function = fuction;
        extiHandlers[handlerNumber].object = object;
        extiHandlers[handlerNumber].triger = triger;
    }	
}

//==============================================================================
//              Включение внешнего прерывания
//------------------------------------------------------------------------------
//  gpio      - пин
//==============================================================================

<<<<<<< HEAD
void GPIOExtiEnable (eGpioTriger triger, sGpio* gpio)
{    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_EXTILineConfig(gpio->portSource, gpio->pin);
    
    if(extiHandlers[gpio->pin].function != 0)
    {
<<<<<<< HEAD
        extiHandlers[gpio->pin].state   = 1;
=======
>>>>>>> origin/master
        EXTI_InitTypeDef    EXTI_InitStructure;
        EXTI_InitStructure.EXTI_Line    = (1 << gpio->pin);
        EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;  
        EXTI_InitStructure.EXTI_Trigger = (EXTITrigger_TypeDef)triger;
        EXTI_InitStructure.EXTI_LineCmd = ENABLE;
        EXTI_Init(&EXTI_InitStructure);             
    }
=======
void GPIOExtiEnable (sGpio* gpio)
{
    for(uint8_t extiNumber = 0; extiNumber < EXTI_VECTORS_COUNT; extiNumber++)
        if(gpio == extiHandlers[extiNumber].gpio)
        {
            EXTI_InitTypeDef    EXTI_InitStructure;
            EXTI_InitStructure.EXTI_Line    = (1 << extiHandlers[extiNumber].gpio->pin);
            EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;  
            EXTI_InitStructure.EXTI_Trigger = (EXTITrigger_TypeDef)extiHandlers[extiNumber].triger;
            EXTI_InitStructure.EXTI_LineCmd = ENABLE;
            EXTI_Init(&EXTI_InitStructure); 
            return;
        }
>>>>>>> parent of b5c1293... Добавлен флаг состояния прерывания (вкл\выкл)
}

//==============================================================================
//              Выключение внешнего прерывания
//------------------------------------------------------------------------------
//  gpio      - пин
//==============================================================================

void GPIOExtiDisable (sGpio* gpio)
<<<<<<< HEAD
{    
<<<<<<< HEAD
    extiHandlers[gpio->pin].state = 0;   
=======
>>>>>>> origin/master
    EXTI_InitTypeDef    EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line    = 1 << gpio->pin;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt; 
    EXTI_InitStructure.EXTI_LineCmd = DISABLE;
    EXTI_Init(&EXTI_InitStructure); 
=======
{
    for( uint8_t extiNumber = 0; extiNumber < EXTI_VECTORS_COUNT; extiNumber++)
        if(gpio == extiHandlers[extiNumber].gpio)
        {
            EXTI_InitTypeDef    EXTI_InitStructure;
            EXTI_InitStructure.EXTI_Line    = (1 << extiHandlers[extiNumber].gpio->pin);
            EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;  
            EXTI_InitStructure.EXTI_Trigger = (EXTITrigger_TypeDef)extiHandlers[extiNumber].triger;
            EXTI_InitStructure.EXTI_LineCmd = DISABLE;
            EXTI_Init(&EXTI_InitStructure); 
            return;
        }
>>>>>>> parent of b5c1293... Добавлен флаг состояния прерывания (вкл\выкл)
}

//==============================================================================
//              Общий обработчик внешнего прерывания
//------------------------------------------------------------------------------
//  extiNumber      - номер внешнего прерывания
//==============================================================================

void GPIOExtiHandler (uint8_t extiNumber)
{
    if(EXTI_GetITStatus(1 << extiHandlers[extiNumber].gpio->pin) != RESET)
        if(extiHandlers[extiNumber].function)
            extiHandlers[extiNumber].function(extiHandlers[extiNumber].object); 
    EXTI_ClearITPendingBit(1 << extiHandlers[extiNumber].gpio->pin);
}


void EXTI0_IRQHandler (void)
{    
    GPIOExtiHandler(0);
}     

void EXTI1_IRQHandler (void)
{
    GPIOExtiHandler(1);
}     

void EXTI2_IRQHandler (void)
{
    GPIOExtiHandler(2);
}     

void EXTI3_IRQHandler (void)
{
    GPIOExtiHandler(3);
}     

void EXTI4_IRQHandler (void)
{
    GPIOExtiHandler(4);
}

void EXTI9_5_IRQHandler (void)
{
    GPIOExtiHandler(5);
}

void EXTI15_10_IRQHandler (void)
{
    GPIOExtiHandler(6);
}

