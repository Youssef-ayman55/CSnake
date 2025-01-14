#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <time.h>


#define WIDTH 900
#define HEIGHT 600
#define CELL 20
#define LINE_THICKNESS 1

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3


#define BACKGROUND_COLOR 0x000000
#define SNAKE_COLOR 0xfffff
#define APPLE_COLOR 0xff0000

struct snake_node{
    struct snake_node* next;
    struct snake_node* prev;
    int x;
    int y;
};

int apple_x = 0;
int apple_y = 0;

bool cell_on_snake(struct snake_node* head, int x, int y) {
    while (head != NULL) {
        if (head->x == x && head->y == y) {
            return true;
        }
        head = head->next;
    }
    return false;
}
void grid(SDL_Window * window, SDL_Surface* surface) {
    SDL_Rect line = {0, 0, WIDTH, LINE_THICKNESS};
    for (int i = 0; i < HEIGHT / CELL; i++) {
        line.y = i * CELL;
        SDL_FillRect(surface, &line, 0x1f1f1f1f);
    }
    line.w = LINE_THICKNESS;
    line.h = HEIGHT;
    line.y = 0;
    for (int i = 0; i < WIDTH / CELL; i++) {
        line.x = i * CELL;
        SDL_FillRect(surface, &line, 0x1f1f1f1f);
    }
    SDL_UpdateWindowSurface(window);
}

struct snake_node* initialize_snake(SDL_Surface * surface) {
    struct snake_node* head = malloc(sizeof(struct snake_node));
    head->x = 200;
    head->y = 200;
    head->next = NULL;
    head->prev = NULL;
    SDL_Rect cell = {200, 200, CELL, CELL};
    SDL_FillRect(surface, &cell, 0xfffff);
    return head;
}
void generate_apple(int * apple_x, int* apple_y, struct snake_node* snake, SDL_Surface* surface) {
    *apple_x = (rand() % (WIDTH / CELL)) * CELL;
    *apple_y = (rand() % (HEIGHT / CELL)) * CELL;
    while (cell_on_snake(snake, *apple_x, *apple_y)) {
        *apple_x = (rand() % (WIDTH / CELL)) * CELL;
        *apple_y = (rand() % (HEIGHT / CELL)) * CELL;
    }
    SDL_Rect rect = {*apple_x, *apple_y, CELL, CELL};
    SDL_FillRect(surface, &rect, APPLE_COLOR);
}
void move_snake(struct snake_node** head, struct snake_node** tail, int direction, SDL_Window* window, SDL_Surface* surface) {
    struct snake_node * new_node = malloc(sizeof(struct snake_node));
    new_node->prev = (*head);
    new_node->next = NULL;
    (*head)->next = new_node;
    (*head) = (*head)->next;

    if (direction == UP) {
        if ((*head)->prev->y == 0) {
            (*head)->x = (*head)->prev->x;
            (*head)->y = HEIGHT - CELL;
        }
        else {
            (*head)->x = (*head)->prev->x;
            (*head)->y = (*head)->prev->y - CELL;
        }
    }
    else if (direction == DOWN) {
        if ((*head)->prev->y == HEIGHT - CELL) {
            (*head)->x = (*head)->prev->x;
            (*head)->y = 0;
        }
        else {
            (*head)->x = (*head)->prev->x;
            (*head)->y = (*head)->prev->y + CELL;
        }
    }
    else if (direction == LEFT) {
        if ((*head)->prev->x == 0) {
            (*head)->x = WIDTH - CELL;
            (*head)->y = (*head)->prev->y;
        }
        else {
            (*head)->x = (*head)->prev->x - CELL;
            (*head)->y = (*head)->prev->y;
        }
    }
    else if (direction == RIGHT) {
        if ((*head)->prev->x == WIDTH - CELL) {
            (*head)->x = 0;
            (*head)->y = (*head)->prev->y;
        }
        else {
            (*head)->x = (*head)->prev->x + CELL;
            (*head)->y = (*head)->prev->y;
        }
    }
    SDL_Rect cell = {(*head)->x, (*head)->y, CELL, CELL};
    SDL_FillRect(surface, &cell, SNAKE_COLOR);
    if ((*head)->x == apple_x && (*head)->y == apple_y) {
        generate_apple(&apple_x, &apple_y, *head, surface);
        return;
    }
    new_node = (*tail);
    cell.x = (*tail)->x;
    cell.y = (*tail)->y;
    SDL_FillRect(surface, &cell, BACKGROUND_COLOR);
    (*tail) = (*tail)->next;
    free(new_node);
    (*tail)->prev = NULL;
}


int main(void) {
    srand(time(NULL));
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("Snake",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WIDTH,HEIGHT, 0);
    SDL_Surface* surface = SDL_GetWindowSurface(window);


    struct snake_node* head = initialize_snake(surface);
    struct snake_node* tail = head;
    generate_apple(&apple_x, &apple_y, head, surface);


    SDL_Event ev;
    bool running = true;
    int direction = RIGHT;
    while (running) {
        while (SDL_PollEvent(&ev) != 0) {
            if (ev.type == SDL_QUIT) {
                running = false;
            }
            else if (ev.type == SDL_KEYDOWN) {
                switch (ev.key.keysym.sym) {
                    case SDLK_UP:
                        if (direction != DOWN)
                            direction = UP;
                    break;
                    case SDLK_DOWN:
                        if (direction != UP)
                            direction = DOWN;
                    break;
                    case SDLK_LEFT:
                        if (direction != RIGHT)
                            direction = LEFT;
                    break;
                    case SDLK_RIGHT:
                        if (direction != LEFT)
                            direction = RIGHT;
                    break;
                }
            }
        }
        move_snake(&head, &tail, direction, window, surface);
        grid(window, surface);
        SDL_Delay(100);
    }
    printf("Hello, World!\n");
    return 0;
}