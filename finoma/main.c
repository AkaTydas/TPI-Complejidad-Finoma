#include <stdio.h>
#include "finoma.h"

int prompt_int(char * message);
void number_pressed(char * message, int required_number);
void carga_manual(int** array, int* size, int* goal);
void carga_aleatoria(int ** array, int* size, int *goal);

int main(void)
{
    char* limpiar_pantalla;

    #ifdef _WIN32
        limpiar_pantalla = "cls";
    #elif __linux__
        limpiar_pantalla = "clear";
    #endif

    srand(time(NULL));

    int *array = NULL; int array_size; int goal;
    int opcion; int metodo;
   
    do{
        printf("=============================================================\n");
        printf("\t=== CORRECCION ORBITAL MINIMA - TPI FINOMA ===\n");
        printf("=============================================================\n");
        printf("1. Modo Manual (Ingresar todos los datos a mano)\n");
        printf("2. Modo Aleatorio (Generar arreglo y objetivo aleatorios)\n");
        printf("3. Arreglos precargados de prueba\n");
        printf("4. Salir\n");
        printf("=============================================================\n");
        opcion = prompt_int("Seleccione una opcion: ");


        switch (opcion){
            case 1:{
                carga_manual(&array, &array_size, &goal); 
                break;
            }

            case 2: {
                carga_aleatoria(&array, &array_size, &goal);
                break;
            }
            case 4: (opcion = 4); break;
        }

        if (opcion != 4){
            do{
                system(limpiar_pantalla);
                printf("=============================================================\n");
                printf("Arreglo:[");
                for (int i = 0; i < array_size; i++) {
                    printf("%d%s", array[i], (i == array_size - 1) ? "" : ", ");
                }
                printf("]\t");
                printf("\t\tNumero objetivo (B): %d\n", goal);

                printf("=============================================================\n");
                printf("\t\t=== SELECCIONE EL ALGORITMO ===\n");
                printf("1. Greedy\n");
                printf("2. Backtracking\n");
                printf("3. Programacion Dinamica\n");
                printf("4. Volver a la pantalla anterior\n");
                printf("=============================================================\n");
                metodo = prompt_int("Seleccione una opcion: ");

                switch (metodo){
                    case 1: {
                        voraz(array, array_size, goal); 
                        number_pressed("presione 1 para continuar: ", 1); 
                        break;}
                    case 2: {
                        backtracking(array, array_size, goal);
                        number_pressed("presione 1 para continuar: ", 1);
                        break;}
                    case 3: {
                        programacion_dinamica_pura(array, array_size, goal);
                        number_pressed("presione 1 para continuar: ", 1);
                        break;
                    }
                    case 4: {opcion = 0; system(limpiar_pantalla); break;}
                }
            } while (metodo != 4);
        }

    } while (opcion != 4);
    limpiar_pd();
    if (array != NULL)
    {
        free(array);
        array = NULL;
    }
    
    return 0;
}


// Get an Integer from the User.
int prompt_int(char * message)
{
    int number, numRead;
    printf("%s", message);
    numRead = scanf("%d", &number);
    while (numRead != 1)
    {
        printf("%s", message);
        scanf("%*[^\n]");
        numRead = scanf("%d", &number);
    }
    while (getchar() != '\n');  // Empty stdin
    return number;
}

// Makes a loop that breaks only when certain number is typed.
void number_pressed(char * message, int required_number)
{
    int num; 
    do
    {
        num = prompt_int(message);
    } while (num != required_number);
}

void carga_manual(int** array, int* size, int* goal){
    printf("Ingrese el tamanio del arreglo (N): ");
        while (scanf("%d", size) != 1 || *size < 0) {
            printf("Tamanio invalido. Intente de nuevo: ");
            while (getchar() != '\n'); // limpia el buffer
        }
            
    *array = (int*)malloc((*size) * sizeof(int));
    printf("Ingrese los %d enteros positivos (sin repetir) separados por espacios:\n", *size);
    for (int i = 0; i < *size; i++) {
        int input_val;
                
        while(1){
            if (scanf("%d", &input_val) != 1 || input_val <= 0) {
                printf("Valor incorrecto. Intente nuevamente: ");
                while (getchar() != '\n');
            }

            else if (is_duplicate(*array, i, input_val)) {
                printf("El numero %d ya esta en el arreglo. Ingrese uno unico: ", input_val);
            }

            else{break;}
            }
        (*array)[i] = input_val;
    }
    
            
    printf("Ingrese el numero objetivo positivo (B): ");
    while (scanf("%d", goal) != 1 || *goal < 0) {
        printf("Objetivo invalido. Intente de nuevo: ");
        while (getchar() != '\n'); // limpia el buffer
    }
    printf("Numero objetivo (B): %d\n", *goal);
    printf("=============================================================\n");
    number_pressed("presione 1 para continuar: ", 1);
}

void carga_aleatoria(int ** array, int* size, int *goal){
    printf("Ingrese el tamanio del arreglo (N): ");
        while (scanf("%d", size) != 1 || *size < 0) {
            printf("Tamanio invalido. Intente de nuevo: ");
            while (getchar() != '\n'); // limpia el buffer
        }
            
    *array = (int*)malloc((*size) * sizeof(int));
    printf("Arreglo generado aleatoriamente:\n[");
    for (int i = 0; i < *size; i++) {
        int rand_val;
        do {
            rand_val = rand() % 35 + 1; 
        } while (is_duplicate(*array, i, rand_val));
        (*array)[i] = rand_val;
        printf("%d%s", (*array)[i], (i == *size - 1) ? "" : ", ");
    }
    printf("]\n");

    *goal = rand() % 200 + 10;
    printf("Numero objetivo aleatorio (B): %d\n", *goal);
    printf("=============================================================\n");
    number_pressed("presione 1 para continuar: ", 1);

    
}