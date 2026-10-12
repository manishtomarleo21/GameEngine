#pragma once

#include <SDL3/SDL.h>

class Time {
    public:
        static void Init(); 
        static void Update(); //Call at beginning of every Frame

        static float GetDeltaTime(); // Time since last frame (in seconds)
        static float GetTime(); // Total time since start
        static float GetFPS();
    
    private:
        static Uint64 s_LastTicks;
        static Uint64 s_StartTicks;
        static float s_DeltaTime;
        static float s_TotalTime;

};