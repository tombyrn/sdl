#include "./header.h"

int screen_width = DEFAULT_SCREEN_WIDTH;
int screen_height = DEFAULT_SCREEN_HEIGHT;

int last_frame_time = 0;
int game_is_running = 0;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* dvd_logo_texture = NULL;

struct render_queue {
	SDL_Rect rect;
	SDL_Color color;
	int vel_x, vel_y;
	struct render_queue* next;
};

struct render_queue* head;

struct render_queue* init_new_dvd(int x, int y) {
	struct render_queue* new_node = (struct render_queue*)malloc(sizeof(struct render_queue));
	new_node->rect.x = x - DVD_WIDTH/2;
	new_node->rect.y = y - DVD_HEIGHT/2;
	new_node->rect.w = DVD_WIDTH;
	new_node->rect.h = DVD_HEIGHT;

	new_node->color.r = 255;
	new_node->color.g = 255;
	new_node->color.b = 255;
	new_node->color.a = 255;

	new_node->vel_x = 100 * (rand() > RAND_MAX/2 ? 1 : -1);
	new_node->vel_y = 100 * (rand() > RAND_MAX/2 ? 1 : -1);

	new_node->next = NULL;

	return new_node;
}

void destroy_window() {
	struct render_queue* current = head;
	while(current != NULL) {
		struct render_queue* next = current->next;
		free(current);
		current = next;
	}
	SDL_DestroyTexture(dvd_logo_texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void render() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	struct render_queue* current = head;
	while(current != NULL ) {
		// SDL_SetRenderDrawColor(renderer, current->color.r, current->color.g, current->color.b, current->color.a);
		// SDL_RenderFillRect(renderer, &current->rect);
		SDL_SetTextureColorMod(dvd_logo_texture, current->color.r, current->color.g, current->color.b);
		SDL_RenderCopy(renderer, dvd_logo_texture, NULL, &(current->rect));
		current = current->next;
	}

	SDL_RenderPresent(renderer);
}

void update() {
	int time_to_wait = MS_PER_FRAME - (SDL_GetTicks64() - last_frame_time);
	if( time_to_wait > 0 && time_to_wait <= MS_PER_FRAME)
		SDL_Delay(time_to_wait);
	
	// get a delta time factor converted to seconds to be used to update objects displayed
	float delta_time = (SDL_GetTicks64() - last_frame_time) / 1000.0f;

	// check every dvd logo
	struct render_queue* current = head;
	while(current != NULL) {
		// check if logo has hit corner
		if (
			(current->rect.x == 0 && current->rect.y == 0) ||
			(current->rect.x >= screen_width - current->rect.w && current->rect.y == 0) ||
			(current->rect.x == 0 && current->rect.y >= screen_height - current->rect.h) ||
			(current->rect.x == screen_width - current->rect.w && current->rect.y == screen_height - current->rect.h)
		) {
			current->color.r = rand() % 256;
			current->color.g = rand() % 256;
			current->color.b = rand() % 256;
		}
		current->rect.x +=  current->vel_x * delta_time;
		current->rect.y += current->vel_y * delta_time;
		if(current->rect.x >= (screen_width - (current->rect.w)) || current->rect.x <= 0)
			current->vel_x *= -1;

		if(current->rect.y >= (screen_height - (current->rect.h)) || current->rect.y <= 0) 
			current->vel_y *= -1;

		current = current->next;
	}

	last_frame_time = SDL_GetTicks64();
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

		// set screen dimensions on window resize
		case SDL_WINDOWEVENT:
			if(event.window.event  == SDL_WINDOWEVENT_RESIZED) {
				screen_width = event.window.data1;
				screen_height = event.window.data2;
			}
			break;

		// create new dvd logo on left mouse click
		case SDL_MOUSEBUTTONDOWN:
			if(event.button.button == SDL_BUTTON_LEFT) {
				int new_x = event.button.x -  (DVD_WIDTH / 2);
				int new_y = event.button.y - (DVD_HEIGHT / 2);

				// ensure it'll be in bounds
				if(
					new_x < 0 || 
					new_x + DVD_WIDTH > screen_width  || 
					new_y < 0 || 
					new_y + DVD_HEIGHT > screen_height
				)
					break;
				
				// create new logo and add it to render queue
				struct render_queue* new_node = init_new_dvd(event.button.x, event.button.y);
				struct render_queue* current = head;
				while(current->next != NULL) {
					current = current->next;
				}
				current->next = new_node;
			}

			break;		
	}
}

void setup() {
	head = init_new_dvd(DEFAULT_SCREEN_WIDTH/2, DEFAULT_SCREEN_HEIGHT/2);
}

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
	
	renderer = SDL_CreateRenderer(window, -1, 0);
	if(!renderer) {
		fprintf(stderr, "Error creating SDL Renderer.\n");
		return 0;
	}

	SDL_Surface* dvd_logo_surface = IMG_Load("./src/dvd.png");

	if(!dvd_logo_surface) {
		fprintf(stderr, "Error loading dvd logo image.\n");
		return 0;
	}

	dvd_logo_texture = SDL_CreateTextureFromSurface(renderer, dvd_logo_surface);
	if(!dvd_logo_texture) {
		fprintf(stderr, "Error creating dvd logo texture.\n");
		return 0;
	}

	SDL_FreeSurface(dvd_logo_surface);

	return 1;	
}

int main() {
	srand(time(NULL)); 
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