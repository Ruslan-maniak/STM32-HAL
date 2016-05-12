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
//                      Константы и макросы
//==============================================================================

    // использование внешних прерываний (закоментировать если не используются)
    #define USE_EXTI

//==============================================================================
//                  Структуры и типы
//==============================================================================

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

#ifdef USE_EXTI

    // тип тригер
    typedef enum
    {
        eGpioTriger_RISE  = EXTI_Trigger_Rising,            // от земли к питанию
        eGpioTriger_FALL  = EXTI_Trigger_Falling,           // от питания к земле
        eGpioTriger_ALL   = EXTI_Trigger_Rising_Falling,    // оба варианта
    } eGpioTriger;
       
    // структура внешнего прерывания
    typedef struct
    {    
        uint8_t         state;              // состояние внешнего прерывания (0 - выкл, 1 - вкл)
        void            (*function)(void*); // указатель на внешний обработчик
        void            (*object);          // объект использующий прерывание
    } sExtiHandler;

#endif //USE_EXTI
   
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

#ifdef USE_EXTI    
    
    // конфигурация внешнего прерывания
    void GPIOExtiConfig (uint8_t priority, void (*fuction)(void*), void (*object), sGpio* gpio);
    
    // включение внешнего прерывания
    void GPIOExtiEnable (eGpioTriger triger, sGpio* gpio);
    
    // выключение внешнего прерывания
    void GPIOExtiDisable (sGpio* gpio);
    
#endif //USE_EXTI
    
#endif //__GPIO_H
