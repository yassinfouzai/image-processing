#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <stdbool.h>

// Button structure
typedef struct {
    SDL_Rect rect;
    SDL_Color color;
    SDL_Color hoverColor;
    SDL_Color clickColor;
    bool isHovered;
    bool isClicked;
} Button;

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

// Function to render a button
void renderButton(SDL_Renderer *renderer, Button *button) {
    if (button->isClicked) {
        SDL_SetRenderDrawColor(renderer, button->clickColor.r, button->clickColor.g, button->clickColor.b, 255);
    } else if (button->isHovered) {
        SDL_SetRenderDrawColor(renderer, button->hoverColor.r, button->hoverColor.g, button->hoverColor.b, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, button->color.r, button->color.g, button->color.b, 255);
    }
    SDL_RenderFillRect(renderer, &button->rect);
}

// Function to check if a point is inside a rectangle
bool isPointInsideRect(int x, int y, SDL_Rect *rect) {
    return x > rect->x && x < rect->x + rect->w && y > rect->y && y < rect->y + rect->h;
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
    
    // Create a button
    Button button = {
        .rect = {350, 250, 100, 50}, // x, y, width, height
        .color = {0, 128, 255, 255}, // Default color (blue)
        .hoverColor = {0, 200, 255, 255}, // Hover color (light blue)
        .clickColor = {0, 100, 200, 255}, // Click color (dark blue)
        .isHovered = false,
        .isClicked = false
    };

    // Define a destination rectangle for the image (scaled down)
    SDL_Rect dstRect = {100, 100, 400, 300};  // Position (100, 100) with size (400, 300)
    
    bool quit = false;
    SDL_Event event;
    while(!quit){
        while(SDL_PollEvent(&event)){
            switch (event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_MOUSEMOTION:
                    // Check if mouse is over the button
                    button.isHovered = isPointInsideRect(event.motion.x, event.motion.y, &button.rect);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    // Check if button is clicked
                    if (button.isHovered && event.button.button == SDL_BUTTON_LEFT) {
                        button.isClicked = true;
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    // Reset button click state
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        button.isClicked = false;
                    }
                    break;
            }

        }

        // Set the background color (e.g., light gray: RGB(200, 200, 200))
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderClear(renderer);
        
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, &dstRect);
        // Render the button
        renderButton(renderer, &button);
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
