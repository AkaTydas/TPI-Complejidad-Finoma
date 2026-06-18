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
    struct Node *hijos[NUMERO_OPERACIONES];
} Node;

// Crear un nodo con un valor
Node* crear_nodo(int valor){ 
    Node* nuevo = malloc(sizeof(Node));
    nuevo->value = valor;
    nuevo->padre = NULL;

    for (int i=0; i<NUMERO_OPERACIONES; i++)
    {
        nuevo->hijos[i] = NULL;    
    }

    return nuevo;
}


int main(void)
{
    printf("PATATA\n");
}