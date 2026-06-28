#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <time.h>

#define NUMERO_OPERACIONES 8

typedef enum { 
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
        case OP_SUM:
            result.value = a + b;
            result.is_valid = 1;
            break;
        case OP_RES:
            result.value = a - b;
            result.is_valid = 1;
            break;
        case OP_MUL:
            result.value = a * b;
            result.is_valid = 1;
            break;
        case OP_DIV:
            if (b != 0) {
                result.value = a / b;
                result.is_valid = 1;
            }
            break;
        case OP_MOD:
            if (b != 0) {
                result.value = a % b;
                result.is_valid = 1;
            }
            break;
        case OP_POW: {
            double temporal = pow(a, b);
            if (temporal > INT_MAX || temporal < INT_MIN) {
                result.is_valid = 0;
            } else {
                result.value = (int)temporal;
                result.is_valid = 1;
            } 
            break;
        }
        case OP_ROOT:
            if (b != 0 && a >= 0) {
                result.value = (int)pow(a, 1.0 / b);
                result.is_valid = 1;
            }
            break;
        case OP_LOG:
            if (a > 0 && b > 0 && b != 1) {
                result.value = (int)(log((double)a) / log((double)b));
                result.is_valid = 1;
            }
            break;
        default:
            result.is_valid = 0;
            break; 
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

// --- CORE DEL ALGORITMO OPTIMIZADO ---

void construir_arbol_implicito(int array[], int size, int goal, int nivel, int acumulador, int *mejor_solucion, Step *camino_actual, Step *mejor_camino) {
    
    if (nivel >= *mejor_solucion) {
        return; 
    }

    if (acumulador == goal) {
        *mejor_solucion = nivel;
        for (int i = 0; i < nivel; i++) {
            mejor_camino[i] = camino_actual[i];
        }
        return;
    }

    if (nivel == 14) return;

    Move moves[size * NUMERO_OPERACIONES];
    int valid_moves_count = 0;

    for (int k = 0; k < size; k++) {
        for (int i = 0; i < NUMERO_OPERACIONES; i++) {
            
            MathResult resultado = calcular_operacion(acumulador, array[k], (TipoOperacion)i);

            if (resultado.is_valid && resultado.value != acumulador) {
                moves[valid_moves_count].operand = array[k];
                moves[valid_moves_count].op = (TipoOperacion)i;
                moves[valid_moves_count].result_value = resultado.value;
                moves[valid_moves_count].distance = abs(goal - resultado.value);
                
                valid_moves_count++;
            }
        }
    }

    qsort(moves, valid_moves_count, sizeof(Move), compare_moves);

    for (int m = 0; m < valid_moves_count; m++) {
        camino_actual[nivel].operand = moves[m].operand;
        camino_actual[nivel].op = moves[m].op;
        
        construir_arbol_implicito(array, size, goal, nivel + 1, moves[m].result_value, mejor_solucion, camino_actual, mejor_camino);
    }
}

void backtracking(int array[], int size, int goal) {
    int max_depth = 15; 
    int mejor_solucion = max_depth;
    
    Step *camino_actual = (Step *)calloc(max_depth, sizeof(Step));
    Step *mejor_camino = (Step *)calloc(max_depth, sizeof(Step));

    printf("\nBuscando la ruta optima en el espacio de estados...\n");
    
    construir_arbol_implicito(array, size, goal, 0, 0, &mejor_solucion, camino_actual, mejor_camino);

    if (mejor_solucion < max_depth) {
        printf("\n========================================\n");
        printf(" EXITO: Solucion encontrada en %d pasos\n", mejor_solucion);
        printf("========================================\n");
        
        int ac_display = 0; 
        for (int i = 0; i < mejor_solucion; i++) {
            MathResult temp = calcular_operacion(ac_display, mejor_camino[i].operand, mejor_camino[i].op);
            
            printf("[Paso %2d] Acumulador actual: %-6d | Operacion: %-4s %-4d | Resultado: %d\n", 
                   i + 1, 
                   ac_display, 
                   get_op_symbol(mejor_camino[i].op), 
                   mejor_camino[i].operand, 
                   temp.value);
                   
            ac_display = temp.value;
        }
        printf("========================================\n\n");
    } else {
        printf("\n[!] No se encontro ninguna solucion en %d pasos o menos.\n\n", max_depth - 1);
    }

    free(camino_actual);
    free(mejor_camino);
}

int main(void) {
    int *array;
    int array_size;
    int goal;

    srand(time(NULL));

    printf("Tamanio del arreglo: ");
    if (scanf("%d", &array_size) != 1) return 1;

    array = (int *)calloc(array_size, sizeof(int));

    int elementos_generados = 0;
    while (elementos_generados < array_size) {
        int num_aleatorio = (rand() % 100) + 1; 
        
        int repetido = 0;
        for (int j = 0; j < elementos_generados; j++) {
            if (array[j] == num_aleatorio) {
                repetido = 1;
                break;
            }
        }

        if (!repetido) {
            array[elementos_generados] = num_aleatorio;
            elementos_generados++;
        }
    }

    qsort(array, array_size, sizeof(int), compare_desc);

    printf("\nArreglo generado (Ordenado Descendente):\n[ ");
    for (int i = 0; i < array_size; i++) {
        printf("%d ", array[i]);
    }
    printf("]\n\n");

    printf("Ingrese el numero al que quiere llegar (B): ");
    if (scanf("%d", &goal) != 1) return 1;

    backtracking(array, array_size, goal);

    free(array);
    return 0;
}