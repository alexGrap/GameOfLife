/*
 ██████╗  █████╗ ███╗   ███╗███████╗     ██████╗ ███████╗    ██╗     ██╗███████╗███████╗
██╔════╝ ██╔══██╗████╗ ████║██╔════╝    ██╔═══██╗██╔════╝    ██║     ██║██╔════╝██╔════╝
██║  ███╗███████║██╔████╔██║█████╗      ██║   ██║█████╗      ██║     ██║█████╗  █████╗  
██║   ██║██╔══██║██║╚██╔╝██║██╔══╝      ██║   ██║██╔══╝      ██║     ██║██╔══╝  ██╔══╝  
╚██████╔╝██║  ██║██║ ╚═╝ ██║███████╗    ╚██████╔╝██║         ███████╗██║██║     ███████╗
 ╚═════╝ ╚═╝  ╚═╝╚═╝     ╚═╝╚══════╝     ╚═════╝ ╚═╝         ╚══════╝╚═╝╚═╝     ╚══════╝ 
                  made by @ivarstet @aquamanp @prewettf
*/

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// размеры поля
#define X_SIZE 80
#define Y_SIZE 25

// состояние char 0 - мертв 1 - жив
#define DEAD '0'
#define ALIVE '1'

void draw_start_menu(int rows, int cols, char pres);

char **malloc_matrix(int rows, int cols);  //  выделение памяти под матрицу
void free_matrix(char **matrix);  // освобождение памяти
void initialize_matrix(char **matrix, int rows,
                       int cols);  // инициализация матрицы из stdin

void copy_matrix(
    char **matrix, char **coppy_matrix, int rows,
    int cols);  // создает копию матрицы и возвращает указатель на нее

int count_neighbors(
    char **matrix, int current_row,
    int current_col);  // подсчитывает количество соседей для текущей клетки

int count_with_vertical_side(char **matrix, int current_row, int current_col);

int count_with_horizontal_side(char **matrix, int current_row, int current_col);

int count_with_corner(char **matrix, int row, int col);

int is_alive(char elem);

int count_from_to(char **matrix, int from_i, int to_i, int from_j,
                  int to_j);  // подсчет в матрице 3 на 3

char next_status(char **matrix, int current_row,
                 int current_col);  // следущее состояние для текущей клетки

void modify(char **matrix, char **coppy_matrix, int rows, int cols,
            int *generation);  // изменение всего поля
void draw(char **matrix, int rows, int cols, int speed,
          int generation);  // отрисока поля

int end_of_game(char **matrix, int rows, int cols,
                char sym);  // условия выхода из игры
void change_speed(int *time_to_delay, char sym,
                  int *speed);  // функция изменения задержки отрисовки

char compare_matrix_static(char **first_matrix, char **second_matrix, int rows,
                           int cols);

void draw_game_over();  //  отрисовка в конце игры

char read_type_of_figure();  // возвращет какой файл запускаем

int main() {
  initscr();  // инициализация окна для работы с библиотекой ncurses
  char pres = read_type_of_figure();
  char **matrix;  // Объявляем матрицу,с которой будем играть
  matrix = malloc_matrix(Y_SIZE, X_SIZE);  // Выделяем память под матрицу
  initialize_matrix(matrix, Y_SIZE, X_SIZE);  // Заполняем матрицу Н.У.
  int speed = 10;
  int generation = 0;
  int time_to_delay = 100000;
  // инициализация main-матрицы
  char **matrix_for_copy = malloc_matrix(
      Y_SIZE,
      X_SIZE);  // дополнительная матрица для призведения перстановок значений
  draw_start_menu(Y_SIZE, X_SIZE, pres);
  char sym;
  scanf("%c", &sym);
  draw(matrix, Y_SIZE, X_SIZE, speed,
       generation);  // отрисовка "нулевого" положения; matrix объявлена у
  // Андрея и ее объявление с инициализацией надо взять у
  // него
  // sleep(5);
  // usleep(time_to_delay);  // задержка анимации
  nodelay(stdscr, TRUE);  // включение неблокируемого режима консоли
  noecho();
  while (end_of_game(matrix, Y_SIZE, X_SIZE,
                     sym)) {  // установил для ясности. Очевидное дополнение
    // в дальнейшем через
    // флаг end_game
    sym = getch();
    change_speed(&time_to_delay, sym,
                 &speed);  // функция отслеживания нажатий + и - для
                           // увеличения или уменьшения скорости игры
    modify(matrix, matrix_for_copy, Y_SIZE, X_SIZE,
           &generation);  // модификация маатрицы для следующего поколения
    draw(matrix, Y_SIZE, X_SIZE, speed,
         generation);  // отрисовка следующего положения
    usleep(time_to_delay);  // задержка с поправками на пользовательские нажатия
  }
  free_matrix(matrix);  // Чистим хвосты
  free_matrix(matrix_for_copy);

  draw_game_over();

  endwin();
  return 0;
}

char **malloc_matrix(int rows, int cols) {
  char **matrix = NULL;
  matrix = (char **)malloc(rows * sizeof(char *) + rows * cols * sizeof(char));
  char *pmatrix = (char *)(matrix + rows);
  for (int i = 0; i < cols; i++) matrix[i] = pmatrix + cols * i;
  return matrix;
}

void free_matrix(char **matrix) { free(matrix); }

void initialize_matrix(char **matrix, int rows, int cols) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      matrix[i][j] = getchar();
      getchar();
    }
  }
}

void modify(char **matrix, char **coppy_matrix, int rows, int cols,
            int *generation) {
  (*generation)++;
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      coppy_matrix[i][j] = next_status(matrix, i, j);
    }
  }
  end_of_game(matrix, rows, cols,
              compare_matrix_static(coppy_matrix, matrix, rows, cols));
  copy_matrix(matrix, coppy_matrix, rows, cols);
}

void draw(char **matrix, int rows, int cols, int speed, int generation) {
  clear();
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      if (matrix[i][j] == DEAD) {
        printw("_");
      } else if (matrix[i][j] == ALIVE) {
        printw("*");
      }
    }
    printw("\n");
  }
  printw("\nSpeed: %d                                Generation: %d", speed,
         generation);
  refresh();
}

int end_of_game(char **matrix, int rows, int cols, char sym) {
  int flag = 1;
  int count = 0;
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      if (matrix[i][j] == DEAD) {
        count += 1;
      }
    }
    if (sym == 'q' || sym == 'Q' || count == 2000) {
      flag = 0;
    }
  }
  return flag;
}

void change_speed(int *time_to_delay, char sym, int *speed) {
  if (sym == '+' && *speed < 20) {
    (*time_to_delay) -= 10000;
    (*speed)++;
  } else if (sym == '-' && *speed > 1) {
    (*time_to_delay) += 10000;
    (*speed)--;
  }
}

int count_neighbors(char **matrix, int current_row, int current_col) {
  int count = 0;
  int is_count = 0;
  if ((current_row == 0 && current_col == 0) ||
      (current_row * current_col == (Y_SIZE - 1) * (X_SIZE - 1)) ||
      (current_row == 0 && current_col == X_SIZE - 1) ||
      (current_row == Y_SIZE - 1 && current_col == 0)) {
    count = count_with_corner(matrix, current_row, current_col);
    is_count = 1;
  }

  if ((!is_count) && (current_col == 0 || current_col == X_SIZE - 1)) {
    count = count_with_vertical_side(matrix, current_row, current_col);
    is_count = 1;
  }

  if ((!is_count) && (current_row == 0 || current_row == Y_SIZE - 1)) {
    count = count_with_horizontal_side(matrix, current_row, current_col);
    is_count = 1;
  }
  if (!is_count) {
    count = count_from_to(matrix, current_row - 1, current_row + 1,
                          current_col - 1, current_col + 1);
  }

  return count;
}

int count_from_to(char **matrix, int from_i, int to_i, int from_j, int to_j) {
  int count = 0;
  int cur_i = (from_i + to_i) / 2;
  int cur_j = (from_j + to_j) / 2;
  for (int i = from_i; i <= to_i; i++) {
    for (int j = from_j; j <= to_j; j++) {
      if (!(i == cur_i && j == cur_j) &&
          matrix[i][j] == ALIVE) {  // не считает центральный элемент матрицы
        count++;
      }
    }
  }
  return count;
}

int count_with_vertical_side(char **matrix, int current_row, int current_col) {
  int count = 0;
  if (current_col == 0) {  // касание с левой стенкой
    for (int i = current_row - 1; i <= current_row + 1; i++) {
      for (int j = current_col; j <= current_col + 1; j++) {
        if (!(i == current_row && j == current_col) && matrix[i][j] == ALIVE) {
          count++;
        }
      }
    }

    for (int i = current_row - 1; i <= current_row + 1; i++) {
      if (matrix[i][X_SIZE - 1] == ALIVE) {
        count++;
      }
    }
  } else {  // касание с правой стенкой
    for (int i = current_row - 1; i <= current_row + 1; i++) {
      for (int j = current_col - 1; j <= current_col; j++) {
        if (!(i == current_row && j == current_col) && matrix[i][j] == ALIVE) {
          count++;
        }
      }
    }

    for (int i = current_row - 1; i <= current_row + 1; i++) {
      if (matrix[i][0] == ALIVE) {
        count++;
      }
    }
  }

  return count;
}

int count_with_horizontal_side(char **matrix, int current_row,
                               int current_col) {
  int count = 0;
  if (current_row == 0) {  // касание с верхней стенкой
    for (int i = current_row; i <= current_row + 1; i++) {
      for (int j = current_col - 1; j <= current_col + 1; j++) {
        if (!(i == current_row && j == current_col) && matrix[i][j] == ALIVE) {
          count++;
        }
      }
    }

    for (int i = current_col - 1; i <= current_col + 1; i++) {
      if (matrix[Y_SIZE - 1][i] == ALIVE) {
        count++;
      }
    }
  } else {  //    касание с нижней стенкой
    for (int i = current_row - 1; i <= current_row; i++) {
      for (int j = current_col - 1; j <= current_col + 1; j++) {
        if (!(i == current_row && j == current_col) && matrix[i][j] == ALIVE) {
          count++;
        }
      }
    }

    for (int i = current_col - 1; i <= current_col + 1; i++) {
      if (matrix[0][i] == ALIVE) {
        count++;
      }
    }
  }
  return count;
}

int is_alive(char elem) { return ((elem == ALIVE) ? 1 : 0); }

int count_with_corner(char **matrix, int row, int col) {
  int count = 0;
  if (col * row == 0) {  //  левый верхний угол
    count = is_alive(matrix[0][1]) + is_alive(matrix[1][0]) +
            is_alive(matrix[1][1]) + is_alive(matrix[Y_SIZE - 1][0]) +
            is_alive(matrix[Y_SIZE - 1][1]) +
            is_alive(matrix[Y_SIZE - 1][X_SIZE - 1]) +
            is_alive(matrix[0][X_SIZE - 1]) + is_alive(matrix[1][X_SIZE - 1]);
  }

  if (row == 0 && col == X_SIZE - 1) {  //  правый верхний угол
    count = is_alive(matrix[0][0]) + is_alive(matrix[1][0]) +
            is_alive(matrix[0][X_SIZE - 2]) + is_alive(matrix[1][X_SIZE - 2]) +
            is_alive(matrix[1][X_SIZE - 1]) + is_alive(matrix[Y_SIZE - 1][0]) +
            is_alive(matrix[Y_SIZE - 1][X_SIZE - 2]) +
            is_alive(matrix[Y_SIZE - 1][X_SIZE - 1]);
  }

  if (row == Y_SIZE - 1 && col == 0) {  // левый нижний угол
    count = is_alive(matrix[0][0]) + is_alive(matrix[0][1]) +
            is_alive(matrix[Y_SIZE - 2][0]) + is_alive(matrix[Y_SIZE - 2][1]) +
            is_alive(matrix[Y_SIZE - 1][1]) + is_alive(matrix[0][X_SIZE - 1]) +
            is_alive(matrix[Y_SIZE - 2][X_SIZE - 1]) +
            is_alive(matrix[Y_SIZE - 1][X_SIZE - 1]);
  }

  if (row == Y_SIZE - 1 && col == X_SIZE - 1) {  //  правый нижний угол
    count = is_alive(matrix[0][0]) + is_alive(matrix[Y_SIZE - 1][0]) +
            is_alive(matrix[Y_SIZE - 2][0]) + is_alive(matrix[0][X_SIZE - 2]) +
            is_alive(matrix[0][X_SIZE - 1]) +
            is_alive(matrix[Y_SIZE - 2][X_SIZE - 2]) +
            is_alive(matrix[Y_SIZE - 2][X_SIZE - 1]) +
            is_alive(matrix[Y_SIZE - 1][X_SIZE - 2]);
  }

  return count;
}

char next_status(char **matrix, int current_row, int current_col) {
  char new_status = matrix[current_row][current_col];
  int count_alive = count_neighbors(matrix, current_row, current_col);

  switch (matrix[current_row][current_col]) {
    case DEAD:
      if (count_alive == 3) {
        new_status = ALIVE;
      }
      break;

    case ALIVE:
      if (count_alive != 2 && count_alive != 3) {
        new_status = DEAD;
      }
      break;
    default:
      break;
  }

  return new_status;
}

void copy_matrix(char **matrix, char **coppy_matrix, int rows, int cols) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      matrix[i][j] = coppy_matrix[i][j];
    }
  }
}

char compare_matrix_static(char **first_matrix, char **second_matrix, int rows,
                           int cols) {
  char flag = 'e';
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      if (first_matrix[i][j] != second_matrix[i][j]) {
        flag = ' ';
      }
    }
  }
  return flag;
}

void draw_game_over() {
  clear();
  printw(

      "  GGGGGGGGGGGGG        \n"
      " GGG::::::::::::G          \n"
      "GG:::::::::::::::G            \n"
      "G:::::GGGGGGGG::::G               \n"
      "G:::::G       GGGGGG  aaaaaaaaaaaaa      mmmmmmm    mmmmmmm       "
      "eeeeeeeeeeee            ooooooooooo vvvvvvv           vvvvvvv "
      "eeeeeeeeeeee    rrrrr   rrrrrrrrr   \n"
      "G:::::G                a::::::::::::a   mm:::::::m  m:::::::mm   "
      "ee::::::::::::ee        oo:::::::::::oov:::::v         "
      "v:::::vee::::::::::::ee  r::::rrr:::::::::r  \n"
      "G:::::G                aaaaaaaaa:::::a m::::::::::mm::::::::::m "
      "e::::::eeeee:::::ee     o:::::::::::::::ov:::::v       "
      "v:::::ve::::::eeeee:::::eer:::::::::::::::::r \n"
      "G:::::G    GGGGGGGGGG           a::::a m::::::::::::::::::::::me::::::e "
      "    e:::::e     o:::::ooooo:::::o v:::::v     v:::::ve::::::e     "
      "e:::::err::::::rrrrr::::::r\n"
      "G:::::G    G::::::::G    aaaaaaa:::::a "
      "m:::::mmm::::::mmm:::::me:::::::eeeee::::::e     o::::o     o::::o  "
      "v:::::v   v:::::v e:::::::eeeee::::::e r:::::r     r:::::r\n"
      "G:::::G    GGGGG::::G  aa::::::::::::a m::::m   m::::m   "
      "m::::me:::::::::::::::::e      o::::o     o::::o   v:::::v v:::::v  "
      "e:::::::::::::::::e  r:::::r     rrrrrrr\n"
      "G:::::G        G::::G a::::aaaa::::::a m::::m   m::::m   "
      "m::::me::::::eeeeeeeeeee       o::::o     o::::o    v:::::v:::::v   "
      "e::::::eeeeeeeeeee   r:::::r            \n"
      "G:::::G       G::::Ga::::a    a:::::a m::::m   m::::m   m::::me:::::::e "
      "               o::::o     o::::o     v:::::::::v    e:::::::e           "
      " r:::::r            \n"
      "G:::::GGGGGGGG::::Ga::::a    a:::::a m::::m   m::::m   m::::me::::::::e "
      "              o:::::ooooo:::::o      v:::::::v     e::::::::e           "
      "r:::::r            \n"
      "GG:::::::::::::::Ga:::::aaaa::::::a m::::m   m::::m   m::::m "
      "e::::::::eeeeeeee       o:::::::::::::::o       v:::::v       "
      "e::::::::eeeeeeee   r:::::r            \n"
      " GGG::::::GGG:::G a::::::::::aa:::am::::m   m::::m   m::::m  "
      "ee:::::::::::::e        oo:::::::::::oo         v:::v         "
      "ee:::::::::::::e   r:::::r            \n"
      "   GGGGGG   GGGG  aaaaaaaaaa  aaaammmmmm   mmmmmm   mmmmmm    "
      "eeeeeeeeeeeeee          ooooooooooo            vvv            "
      "eeeeeeeeeeeeee   rrrrrrr            \n");
  refresh();
  sleep(3);
}

void draw_start_menu(int rows, int cols, char pres) {
  clear();
  int r = rows - 6;
  int choosen_preset = 0;
  switch (pres) {
    case '1':
      choosen_preset = 8;
      break;
    case '2':
      choosen_preset = 9;
      break;
    case '3':
      choosen_preset = 10;
      break;
    case '4':
      choosen_preset = 11;
      break;
    case '5':
      choosen_preset = 12;
      break;
    case '6':
      choosen_preset = 13;
      break;
  }
  for (int i = 0; i < r; i++) {
    for (int j = 0; j < cols; j++) {
      if (i == 0 || i == r - 1) {
        printw("_");
      } else if (j == 0 || j == cols - 1) {
        printw("|");
      } else if (i == 2 && j == 4) {
        printw(
            " #####                                           #                "
            "         |\n"
            "|   #     #   ##   #    # ######     ####  ######    #       # "
            "###### ######   |\n"
            "|   #        #  #  ##  ## #         #    # #         #       # #  "
            "    #        | \n"
            "|   #  #### #    # # ## # #####     #    # #####     #       # "
            "#####  #####    | \n"
            "|   #     # ###### #    # #         #    # #         #       # #  "
            "    #        | \n"
            "|   #     # #    # #    # #         #    # #         #       # #  "
            "    #        | \n"
            "|    #####  #    # #    # ######     ####  #         ####### # #  "
            "    ######   | ");
        j += 80;
      } else if (i == choosen_preset && j == 24) {
        printw(">>>");
        j += 2;
      } else if (i == 7 && j == 36) {
        printw("Presets:");
        j += 7;
      } else if (i == 8 && j == 28) {
        printw("1. Sliders");
        j += 9;
      } else if (i == 9 && j == 28) {
        printw("2. Steady figures + oscillators");
        j += 30;
      } else if (i == 10 && j == 28) {
        printw("3. Longevity R pentomino");
        j += 23;
      } else if (i == 11 && j == 28) {
        printw("4. Reflector gun - devourer");
        j += 26;
      } else if (i == 12 && j == 28) {
        printw("5. Random");
        j += 8;
      } else if (i == 13 && j == 28) {
        printw("6. Train");
        j += 7;
      } else if (i == 16 && j == 14) {
        printw("Use \"+\" and \"-\" to change game speed and \"q\" to quit.");
        j += 52;
      } else if (i == 17 && j == 28) {
        printw("Press any key to start.");
        j += 22;
      } else {
        printw(" ");
      }
    }
    printw("\n");
  }
  refresh();
}

char read_type_of_figure() {
  char symbol;
  if (scanf("%c\n", &symbol) != 1) {
    symbol = '0';
  }
  return symbol;
}
