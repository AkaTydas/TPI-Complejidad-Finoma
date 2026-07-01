#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <stdint.h>
#include <time.h>
#include "finoma.h"


// Stores the metadata for backtracking
typedef struct {
    int prev_val;
    TipoOperacion op;
    int operand;
} PathTracker;

int8_t **dp = NULL; 
PathTracker **tracker;

// --- CORE: PURE DYNAMIC PROGRAMMING (2D TABULATION) ---

void programacion_dinamica_pura(int array[], int size, int goal) {
    if (goal < 0 || goal >= MAX_ESTADO) {
        printf("[!] Error: Objetivo fuera de limites.\n");
        return;
    }

    printf("\n--- INICIANDO DP PURA (TABULACION 2D) ---\n");
    
    // 1. Allocation of 2D arrays on the heap (to avoid stack overflow)
    // dp[step][value] indicates if 'value' is reachable exactly at 'step'
    dp = (int8_t **)malloc((MAX_DEPTH + 1) * sizeof(int8_t *));
    tracker = (PathTracker **)malloc((MAX_DEPTH + 1) * sizeof(PathTracker *));

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
        printf("========================================\n");
        
        free(path_values); free(path_ops); free(path_operands);
    } else {
        printf("\n[!] Objetivo inalcanzable en %d pasos.\n", MAX_DEPTH);
        printf("========================================\n");
    }

    // 5. Cleanup 2D Memory
    for (int i = 0; i <= MAX_DEPTH; i++) {
        free(dp[i]);
        free(tracker[i]);
    }
    free(dp);
    free(tracker);
}

void limpiar_pd(){
    if (dp != NULL) {
        free(dp);
        dp = NULL;
    }
    if (tracker != NULL) {
        free(tracker);
        tracker = NULL;
    }
}