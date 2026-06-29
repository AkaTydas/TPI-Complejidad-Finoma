#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <stdbool.h>

#define NUMERO_OPERACIONES 8

typedef enum{ 
    OP_SUM,
    OP_RES,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_POW,
    OP_ROOT,
    OP_LOG,
    OP_INVALIDA
} TipoOperacion;

typedef struct {
    int value;
    int is_valid;
} MathResult;

// Estructura para recordar las operaciones del camino
typedef struct {
    int operand;
    TipoOperacion op;
} Step;

// Estructura para evaluar y ordenar los siguientes movimientos (Heurística)
typedef struct {
    int operand;
    TipoOperacion op;
    int result_value;
    int distance;
} Move;

const char* get_op_symbol(TipoOperacion op);
int compare_desc(const void *a, const void *b);
int compare_moves(const void *a, const void *b);
MathResult calcular_operacion(int a, int b, TipoOperacion operacion);
void voraz(int array[], int size, int goal); // algoritmo greedy
void backtracking(int array[], int size, int goal); // algoritmo backtracking
void solve(int* A, int n, int B); // algoritmo programacion dinamica
void limpiar_pd(void);
bool is_duplicate(int* arr, int size, int val);

//void imprimir_paso(int numero_paso, int ac_previo, TipoOperacion op, int operando, int ac_nuevo);