#include <stdio.h>
#include "finoma.h"

void voraz(int array[], int size, int goal) {
    int ac = 0; 
    int distancia_anterior = abs(ac - goal);
    int pasos_totales = 0;
    
    printf("\n--- INICIANDO ALGORITMO VORAZ ---\n");
    printf("Objetivo (B): %d | Estado Inicial: %d\n", goal, ac);
    printf("========================================\n");

    while (ac != goal) {
        int menor_distancia = INT_MAX;
        TipoOperacion mejor_op = OP_INVALIDA;
        int mejor_elemento = 0;

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < NUMERO_OPERACIONES; j++) {
                MathResult res = calcular_operacion(ac, array[i], (TipoOperacion)j);
                
                // Poda de redundancia para el Voraz: no hacer operaciones que no muten el estado
                if (res.is_valid && res.value != ac) {
                    int distancia_actual = abs(res.value - goal);

                    if (distancia_actual < menor_distancia) {
                        menor_distancia = distancia_actual;
                        mejor_op = (TipoOperacion)j;
                        mejor_elemento = array[i];
                    }
                }
            }
        }

        // Condición de falla: Si el Voraz no puede achicar la distancia, se traba.
        if (menor_distancia >= distancia_anterior || mejor_op == OP_INVALIDA) {
            printf("\n[!] FALLO CRITICO: El algoritmo Voraz ha caido en un optimo local.\n");
            printf("    No existen operaciones validas que reduzcan la distancia actual (%d).\n", distancia_anterior);
            printf("    El algoritmo no puede hacer Backtracking. Abortando...\n");
            printf("========================================\n\n");
            return;
        }

        // Aplicamos el mejor movimiento encontrado
        MathResult final_res = calcular_operacion(ac, mejor_elemento, mejor_op);
        int prev_ac = ac;
        ac = final_res.value;
        distancia_anterior = menor_distancia;
        pasos_totales++;
        
        printf("[Paso %2d] Acumulador actual: %-6d | Operacion: %-4s %-4d | Resultado: %d\n", 
               pasos_totales, 
               prev_ac, 
               get_op_symbol(mejor_op), 
               mejor_elemento, 
               ac);
    }

    printf("========================================\n");
    printf(" EXITO: Solucion encontrada en %d pasos\n", pasos_totales);
    printf("========================================\n\n");
}