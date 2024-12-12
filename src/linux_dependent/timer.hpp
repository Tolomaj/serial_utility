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
        start = clock() / (CLOCKS_PER_SEC/1000);
        delay = millis;
    }

    bool ticked(){
        clock_t now = clock() / (CLOCKS_PER_SEC/1000);
        if(now > start + delay){
            start = clock()/ (CLOCKS_PER_SEC/1000);
            return true;
        }
        return false;
    }

    //resetuje timer
    void reset(){
        start = clock() / (CLOCKS_PER_SEC/1000);
    }
};

// funkce zajišťující obsluhu timerů
void ProcessTimerMessages(){ } // pro linux se nepoužívá


void uni_sleep(int i){
    usleep(i);
}

#endif