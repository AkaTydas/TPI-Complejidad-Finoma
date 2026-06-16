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

// Helper function to return the string representation of the operation
const char* get_op_symbol(TipoOperacion op) {
    switch (op) {
        case OP_SUM: return "+";
        case OP_RES: return "-";
        case OP_MUL: return "*";
        case OP_DIV: return "/";
        case OP_POW: return "^";
        case OP_ROOT: return "root";
        case OP_LOG: return "log";
        default: return "?";
    }
}

void voraz(int array[], int size, int goal)
{
    int ac = 0; 
    int distancia_anterior = abs(ac - goal);
    int pasos_totales = 0;
    
    int estancamientos_consecutivos = 0; 
    int blacklisted_element_index = -1;

    printf("\n--- INICIANDO ALGORITMO VORAZ ---\n");
    printf("Objetivo (B): %d | Estado Inicial: %d\n\n", goal, ac);

    while (ac != goal)
    {
        int menor_distancia = INT_MAX;
        TipoOperacion mejor_op = OP_INVALIDA;
        int mejor_elemento = 0;
        int comodin_usado = 0;

        for (int i=0; i<size; i++){
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
                for(int k = size - 1; k >= 0; k--) {
                    if (array[k] > 0 && array[k] != 1) {
                        joker_base = array[k];
                        break;
                    }
                }
                
                if (joker_base != -1) {
                    int prev_ac = ac;
                    
                    if (ac < goal) {
                        ac += 1; 
                        printf("[Paso %d] [COMODIN]: %d + log_%d(%d) -> Nuevo AC: %d\n", pasos_totales + 1, prev_ac, joker_base, joker_base, ac);
                    } else if (ac > goal) {
                        ac -= 1; 
                        printf("[Paso %d] [COMODIN]: %d - log_%d(%d) -> Nuevo AC: %d\n", pasos_totales + 1, prev_ac, joker_base, joker_base, ac);
                    }
                    
                    pasos_totales += 2; 
                    distancia_anterior = abs(ac - goal);
                    estancamientos_consecutivos = 0; 
                    comodin_usado = 1;
                    
                    for (int idx = 0; idx < size; idx++) {
                        if (array[idx] == mejor_elemento) {
                            blacklisted_element_index = idx;
                            break;
                        }
                    }
                    printf(">>> [Sistema]: Blacklisting temporal del elemento %d.\n", mejor_elemento);
                    
                } else {
                    printf("CRITICAL FAILURE: Imposible crear el comodin con este arreglo.\n");
                    return;
                }
            } else {
                // Silent the first warning to keep the trace clean, it's just an internal check anyway.
                // printf("[Warning] Rebote detectado. Intentando una vez mas...\n");
            }
        } else {
             blacklisted_element_index = -1;
        }

        if (!comodin_usado) {
            MathResult final_res = calcular_operacion(ac, mejor_elemento, mejor_op);
            int prev_ac = ac;
            ac = final_res.value;
            distancia_anterior = menor_distancia;
            pasos_totales++;
            
            // The requested trace print
            printf("[Paso %d] Operacion: %d %s %d -> Nuevo AC: %d | (Distancia restante: %d)\n", 
                   pasos_totales, prev_ac, get_op_symbol(mejor_op), mejor_elemento, ac, abs(ac - goal));
        }
    }

    printf("\n>>> Completado con exito en %d pasos!\n", pasos_totales);
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

    printf("\nArreglo generado (Ordenado Descendente):\n[ ");
    for (int i = 0; i < array_size; i++) {
        printf("%d ", array[i]);
    }
    printf("]\n\n");

    printf("Ingrese el numero al que quiere llegar (B): ");
    if (scanf("%d", &goal) != 1) return 1;

    voraz(array, array_size, goal);

    free(array);
    return 0;
}