/** toto je nepřesný časovač (PRO_WINDOWS) */

// timer může být přepsán tůdíž muáže trvat 2 tick pro vizvednutí. nepoužívat na přesné časování !

#include <windows.h>
#include <iostream>

// proměnná ohlašující který timer ticknul
uint64_t timer_that_ticks = 0;

// napsání který timer zrovna ticknul
void CALLBACK timer_interupt(HWND hwnd, UINT uMsg, UINT timerId, DWORD dwTime) {
  timer_that_ticks = (uint64_t)timerId; 
 /// std::cout << "timer tick " << timerId << std::endl;
}

// inicializuje timer a vrátí jeho id
uint64_t initTimer(int millis){
    return (uint64_t)SetTimer(NULL, 0, millis, &timer_interupt);
}

// funkce zajišťující obsluhu timerů
void ProcessTimerMessages(){
    MSG msg;                // message structure 

    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))  { 
        // Post WM_TIMER messages. 
        if (msg.message == WM_TIMER) { 
            msg.hwnd = NULL; 
        } 
        TranslateMessage(&msg); // translates virtual-key codes 
        DispatchMessage(&msg);  // dispatches message to window 
    } 
}
