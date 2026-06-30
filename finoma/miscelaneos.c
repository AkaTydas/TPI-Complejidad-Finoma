#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include "finoma.h"


// Función para ordenar el arreglo inicial
int compare_desc(const void *a, const void *b) {
    return (*(int*)b - *(int*)a);
}

// Función para ordenar los movimientos por distancia al objetivo (Heurística)
int compare_moves(const void *a, const void *b) {
    Move *moveA = (Move *)a;
    Move *moveB = (Move *)b;
    return moveA->distance - moveB->distance;
}


MathResult calcular_operacion(int a, int b, TipoOperacion operacion) {
    MathResult result = {0, 0}; 
    switch (operacion) {
        case OP_SUM: result.value = a + b; result.is_valid = 1; break;
        case OP_RES: result.value = a - b; result.is_valid = 1; break;
        case OP_MUL: 
            // Prevenir overflow antes de operar
            if (a != 0 && b > INT_MAX / a) { result.is_valid = 0; break; }
            result.value = a * b; result.is_valid = 1; break;
        case OP_DIV:
            if (b != 0) { result.value = a / b; result.is_valid = 1; } break;
        case OP_MOD:
            if (b != 0) { result.value = a % b; result.is_valid = 1; } break;
        case OP_POW: {
            double temporal = pow(a, b);
            if (temporal > MAX_ESTADO || temporal < 0) {
                result.is_valid = 0;
            } else {
                result.value = (int)temporal; result.is_valid = 1;
            } break;
        }
        case OP_ROOT:
            if (b != 0 && a >= 0) {
                result.value = (int)pow(a, 1.0 / b); result.is_valid = 1;
            } break;
        case OP_LOG:
            if (a > 0 && b > 0 && b != 1) {
                result.value = (int)(log((double)a) / log((double)b)); result.is_valid = 1;
            } break;
        default: result.is_valid = 0; break; 
    }
    return result;
}

const char* get_op_symbol(TipoOperacion op) {
    switch (op) {
        case OP_SUM: return "+";
        case OP_RES: return "-";
        case OP_MUL: return "*";
        case OP_DIV: return "/";
        case OP_MOD: return "%";
        case OP_POW: return "^";
        case OP_ROOT: return "root";
        case OP_LOG: return "log";
        default: return "?";
    }
}

bool is_duplicate(int* arr, int size, int val) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == val) return true;
    }
    return false;
}