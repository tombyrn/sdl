#include "./header.h"

int last_frame_time = 0;
int game_is_running = 0;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

struct player p;

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
		SCREEN_WIDTH, 
		SCREEN_HEIGHT,
		SDL_WINDOW_SKIP_TASKBAR
	);

	if(window == NULL) {
		fprintf(stderr, "Error creating SDL Window.\n");
		return 0;
	}
	
	renderer = SDL_CreateRenderer(window, -1, 0);
	if(!renderer) {
		fprintf(stderr, "Error creating SDL Renderer.\n");
		return 0;
	}

	return 1;	
}

void setup() {
	p.dx = 0;
	p.dy = 0;
	p.rect.w = 64;
	p.rect.h = 64;
	p.rect.x = 4 * p.rect.w;
	p.rect.y = 3 * p.rect.h;

	setup_level(1);
}

void process_input() {
	SDL_Event event;
	SDL_PollEvent(&event);
	
	switch(event.type) {
		case SDL_QUIT:
			game_is_running = 0;
			break;
		case SDL_KEYDOWN:
			if(event.key.keysym.sym == SDLK_ESCAPE)
				game_is_running = 0;
			break;
		
		// PROCESS MORE INPUT HERE
	}

	const Uint8* keystates = SDL_GetKeyboardState(NULL);
	if (keystates[SDL_SCANCODE_A]) { p.dx = -5; }
	if (keystates[SDL_SCANCODE_D]) { p.dx = 5; }
	if (keystates[SDL_SCANCODE_W]) { p.dy = -900; }
}

void update() {
	int time_to_wait = MS_PER_FRAME - (SDL_GetTicks() - last_frame_time);
	if( time_to_wait > 0 && time_to_wait <= MS_PER_FRAME)
		SDL_Delay(time_to_wait);
	
	// get a delta time factor converted to seconds to be used to update objects displayed
	float delta_time = (SDL_GetTicks64() - last_frame_time) / 1000.0f;

	update_player(delta_time);

	last_frame_time = SDL_GetTicks();
}

void render() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	render_level(1);
	render_player();

	SDL_RenderPresent(renderer);
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
