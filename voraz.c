#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
// gcc voraz.c -o voraz -lm

#define NUMERO_OPERACIONES 7

/*
    El enfoque Greedy toma decisiones óptimas locales 
    en cada paso con la esperanza de llegar al óptimo global. 
    No realiza pasos hacia atrás ni evalúa alternativas futuras.

    **Heurística elegida**: En cada iteración i , se evalúan las 7xn
    combinaciones posibles a partir del valor actual del acumulador.
    Se selecciona la combinación que minimice la distancia absoluta al objetivo 
    B = min | Acumulador Actual ∘ a k − B |

    **Limitación teórica**: Es altamente propenso a caer en óptimos locales. 
    Si el arreglo contiene el número 1, el algoritmo voraz puede optar por sumar 
    1 repetidamente porque cada paso reduce la distancia a B en una unidad, 
    ignorando una multiplicación potencial que requeriría un desvío temporal pero
    acortaría drásticamente los pasos globales.

*/
typedef enum{ // Enumeracion para hacer el CASE con las operaciones
    OP_SUM,
    OP_RES,
    OP_MUL,
    OP_DIV,
    OP_POW,
    OP_ROOT,
    OP_LOG,
    OP_INVALIDA
} TipoOperacion;


int calcular_operacion(int a, int b, TipoOperacion operacion)
{
    switch (operacion)
    {
        case OP_SUM:
            return a + b;
        case OP_RES:
            return a - b;
        case OP_MUL:
            return a * b;
        case OP_DIV:
            return a / b;
        case OP_POW:
            return pow(a,b);
        case OP_ROOT:
            return pow(a, 1.0 / b); // por propiedad de la raiz es equivalente a la potencia de la inversa
        case OP_LOG:
            return (log((double)a) / log((double)b)); // logaritmo en base b del argumento a 
        default:
            printf("operacion invalida");
            return 0; 
    }
}

void voraz(int array[], int size, int goal)
{
    int ac = 0; // acumulador
    int distancia_anterior = abs(ac - goal);

    while (ac != goal)
    {
        int menor_distancia = INT_MAX;
        TipoOperacion mejor_op = OP_INVALIDA;
        int mejor_elemento = 0;

        for (int i=0; i<size; i++){

            for (int j=0; j<NUMERO_OPERACIONES; j++){
                int resultado = calcular_operacion(ac, array[i], j);
                int distancia_actual = abs(resultado - goal);


                if (distancia_actual < menor_distancia)
                {
                    menor_distancia = distancia_actual;
                    mejor_op = (TipoOperacion)j;
                    mejor_elemento = array[i];
                }
            }
        }

        // Pa que el bucle no sea infinito
        if (menor_distancia >= distancia_anterior)
        {
            printf("Estancado en un optimo local\n");
            printf("NO ES POSIBLE RESOLVER EL PROBLEMA CON ESTAS CONFIGURACIONES SIGUIENDO ESTA HEURISTICA\n");
            return;
        }

        ac = calcular_operacion(ac, mejor_elemento, mejor_op);
        distancia_anterior = menor_distancia;

        printf("Valor del acumulador nuevo es: %d\n", ac);
    }

    printf("Completado con exito!\n");
};


int main(void){
    int *array;
    int array_size;
    int goal;

    printf("tamanio del arreglo: ");
    if (scanf("%d", &array_size) != 1) return 1;

    array = (int *)calloc(array_size, sizeof(int));

    for (int i=0; i < array_size; i++){
        printf("ingrese elemento %d: ", i+1);
        scanf("%d", &array[i]);
    }

    printf("ingrese el numero al que quiere llegar: ");
    if (scanf("%d", &goal) != 1) return 1;

    voraz(array, array_size, goal);


    free(array);
    return 0;
}
