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
    #define EXTI_VECTORS_COUNT      16

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
    #define OUT_PP      GPIO_Mode_Out_PP        // выход с подтяжкой
    #define OUT_OD      GPIO_Mode_Out_OD        // выход с открытым коллектором
    #define IN_PU       GPIO_Mode_IPU           // вход подтянутый к питанию
    #define IN_PD       GPIO_Mode_IPD           // вход подтянутый к земле
    #define IN_FLOAT    GPIO_Mode_IN_FLOATING   // вход Hi-Z
    #define IN_ADC      GPIO_Mode_AIN           // вхо аналоговый

    // определение альтернативных функций
    #define USART_TX    GPIO_Mode_AF_PP         // USART отправитель 
    #define USART_RX    IN_FLOAT                // USART приёмник
    #define CAN_TX      GPIO_Mode_AF_PP         // CAN отправитель
    #define CAN_RX      IN_PU                   // CAN приёмник
    #define SPI_SCK     GPIO_Mode_AF_PP         // SPI тактирование
    #define SPI_MOSI    GPIO_Mode_AF_PP         // SPI мастер -> слэйв
    #define SPI_MISO    GPIO_Mode_AF_PP         // SPI слэйв -> мастер

//==============================================================================
//                      Глобальные переменные
//==============================================================================

    // пин кнопки 1
    sGpio gpioKey1 = {C, 12, HIGH, IN_PU};

    // пин лампочки 1
    sGpio gpioLed1 = {B, 7, HIGH, OUT_PP};
    
#ifdef USE_EXTI 
    
    // внешние прерывания
    sExtiHandler  extiHandlers[EXTI_VECTORS_COUNT];
    
#endif //USE_EXTI
    
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

#ifdef USE_EXTI

//==============================================================================
//              Конфигурация внешнего прерывания
//------------------------------------------------------------------------------
//  priority    - приоритет прерывания (0..15)
//  fuction     - внешний обработчик
//  object      - объект использующий прерывание
//  gpio        - пин
//==============================================================================

void GPIOExtiConfig (uint8_t priority, void (*fuction)(void*), void (*object), sGpio* gpio)
{    
    NVIC_InitTypeDef   NVIC_InitStructure; 
        
    if(gpio->pin >= 10)
    {
        NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;                
    }
    else if(gpio->pin >= 5)
    {
        NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    }
    else
    {     
        NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn + gpio->pin;
    }
    
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = priority;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);   
    
    extiHandlers[gpio->pin].function    = fuction;
    extiHandlers[gpio->pin].object      = object;
}

//==============================================================================
//              Включение внешнего прерывания
//------------------------------------------------------------------------------
//  gpio      - пин
//==============================================================================

void GPIOExtiEnable (eGpioTriger triger, sGpio* gpio)
{    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_EXTILineConfig(gpio->portSource, gpio->pin);
    
    if(extiHandlers[gpio->pin].function != 0)
    {
        extiHandlers[gpio->pin].state   = 1;
        EXTI_InitTypeDef    EXTI_InitStructure;
        EXTI_InitStructure.EXTI_Line    = (1 << gpio->pin);
        EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;  
        EXTI_InitStructure.EXTI_Trigger = (EXTITrigger_TypeDef)triger;
        EXTI_InitStructure.EXTI_LineCmd = ENABLE;
        EXTI_Init(&EXTI_InitStructure);             
    }
}

//==============================================================================
//              Выключение внешнего прерывания
//------------------------------------------------------------------------------
//  gpio      - пин
//==============================================================================

void GPIOExtiDisable (sGpio* gpio)
{    
    extiHandlers[gpio->pin].state = 0;   
    EXTI_InitTypeDef    EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line    = 1 << gpio->pin;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt; 
    EXTI_InitStructure.EXTI_LineCmd = DISABLE;
    EXTI_Init(&EXTI_InitStructure); 
}

//==============================================================================
//                  Обработчик внешнего прерывания линии 0
//==============================================================================

void EXTI0_IRQHandler (void)
{    
    if(EXTI_GetITStatus(EXTI_Line0) == SET) 
    {
        extiHandlers[0].function(extiHandlers[0].object);
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}     

//==============================================================================
//                  Обработчик внешнего прерывания линии 1
//==============================================================================

void EXTI1_IRQHandler (void)
{
    if(EXTI_GetITStatus(EXTI_Line1) == SET) 
    {
        extiHandlers[1].function(extiHandlers[1].object);
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
}     

//==============================================================================
//                  Обработчик внешнего прерывания линии 2
//==============================================================================

void EXTI2_IRQHandler (void)
{
    if(EXTI_GetITStatus(EXTI_Line2) == SET) 
    {
        extiHandlers[2].function(extiHandlers[2].object);
        EXTI_ClearITPendingBit(EXTI_Line2);
    }
}     

//==============================================================================
//                  Обработчик внешнего прерывания линии 3
//==============================================================================

void EXTI3_IRQHandler (void)
{
    if(EXTI_GetITStatus(EXTI_Line3) == SET) 
    {
        extiHandlers[3].function(extiHandlers[3].object);
        EXTI_ClearITPendingBit(EXTI_Line3);
    }
}     

//==============================================================================
//                  Обработчик внешнего прерывания линии 4
//==============================================================================

void EXTI4_IRQHandler (void)
{
     if(EXTI_GetITStatus(EXTI_Line4) == SET) 
    {
        extiHandlers[4].function(extiHandlers[4].object);
        EXTI_ClearITPendingBit(EXTI_Line4);
    }
}

//==============================================================================
//                  Обработчик внешних прерываний линий 5-9
//==============================================================================

void EXTI9_5_IRQHandler (void)
{
    if(EXTI_GetITStatus(EXTI_Line5) == SET) 
    {
        extiHandlers[5].function(extiHandlers[5].object);
        EXTI_ClearITPendingBit(EXTI_Line5);
    }
    if(EXTI_GetITStatus(EXTI_Line6) == SET) 
    {
        extiHandlers[6].function(extiHandlers[6].object);
        EXTI_ClearITPendingBit(EXTI_Line6);
    }
    if(EXTI_GetITStatus(EXTI_Line7) == SET) 
    {
        extiHandlers[7].function(extiHandlers[7].object);
        EXTI_ClearITPendingBit(EXTI_Line7);
    }
    if(EXTI_GetITStatus(EXTI_Line8) == SET) 
    {
        extiHandlers[8].function(extiHandlers[8].object);
        EXTI_ClearITPendingBit(EXTI_Line8);
    }
    if(EXTI_GetITStatus(EXTI_Line9) == SET) 
    {
        extiHandlers[9].function(extiHandlers[9].object);
        EXTI_ClearITPendingBit(EXTI_Line9);
    }
}

//==============================================================================
//                  Обработчик внешних прерываний линий 10-15
//==============================================================================

void EXTI15_10_IRQHandler (void)
{
    if(EXTI_GetITStatus(EXTI_Line10) == SET) 
    {
        extiHandlers[10].function(extiHandlers[10].object);
        EXTI_ClearITPendingBit(EXTI_Line10);
    }
    if(EXTI_GetITStatus(EXTI_Line11) == SET) 
    {
        extiHandlers[11].function(extiHandlers[11].object);
        EXTI_ClearITPendingBit(EXTI_Line11);
    }
    if(EXTI_GetITStatus(EXTI_Line12) == SET)
    {
        extiHandlers[12].function(extiHandlers[12].object);
        EXTI_ClearITPendingBit(EXTI_Line12);
    }
    if(EXTI_GetITStatus(EXTI_Line13) == SET) 
    {
        extiHandlers[13].function(extiHandlers[13].object);
        EXTI_ClearITPendingBit(EXTI_Line13);
    }
    if(EXTI_GetITStatus(EXTI_Line14) == SET) 
    {
        extiHandlers[14].function(extiHandlers[14].object);
        EXTI_ClearITPendingBit(EXTI_Line14);
    }
    if(EXTI_GetITStatus(EXTI_Line15) == SET) 
    {
        extiHandlers[15].function(extiHandlers[15].object);
        EXTI_ClearITPendingBit(EXTI_Line15);
    }
}

#endif //USE_EXTI