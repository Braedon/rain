#include <curses.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define GENNUM 16

int current_getch;
static WINDOW *mainwnd;
int y, x;
char** array;
int genNum = GENNUM;
char* test = "hello";

struct Input {
  int currVal;
  char str[256];
};

struct Inputs {
  int width;
  int full;
  struct Input** inputs;
};

struct Inputs inputs;

void screen_init(void) {
  int i, j;
  char* p;

  x = COLS;
  y = LINES;

  inputs.width = x;
  inputs.full = 0;

  inputs.inputs = (struct Input**) malloc(sizeof(struct Inputs*) * x);

  array = (char**) malloc(sizeof(char*) * y);

  for (j = 0; j < x; j++) {
    inputs.inputs[j] = NULL;
  }

  for (i = 0; i < y; i++) {
    array[i] = (char*) malloc(sizeof(char) * x + 1);
    for (j = 0; j < x; j++) {
      array[i][j] = ' ';
    }
    array[i][x] = 0;
  }

  mainwnd = initscr();
  noecho();
  cbreak();
  nodelay(mainwnd, TRUE);
  refresh(); // 1)
  wrefresh(mainwnd);
}

void updateSize(int newy, int newx) {
  int i, j;

  if (x != newx || y != newy) {
    inputs.width = newx;
    inputs.full = 0;

    inputs.inputs = (struct Input**) realloc(inputs.inputs, sizeof(struct Inputs*) * newx);

    for (j = x; j < newx; j++) {
      inputs.inputs[j] = NULL;
    }

    for (j = 0; j < newx; j++) {
      if (inputs.inputs[j] != NULL) {
        inputs.full++;
      }
    }

    array = (char**) realloc(array, sizeof(char*) * newy);

    for (i = y; i < newy; i++) {
      array[i] = NULL;
    }

    for (i = 0; i < newy; i++) {
      array[i] = (char*) realloc(array[i], sizeof(char) * newx + 1);
      if (newx > x) {
        for (j = x; j < newx; j++) {
          array[i][j] = ' ';
        }
      }

      if (i >= y ) {
        for (j = 0; j < x; j++) {
          array[i][j] = ' ';
        }
      }
      array[i][newx] = 0;
    }

    y = newy;
    x = newx;
  }
}

void shiftAndPrint() {
  int i, j;
  struct Input* input;
  char character;
  char* bottomRow;

  bottomRow = array[y - 1];

  for (i = y - 1; i > 0; i--) {
    array[i] = array[i - 1];
    mvwprintw(mainwnd,i, 0,"%s", array[i]);
  }

  array[0] = bottomRow;

  for (j = 0; j < x; j++) {
    if (inputs.inputs[j] == NULL) {
      array[0][j] = ' ';
    } else {
      input = inputs.inputs[j];
      if (input->currVal < 0) {
        inputs.inputs[j] = NULL;
        inputs.full--;
        array[0][j] = ' ';
      } else {
        array[0][j] = input->str[input->currVal];
        input->currVal--;
      }
    }
  }
  mvwprintw(mainwnd,0, 0,"%s", array[0]);
}

static void update_display(void) {
  int i, j;

  curs_set(0);

  shiftAndPrint();

  wrefresh(mainwnd);
  refresh();
}

void screen_end(void) {
  endwin();
}

void wait (int ms, clock_t startWait) {
  clock_t endWait;
  endWait = startWait + ms * CLOCKS_PER_SEC / 1000;
  while (clock() < endWait) {}
}

int main(void) {
  struct Input* input;
  clock_t startWait;
  screen_init();
  int j, a, b, c, d, newx, newy;

  while (true) {
    startWait = clock();

    updateSize(LINES, COLS);
    //if (!fseek(stdin, 1, SEEK_CUR)) {
    //  fseek(stdin, -1, SEEK_CUR);
      if (inputs.full < inputs.width) {
        //d = rand() % (int)((inputs.width - inputs.full) / sqrt(inputs.width));
        d = rand() % (int)((inputs.width - inputs.full) / (inputs.width / 4));
        for (c = 0; c < d; c++) {
          input = (struct Input*) malloc(sizeof(struct Input));
          scanf("%[^\n]\n", input->str);
          //fgets(input->str, 256, stdin);
          //scanf("%s", input->str);
          input->currVal = strlen(input->str) - 1;

          b = rand() % (inputs.width - inputs.full);
          a = 0;

          for (j = 0; j < inputs.width; j++) {
            if (inputs.inputs[j] == NULL) {
              if ( a == b ) {
                inputs.inputs[j] = input;
                inputs.full++;
                break;
              }
              a++;
            }
          }
        }
      }
    //}
    update_display();
    wait(40, startWait);
  }
  screen_end();
  return 0;
}
