#include "includes.h"

void init_windows(struct window_properties windows[NUM_WINDOWS]) {
    // initialize two windows
    for(int i = 0; i < NUM_WINDOWS; i++) {
        char title[100] = {0};
        sprintf(title, "Window %d", i);
        windows[i].window = SDL_CreateWindow(title,  100 + (i * SCREEN_WIDTH), 100 + (i * SCREEN_HEIGHT), SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        windows[i].renderer = SDL_CreateRenderer(windows[i].window, -1, SDL_RENDERER_ACCELERATED);
        windows[i].id = SDL_GetWindowID(windows[i].window);
    }
}

void get_window_position(SDL_Window* win, int* x, int* y) {
    SDL_GetWindowPosition(win, x, y);
}

void update_windows(struct window_properties windows[NUM_WINDOWS]) {
    // get position of windows on screen for both windows
    int x0, y0, x1, y1;
    get_window_position(windows[0].window, &x0, &y0);
    get_window_position(windows[1].window, &x1, &y1);

    int dx = abs((x0 + SCREEN_WIDTH/2) - (x1 + SCREEN_WIDTH/2));
    int dy = abs((y0 + SCREEN_WIDTH/2) - (y1 + SCREEN_WIDTH/2));
    docked = (dx < DOCK_THRESHOLD && dy < DOCK_THRESHOLD);


    // printf("x0: %d, y0: %d, x1: %d, y2: %d, dx: %d, dy: %dy\n", x0, y0, x1, y1, dx, dy);
    right_idx = (x0 < x1) ? 1 : 0;
    left_idx = (x0 < x1) ? 0 : 1;
}

void render_windows(struct window_properties windows[NUM_WINDOWS]) {
    for(int i = 0; i < NUM_WINDOWS; i++)
        SDL_RenderPresent(windows[i].renderer);
}