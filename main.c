#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <time.h>

#define WIDTH 900
#define HEIGHT 600
#define CELL 150
#define CELLCOUNT ((WIDTH * HEIGHT) / (CELL * CELL))
#define ROWS (HEIGHT / CELL)
#define COLS (WIDTH / CELL)
#define LINE_THICKNESS 1

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3


#define BACKGROUND_COLOR 0x000000
#define SNAKE_COLOR 0xfffff
#define APPLE_COLOR 0xff0000

int cells[CELLCOUNT];
int cell_index[CELLCOUNT];
int snake_size = 1;
struct snake_node{
    struct snake_node* next;
    struct snake_node* prev;
    int cell;
    int x;
    int y;
};


int apple = 0;


void swap(int * x, int * y) {
    int temp = *x;
    *x = *y;
    *y = temp;
}
int mod(int cell, int d) {
    return (cell + d) % d;
}

bool cell_on_snake(struct snake_node* head, int cell) {
    while (head != NULL) {
        if (head->cell == cell) {
            return true;
        }
        head = head->prev;
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
    int cord = rand() % CELLCOUNT;
    head->cell = cord;
    swap(&cells[cell_index[cord]], &cells[CELLCOUNT - snake_size]);
    swap(&cell_index[cord], &cell_index[cells[cell_index[cord]]]);

    SDL_Rect cell = {(cord % COLS) * CELL, (cord / COLS) * CELL, CELL, CELL};
    SDL_FillRect(surface, &cell, 0xfffff);
    return head;
}
void generate_apple(int* apple, SDL_Surface* surface) {
    *apple = cells[rand() % (CELLCOUNT - snake_size)];
    SDL_Rect rect = {(*apple % COLS) * CELL, (*apple / COLS) * CELL, CELL, CELL};
    SDL_FillRect(surface, &rect, APPLE_COLOR);
}
void move_snake(struct snake_node** head, struct snake_node** tail, int direction, SDL_Window* window, SDL_Surface* surface) {
    struct snake_node * new_node = malloc(sizeof(struct snake_node));

    new_node->prev = (*head);
    new_node->next = NULL;
    (*head)->next = new_node;
    (*head) = (*head)->next;


    if (direction == UP) {
        (*head)->cell = mod((*head)->prev->cell - COLS, CELLCOUNT);
    }
    else if (direction == DOWN) {
        (*head)->cell = mod((*head)->prev->cell + COLS, CELLCOUNT);
    }
    else if (direction == LEFT) {
        (*head)->cell = (*head)->prev->cell - (*head)->prev->cell % COLS + mod((*head)->prev->cell % COLS - 1, COLS);
    }
    else {
        (*head)->cell = (*head)->prev->cell - (*head)->prev->cell % COLS + mod((*head)->prev->cell % COLS + 1, COLS);
    }

    SDL_Rect cell = {((*head)->cell % COLS) * CELL, ((*head)->cell / COLS) * CELL, CELL, CELL};
    SDL_FillRect(surface, &cell, SNAKE_COLOR);
    if ((*head)->cell == apple) {
        snake_size++;
        swap(&cells[cell_index[(*head)->cell]], &cells[CELLCOUNT - snake_size]);
        swap(&cell_index[(*head)->cell], &cell_index[cells[cell_index[(*head)->cell]]]);
        generate_apple(&apple, surface);
        return;
    }
    swap(&cells[cell_index[(*head)->cell]], &cells[cell_index[(*tail)->cell]]);
    swap(&cell_index[(*head)->cell], &cell_index[(*tail)->cell]);
    new_node = (*tail);
    cell.x = ((*tail)->cell % COLS) * CELL;
    cell.y = ((*tail)->cell / COLS) * CELL;
    SDL_FillRect(surface, &cell, BACKGROUND_COLOR);
    (*tail) = (*tail)->next;
    free(new_node);
    (*tail)->prev = NULL;
}


int main(void) {
    srand(time(0));
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("Snake",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WIDTH,HEIGHT, 0);
    SDL_Surface* surface = SDL_GetWindowSurface(window);

    for (int i = 0; i < CELLCOUNT; i++) {
        cells[i] = i;
        cell_index[i] = i;
    }

    struct snake_node* head = initialize_snake(surface);
    struct snake_node* tail = head;
    generate_apple(&apple, surface);

    SDL_Event ev;
    bool running = true;
    int direction = RIGHT;
    while (running) {
        int curdir = direction;
        while (SDL_PollEvent(&ev) != 0) {
            if (ev.type == SDL_QUIT) {
                running = false;
            }
            else if (ev.type == SDL_KEYDOWN) {
                switch (ev.key.keysym.sym) {
                    case SDLK_UP:
                        if (curdir != DOWN)
                            direction = UP;
                    break;
                    case SDLK_DOWN:
                        if (curdir != UP)
                            direction = DOWN;
                    break;
                    case SDLK_LEFT:
                        if (curdir != RIGHT)
                            direction = LEFT;
                    break;
                    case SDLK_RIGHT:
                        if (curdir != LEFT)
                            direction = RIGHT;
                    break;
                }

            }
        }
        move_snake(&head, &tail, direction, window, surface);
        grid(window, surface);
        for (int i =0; i < CELLCOUNT; i++) {
            printf("%d,", cells[i]);
        }
        printf("\n");
        for (int i = 0; i < CELLCOUNT; i++) {
            printf("%d,", cell_index[i]);
        }
        printf("\n");
        SDL_Delay(1000);
    }
    printf("Hello, World!\n");
    return 0;
}