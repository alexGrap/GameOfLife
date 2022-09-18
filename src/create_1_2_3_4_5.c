#include <stdio.h>
#include <stdlib.h>

#define X_SIZE 80
#define Y_SIZE 25

char **malloc_matrix(int rows, int cols); //  выделение памяти под матрицу
void free_matrix(char **matrix); // освобождение памяти
void create_matrix(char **matrix, int rows,
                   int cols); // инициализация матрицы из stdin

int main() {
  char **matrix; // Объявляем матрицу,с которой будем играть
  matrix = malloc_matrix(Y_SIZE, X_SIZE); //Выделяем память под матрицу
  create_matrix(matrix, Y_SIZE, X_SIZE); //Заполняем матрицу Н.У.
  free_matrix(matrix); //Чистим хвосты
  return 0;
}

char **malloc_matrix(int rows, int cols) {
  char **matrix = NULL;
  matrix = (char **)malloc(rows * sizeof(char *) + rows * cols * sizeof(char));
  char *pmatrix = (char *)(matrix + rows);
  for (int i = 0; i < rows; i++)
    matrix[i] = pmatrix + cols * i;
  return matrix;
}

void free_matrix(char **matrix) { free(matrix); }

void create_matrix(char **matrix, int rows, int cols) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
        matrix[i][j]='0';
    }
  }
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      if (j != cols - 1) {
        printf("%c ", matrix[i][j]);
      } else {
        printf("%c\n", matrix[i][j]);
      }
    }
  }
}
