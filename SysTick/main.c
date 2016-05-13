
#include "SysTick\SysTick.h"
#include "Relay\Relay.h"

//==============================================================================
//                          Глобальные переменные
//==============================================================================

    // внешне определенные переменные
    extern uint32_t globalTimer;
    extern sRelay   led1, 
                    led2,
                    led3,
                    led4;
    
    // таймер обработки светодиодов
    uint32_t ledsPollTimer;
    
//==============================================================================
//                         Основная программа
//==============================================================================

int main (void)
{   
    // инициализация глобального таймера
    SysTickInit(1000, 0);
    
    // инициализация светодиодов
    RelayInit(&led1);
    RelayInit(&led2);   
    RelayInit(&led3);
    RelayInit(&led4);
    
    // установка режима работы для светодиодов
    RelaySetTimeOn(5000, &led1);
    RelaySetPeriodOn(200, 500, &led2);
    RelaySetPeriodOn(400, 2000, &led3);
    RelaySetPeriodOn(50, 100, &led4);
    
    while(1) 
    {
        // периодическая обработка светодиодов
        if(globalTimer - ledsPollTimer >= 5)
        {
            RelayPoll(&led1);
            RelayPoll(&led2);
            RelayPoll(&led3);
            RelayPoll(&led4);
        }        
        
        // по истечении 30 секунд выключение светодиода 4
        if(globalTimer >= 30000)
            RelayOff(&led4);
    }
}
