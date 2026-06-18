#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <time.h>

// ESTO ES TODO IGUAL QUE EL VORAZ
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

// ACA EMPIEZA LO ORIGINAL

typedef struct Node{
    int value;
    struct Node *padre;
    struct Node **hijos;
} Node;

// Crear un nodo con un valor
Node* crear_nodo(int valor, int total_hijos){ 
    Node* nuevo = malloc(sizeof(Node));
    nuevo->value = valor;
    nuevo->padre = NULL;

    nuevo->hijos = malloc(total_hijos * sizeof(Node*));

    for (int i=0; i< total_hijos; i++)
    {
        nuevo->hijos[i] = NULL;    
    }
    printf("se creo el nodo con el valor %d\n", valor);
    return nuevo;
}

// Pa que no quede leekeando memoria
void liberar_arbol(Node* nodo, int total_hijos){
    if (nodo == NULL) return;

    for (int i=0; i<total_hijos; i++)
    {
        if (nodo->hijos[i] != NULL){
            liberar_arbol(nodo->hijos[i], total_hijos);
        }
    }

    free(nodo->hijos);
    free(nodo);
}


int main(void)
{
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

    int total_hijos = NUMERO_OPERACIONES * goal;
    Node* raiz = crear_nodo(0,total_hijos);

    free(array);
    liberar_arbol(raiz, total_hijos);
}