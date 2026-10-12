#include "Engine/Input/Input.h"
#include <cstring>

const bool* Input::s_CurrentKeyState = nullptr;
bool Input::s_PreviousKeyState[SDL_SCANCODE_COUNT] = { false };

void Input::Update(){
    s_CurrentKeyState = SDL_GetKeyboardState(nullptr);
}

bool Input::IsKeyPressed(SDL_Scancode key){
    if(!s_CurrentKeyState) return false;
    return s_CurrentKeyState[key];
}

bool Input::IsKeyJustPressed(SDL_Scancode key){
    if(!s_CurrentKeyState) return false;

    bool currentlyPressed = s_CurrentKeyState[key];
    bool wasPressed = s_PreviousKeyState[key];

    return currentlyPressed && !wasPressed;
}

void Input::EndFrame(){
    if(s_CurrentKeyState){
        std::memcpy(s_PreviousKeyState, s_CurrentKeyState, sizeof(s_PreviousKeyState));
    }
}
