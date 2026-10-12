#include "Engine/Core/Time.h"

Uint64 Time::s_LastTicks  = 0;
Uint64 Time::s_StartTicks = 0;
float  Time::s_DeltaTime  = 0.0f;
float  Time::s_TotalTime  = 0.0f;

void Time::Init(){
    s_StartTicks = SDL_GetTicks();
    s_LastTicks = s_StartTicks;
    s_DeltaTime = 0.0f;
    s_TotalTime = 0.0f;
}

void Time::Update(){
    Uint64 currentTicks = SDL_GetTicks();
    s_DeltaTime = (currentTicks - s_LastTicks) / 1000.0f;
    s_TotalTime = (currentTicks - s_StartTicks) / 1000.0f;

    s_LastTicks = currentTicks;

    //Prevent Lag spikes
    if(s_DeltaTime > 0.1f) s_DeltaTime = 0.1f;
}

float Time::GetDeltaTime(){
    return s_DeltaTime;
}

float Time::GetTime(){
    return s_TotalTime;
}

float Time::GetFPS(){
    if (s_DeltaTime <= 0.0f) return 0.0f;
    return 1.0f / s_DeltaTime;
}

