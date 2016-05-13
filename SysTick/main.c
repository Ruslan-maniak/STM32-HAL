
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
    
    uint32_t ledsPollTimer;
    
//==============================================================================
//                         Основная программа
//==============================================================================

int main (void)
{   
    SysTickInit(1000, 0);
    
    // инициализация пинов
    RelayInit(&led1);
    RelayInit(&led2);   
    RelayInit(&led3);
    RelayInit(&led4);
    
    RelaySetTimeOn(5000, &led1);
    RelaySetPeriodOn(200, 500, &led2);
    RelaySetPeriodOn(400, 2000, &led3);
    RelaySetPeriodOn(50, 100, &led4);
    
    while(1) 
    {
        if(globalTimer - ledsPollTimer >= 5)
        {
            RelayPoll(&led1);
            RelayPoll(&led2);
            RelayPoll(&led3);
            RelayPoll(&led4);
        }        
    }
}
