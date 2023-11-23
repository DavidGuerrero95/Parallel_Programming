#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

// Estructura para almacenar datos de la matriz y los rangos de procesamiento
typedef struct {
    double ***a, ***b, ***c;  // Punteros a matrices tridimensionales
    int matrixSize;           // Tamaño de la matriz (nxn)
    int startPair, endPair;   // Indica el rango de pares de matrices a procesar
} MatrixData;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex para control de concurrencia

double ***allocateMatrix(int layers, int rows, int cols) {
    // Asignar memoria para un conjunto de matrices (layers)
    double ***matrix = (double ***)malloc(layers * sizeof(double**));
    for (int l = 0; l < layers; l++) {
        // Asignar memoria para las filas de cada matriz
        matrix[l] = (double **)malloc(rows * sizeof(double*));
        for (int r = 0; r < rows; r++) {
            // Asignar memoria para las columnas de cada fila
            matrix[l][r] = (double *)malloc(cols * sizeof(double));
        }
    }
    return matrix;
}

void freeMatrix(double ***matrix, int layers, int rows) {
    // Liberar la memoria asignada para cada capa de la matriz
    for (int l = 0; l < layers; l++) {
        for (int r = 0; r < rows; r++) {
            free(matrix[l][r]); // Liberar cada fila
        }
        free(matrix[l]); // Liberar cada matriz
    }
    free(matrix); // Liberar el conjunto de matrices
}

void *mm(void *arg) {
    // Convertir arg a tipo MatrixData*
    MatrixData *data = (MatrixData *)arg;
    // Recorrer cada par asignado a este hilo
    for (int pair = 0; pair < data->endPair - data->startPair; pair++) {
        // Realizar la multiplicación de matrices
        for (int i = 0; i < data->matrixSize; i++) {
            for (int j = 0; j < data->matrixSize; j++) {
                double sum = 0.0;
                // Sumar el producto de las filas y columnas correspondientes
                for (int k = 0; k < data->matrixSize; k++) {
                    sum += data->a[pair][i][k] * data->b[pair][k][j];
                }
                data->c[pair][i][j] = sum; // Asignar el resultado
            }
        }
    }
    return NULL; // Terminar el hilo
}

void writeResults(FILE *file, MatrixData *data, int numPairs) {
    pthread_mutex_lock(&mutex); // Bloquear el mutex
    // Escribir los resultados en el archivo
    for (int pair = 0; pair < numPairs; pair++) {
        for (int i = 0; i < data->matrixSize; i++) {
            for (int j = 0; j < data->matrixSize; j++) {
                fprintf(file, "%lf ", data->c[pair][i][j]);
            }
            fprintf(file, "\n");
        }
    }
    pthread_mutex_unlock(&mutex); // Desbloquear el mutex
}

int main(int argc, char *argv[]) {
    // Validar argumentos
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <file_path> <number_of_threads>\n", argv[0]);
        return 1;
    }

    // Convertir argumentos a valores enteros
    int numThreads = atoi(argv[2]);
    pthread_t threads[numThreads]; // Crear hilos
    MatrixData threadData[numThreads]; // Datos para los hilos
    int nmats, matrixSize; // Variables para almacenar cantidad de matrices y su tamaño

    // Abrir archivo de entrada
    FILE *fh = fopen(argv[1], "r");
    // Si hay un error al abrir el archivo, muestra un mensaje y finaliza
    if (!fh) {
        perror("Error opening file");
        return 1;
    }

    fscanf(fh, "%d %d\n", &nmats, &matrixSize); // Lee el número de pares de matrices y el tamaño de las matrices del archivo

    // Calcula la cantidad de pares de matrices que procesará cada hilo
    int pairsPerThread = nmats / numThreads;
    int remainingPairs = nmats % numThreads;
    int currentPair = 0;

    // Inicia un temporizador para medir el tiempo de ejecución
    clock_t start_time, end_time;
    double cpu_time_used;
    start_time = clock();

    // Ciclo para crear cada hilo y asignarle los datos necesarios
    for (int i = 0; i < numThreads; i++) {
        int start = currentPair;
        int pairsThisThread = pairsPerThread + (i < remainingPairs ? 1 : 0);
        int end = start + pairsThisThread;
        currentPair += pairsThisThread;

        // Asigna el tamaño de la matriz y los rangos de pares a procesar
        threadData[i].matrixSize = matrixSize;
        threadData[i].startPair = start;
        threadData[i].endPair = end;
        // Asigna memoria para las matrices A, B y C
        threadData[i].a = allocateMatrix(pairsThisThread, matrixSize, matrixSize);
        threadData[i].b = allocateMatrix(pairsThisThread, matrixSize, matrixSize);
        threadData[i].c = allocateMatrix(pairsThisThread, matrixSize, matrixSize);

        // Lee los datos de las matrices A y B del archivo
        for (int pair = start; pair < end; pair++) {
            for (int r = 0; r < matrixSize; r++) {
                for (int c = 0; c < matrixSize; c++) {
                    fscanf(fh, "%lf", &threadData[i].a[pair - start][r][c]);
                }
            }
            for (int r = 0; r < matrixSize; r++) {
                for (int c = 0; c < matrixSize; c++) {
                    fscanf(fh, "%lf", &threadData[i].b[pair - start][r][c]);
                }
            }
        }
        // Crea hilo para realizar la multiplicación de matrices
        pthread_create(&threads[i], NULL, mm, &threadData[i]);
    }

    //FILE *outputFile = fopen("output_threadings.txt", "w");
    //if (!outputFile) {
    //    perror("Error opening output file");
    //    return 1;
    //}

    // Ciclo para esperar que todos los hilos terminen su ejecución
    for (int i = 0; i < numThreads; i++) {
        int pairsThisThread = pairsPerThread + (i < remainingPairs ? 1 : 0); // Re-declare pairsThisThread for this scope
        pthread_join(threads[i], NULL);
        //writeResults(outputFile, &threadData[i], pairsThisThread);
        // Libera la memoria asignada a las matrices de cada hilo
        freeMatrix(threadData[i].a, pairsThisThread, matrixSize);
        freeMatrix(threadData[i].b, pairsThisThread, matrixSize);
        freeMatrix(threadData[i].c, pairsThisThread, matrixSize);
    }

    //fclose(fh);
    //fclose(outputFile);
    // Cierra los archivos y destruye el mutex
    pthread_mutex_destroy(&mutex);
    // Calcula el tiempo total de ejecución y lo muestra
    end_time = clock();
    cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    printf("%f", cpu_time_used);

    return 0;
}
