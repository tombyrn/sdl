#include "./header.h"

int last_frame_time = 0;
int game_is_running = 0;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;


int screen_width = 600;
int screen_height = 400;

int offset_x = 0;
int offset_y = 0;

int resize_event(void *userdata, SDL_Event* event);

int initialize_window(void) {

	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(stderr, "Error initializing SDL\n");
		return 0;
	}

	if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
		fprintf(stderr, "Error initializing SDL Image\n");
		return 0;
	}

	window = SDL_CreateWindow(
		NULL, 
		SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED, 
		screen_width, 
		screen_height,
		SDL_WINDOW_RESIZABLE
	);

	if(window == NULL) {
		fprintf(stderr, "Error creating SDL Window.\n");
		return 0;
	}
	
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(!renderer) {
		fprintf(stderr, "Error creating SDL Renderer.\n");
		return 0;
	}

	SDL_AddEventWatch(resize_event, NULL);

	return 1;	
}

void setup() {
	// SETUP STUFF HERE
}

void process_input() {
	SDL_Event event;
	
	
	while(SDL_PollEvent(&event)) {
		switch(event.type) {
			case SDL_QUIT:
				game_is_running = 0;
				break;
			case SDL_KEYDOWN:
				if(event.key.keysym.sym == SDLK_ESCAPE)
					game_is_running = 0;
				break;
			case SDL_MOUSEWHEEL:
				if(event.wheel.x != 0) {
					printf("x %d\t%d\n", event.wheel.x, offset_x);
					offset_x +=event.wheel.x;
				}

				if(event.wheel.y != 0) {
					printf("y %d\t%d\n", event.wheel.y, offset_y);
					offset_y += event.wheel.y;
				}

				break;
		}
	}
}

void update() {
	int time_to_wait = MS_PER_FRAME - (SDL_GetTicks() - last_frame_time);
	if( time_to_wait > 0 && time_to_wait <= MS_PER_FRAME)
		SDL_Delay(time_to_wait);
	
	// get a delta time factor converted to seconds to be used to update objects displayed
	float delta_time = (SDL_GetTicks64() - last_frame_time) / 1000.0f;

	SDL_GetWindowSize(window, &screen_width, &screen_height);
	if((abs(offset_x) > GRID_SIZE)) offset_x = 0;
	if((abs(offset_y) > GRID_SIZE)) offset_y = 0;

	last_frame_time = SDL_GetTicks();
}

void render() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	for(int x = 0; x <= screen_width; x += GRID_SIZE) {
		SDL_RenderDrawLine(renderer, x-offset_x, 0, x-offset_x, screen_height);
	}
	for(int y = 0; y <= screen_height; y += GRID_SIZE) {
		SDL_RenderDrawLine(renderer, 0, y-offset_y, screen_width, y-offset_y);
	}
	SDL_RenderPresent(renderer);

}

int resize_event(void *userdata, SDL_Event* event) {
  if (event->type == SDL_WINDOWEVENT) {
    if (event->window.event == SDL_WINDOWEVENT_EXPOSED) {
		update();
    	render();
    }
  }
  return 1;
}



void destroy_window() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int main() {
	game_is_running = initialize_window();	

	setup();

	while(game_is_running) {
		process_input();
		update();
		render();
	}

	destroy_window();
	return 0;
}
