#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <time.h> 

#define NUMERO_OPERACIONES 7

typedef enum{ 
    OP_SUM,
    OP_RES,
    OP_MUL,
    OP_DIV,
    OP_POW,
    OP_ROOT,
    OP_LOG,
    OP_INVALIDA
} TipoOperacion;

typedef struct {
    int value;
    int is_valid;
} MathResult;

int compare_desc(const void *a, const void *b) {
    return (*(int*)b - *(int*)a);
}

MathResult calcular_operacion(int a, int b, TipoOperacion operacion)
{
    MathResult result = {0, 0}; 

    switch (operacion)
    {
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
        case OP_POW:
            result.value = pow(a, b);
            result.is_valid = 1;
            break;
        case OP_ROOT:
            if (b != 0 && a >= 0) {
                result.value = pow(a, 1.0 / b);
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
            printf("operacion invalida");
            break; 
    }
    return result;
}

void voraz(int array[], int size, int goal)
{
    int ac = 0; 
    int distancia_anterior = abs(ac - goal);
    int pasos_totales = 0;
    
    int estancamientos_consecutivos = 0; 
    
    // We introduce a 'forced offset' to avoid checking the same array element 
    // immediately after using a joker if it was causing the loop.
    // However, in a pure greedy, the 'for' loop checks ALL elements every time.
    // The issue you noted is that if the Greedy logic ALWAYS picks the exact same 
    // 'mejor_elemento' and 'mejor_op' even after a +1/-1 nudge, it will stall again.
    // By forcing a different base for the joker or temporarily blacklisting an element,
    // we can break the cycle.
    
    int blacklisted_element_index = -1;

    while (ac != goal)
    {
        int menor_distancia = INT_MAX;
        TipoOperacion mejor_op = OP_INVALIDA;
        int mejor_elemento = 0;
        int comodin_usado = 0;

        for (int i=0; i<size; i++){
            // Skip the blacklisted element for this iteration if there is one
            if (i == blacklisted_element_index) continue;
            
            for (int j=0; j<NUMERO_OPERACIONES; j++){
                MathResult res = calcular_operacion(ac, array[i], j);
                
                if (res.is_valid) {
                    int distancia_actual = abs(res.value - goal);

                    if (distancia_actual < menor_distancia)
                    {
                        menor_distancia = distancia_actual;
                        mejor_op = (TipoOperacion)j;
                        mejor_elemento = array[i];
                        // We track which index gave us the "best" so far
                        // just in case we need to blacklist it later
                    }
                }
            }
        }

        if (menor_distancia >= distancia_anterior || mejor_op == OP_INVALIDA)
        {
            estancamientos_consecutivos++;
            
            if (estancamientos_consecutivos >= 2) {
                printf("\n--- [FALLBACK ENGAGED] Optimo local detectado. ---\n");
                
                int joker_base = -1;
                // We pick a joker base
                for(int k = size - 1; k >= 0; k--) {
                    if (array[k] > 0 && array[k] != 1) {
                        joker_base = array[k];
                        break;
                    }
                }
                
                if (joker_base != -1) {
                    printf(">>> Forzando comodin: Creando un '1' usando log_%d(%d)\n", joker_base, joker_base);
                    
                    if (ac < goal) {
                        ac += 1; 
                        printf(">>> Sumando el comodin al acumulador.\n");
                    } else if (ac > goal) {
                        ac -= 1; 
                        printf(">>> Restando el comodin al acumulador.\n");
                    }
                    
                    pasos_totales += 2; 
                    distancia_anterior = abs(ac - goal);
                    estancamientos_consecutivos = 0; 
                    comodin_usado = 1;
                    
                    // The core fix you mentioned: 
                    // To prevent the exact same evaluation loop on the next iteration,
                    // we could 'blacklist' the element that led us to this stall for ONE cycle,
                    // forcing the Greedy algorithm to pick the second-best option and explore a new branch.
                    // However, in this specific Greedy implementation, the loop always starts from index 0
                    // and checks ALL elements. If we just nudged the accumulator by +/- 1, the landscape 
                    // has mathematically changed, so the 'best' option MIGHT naturally be different.
                    // But to guarantee we don't fall into the exact same oscillation, we can temporarily 
                    // ignore the element that caused the stall.
                    
                    // Find the index of 'mejor_elemento' to blacklist it next turn
                    for (int idx = 0; idx < size; idx++) {
                        if (array[idx] == mejor_elemento) {
                            blacklisted_element_index = idx;
                            break;
                        }
                    }
                    printf(">>> Blacklisting temporalmente el elemento %d para forzar una nueva ruta.\n", mejor_elemento);
                    
                } else {
                    printf("CRITICAL FAILURE: Imposible crear el comodin con este arreglo.\n");
                    return;
                }
            } else {
                printf("[Warning] Rebote detectado. Intentando una vez mas...\n");
            }
        } else {
             // If we found a genuinely better path without stalling, clear any blacklist
             blacklisted_element_index = -1;
        }

        if (!comodin_usado) {
            MathResult final_res = calcular_operacion(ac, mejor_elemento, mejor_op);
            ac = final_res.value;
            distancia_anterior = menor_distancia;
            pasos_totales++;
        }

        printf("Valor del acumulador nuevo es: %d\n", ac);
    }

    printf("\nCompletado con exito en %d pasos!\n", pasos_totales);
};

int main(void){
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

    printf("Arreglo generado (Ordenado Descendente): [ ");
    for (int i = 0; i < array_size; i++) {
        printf("%d ", array[i]);
    }
    printf("]\n");

    printf("Ingrese el numero al que quiere llegar: ");
    if (scanf("%d", &goal) != 1) return 1;

    voraz(array, array_size, goal);

    free(array);
    return 0;
}
