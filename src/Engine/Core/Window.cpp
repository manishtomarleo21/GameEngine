#include "Engine/Core/Window.h"
#include <iostream>

Window::Window(const std::string& title, int width, int height){
    if(!SDL_Init(SDL_INIT_VIDEO)){
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return;
    }

    m_Window = SDL_CreateWindow(
        title.c_str(),
        width,
        height,
        SDL_WINDOW_RESIZABLE
    );

    if(!m_Window){
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return;
    }
    m_Renderer = SDL_CreateRenderer(m_Window, nullptr);
    if(!m_Renderer){
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(m_Window);
        SDL_Quit();
        return;
    }
    std::cout << "Window created successfully!" << std::endl;
}

Window::~Window(){
    if(m_Renderer){
        SDL_DestroyRenderer(m_Renderer);
    }
    if(m_Window){
        SDL_DestroyWindow(m_Window);
    }
    SDL_Quit();
}

void Window::PollEvents(){
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if(event.type == SDL_EVENT_QUIT){
            m_ShouldClose = true;
        }
    }
    
}

void Window::Clear(float r, float g, float b){
    Uint8 red = static_cast<Uint8>(r * 255.0f); 
    Uint8 green = static_cast<Uint8>(g * 255.0f); 
    Uint8 blue = static_cast<Uint8>(b * 255.0f); 

    SDL_SetRenderDrawColor(m_Renderer, red, green, blue, 255);
    SDL_RenderClear(m_Renderer);
}

void Window::Present(){
    SDL_RenderPresent(m_Renderer);
}

void Window::SwapBuffers(){

}

void Window::GetSize(int& width, int& height) const {
    SDL_GetWindowSize(m_Window, &width, &height);
}


