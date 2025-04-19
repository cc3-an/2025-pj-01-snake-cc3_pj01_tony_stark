#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

// Definiciones de funciones de ayuda.
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
      if (i == 0 || i == 17 || j == 0 || j== 19){
        state->board[i][j] = '#';
        }else{
        
          state->board[i][j] = ' ';//Espacios vacios en el interior.
      }
    }
    //Esto agrega el caracter nulo al final de cada fila.
    state->board[i][20] = '\0';
  }


  //esta sera la poicion inicial que tendra la fruta y la snake.
  state->board[2][2] = 'd';//cola de snake
  state->board[2][3] = '>';//cuerpo de snake
  state->board[2][4] = 'D';//cabeza de snake
  state->board[2][9] = '*';//fruta

  //Configura la posicion de la cola y cabeza de la serpiente.
  state->snakes = malloc(state->num_snakes * sizeof(snake_t));

  state->snakes[0].tail_row = 2;
  state->snakes[0].tail_col = 2;
  state->snakes[0].head_row = 2;
  state->snakes[0].head_col = 4;
  state->snakes[0].live = true;// esta instruccion le da vida o bien para que se mueva.


  return state; // estado del juego.
    
  
}


/* Tarea 2 */
void free_state(game_state_t* state) {
  // TODO: Implementar esta funcion.
  //if (state == NULL) return;
  for(unsigned int i = 0; i < state->num_rows; i++) {
    free(state->board[i]);
  }
  free(state->board);
  free(state->snakes);
  free(state);
  
}


/* Tarea 3 */
void print_board(game_state_t* state, FILE* fp) {
  // TODO: Implementar esta funcion.
  for (unsigned int i = 0; i < state->num_rows; i++){
    fprintf(fp, "%s\n", state->board[i]);
  }
  
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
  //return true;
}


/**
 * Retorna true si la variable c es parte de la cabeza de una snake.
 * La cabeza de una snake consiste de los caracteres: "WASDx"
 * Retorna false de lo contrario.
*/
static bool is_head(char c) {
  return c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x';
  // TODO: Implementar esta funcion.
  //return true;
}


/**
 * Retorna true si la variable c es parte de una snake.
 * Una snake consiste de los siguientes caracteres: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  return is_tail(c) || is_head(c) || c == '^' || c == '<' || c == 'v' || c == '>';
  // TODO: Implementar esta funcion.
  //return true;
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
  snake_t s = state->snakes[snum];
  char head = get_board_at(state, s.head_row, s.head_col);
  unsigned int next_r = get_next_row(s.head_row, head);
  unsigned int next_c = get_next_col(s.head_col, head);
  return get_board_at(state, next_r, next_c);
}


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
  snake_t* s = &state->snakes[snum];
  char head = get_board_at(state, s->head_row, s->head_col);
  set_board_at(state, s->head_row, s->head_col, head_to_body(head));
  s->head_row = get_next_row(s->head_row, head);
  s->head_col = get_next_col(s->head_col, head);
  set_board_at(state, s->head_row, s->head_col, head);
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
  snake_t* s = &state->snakes[snum];
  char tail = get_board_at(state, s->tail_row, s->tail_col);
  set_board_at(state, s->tail_row, s->tail_col, ' ');
  s->tail_row = get_next_row(s->tail_row, tail);
  s->tail_col = get_next_col(s->tail_col, tail);
  char next = get_board_at(state, s->tail_row, s->tail_col);
  set_board_at(state, s->tail_row, s->tail_col, body_to_tail(next));
}

/* Tarea 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
  // TODO: Implementar esta funcion.
  for (unsigned int i = 0; i < state->num_snakes; i++) {
    if (!state->snakes[i].live) continue;
    char next = next_square(state, i);
    if (next == '#' || is_snake(next)) {
      set_board_at(state, state->snakes[i].head_row, state->snakes[i].head_col, 'x');
      state->snakes[i].live = false;
    } else {
      update_head(state, i);
      if (next != '*') {
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
  if (f == NULL) {
    return NULL;
  }

  game_state_t* state = malloc(sizeof(game_state_t));
  state->num_rows = 0;
  state->board = NULL;
  state->snakes = NULL;
  state->num_snakes = 0;

  char buffer[1024];
  while (fgets(buffer, sizeof(buffer), f)) {
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
      buffer[len - 1] = '\0';
      len--;
    }
    state->board = realloc(state->board, (state->num_rows + 1) * sizeof(char*));
    state->board[state->num_rows] = malloc((len + 1) * sizeof(char));
    strcpy(state->board[state->num_rows], buffer);
    state->num_rows++;
  }

  fclose(f);
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
static void find_head(game_state_t* state, unsigned int snum) {
  // TODO: Implementar esta funcion.
  snake_t* s = &state->snakes[snum];
  unsigned int r = s->tail_row;
  unsigned int c = s->tail_col;

  while (true) {
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
game_state_t* initialize_snakes(game_state_t* state) {
  // TODO: Implementar esta funcion.
  unsigned int count = 0;

  for (unsigned int i = 0; i < state->num_rows; i++) {
    for (unsigned int j = 0; state->board[i][j] != '\0'; j++) {
      if (is_tail(state->board[i][j])) {
        count++;
      }
    }
  }

  state->num_snakes = count;
  state->snakes = malloc(count * sizeof(snake_t));

  unsigned int snum = 0;
  for (unsigned int i = 0; i < state->num_rows; i++) {
    for (unsigned int j = 0; state->board[i][j] != '\0'; j++) {
      if (is_tail(state->board[i][j])) {
        state->snakes[snum].tail_row = i;
        state->snakes[snum].tail_col = j;
        state->snakes[snum].live = true;
        find_head(state, snum);
        snum++;
      }
    }
  }

  return state;
}


