/** toto je nepřesný časovač (PRO_WINDOWS) */

// timer může být přepsán tůdíž muáže trvat 2 tick pro vizvednutí. nepoužívat na přesné časování !

#include <windows.h>
#include <iostream>

#ifndef TIMER_WIN_LIB
#define TIMER_WIN_LIB

// proměnná ohlašující který timer ticknul
uint64_t timer_that_ticks = 0;

// napsání který timer zrovna ticknul
void CALLBACK timer_interupt(HWND hwnd, UINT uMsg, UINT timerId, DWORD dwTime) {
  timer_that_ticks = (uint64_t)timerId; 
 /// std::cout << "timer tick " << timerId << std::endl;
}


class Timer {
    int timer_id;
public:
    Timer(int millis){
        timer_id = (uint64_t)SetTimer(NULL, 0, millis, &timer_interupt);
    }

    bool ticked(){
        bool ticked = timer_that_ticks == timer_id;
        timer_that_ticks = 0;
        return ticked;
    }
};


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


void uni_sleep(int i){
    Sleep(i);// nechceme vytěžovat procesor a čekat můžeme
}

#endif