#pragma once

#include <SDL3/SDL.h>

class Input {
    public:
        static void Update();
        static bool IsKeyPressed(SDL_Scancode key);
        static bool IsKeyJustPressed(SDL_Scancode key);
        static void EndFrame();

    private:
        static const bool* s_CurrentKeyState;
        static bool s_PreviousKeyState[SDL_SCANCODE_COUNT]; 

};