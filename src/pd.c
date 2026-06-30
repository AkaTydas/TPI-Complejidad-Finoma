#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <time.h>

#define NUMERO_OPERACIONES 8
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

// Estructura para reconstruir el camino
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

// --- CORE: PROGRAMACIÓN DINÁMICA + BFS (COLA ÓPTIMA) ---

void programacion_dinamica_bfs(int array[], int size, int goal) {
    if (goal < 0 || goal >= MAX_ESTADO) {
        printf("[!] Error: El objetivo supera el MAX_ESTADO definido para la tabla DP.\n");
        return;
    }

    printf("\n--- INICIANDO DP (BFS + TABULACION) ---\n");
    printf("Objetivo (B): %d | Estado Inicial: 0\n", goal);
    printf("Universo acotado a: %d\n", MAX_ESTADO);
    printf("========================================\n");

    // 1. Reservas de memoria
    int *dp = (int *)malloc(MAX_ESTADO * sizeof(int));
    PathTracker *tracker = (PathTracker *)malloc(MAX_ESTADO * sizeof(PathTracker));
    
    // Implementación de Cola (Queue) basada en arreglos
    // Como nunca visitamos un estado dos veces, el tamaño máximo de la cola es el MAX_ESTADO
    int *queue = (int *)malloc(MAX_ESTADO * sizeof(int));

    if (!dp || !tracker || !queue) {
        printf("Error de memoria (OOM).\n");
        if (dp) free(dp);
        if (tracker) free(tracker);
        if (queue) free(queue);
        return;
    }

    // 2. Inicialización
    for (int i = 0; i < MAX_ESTADO; i++) {
        dp[i] = INF;
    }
    
    int head = 0; // Puntero de lectura de la cola
    int tail = 0; // Puntero de escritura de la cola

    // Caso base
    dp[0] = 0;
    queue[tail++] = 0; // Push del 0 a la cola

    int solucion_encontrada = 0;
    int nodos_explorados = 0;

    // 3. Exploración BFS
    while (head < tail) {
        int curr_val = queue[head++]; // Pop de la cola
        nodos_explorados++;

        // Si encontramos el objetivo, terminamos. BFS garantiza que es el camino óptimo.
        if (curr_val == goal) {
            solucion_encontrada = 1;
            break; 
        }

        // Poda: Si ya alcanzamos el límite de profundidad, no generamos hijos para este nodo.
        if (dp[curr_val] >= MAX_DEPTH) {
            continue;
        }

        // Generamos los movimientos válidos solo para el estado actual
        for (int k = 0; k < size; k++) {
            for (int op = 0; op < NUMERO_OPERACIONES; op++) {
                
                MathResult res = calcular_operacion(curr_val, array[k], (TipoOperacion)op);

                // Si es válido y está dentro de los límites
                if (res.is_valid && res.value >= 0 && res.value < MAX_ESTADO) {
                    
                    // RELAJACIÓN DP: ¿Es la primera vez que vemos este número?
                    if (dp[res.value] == INF) {
                        
                        dp[res.value] = dp[curr_val] + 1; // Guardamos la distancia mínima
                        
                        tracker[res.value].prev_val = curr_val;
                        tracker[res.value].op = (TipoOperacion)op;
                        tracker[res.value].operand = array[k];
                        
                        queue[tail++] = res.value; // Push a la cola para explorarlo luego
                    }
                }
            }
        }
    }

    // 4. Reconstrucción e impresión
    if (solucion_encontrada) {
        int pasos_totales = dp[goal];
        printf(" EXITO: Solucion optima encontrada en %d pasos\n", pasos_totales);
        printf(" Nodos explorados (rendimiento real): %d\n", nodos_explorados);
        printf("========================================\n");

        int *path_values = (int *)malloc((pasos_totales + 1) * sizeof(int));
        TipoOperacion *path_ops = (TipoOperacion *)malloc((pasos_totales + 1) * sizeof(TipoOperacion));
        int *path_operands = (int *)malloc((pasos_totales + 1) * sizeof(int));

        int trace_val = goal;
        for (int i = pasos_totales - 1; i >= 0; i--) {
            path_values[i] = trace_val;
            path_ops[i] = tracker[trace_val].op;
            path_operands[i] = tracker[trace_val].operand;
            trace_val = tracker[trace_val].prev_val;
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
        printf("\n[!] No se encontro solucion en %d pasos o el objetivo es inalcanzable.\n", MAX_DEPTH);
        printf(" Nodos explorados antes de abortar: %d\n\n", nodos_explorados);
    }

    free(dp);
    free(tracker);
    free(queue);
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

    programacion_dinamica_bfs(array, array_size, goal);

    free(array);
    return 0;
}