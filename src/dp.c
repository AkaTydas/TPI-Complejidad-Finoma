#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <stdint.h>
#include <time.h>

#define NUMERO_OPERACIONES 8
#define MAX_ESTADO 1000000 
#define MAX_DEPTH 15

typedef enum { 
    OP_SUM, OP_RES, OP_MUL, OP_DIV, 
    OP_MOD, OP_POW, OP_ROOT, OP_LOG 
} TipoOperacion;

typedef struct {
    int value;
    int is_valid;
} MathResult;

// Stores the metadata for backtracking
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

// --- CORE: PURE DYNAMIC PROGRAMMING (2D TABULATION) ---

void programacion_dinamica_pura(int array[], int size, int goal) {
    if (goal < 0 || goal >= MAX_ESTADO) {
        printf("[!] Error: Objetivo fuera de limites.\n");
        return;
    }

    printf("\n--- INICIANDO DP PURA (TABULACION 2D) ---\n");
    
    // 1. Allocation of 2D arrays on the heap (to avoid stack overflow)
    // dp[step][value] indicates if 'value' is reachable exactly at 'step'
    int8_t **dp = (int8_t **)malloc((MAX_DEPTH + 1) * sizeof(int8_t *));
    PathTracker **tracker = (PathTracker **)malloc((MAX_DEPTH + 1) * sizeof(PathTracker *));

    for (int i = 0; i <= MAX_DEPTH; i++) {
        dp[i] = (int8_t *)calloc(MAX_ESTADO, sizeof(int8_t));
        tracker[i] = (PathTracker *)malloc(MAX_ESTADO * sizeof(PathTracker));
    }

    // 2. Base Case Initialization
    dp[0][0] = 1; // It is possible to reach value 0 at step 0.

    int iteraciones_totales = 0;
    int target_step = -1;

    // 3. Tabulation Process (Bottom-Up)
    for (int s = 1; s <= MAX_DEPTH; s++) {
        
        // Scan the entire universe of values from the PREVIOUS step
        for (int u = 0; u < MAX_ESTADO; u++) {
            
            // If the value 'u' was not reachable in step 's-1', skip it
            if (!dp[s - 1][u]) continue;

            // Apply all operations with all array elements
            for (int k = 0; k < size; k++) {
                for (int op = 0; op < NUMERO_OPERACIONES; op++) {
                    
                    iteraciones_totales++; // Tracking computational cost

                    MathResult res = calcular_operacion(u, array[k], (TipoOperacion)op);

                    if (res.is_valid && res.value >= 0 && res.value < MAX_ESTADO) {
                        
                        // If we haven't already reached this value at THIS step
                        if (!dp[s][res.value]) {
                            dp[s][res.value] = 1;
                            
                            // Save backtracking metadata explicitly for this step
                            tracker[s][res.value].prev_val = u;
                            tracker[s][res.value].op = (TipoOperacion)op;
                            tracker[s][res.value].operand = array[k];

                            // Early exit if goal is found
                            if (res.value == goal) {
                                target_step = s;
                                goto print_solution; // Break out of all nested loops
                            }
                        }
                    }
                }
            }
        }
    }

print_solution:
    if (target_step != -1) {
        printf(" EXITO: Solucion optima encontrada en %d pasos\n", target_step);
        printf(" Bucles ejecutados (Costo O(S*V*K)): %d\n", iteraciones_totales);
        printf("========================================\n");

        int *path_values = (int *)malloc((target_step + 1) * sizeof(int));
        TipoOperacion *path_ops = (TipoOperacion *)malloc((target_step + 1) * sizeof(TipoOperacion));
        int *path_operands = (int *)malloc((target_step + 1) * sizeof(int));

        // 4. Backtracking through the 2D Tracker Matrix
        int current_val = goal;
        for (int s = target_step; s > 0; s--) {
            path_values[s-1] = current_val;
            path_ops[s-1] = tracker[s][current_val].op;
            path_operands[s-1] = tracker[s][current_val].operand;
            
            // Move up one step in the matrix to the previous value
            current_val = tracker[s][current_val].prev_val; 
        }

        int ac_display = 0;
        for (int i = 0; i < target_step; i++) {
            printf("[Paso %2d] Acumulador: %-6d | Operacion: %-4s %-4d | Resultado: %d\n", 
                   i + 1, ac_display, get_op_symbol(path_ops[i]), path_operands[i], path_values[i]);
            ac_display = path_values[i];
        }
        
        free(path_values); free(path_ops); free(path_operands);
    } else {
        printf("\n[!] Objetivo inalcanzable en %d pasos.\n", MAX_DEPTH);
    }

    // 5. Cleanup 2D Memory
    for (int i = 0; i <= MAX_DEPTH; i++) {
        free(dp[i]);
        free(tracker[i]);
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

    int rango_maximo = (array_size >= 10) ? array_size * 5 : 50; 

    int elems = 0;
    while (elems < array_size) {
        int num = (rand() % rango_maximo) + 1; 
        
        int repetido = 0;
        for (int j = 0; j < elems; j++) {
            if (array[j] == num) { 
                repetido = 1; 
                break; 
            }
        }
        
        if (!repetido) { 
            array[elems++] = num; 
        }
    }

    qsort(array, array_size, sizeof(int), compare_desc);

    printf("\nArreglo generado:\n[ ");
    for (int i = 0; i < array_size; i++) printf("%d ", array[i]);
    printf("]\n\n");

    printf("Ingrese el numero objetivo (B): ");
    if (scanf("%d", &goal) != 1) return 1;

    programacion_dinamica_pura(array, array_size, goal);

    free(array);
    return 0;
}