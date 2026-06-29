#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <time.h>

#define NUMERO_OPERACIONES 8
// Para hacer DP Pura, necesitamos un universo finito. Limitamos el acumulador máximo.
#define MAX_ESTADO 1000000 
#define INF 999999
#define MAX_DEPTH 15

typedef enum { 
    OP_SUM, OP_RES, OP_MUL, OP_DIV, 
    OP_MOD, OP_POW, OP_ROOT, OP_LOG, 
    OP_INVALIDA
} TipoOperacion;

typedef struct {
    int value;
    int is_valid;
} MathResult;

// Estructura para reconstruir el camino en la tabla DP
typedef struct {
    int prev_val;
    TipoOperacion op;
    int operand;
} PathTracker;

int compare_desc(const void *a, const void *b) {
    return (*(int*)b - *(int*)a);
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
        case OP_SUM: return "+"; case OP_RES: return "-";
        case OP_MUL: return "*"; case OP_DIV: return "/";
        case OP_MOD: return "%"; case OP_POW: return "^";
        case OP_ROOT: return "root"; case OP_LOG: return "log";
        default: return "?";
    }
}

// --- CORE: PROGRAMACIÓN DINÁMICA PURA (TABULACIÓN) ---

void programacion_dinamica_pura(int array[], int size, int goal) {
    if (goal < 0 || goal >= MAX_ESTADO) {
        printf("[!] Error: El objetivo supera el MAX_ESTADO definido para la tabla DP.\n");
        return;
    }

    printf("\n--- INICIANDO DP PURA (TABULACION BOTTOM-UP) ---\n");
    printf("Objetivo (B): %d | Estado Inicial: 0\n", goal);
    printf("Universo acotado a: %d\n", MAX_ESTADO);
    printf("========================================\n");

    // 1. Crear la Tabla DP: dp[i] = "mínimos pasos para llegar al número i"
    int *dp = (int *)malloc(MAX_ESTADO * sizeof(int));
    PathTracker *tracker = (PathTracker *)malloc(MAX_ESTADO * sizeof(PathTracker));

    if (!dp || !tracker) {
        printf("Error de memoria al crear la tabla DP.\n");
        if (dp) free(dp);
        if (tracker) free(tracker);
        return;
    }

    // 2. Inicializar casos base
    for (int i = 0; i < MAX_ESTADO; i++) {
        dp[i] = INF;
    }
    dp[0] = 0; // Se necesitan 0 pasos para llegar a 0

    int solucion_encontrada = 0;

    // 3. Tabulación: Iteramos construyendo las soluciones óptimas paso a paso
    for (int step = 1; step <= MAX_DEPTH; step++) {
        int hubo_cambios = 0;

        // Recorremos todo el espacio de estados
        for (int val = 0; val < MAX_ESTADO; val++) {
            
            // Solo intentamos avanzar desde estados que fueron alcanzados en el paso anterior
            if (dp[val] == step - 1) {
                
                for (int k = 0; k < size; k++) {
                    for (int op = 0; op < NUMERO_OPERACIONES; op++) {
                        
                        MathResult res = calcular_operacion(val, array[k], (TipoOperacion)op);

                        // Si el resultado es válido y cae dentro de nuestro universo DP
                        if (res.is_valid && res.value >= 0 && res.value < MAX_ESTADO) {
                            
                            // RELAJACIÓN DP: Si encontramos un camino más corto al estado
                            if (dp[val] + 1 < dp[res.value]) {
                                dp[res.value] = dp[val] + 1;
                                
                                // Guardamos el rastro para reconstruir el camino luego
                                tracker[res.value].prev_val = val;
                                tracker[res.value].op = (TipoOperacion)op;
                                tracker[res.value].operand = array[k];
                                hubo_cambios = 1;
                            }
                        }
                    }
                }
            }
        }

        // Si ya alcanzamos el objetivo con esta cantidad de pasos, paramos
        if (dp[goal] != INF) {
            solucion_encontrada = 1;
            break;
        }

        // Si iteramos toda la tabla y no hubo ningún cambio, el algoritmo convergió
        if (!hubo_cambios) break;
    }

    // 4. Reconstrucción de la solución óptima
    if (solucion_encontrada) {
        int pasos_totales = dp[goal];
        printf(" EXITO: Solucion optima encontrada en %d pasos\n", pasos_totales);
        printf("========================================\n");

        int *path_values = (int *)malloc((pasos_totales + 1) * sizeof(int));
        TipoOperacion *path_ops = (TipoOperacion *)malloc((pasos_totales + 1) * sizeof(TipoOperacion));
        int *path_operands = (int *)malloc((pasos_totales + 1) * sizeof(int));

        int curr_val = goal;
        for (int i = pasos_totales - 1; i >= 0; i--) {
            path_values[i] = curr_val;
            path_ops[i] = tracker[curr_val].op;
            path_operands[i] = tracker[curr_val].operand;
            curr_val = tracker[curr_val].prev_val;
        }

        int ac_display = 0;
        for (int i = 0; i < pasos_totales; i++) {
            printf("[Paso %2d] Acumulador actual: %-6d | Operacion: %-4s %-4d | Resultado: %d\n", 
                   i + 1, 
                   ac_display, 
                   get_op_symbol(path_ops[i]), 
                   path_operands[i], 
                   path_values[i]);
            ac_display = path_values[i];
        }
        printf("========================================\n\n");

        free(path_values);
        free(path_ops);
        free(path_operands);
    } else {
        printf("\n[!] No se encontro ninguna solucion en %d pasos o menos dentro del límite MAX_ESTADO.\n\n", MAX_DEPTH);
    }

    free(dp);
    free(tracker);
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

    programacion_dinamica_pura(array, array_size, goal);

    free(array);
    return 0;
}