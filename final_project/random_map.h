// row and col size must be even integers in memory game
#define ROL_SIZE 4
#define COL_SIZE 3
#define CAT_NUM 6

void print_pattern(int pattern[ROL_SIZE][COL_SIZE]);
// void Generator(int array[ROL_SIZE][COL_SIZE], int row, int col, int cat_num);
// void swap(int *a, int *b);
void Change_order(int pattern[ROL_SIZE][COL_SIZE], int times);
// void Get_random_pattern(int pattern[ROL_SIZE][COL_SIZE], int rol_size, int col_size, int cat_num);
void Get_uniform_pattern(int pattern[ROL_SIZE][COL_SIZE], int rol_size, int col_size, int cat_num);
