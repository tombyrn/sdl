#include "header.h"

void update_player(float dt) {
    p.dy += GRAVITY * dt * PLAYER_SPEED;
    p.dx > 0 ? p.dx-- : p.dx++;
    
    // horizontal collision
    p.rect.x += p.dx * dt * PLAYER_SPEED;

    struct collision_block* c_block = c_block_head;
    while(c_block != NULL) {
        if(SDL_HasIntersection(&p.rect, &c_block->r)) {
            // moving right
            if(p.dx > 0) {
                p.rect.x = c_block->r.x - p.rect.w; // push to left of block
                // printf("x|%d\n", p.rect.x);
            }
            // moving left
            if(p.dx < 0) {
                p.rect.x = c_block->r.x + c_block->r.w; // push to right of block
            }
            p.dx = 0; 
        }

        c_block = c_block->next;
    }

    // vertical collision
    p.rect.y += p.dy * dt;

    c_block = c_block_head;
    while(c_block != NULL) {
        if(SDL_HasIntersection(&p.rect, &c_block->r)) {
            // floor
            if (p.dy > 0)  
                p.rect.y = c_block->r.y - p.rect.h; // stand on top
            // jumping into ceiling
            if (p.dy < 0)  
                p.rect.y = c_block->r.y + c_block->r.h;
            
            p.dy = 0; 
        }

        c_block = c_block->next;
    }

    // left screen bound
    if(p.rect.x < 0) 
        p.rect.x = 0;

    // right screen bound
    if(p.rect.x > SCREEN_WIDTH - p.rect.w) 
        p.rect.x = SCREEN_WIDTH - p.rect.w;

    // ceiling
    if(p.rect.y < 0)
        p.rect.y = 0;

    // floor
    if(p.rect.y > SCREEN_HEIGHT - p.rect.h) {
        p.dy = 0;
        p.rect.y = SCREEN_HEIGHT - p.rect.h;
    }

}


void render_player() {
    // printf("Player: x=%d y=%d w=%d h=%d\tdx%d dy %d\n", p.rect.x, p.rect.y, p.rect.w, p.rect.h, p.dx, p.dy);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &(p.rect));
}