#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

//Definiciones de funciones de ayuda.
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);

/* Tarea 1 */
game_state_t* create_default_state() {
  // TODO: Implementar esta funcion.
  game_state_t* state = malloc(sizeof(game_state_t)); //reservacion de memoria
  state->num_rows = 18;// Numero de filas que habra en el tablero.
  state->num_snakes = 1;// Significa que habra solo una snake 

  //Reservacion de memoria exclusivamente para las filas que habra en el tablero.
  state->board = malloc(state->num_rows * sizeof(char*));

  
//Este ciclo recorre cada fila del tablero 
  for(unsigned int i= 0; i < state->num_rows; i++){
    
    state->board[i] = malloc(21 * sizeof(char));//Se reserva memoria para 21 columnas por fila
    for (unsigned int j = 0; j < 20; j++){// Recore la parte de las columnas.
      //Los bordes del tablero se llenan con "#"
      state->board[i][j] = (i == 0 || i == 17 || j == 0 || j == 19) ? '#' : ' ';//Espacios vacios en el interior.
      }
      state->board[i][20] = '\0';//Esto agrega el caracter nulo al final de cada fila.
    }
    

  //esta sera la posicion inicial que tendra la fruta y la snake.
  state->board[2][2] = 'd';//cola de snake
  state->board[2][3] = '>';//cuerpo de snake
  state->board[2][4] = 'D';//cabeza de snake
  state->board[2][9] = '*';//fruta

  //Configura la posicion de la cola y cabeza de la serpiente.
  state->snakes = malloc(sizeof(snake_t));

  state->snakes[0].tail_row = 2;
  state->snakes[0].tail_col = 2;
  state->snakes[0].head_row = 2;
  state->snakes[0].head_col = 4;
  state->snakes[0].live = true;// esta instruccion le da vida o bien para que se mueva.


  return state; // estado del juego.
    
  
}


/* Tarea 2 */
void free_state(game_state_t* state) {
  for(unsigned int i = 0; i < state->num_rows; i++) {
    free(state->board[i]);
  }
  free(state->board);
  free(state->snakes);
  free(state);
  
}
/* Tarea 2 se implemento la funcion free_state para liberar la memoria .*/


/* Tarea 3 */
void print_board(game_state_t* state, FILE* fp) {
  for (unsigned int i = 0; i < state->num_rows; i++){
    fprintf(fp, "%s\n", state->board[i]);
  }
  
}

/* Ciclo para imprimir tablero del juego. */


/* Tarea 4.1 */


/**
 * Funcion de ayuda que obtiene un caracter del tablero dado una fila y columna
 * (ya implementado para ustedes).
*/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}


/**
 * Funcion de ayuda que actualiza un caracter del tablero dado una fila, columna y
 * un caracter.
 * (ya implementado para ustedes).
*/
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}


/**
 * Retorna true si la variable c es parte de la cola de una snake.
 * La cola de una snake consiste de los caracteres: "wasd"
 * Retorna false de lo contrario.
*/
static bool is_tail(char c) {
  return c == 'w' || c == 'a' || c == 's' || c == 'd';
  // TODO: Implementar esta funcion.
}


/**
 * Retorna true si la variable c es parte de la cabeza de una snake.
 * La cabeza de una snake consiste de los caracteres: "WASDx"
 * Retorna false de lo contrario.
*/
static bool is_head(char c) {
  return c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x';
  // TODO: Implementar esta funcion.
}


/**
 * Retorna true si la variable c es parte de una snake.
 * Una snake consiste de los siguientes caracteres: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  return is_tail(c) || is_head(c) || c == '^' || c == '<' || c == 'v' || c == '>';
  // TODO: Implementar esta funcion.
}


/**
 * Convierte un caracter del cuerpo de una snake ("^<v>")
 * al caracter que correspondiente de la cola de una
 * snake ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implementar esta funcion.
  switch (c){
    case '^': return 'w';
    case '<': return 'a';
    case 'v': return 's';
    case '>': return 'd';
    default: return '?';
    
  }
}


/**
 * Convierte un caracter de la cabeza de una snake ("WASD")
 * al caracter correspondiente del cuerpo de una snake
 * ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implementar esta funcion.
  switch (c){
    case 'W': return '^';
    case 'A': return '<';
    case 'S': return 'v';
    case 'D': return '>';
    default: return '?';
    
  }
}

/**
 * Retorna cur_row + 1 si la variable c es 'v', 's' o 'S'.
 * Retorna cur_row - 1 si la variable c es '^', 'w' o 'W'.
 * Retorna cur_row de lo contrario
*/

static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implementar esta funcion.
  if (c == 'v' || c == 's' || c == 'S') return cur_row + 1;
  if (c == '^' || c == 'w' || c == 'W') return cur_row - 1;
  return cur_row;
}

/**
 * Retorna cur_col + 1 si la variable c es '>' or 'd' or 'D'.
 * Retorna cur_col - 1 si la variable c es '<' or 'a' or 'A'.
 * Retorna cur_col de lo contrario
*/

static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implementar esta funcion.
  if (c == '>' || c == 'd' || c == 'D') return cur_col + 1;
  if (c == '<' || c == 'a' || c == 'A') return cur_col - 1;
  return cur_col;
}

/**
 * Tarea 4.2
 *
 * Funcion de ayuda para update_state. Retorna el caracter de la celda
 * en donde la snake se va a mover (en el siguiente paso).
 *
 * Esta funcion no deberia modificar nada de state.
*/

static char next_square(game_state_t* state, unsigned int snum) {
  // TODO: Implementar esta funcion.
  snake_t s = state->snakes[snum]; /* obtiene snake desde el estado actual del juego. */
  char head = get_board_at(state, s.head_row, s.head_col); /* se obtiene el caracter actual de la cabeza. */
  unsigned int next_r = get_next_row(s.head_row, head); /* calcula la fila del siguiente movimiento. */
  unsigned int next_c = get_next_col(s.head_col, head); /* calcula la columna del siguiente movimiento. */
  return get_board_at(state, next_r, next_c); /* retorna el caracter de la casilla */
}

/* revision de la siguiente casilla e implementacion de get_next_row y get_next_col. */


/**
 * Tarea 4.3
 *
 * Funcion de ayuda para update_state. Actualiza la cabeza de la snake...
 *
 * ... en el tablero: agregar un caracter donde la snake se va a mover (¿que caracter?)
 *
 * ... en la estructura del snake: actualizar el row y col de la cabeza
 *
 * Nota: esta funcion ignora la comida, paredes, y cuerpos de otras snakes
 * cuando se mueve la cabeza.
*/
static void update_head(game_state_t* state, unsigned int snum) {
  // TODO: Implementar esta funcion.
  snake_t* s = &state->snakes[snum]; /* se obtiene un puntero a la serpiente. */
  char head = get_board_at(state, s->head_row, s->head_col); /* se obtiene el caracter actual de la cabeza (W,A,S,D) */
  set_board_at(state, s->head_row, s->head_col, head_to_body(head)); /* Se actualiza la casilla donde estaba la cabeza, convirtiéndola en parte del cuerpo ('^', '>', '<', 'v') */
  s->head_row = get_next_row(s->head_row, head); /* Se calcula y actualiza la nueva posición de la cabeza (fila) */
  s->head_col = get_next_col(s->head_col, head); /* Se calcula y actualiza la nueva posición de la cabeza (columna) */
  set_board_at(state, s->head_row, s->head_col, head); /* se coloca el nuevo caracter en la cabeza de la snake. */
}


/**
 * Tarea 4.4
 *
 * Funcion de ayuda para update_state. Actualiza la cola de la snake...
 *
 * ... en el tablero: colocar un caracter blanco (spacio) donde se encuentra
 * la cola actualmente, y cambiar la nueva cola de un caracter de cuerpo (^<v>)
 * a un caracter de cola (wasd)
 *
 * ...en la estructura snake: actualizar el row y col de la cola
*/
static void update_tail(game_state_t* state, unsigned int snum) {
  // TODO: Implementar esta funcion.
  snake_t* s = &state->snakes[snum]; /* se obtiene un puntero a la snake */
  char tail = get_board_at(state, s->tail_row, s->tail_col); /* se obtiene el caracter de la cola.(w,a,s,d) */
  set_board_at(state, s->tail_row, s->tail_col, ' '); /* se borra el caracter de la cola. */
  s->tail_row = get_next_row(s->tail_row, tail); /* se calcula la nueva posicion de la cola con la nueva direccion.(fila) */
  s->tail_col = get_next_col(s->tail_col, tail); /* se calcula la nueva posicion de la cola con la nueva direccion.(columna) */
  char next = get_board_at(state, s->tail_row, s->tail_col); /* se obtiene caracter en la nueva posicion.(cuerpo) */
  set_board_at(state, s->tail_row, s->tail_col, body_to_tail(next)); /* se actualiza estado de la cola + 1 */
}

/* Tarea 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
  // TODO: Implementar esta funcion.
  for (unsigned int i = 0; i < state->num_snakes; i++) {
    if (!state->snakes[i].live) continue; /* revisa que las serpientes sigan vivas. */
    char next = next_square(state, i);
    if (next == '#' || is_snake(next)) { /* confirma el choque contra un muro u otra serpiente. */
      set_board_at(state, state->snakes[i].head_row, state->snakes[i].head_col, 'x');
      state->snakes[i].live = false;
    } else {
      update_head(state, i);
      if (next != '*') { /* revisa si hay o no una fruta al frente. */
        update_tail(state, i);
      } else {
        add_food(state);
      }
    }
  }
}

/* Tarea 5 */
game_state_t* load_board(char* filename) { 
  // TODO: Implementar esta funcion.
  FILE* f = fopen(filename, "r");
  if (f == NULL) { /* revisa si el archivo existe o devuelve NULL si no. */
    return NULL;
  }

  game_state_t* state = malloc(sizeof(game_state_t)); /* reserva la memoria para el estado del juego. */
  state->num_rows = 0;
  state->board = NULL; /* el tablero se contruira de forma dinamica. */
  state->snakes = NULL; /* la snake inicia nula. */
  state->num_snakes = 0; 
  /* inicia los campos de los estados. */


  char buffer[1024]; /* leera linea por linea del archivo */
  while (fgets(buffer, sizeof(buffer), f)) {
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
      buffer[len - 1] = '\0';
      len--;
    }
    state->board = realloc(state->board, (state->num_rows + 1) * sizeof(char*)); /* redimensionamos el array de las filas para agregar una nueva. */
    state->board[state->num_rows] = malloc((len + 1) * sizeof(char)); /* reserva de espacio para copiar linea completa. */
    strcpy(state->board[state->num_rows], buffer);
    state->num_rows++;
  }

  fclose(f); /* cerramos archivo. */
  return state;
}


/**
 * Tarea 6.1
 *
 * Funcion de ayuda para initialize_snakes.
 * Dada una structura de snake con los datos de cola row y col ya colocados,
 * atravezar el tablero para encontrar el row y col de la cabeza de la snake,
 * y colocar esta informacion en la estructura de la snake correspondiente
 * dada por la variable (snum)
*/
static void find_head(game_state_t* state, unsigned int snum) { /* actualiza la estructura de la snake por medio de head_row y head_col. */
  // TODO: Implementar esta funcion.
  snake_t* s = &state->snakes[snum];
  unsigned int r = s->tail_row; /* comenzamos desde la posicion actual de la cola. */
  unsigned int c = s->tail_col;

  while (true) { /* recorremos el cuerpo de la serpiente hasta encontrar la cabeza. */
    char ch = get_board_at(state, r, c);
    unsigned int next_r = get_next_row(r, ch);
    unsigned int next_c = get_next_col(c, ch);
    char next_ch = get_board_at(state, next_r, next_c);
    if (is_head(next_ch)) {
      s->head_row = next_r;
      s->head_col = next_c;
      return;
    }
    r = next_r;
    c = next_c;
  }
}

/* Tarea 6.2 */
game_state_t* initialize_snakes(game_state_t* state) { /* se reserva memoria para todas las snakes. */
  // TODO: Implementar esta funcion.
  unsigned int count = 0;

  for (unsigned int i = 0; i < state->num_rows; i++) { /* se recorre el tablero. */
    for (unsigned int j = 0; state->board[i][j] != '\0'; j++) {
      if (is_tail(state->board[i][j])) {
        count++;
      }
    }
  }

  state->num_snakes = count;
  state->snakes = malloc(count * sizeof(snake_t));

  unsigned int snum = 0;
  for (unsigned int i = 0; i < state->num_rows; i++) { /* se recorre el tablero nuevamente para guardar las nuevas snakes y sus movimientos. */
    for (unsigned int j = 0; state->board[i][j] != '\0'; j++) {
      if (is_tail(state->board[i][j])) { /* si se apunta a una cola se inicializa la snake. */
        state->snakes[snum].tail_row = i;
        state->snakes[snum].tail_col = j;
        state->snakes[snum].live = true;
        find_head(state, snum);
        snum++;
      }
    }
  }

  return state; /* retorna el estado actual del tablero con la snakes creadas. */
}

/**
 * Guarda el estado actual a un archivo. No modifica el objeto/struct state.
 * (ya implementada para que la utilicen)
*/


void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}