//******************************************************************************
// Имя файла    :  'GPIO.h'
// заголовок    :  Драйвер портов ввода-вывода (F1)
// Автор        :  Маньянов Р.Р.
// Контакты     :  
// Дата         :  25.07.2015
//******************************************************************************

#ifndef __GPIO_H
#define __GPIO_H

#include "stm32F10x.h" 

//==============================================================================
//                  Структуры и типы
//==============================================================================

    // тригер
    typedef enum
    {
        eGpioTriger_RISE  = EXTI_Trigger_Rising,
        eGpioTriger_FALL  = EXTI_Trigger_Falling,        
        eGpioTriger_ALL   = EXTI_Trigger_Rising_Falling,
    } eGpioTriger;
       
    // Структура пина
    typedef struct 
    {
        GPIO_TypeDef*       port;       // порт (A..H)
        uint32_t            rcc;        // источник тактирования
        uint8_t             portSource; // портсоурс 
        uint8_t             pin;        // номер пина (0..15)
        GPIOSpeed_TypeDef   speed;      // скорость (LOW, MEDIUM, HIGH)
        GPIOMode_TypeDef    mode;       // режим работы пина
        uint32_t            remap;      // ремап
    } sGpio;
    
    typedef struct
    {
        sGpio*              gpio;
        eGpioTriger         triger;
        void                (*function)(void*);
        void                (*object);  
    } sExtiHandler;

//==============================================================================
//                      Прототипы функций
//==============================================================================

    // инициализация пина
    void GPIOInit (sGpio* gpio);

    // установка на пине единицы
    void GPIOSet (sGpio* gpio);

    // установка на пине нуля
    void GPIOReset (sGpio* gpio);

    // переключение состояния пина
    void GPIOToggle (sGpio* gpio);    
    
    // установка на пине значения 
    void GPIOWrite (uint8_t value, sGpio* gpio);    
    
    // чтение значения входа пина
    uint8_t GPIOReadIn (sGpio* gpio);    
    
    // чтение значения выхода пина
    uint8_t GPIOReadOut (sGpio* gpio);
    
    void GPIOExtiConfig (eGpioTriger triger, uint8_t priority, void (*fuction)(void*), void (*object), sGpio* gpio);
    
    void GPIOExtiDisable (sGpio* gpio);
    
    void GPIOExtiEnable (sGpio* gpio);
    
#endif //__GPIO_H








