#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <ncurses.h>

struct window {
  int width;
  int height;
} globals;

struct ball {
  float speed;
  float x, y;
  int dx, dy;
};

struct block {
  float x, y;
  int width, height;
};

int SCORE_TO_WIN = 10;
time_t t;
int idle = 1;
int is_paused = 0;
int key;
int score_a = 0;
int score_b = 0;

int get_random_direction() {
  int next = rand() % 2;

  if (next == 0) {
    return -1;
  }

  return 1;
}

void initialize_globals() {
  getmaxyx(stdscr, globals.height, globals.width);
}

void initialize_ball(struct ball *entity) {
  entity->x = globals.width / 2;
  entity->y = globals.height / 2;
  entity->dx = get_random_direction();
  entity->dy = get_random_direction();
  entity->speed = 1;
}

void initialize_block(struct block *entity, int x, int y, int width, int height) {
  entity->x = x;
  entity->y = y;
  entity->width = width;
  entity->height = height;
}

int check_collision(struct ball *a, struct block *b) {
  if (b->x < a->x && (b->x + b->width) > a->x &&
      b->y < a->y && (b->y + b->height) > a->y)    {
    return 1;
  }

  return 0;
}

void update_ball(struct ball *entity, struct block *left, struct block *right) {
  if (entity->x <= 0) {
    entity->dx = 1;
    score_b++;
    entity->x = globals.width / 2;
    entity->y = globals.height / 2;
    entity->dx = get_random_direction();
    entity->dy = get_random_direction();
    left->y = 8;
    right->y = 8;
    idle = 1;
  }

  if (entity->x >= globals.width) {
    entity->dx = -1;
    score_a++;
    entity->x = globals.width / 2;
    entity->y = globals.height / 2;
    entity->dx = get_random_direction();
    entity->dy = get_random_direction();
    left->y = 8;
    right->y = 8;
    idle = 1;
  }

  if (entity->y <= 0) {
    entity->dy = 1;
  }

  if (entity->y >= globals.height) {
    entity->dy = -1;
  }

  if (check_collision(entity, left)) {
    entity->dx = entity->dx * -1;
  }

  if (check_collision(entity, right)) {
    entity->dx = entity->dx * -1;
  }

  entity->x += entity->speed * entity->dx;
  entity->y += entity->speed * entity->dy;
}

void render_ball(struct ball *entity) {
  int x = ceil(entity->x);
  int y = ceil(entity->y);
  mvprintw(y, x, "o");
}

void update_block(struct block *entity, int up, int down) {
  if (key == up && entity->y > 0) {
    entity->y -= 2;
  }

  if (key == down && (entity->y + entity->height) < globals.height) {
    entity->y += 2;
  }
}

void render_block(struct block *entity) {
  for(int i = 0; i < entity->height; i++) {
    for(int j = 0; j < entity->width; j++) {
      mvaddch(entity->y + i, entity->x + j, 'X');
    }
  }
}

void render_scores() {
  mvprintw(1, 12, "Player A");
  mvprintw(2, 12, "%d", score_a);
  mvprintw(1, 58, "Player B");
  mvprintw(2, 58, "%d", score_b);
}

char get_winner() {
  if (score_a >= SCORE_TO_WIN) {
    return 'A';
  }

  if (score_b >= SCORE_TO_WIN) {
    return 'B';
  }

  return 0;
}

void reset_game(struct ball *entity, struct block *left, struct block *right) {
  is_paused = 0;
  idle = 1;
  entity->x = globals.width / 2;
  entity->y = globals.height / 2;
  entity->dx = get_random_direction();
  entity->dy = get_random_direction();
  left->y = 8;
  right->y = 8;
  score_a = 0;
  score_b = 0;
}

int any_score_registered() {
  if (score_a || score_b) {
    return 1;
  }

  return 0;
}

int main() {
  const Uint32 fps = 30;
  const Uint32 minimum_frame_time = 1000 / fps;

  int running = 1;
  Uint32 last_ms, delta_ms;

#if defined(_DEBUG)
  Uint32 dt_ms;
#endif

  struct ball entity;
  struct block left;
  struct block right;

  initscr();
  keypad(stdscr, TRUE);
  nonl();
  cbreak();
  noecho();
  curs_set(FALSE);
  nodelay(stdscr, TRUE);

  srand((unsigned) time(&t));

  initialize_globals();
  initialize_ball(&entity);
  initialize_block(&left, 8, 8, 2, 10);
  initialize_block(&right, 68, 8, 2, 10);

#if defined(_DEBUG)
  Uint32 last_updated_ms = SDL_GetTicks();
#endif

  while(running) {
    last_ms = SDL_GetTicks();

#if defined(_DEBUG)
    dt_ms = last_ms - last_updated_ms;
#endif

    key = getch();

    if (key == KEY_F(1)) {
      break;
    }

    clear();
    render_scores();

    char winner = get_winner();

    if (is_paused) {
      int y = (globals.height / 2) - 5;
      int x = (globals.width / 2) - 12;

      mvprintw(y, x + 6, "%s", "PAUSE");
      mvprintw(y + 2, x, "%s", "Press <p> to resume");
      mvprintw(y + 3, x, "%s", "Press <r> to reset");
      mvprintw(y + 4, x, "%s", "Press <F1> to quit");

      mvprintw(20, 5, "%s", "Move <W-S>");
      mvprintw(20, 63, "%s", "Move <UP-DOWN>");

      if (key == 'r') {
        reset_game(&entity, &left, &right);
      } else if (key == 'p') {
        is_paused = 0;
      }
    } else if (key == 'p' && idle == 0) {
      is_paused = 1;
    } else if (idle) {
      int y = (globals.height / 2) - 9;
      int x = (globals.width / 2) - 11;

      mvprintw(20, 5, "%s", "Move <W-S>");
      mvprintw(20, 63, "%s", "Move <UP-DOWN>");

      if (any_score_registered() && winner == 0) {
        mvprintw(y, x, "%s", "Press <space> to continue");
        mvprintw(y + 1, x, "%s", "Press <F1> to quit");
      } else {
        mvprintw(y, x + 1, "%s", "RETRO PONG by Diego");
        mvprintw(y + 3, x, "%s", "Press <space> to play");
        mvprintw(y + 4, x, "%s", "Press <F1> to quit");

        if (winner) {
          mvprintw(y + 6, x + 3, "Player %c wins!", winner);
        }
      }

      if (key == ' ') {
        if (winner) {
          reset_game(&entity, &left, &right);
        }

        idle = 0;
      }
    } else {
      update_ball(&entity, &left, &right);
      update_block(&left, 'w', 's');
      update_block(&right, KEY_UP, KEY_DOWN);
    }

    render_ball(&entity);
    render_block(&left);
    render_block(&right);

#if defined(_DEBUG)
    mvprintw(0, 0, "FPS: %d", dt_ms);
#endif

    refresh();

    delta_ms = SDL_GetTicks() - last_ms;

    if (delta_ms < minimum_frame_time) {
      SDL_Delay(minimum_frame_time - delta_ms);
    }

#if defined(_DEBUG)
    last_updated_ms = last_ms;
#endif
  }

  endwin();

  return 0;
}
