#ifndef _TIMER_H
#define _TIMER_H

#include <windows.h>
#include <chrono>

class Timer
{
private:
    std::chrono::time_point<std::chrono::steady_clock> m_StartTime;

public:
    void Start();
    float GetDuration();
};


#endif // _TIMER_H