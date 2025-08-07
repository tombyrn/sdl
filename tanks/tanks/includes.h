#ifndef INCLUDES_H
#define INCLUDES_H

#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define NUM_WINDOWS 2
#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 300
#define DOCK_THRESHOLD 400
#define LINE_WIDTH 2
#define ROOM_MARGIN 15
#define LEFT -1
#define RIGHT 1

struct tank {
    SDL_Rect* rect;
    SDL_Rect* rect2;
    SDL_Renderer* renderer;
    SDL_Renderer* other_renderer;

    int window;
    int dx, dy;
    bool is_moving;
};

struct window_properties {
    SDL_Window* window;
    SDL_Renderer* renderer;

    Uint32 id;
};

void init_tank(struct tank*);
void process_input(struct tank*);
void update_tank(struct tank*);
void render_tank(struct tank*);

void init_windows(struct window_properties windows[NUM_WINDOWS]);
void get_window_position(SDL_Window* win, int* x, int* y);
void update_windows(struct window_properties windows[NUM_WINDOWS]);
void render_windows(struct window_properties windows[NUM_WINDOWS]);

extern bool is_running, docked;
extern int left_idx, right_idx;
extern int mouse_x, mouse_y;

#endif