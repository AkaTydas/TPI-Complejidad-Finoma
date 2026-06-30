#include <stdio.h>
#include "finoma.h"

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

    printf("\n~ BACKTRACKING ~ ");
    printf("Buscando la ruta optima en el espacio de estados...");
    
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