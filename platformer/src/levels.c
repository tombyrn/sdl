#include "./header.h"

struct collision_block* c_block_head = NULL;

int level1[LEVEL_ONE_SIZE] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 292, 0, 0, 292, 292, 292, 292, 0, 0, 292, 292, 292, 292, 292, 0,
    0, 292, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 292, 0,
    0, 292, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 292, 0,
    0, 292, 292, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 292, 0,
    0, 292, 292, 292, 292, 0, 0, 292, 292, 292, 292, 292, 292, 292, 292, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void push_block(int i, int n) {
    struct collision_block* new_block = calloc(1, sizeof(struct collision_block));
    if (!new_block) return;

    new_block->r.x = (i % 16) * 64;
    new_block->r.y = (i / 16) * 64;
    new_block->r.w = 64;
    new_block->r.h = 64;
    new_block->color = n;
    new_block->next = NULL;

    new_block->next = c_block_head;
    c_block_head = new_block;
}

void setup_level(int level) {
    for(int i = 0; i < LEVEL_ONE_SIZE; i++) {
        if(level1[i] != 0)
            push_block(i, level1[i]);
    }
}

void render_level(int level) {
    struct collision_block* c_block = c_block_head;
    while(c_block != NULL) {
        SDL_SetRenderDrawColor(renderer, 7, 244, 67, 255);
        SDL_RenderFillRect(renderer, &(c_block->r));
        c_block = c_block->next;
    }
};