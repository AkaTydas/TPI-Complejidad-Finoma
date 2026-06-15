# TPI: Optimización de Secuencias Operacionales (Complejidad Algorítmica)

Este repositorio contiene la implementación y el análisis de complejidad para el Trabajo Práctico Integrador (TPI). El objetivo es resolver un problema de optimización combinatoria utilizando tres paradigmas fundamentales de las ciencias de la computación: **Algoritmos Voraces (Greedy)**, **Programación Dinámica (DP)** y **Backtracking**.

La totalidad del proyecto está desarrollada en **C (C11)**, priorizando el control de memoria eficiente y el rendimiento bruto sin dependencias externas.

---

## 📋 Descripción del Problema

Dado:
1. Un arreglo $A$ de $n$ números enteros positivos aleatorios, no repetidos: $A = \{a_1, a_2, \dots, a_n\}$.
2. Un número objetivo $B$ ingresado por el usuario ($B \in \mathbb{Z}^+$).
3. Un acumulador inicializado en $0$.
4. Un conjunto de operaciones permitidas: **Suma (+), Resta (-), Multiplicación (*), División (/), Potencia (^), Logaritmo (log) y Raíz Cuadrada (sqrt)**.

**Restricciones estrictas:**
* **Truncamiento absoluto:** Todas las operaciones operan en el dominio de los enteros. Cualquier resultado de operaciones no exactas (división, raíz, logaritmo) es truncado inmediatamente a su parte entera (`floor`).
* **Operandos:** Los componentes de las operaciones solo pueden ser números que existan originalmente en el arreglo $A$.
* **Meta:** Alcanzar exactamente el valor $B$ desde $0$ empleando la **mínima cantidad de operaciones posibles**.

---

## 🛠️ Arquitectura de Soluciones (Cómo pensarlas en C)

### 1. Backtracking (Exploración del Espacio de Estados)
Este enfoque modela el problema como la búsqueda en un **árbol de espacio de estados** donde la raíz es $0$. Cada nivel del árbol representa una operación aplicada.

* **Estructura del Árbol:** Si el arreglo tiene $n$ elementos y hay 7 operaciones posibles, el factor de ramificación (*branching factor*) de cada nodo es de hasta $7 \times n$.
* **Estrategia en C:** Se implementa mediante una función recursiva. Para evitar un desbordamiento de la pila (*stack overflow*) debido a ciclos infinitos (ej. sumar y restar el mismo número), es mandatorio pasar un límite de profundidad máxima o usar un histórico de estados visitados de la rama actual.
* **Poda (*Pruning*):** * Si la profundidad actual supera a la mejor solución global encontrada hasta el momento (`current_depth >= min_operations_found`), se corta la ejecución de esa rama inmediatamente.
  * Control de cotas superiores e inferiores para evitar que el acumulador diverja a valores que hagan imposible retornar al objetivo $B$.

### 2. Programación Dinámica (Estrategia de Tabulación / BFS)
Dado que las operaciones pueden reducir los valores (resta, división, raíz), el problema puede presentar dependencias cíclicas si se piensa de forma lineal estándar. Por lo tanto, la estructura óptima para resolverlo en C es mediante **Tabulación combinada con una Búsqueda en Anchura (BFS)** sobre el espacio de estados.

* **Definición del Estado:** `DP[S]` almacena el número mínimo de operaciones necesarias para alcanzar el valor numérico $S$.
* **Inicialización:** Un arreglo unidimensional de tamaño $M$ (donde $M$ es el límite máximo de cómputo permitido). `DP[0] = 0`, y todos los demás elementos se inicializan en un valor centinela (`INF` o `INT_MAX`).
* **Relación de Recurrencia:**
  $$DP[S] = \min_{a \in A, \text{op} \in O} (DP[S_{\text{prev}}] + 1)$$
  *Donde $S$ es el estado resultante de aplicar la operación `op` con el elemento $a$ sobre el estado previo $S_{\text{prev}}$.*
* **Implementación en C:** Se utiliza una cola explícita para procesar los estados nivel por nivel (generando primero todas las soluciones de 1 paso, luego 2 pasos, etc.). Esto garantiza encontrar la solución óptima global de manera óptima sin evaluar caminos redundantes una vez que `DP[B]` deja de ser `INF`.

### 3. Algoritmo Voraz (Heurística Miope)
El enfoque Greedy toma decisiones óptimas locales en cada paso con la esperanza de llegar al óptimo global. No realiza pasos hacia atrás ni evalúa alternativas futuras.

* **Heurística elegida:** En cada iteración $i$, se evalúan las $7 \times n$ combinaciones posibles a partir del valor actual del acumulador. Se selecciona la combinación que **minimice la distancia absoluta** al objetivo $B$:
  $$\min |\text{Acumulador Actual} \circ a_k - B|$$
* **Limitación teórica:** Es altamente propenso a caer en óptimos locales. Si el arreglo contiene el número `1`, el algoritmo voraz puede optar por sumar `1` repetidamente porque cada paso reduce la distancia a $B$ en una unidad, ignorando una multiplicación potencial que requeriría un desvío temporal pero acortaría drásticamente los pasos globales.

---

## 🚀 Compilación y Ejecución

El proyecto está diseñado bajo el estándar C11 y se compila utilizando `gcc`. Debido al uso de funciones matemáticas de `<math.h>` para logaritmos, potencias y raíces, es indispensable enlazar la biblioteca matemática mediante el flag `-lm`.

### Requisitos del Sistema
* Compilador GCC (compatible con C11).
* Entorno basado en UNIX/Linux (o MinGW/WSL en Windows).

### Instrucciones de Compilación

```bash
gcc -O3 src/backtracking.c -o bin/backtracking -lm
gcc -O3 src/dp.c -o bin/dp -lm
gcc -O3 src/greedy.c -o bin/greedy -lm
