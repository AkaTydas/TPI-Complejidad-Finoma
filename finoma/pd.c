#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "finoma.h"

// Definición de límites prácticos y seguros
#define MAX_VAL 1000000      // Rango del acumulador [-1000000, 1000000]
#define QUEUE_SIZE 2500000   // Tamaño máximo de la lista secuencial para almacenar estados

// Estructura simple solicitada que representa cada estado en la lista plana
typedef struct {
    int value;               // Valor del acumulador en este estado
    int ops;                 // Cantidad de operaciones para llegar aquí
    int parent_idx;          // Índice del estado predecesor en la lista (para reconstrucción)
    char op;                 // Operación: '+', '-', '*', '/', '%', '^', 'L' (log), 'R' (raiz)
    int operand;             // Operando del arreglo A utilizado
} State;

// Estructuras de datos globales ubicadas en Heap para evitar desbordar el Stack
State* Queue = NULL;
bool* Visited = NULL;

// --- FUNCIONES MATEMÁTICAS EN ENTEROS CON TRUNCAMIENTO ABSOLUTO (FLOOR) ---

long long safe_pow(long long base, long long exp) {
    if (exp < 0) return 0;
    if (exp == 0) return 1;
    if (base == 0) return 0;
    if (base == 1) return 1;
    if (base == -1) return (exp % 2 == 0) ? 1 : -1;
    
    long long res = 1;
    for (int i = 0; i < exp; i++) {
        res *= base;
        if (res > MAX_VAL || res < -MAX_VAL) {
            return MAX_VAL + 1; // Indicador de desborde
        }
    }
    return res;
}

int safe_log(int x, int base) {
    if (x <= 0 || base <= 1) return -1;
    int count = 0;
    long long temp = base;
    while (temp <= x) {
        count++;
        if (temp > x / base) break;
        temp *= base;
    }
    return count;
}

int safe_root(int x, int base) {
    if (x < 0 || base <= 0) return -1;
    if (base == 1) return x;
    if (x == 0 || x == 1) return x;
    
    int low = 0, high = x, ans = 0;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        long long p = safe_pow(mid, base);
        if (p <= x && p != MAX_VAL + 1) {
            ans = mid;
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return ans;
}

void init_structures() {
    if (Queue == NULL) {
        Queue = (State*)malloc(QUEUE_SIZE * sizeof(State));
    }
    if (Visited == NULL) {
        Visited = (bool*)calloc(2 * MAX_VAL + 1, sizeof(bool));
    } else {
        for (int i = 0; i < 2 * MAX_VAL + 1; i++) {
            Visited[i] = false;
        }
    }
}

void print_solution(int target_idx) {
    int steps_count = Queue[target_idx].ops;
    int* path = (int*)malloc((steps_count + 1) * sizeof(int));
    
    int curr = target_idx;
    for (int i = steps_count; i >= 0; i--) {
        path[i] = curr;
        curr = Queue[curr].parent_idx;
    }
    
    printf("\n=================================================================\n");
    printf(" ¡SOLUCION ENCONTRADA! Se llego a B en %d operaciones.\n", steps_count);
    printf("=================================================================\n\n");
    
    printf("Estado inicial: Acumulador = 0\n");
    for (int i = 1; i <= steps_count; i++) {
        State step = Queue[path[i]];
        State prev = Queue[path[i-1]];
        
        printf("  [Paso %d]: ", i);
        switch (step.op) {
            case '+':
                printf("%d + %d = %d", prev.value, step.operand, step.value);
                break;
            case '-':
                printf("%d - %d = %d", prev.value, step.operand, step.value);
                break;
            case '*':
                printf("%d * %d = %d", prev.value, step.operand, step.value);
                break;
            case '/':
                printf("%d / %d = %d (truncado)", prev.value, step.operand, step.value);
                break;
            case '%':
                printf("%d %% %d = %d", prev.value, step.operand, step.value);
                break;
            case '^':
                printf("%d ^ %d = %d", prev.value, step.operand, step.value);
                break;
            case 'L':
                printf("log en base %d de (%d) = %d (truncado)", step.operand, prev.value, step.value);
                break;
            case 'R':
                printf("raiz cuadrada de (%d) = %d [operando %d consumido]", prev.value, step.value, step.operand);
                break;
        }
        printf("\n");
    }
    printf("\n=================================================================\n");
    free(path);
}

// Resolución principal mediante BFS con Poda Lógica
void solve(int* A, int n, int B) {
    init_structures();
    
    int head = 0;
    int tail = 0;
    
    Queue[tail++] = (State){.value = 0, .ops = 0, .parent_idx = -1, .op = '\0', .operand = 0};
    Visited[0 + MAX_VAL] = true;
    
    if (B == 0) {
        printf("\nEl acumulador ya se encuentra en el valor objetivo 0 (0 operaciones).\n");
        return;
    }
    
    bool found = false;
    int solution_idx = -1;
    
    while (head < tail) {
        State current = Queue[head];
        int S = current.value;
        
        if (S == B) {
            found = true;
            solution_idx = head;
            break;
        }
        
        // Filtro de colapso global: Si el estado es 0, solo permitimos sumar.
        bool solo_suma = (S == 0);
        
        for (int i = 0; i < n; i++) {
            int a = A[i];
            
            int next_values[8];
            char ops[8] = {'+', '-', '*', '/', '%', '^', 'L', 'R'};
            bool valid[8] = {false};
            
            // 1. Suma (+) -> Siempre válida
            next_values[0] = S + a;
            valid[0] = true;
            
            // 2. Resta (-)
            if (!solo_suma) {
                next_values[1] = S - a;
                valid[1] = true;
            }
            
            // 3. Multiplicación (*) -> Filtro identidad (a=1)
            if (!solo_suma && a != 1) {
                long long mul = (long long)S * a;
                if (mul >= -MAX_VAL && mul <= MAX_VAL) {
                    next_values[2] = (int)mul;
                    valid[2] = true;
                }
            }
            
            // 4. División (/) -> Filtro identidad (a=1) y Filtro colapso (|S| < a)
            if (!solo_suma && a != 0 && a != 1) {
                if (S >= a || S <= -a) {
                    next_values[3] = S / a;
                    valid[3] = true;
                }
            }
            
            // 5. Módulo (%) -> Filtro identidad (S < a) y Filtro colapso (S % a == 0)
            if (!solo_suma && a != 0) {
                if (S >= a && (S % a != 0)) {
                    next_values[4] = S % a;
                    valid[4] = true;
                }
            }
            
            // 6. Potencia (^) -> Filtro identidad (a=1)
            if (!solo_suma && a != 1) {
                long long power = safe_pow(S, a);
                if (power >= -MAX_VAL && power <= MAX_VAL) {
                    next_values[5] = (int)power;
                    valid[5] = true;
                }
            }
            
            // 7. Logaritmo (L) -> Filtro colapso (S < a)
            if (!solo_suma && S >= a && a > 1) {
                int log_res = safe_log(S, a);
                if (log_res != -1) {
                    next_values[6] = log_res;
                    valid[6] = true;
                }
            }
            
            // 8. Raíz Cuadrada (R) -> Filtro identidad (S=1). Índice fijo en 2.
            if (!solo_suma && S > 1) {
                int root_res = safe_root(S, 2);
                if (root_res != -1) {
                    next_values[7] = root_res;
                    valid[7] = true;
                }
            }
            
            // Procesar e insertar los estados válidos
            for (int op_idx = 0; op_idx < 8; op_idx++) {
                if (valid[op_idx]) {
                    int val = next_values[op_idx];
                    
                    if (val >= -MAX_VAL && val <= MAX_VAL) {
                        int idx_offset = val + MAX_VAL;
                        
                        if (!Visited[idx_offset]) {
                            Visited[idx_offset] = true;
                            
                            Queue[tail] = (State){
                                .value = val,
                                .ops = current.ops + 1,
                                .parent_idx = head,
                                .op = ops[op_idx],
                                .operand = a
                            };
                            
                            if (val == B) {
                                found = true;
                                solution_idx = tail;
                                break;
                            }
                            
                            tail++;
                            
                            if (tail >= QUEUE_SIZE) {
                                printf("\n[Advertencia] Se alcanzo el limite maximo de busqueda fisica de la cola.\n");
                                head = tail; 
                                break;
                            }
                        }
                    }
                }
            }
            if (found) break;
        }
        if (found) break;
        head++;
    }
    
    if (found) {
        print_solution(solution_idx);
    } else {
        printf("\nNo se encontro ninguna combinacion posible para llegar a %d en este rango de busqueda.\n", B);
    }
}

void limpiar_pd(){
    if (Queue != NULL) {
        free(Queue);
        Queue = NULL;
    }
    if (Visited != NULL) {
        free(Visited);
        Visited = NULL;
    }
}