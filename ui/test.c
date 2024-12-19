#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <stdbool.h>


bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init ERROR: %s\n", SDL_GetError());
        return false;
    }

    int imgFlags = IMG_INIT_PNG;
    int imgInitResult = IMG_Init(imgFlags);

    if (!(imgInitResult & IMG_INIT_PNG)) {
        printf("IMG_Init ERROR: %s\n", IMG_GetError());
        SDL_Quit();
        return false;
    }

    printf("Initialization successful: SDL and SDL_image (PNG) initialized.\n");
    return true;
}

bool createWindow(SDL_Window **win)
{
    *win = SDL_CreateWindow("image proccessing v0.0.0",
                           SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
                           800,600, 
                           SDL_WINDOW_SHOWN);

    if(*win == NULL){
        printf("ERROR CREATING WINDOW :%s\n",SDL_GetError());
        return false;
    }

    return true;
}

bool createRender(SDL_Window *win,SDL_Renderer **renderer)
{
    *renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    
    if(*renderer == NULL){

        printf("ERROR CREATING RENDERER:%s\n",SDL_GetError());
        return false;
    }

    return true;
}

bool loadPNG(const char *filename,SDL_Renderer *renderer, SDL_Texture **texture)
{
    SDL_Surface *imgSurface = IMG_Load(filename);

    if(imgSurface == NULL){
        printf("ERROR LOADING THE IMAGE:%s\n",SDL_GetError());
        return false;
    }
    
    *texture = SDL_CreateTextureFromSurface(renderer, imgSurface);
    SDL_FreeSurface(imgSurface);

    if(*texture == NULL){
        printf("ERROR LOADING THE IMAGE:%s\n",SDL_GetError());
        return false;
    }

    return true;
}


int main(int argc, char* argv[])
{
    SDL_Window *win = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Texture *texture = NULL;

    if(!init()) return 1;
    if(!createWindow(&win)) goto cleanup;
    if(!createRender(win,&renderer)) goto cleanup;
    if(!loadPNG("file.png",renderer,&texture)) goto cleanup;
    

    bool quit = false;
    SDL_Event event;
    while(!quit){
        while(SDL_PollEvent(&event)){
            if (event.type == SDL_QUIT)
                quit = true;
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }
    
    cleanup:
        if (texture) SDL_DestroyTexture(texture);
        if (renderer) SDL_DestroyRenderer(renderer);
        if (win) SDL_DestroyWindow(win);
        IMG_Quit();
        SDL_Quit();
    return 0;

}
