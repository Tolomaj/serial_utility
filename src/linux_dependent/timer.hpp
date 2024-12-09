/** toto je nepřesný časovač (PRO_LINUX) */

// timer může být přepsán tůdíž muáže trvat 2 tick pro vizvednutí. nepoužívat na přesné časování !
#ifndef TIMER_LNX_LIB
#define TIMER_LNX_LIB

#include <unistd.h>
#include <iostream>
#include <ctime>
#include <time.h>
#include <stdio.h>

class Timer {
    clock_t start;
    int delay;
public:
    Timer(int millis){
        start = clock();
        delay = millis;
    }

    bool ticked(){
        clock_t now = clock();
        clock_t cmp = start;
        if(now > cmp + delay){
            start = clock();
            return true;
        }
        return false;
    }
};

// funkce zajišťující obsluhu timerů
void ProcessTimerMessages(){ } // pro linux se nepoužívá


void uni_sleep(int i){
    usleep(i);
}

#endif