#include <time.h>
#include "timer.h"

static time_t end_time;

/*  Crea un timer di sec secondi */
bool start_timer(int sec)
{
    if(sec < 0) {
        return false;
    }
    end_time = time(NULL);
    end_time += sec;
    return true;
}

/*  Ritorna il tempo rimanente in secondi. 
    ATTENZIONE: non controlla se end_time è settato. */
long remaining_time()
{
    time_t now = time(NULL);
    return (long)difftime(end_time, now);
}