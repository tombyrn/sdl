#ifndef HEADER_H
#define HEADER_H
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define SCREEN_WIDTH 64 * 16
#define SCREEN_HEIGHT  64 * 9
#define GRAVITY 300
#define PLAYER_SPEED 50

#define LEVEL_ONE_SIZE 144

#define FPS 60
#define MS_PER_FRAME (1000 / FPS)


struct player {
    SDL_Rect rect;
    int dx, dy;
    bool jumping;
    // SDL_Texture* sprites[4];
};

struct collision_block {
    SDL_Rect r;
    int color;
    struct collision_block* next;
};
extern struct collision_block* c_block_head;

extern struct player p;

void setup_level(int level);

void update_player(float dt);

void render_level(int level);
void render_player();
extern SDL_Window* window;
extern SDL_Renderer* renderer;
#endif