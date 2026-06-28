# TPI: Optimización de Secuencias Operacionales (Complejidad Algorítmica)

Este repositorio contiene la implementación y el análisis de complejidad para el Trabajo Práctico Integrador (TPI). El objetivo es resolver un problema de optimización combinatoria utilizando tres paradigmas fundamentales de las ciencias de la computación: **Algoritmos Voraces (Greedy)**, **Programación Dinámica (DP)** y **Backtracking**.

La totalidad del proyecto está desarrollada en **C (C11)**, priorizando el control de memoria eficiente y el rendimiento bruto sin dependencias externas.

---

## 🌍 Escenario Práctico: Corrección Orbital

Para dotar al problema de un contexto de aplicación real, el sistema modela la **trayectoria y corrección orbital de un satélite**. 

Un satélite arranca en una órbita de referencia (estado `0`) y tiene a su disposición un conjunto fijo de propulsores con empujes predefinidos (los valores del arreglo $A$). Cada disparo de un propulsor aplica una operación matemática sobre la posición/velocidad orbital actual. Dado que cada disparo consume combustible limitado, el objetivo del sistema de navegación es calcular la secuencia de disparos más corta y eficiente para alcanzar la órbita objetivo exacta (estado $B$).

---

## 📋 Descripción del Problema Matemático

Dado:
1. Un arreglo $A$ de $n$ números enteros positivos aleatorios, no repetidos: $A = \{a_1, a_2, \dots, a_n\}$.
2. Un número objetivo $B$ ingresado por el usuario ($B \in \mathbb{Z}^+$).
3. Un acumulador inicializado en $0$.
4. Un conjunto de operaciones permitidas: **Suma (+), Resta (-), Multiplicación (*), División (/), Módulo (%), Potencia (^), Logaritmo (log) y Raíz Cuadrada (sqrt)**.

### ⚙️ Supuestos y Reglas del Modelo (Acumulador Secuencial)
Para mantener la complejidad del problema dentro de un espectro tratable y evitar la explosión combinatoria de árboles de expresión completos (estilo *Countdown*), se aplican las siguientes reglas estrictas:

* **Procesamiento Lineal Secuencial:** Las operaciones se aplican de manera estricta de izquierda a derecha sobre un acumulador global que representa el estado actual. No se permite agrupar operandos del arreglo en sub-expresiones aisladas antes de aplicarlas al acumulador.
* **Asimetría de Operandos:** El estado actual (Acumulador) actuará *incondicionalmente* como el operando izquierdo en todas las transacciones (Base, Dividendo, Minuendo, etc.), mientras que los elementos del arreglo actuarán como el operando derecho. Es decir: `Acumulador = Acumulador OP Arreglo[k]`.
* **Truncamiento Absoluto:** Todas las operaciones operan en el dominio de los enteros. Cualquier resultado de operaciones no exactas (división, raíz, logaritmo) es truncado inmediatamente a su parte entera (`floor`).
* **Operandos:** Los componentes derechos de las operaciones solo pueden ser números que existan originalmente en el arreglo $A$.

---

## 🛠️ Arquitectura de Soluciones (Cómo pensarlas en C)

### 1. Backtracking (Exploración del Espacio de Estados)
Este enfoque modela el problema como la búsqueda en un **árbol de espacio de estados implícito** utilizando el Call Stack.

* **Estructura:** Factor de ramificación (*branching factor*) de hasta $8 \times n$.
* **Poda y Heurística:** Se ordena la evaluación de los nodos hijos utilizando una heurística de distancia (`abs(goal - resultado)`). Al encontrar rápidamente soluciones viables, se ajusta una cota superior (`mejor_solucion`) que poda agresivamente cualquier rama que alcance una profundidad subóptima, evitando el colapso de la memoria.

### 2. Programación Dinámica (Estrategia de Tabulación / BFS)
Dado que las operaciones pueden reducir los valores, el problema presenta dependencias cíclicas. La estructura óptima en C es mediante **Tabulación combinada con Búsqueda en Anchura (BFS)**.

* **Definición del Estado:** `DP[S]` almacena el número mínimo de operaciones necesarias para alcanzar el valor $S$.
* **Relación de Recurrencia:**
  $$DP[S] = \min_{a \in A, \text{op} \in O} (DP[S_{\text{prev}}] + 1)$$
* **Implementación:** Se procesan los estados nivel por nivel (1 paso, luego 2 pasos, etc.) garantizando encontrar el óptimo global sin evaluar redundancias.
* Optimizaciones Lógicas (Prevención de Estados Basura):
Aunque la tabla DP evita procesar estados repetidos, calcular la operación y buscar en la tabla consume ciclos de reloj. Para optimizar el rendimiento y reducir el factor de ramificación del árbol BFS, se implementaron filtros lógicos estrictos antes de la generación del estado:

Filtros de Identidad: Se bloquea la ejecución de operaciones que, por definición matemática, dejarían al acumulador en su estado actual.

Si el elemento del arreglo evaluado es $a_k = 1$, se omiten las operaciones de multiplicación ($S \times 1$), división ($S / 1$) y potencia ($S^1$).

Filtro Módulo: Si el acumulador es estrictamente menor que el elemento del arreglo ($S < a_k$), se omite la operación módulo ($S \pmod{a_k}$), ya que el resultado es idéntico al estado actual $S$.

Asimismo, si el estado actual es $S = 1$, se omite el cálculo de la raíz cuadrada ($\sqrt{1}$). Esto evita sobrecargar la tabla Hash con cálculos estériles.

Filtros de Destrucción de Información (Prevención de Colapso a Cero): Debido a la regla estricta de truncamiento absoluto (floor), ciertas operaciones de reducción pueden destruir todo el progreso alcanzado. Puesto que el nodo raíz del algoritmo es 0 (el estado inicial), cualquier rama que devuelva el acumulador a 0 es, matemáticamente, el peor camino posible.

Filtro de Reducción y Módulo: La división entera ($S / a_k$) y el logaritmo ($\log_{a_k}(S)$) se bloquean si $S < a_k$. De manera análoga, la operación módulo ($S \pmod{a_k}$) se bloquea siempre que $S$ sea un múltiplo exacto de $a_k$ (es decir, $S \pmod{a_k} == 0$), ya que en todos estos casos el estado colapsaría inmediatamente a 0.

Filtro del Cero: Si el estado actual extraído de la cola es exactamente 0, se omiten las operaciones de multiplicación, división, módulo, potencia y logaritmo, ya que todas resultarán en 0 o en un error de dominio, evaluándose únicamente la suma.

### 3. Algoritmo Voraz (Heurística Miope)
El enfoque Greedy toma decisiones óptimas locales en cada paso con la esperanza de llegar al óptimo global, demostrando el comportamiento de un algoritmo que no puede realizar *backtracking*.

* **Heurística elegida:** En cada iteración, se evalúan las $8 \times n$ combinaciones posibles. Se selecciona la que **minimice la distancia absoluta** al objetivo $B$.
* **Limitación teórica (Óptimo Local):** Es propenso a estancarse si un movimiento requiere alejarse temporalmente del objetivo para habilitar una multiplicación/potencia posterior. En esta implementación, si el algoritmo Voraz no encuentra un movimiento que reduzca la distancia actual, falla intencionalmente, demostrando su debilidad arquitectónica frente a DP y Backtracking.

---

## 🚀 Compilación y Ejecución

El proyecto está diseñado bajo el estándar C11 y se compila utilizando `gcc`. Debido al uso de funciones matemáticas de `<math.h>`, es indispensable enlazar la biblioteca matemática mediante el flag `-lm`.

```bash
gcc -O3 src/back.c -o bin/back -lm
gcc -O3 src/dp.c -o bin/dp -lm
gcc -O3 src/voraz.c -o bin/voraz -lm
