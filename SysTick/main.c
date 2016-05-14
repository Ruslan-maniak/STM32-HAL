
#include "SysTick\SysTick.h"
#include "Relay\Relay.h"

//==============================================================================
//                          Глобальные переменные
//==============================================================================

    // внешне определенные переменные
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
    if(!SysTickInit(1000, 0))
        while(1);
    
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
        if(SysTickGet() - ledsPollTimer >= 5)
        {
            ledsPollTimer = SysTickGet();
            RelayPoll(&led1);
            RelayPoll(&led2);
            RelayPoll(&led3);
            RelayPoll(&led4);
        }        
        
        // по истечении 30 секунд выключение светодиода 4
        if(SysTickGet() >= 30000)
            RelayOff(&led4);
    }
}
