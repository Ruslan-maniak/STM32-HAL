<<<<<<< HEAD
<<<<<<< HEAD
﻿
=======

>>>>>>> origin/master
=======
//******************************************************************************
// Имя файла    :  'main.c'
// заголовок    :  Основная программа
// Автор        :  Маньянов Р.Р.
// Контакты     :  
// Дата         :  22.04.2016
//******************************************************************************

>>>>>>> parent of b5c1293... Добавлен флаг состояния прерывания (вкл\выкл)
#include "GPIO\GPIO.h"

//==============================================================================
//                          Глобальные переменные
//==============================================================================

    // внешне определенные переменные
    extern sGpio        gpioKey,    
                        gpioLed;

//==============================================================================
//              Инициализация пина
//------------------------------------------------------------------------------
//  gpioLed    - пин управления светодиодом
//==============================================================================    
    
<<<<<<< HEAD
<<<<<<< HEAD
void LedToggle (void* gpioLed)
{   
    GPIOToggle((sGpio*)gpioLed);
=======
void LedToggle (void* gpio)
{   
    GPIOToggle(gpio);
>>>>>>> origin/master
=======
void LedCtrl (void* gpioLed)
{   
    GPIOWrite(!GPIOReadIn(&gpioKey), led);
>>>>>>> parent of b5c1293... Добавлен флаг состояния прерывания (вкл\выкл)
}
    
//==============================================================================
//                            Основная программа
//==============================================================================
<<<<<<< HEAD

<<<<<<< HEAD
int main (void)
=======
int main(void)
>>>>>>> origin/master
=======
    
int main(void)
>>>>>>> parent of b5c1293... Добавлен флаг состояния прерывания (вкл\выкл)
{   
    // получение частот тактирования
    RCC_GetClocksFreq(&RCC_Clocks);
    
    // инициализация глобального таймера
    InitSysTick(1000);

    // инициализация пинов
    GPIOInit(&gpioKey);
    GPIOInit(&gpioLed);

    // конфигурация внешнего прерывания
    GPIOExtiConfig(eGpioTriger_ALL, 2, LedCtrl, (void*)&gpioLed, &gpioKey);
    
    // включение внешнего прерывания
    GPIOExtiEnable (&gpioKey);

    while(1) 
    {           
       
    }
}