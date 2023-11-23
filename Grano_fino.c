//Matrix multiplication

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int ID = 0;

double **a, **b, **c;

int matrixSize;


double **allocateMatrix() {
	int i;
	double *vals, **temp;

	// allocate space for values of a matrix
	vals = (double *) malloc (matrixSize * matrixSize * sizeof(double));

	// allocate vector of pointers to create the 2D array
	temp = (double **) malloc (matrixSize * sizeof(double*));

	for(i=0; i < matrixSize; i++)
		temp[i] = &(vals[i * matrixSize]);

	return temp;
}

void *mm(void *arg) {
	int i,j,k, IDR;
	double sum;
	IDR=ID;
	ID++;
	
	int *Tareasmm = (int *)arg;
	int inicio=Tareasmm[IDR*2];
	int fin=Tareasmm[IDR*2+1];
	// printf("ID: %d, Inicio: %d, Fin: %d \n",IDR, inicio,fin);
	// matrix multiplication
	for (i = inicio; i <= fin; i++) {
		//printf("a");
		for (j = 0; j < matrixSize; j++) {
			//printf("b");
			sum = 0.0;
			// dot product
			for (k = 0; k < matrixSize; k++) {
				//printf("c");
				sum = sum + a[i][k] * b[k][j];
				//printf("%f", sum);
			}
			c[i][j] = sum;
			//printf(" %f",sum);
		}
	}
}
void printResult(void){
	int i, j;	
	for(i=0;i<matrixSize;i++){
		for(j=0;j<matrixSize;j++){
			printf("%lf ", c[i][j]);
		}
		printf("\n");
	}
}



int main(int argc, char *argv[]) {

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <file_path> <number_of_threads>\n", argv[0]);
        return 1;
    }
    int i, j, k;
    int nmats;
    char *fname = "matrices_dev.dat"; //Change to matrices_large.dat for performance evaluation
    int NUM_HILOS = atoi(argv[2]);

    //printf("El valor que ingresaste es: %d\n", NUM_HILOS);

    FILE *fh = fopen(argv[1], "r");
	//First line indicates how many pairs of matrices there are and the matrix size
	fscanf(fh, "%d %d\n", &nmats, &matrixSize);

	int Tareas[NUM_HILOS];

	int totalProcesos = matrixSize;
    int numHilos = NUM_HILOS;

    int procesosPorHilo = totalProcesos / numHilos;
    int resto = totalProcesos % numHilos;

    //printf("Procesos por hilo: %d\n", procesosPorHilo);
    //printf("Resto: %d\n", resto);

    // Distribuir los procesos equitativamente
    for (int hilo = 0; hilo < numHilos; ++hilo) {
        int procesosParaEsteHilo = procesosPorHilo + (hilo < resto ? 1 : 0);
        Tareas[hilo]=procesosParaEsteHilo;
		
    }

	int longitud = sizeof(Tareas) / sizeof(Tareas[0]);

	
	int Tareas_aux[NUM_HILOS];
	
	int Tareasmm[2*NUM_HILOS];

	for (int i = 0; i < longitud; i++) {
        Tareas_aux[i] = Tareas[i];
    }

	int sumaTotal = 0;
    for (int i = 1; i < longitud; i++) {
        Tareas[i] += Tareas[i - 1];
        sumaTotal += Tareas[i];
    }

	for (int i=0; i<longitud; i++){

		Tareasmm[i*2]=Tareas[i]-Tareas_aux[i];
		Tareasmm[i*2+1]=Tareas[i]-1;
	}

    // Imprimir los elementos del array
    //printf("Elementos del array: ");
    //for (int i = 0; i < longitud*2; i++) {
    //    printf("%d ", Tareasmm[i]);
    //}
	//printf("\n");

	pthread_t hilos[NUM_HILOS];

	//printf("Start\n");
	

	//Dynamically create matrices of the size needed
	a = allocateMatrix();
	b = allocateMatrix();
	c = allocateMatrix();

	//printf("Loading %d pairs of square matrices of size %d from %s...\n", nmats, matrixSize, fname);

    clock_t start, end;
    double cpu_time_used;
    start = clock();
	for(k=0;k<nmats;k++){
		for(i=0;i<matrixSize;i++){
			for(j=0;j<matrixSize;j++){
				fscanf(fh, "%lf", &a[i][j]);
			}
		}
		for(i=0;i<matrixSize;i++){
			for(j=0;j<matrixSize;j++){
				fscanf(fh, "%lf", &b[i][j]);
			}
		}

		//printf("Multiplying two matrices...\n"); //Remove this line for performance tests

			for (int i = 0; i < NUM_HILOS; ++i) {
				int ID = i ;  
				pthread_create(&hilos[i], NULL, mm, (void *)Tareasmm);

			}
			// Esperar a que todos los hilos terminen
			for (int i = 0; i < NUM_HILOS; ++i) {
				pthread_join(hilos[i], NULL);
				
			}
		ID=0;
		//mm();
		//printResult(); //Remove this line for performance tests
	}
	fclose(fh);

	// Free memory
	free(*a);
	free(a);
	free(*b);
	free(b);
	free(*c);
	free(c);
	//printf("Done.\n");
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("%f", cpu_time_used);

    return 0;
}

