//Matrix multiplication

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

void mm(void) {
	int i,j,k;
	double sum;
	// matrix multiplication
	for (i = 0; i < matrixSize; i++) {
		for (j = 0; j < matrixSize; j++) {
			sum = 0.0;
			// dot product
			for (k = 0; k < matrixSize; k++) {
				sum = sum + a[i][k] * b[k][j];
			}
			c[i][j] = sum;
		}
	}
}
void printResult(FILE *file){
	int i, j;	
	for(i=0;i<matrixSize;i++){
		for(j=0;j<matrixSize;j++){
            fprintf(file, "%lf ", c[i][j]);
			printf("%lf ", c[i][j]);
		}
        fprintf(file, "\n");
		printf("\n");
	}
}

int main(int argc, char *argv[]){
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        return 1;
    }
	int i, j, k;
	int nmats;

	//printf("Start\n");
    FILE *fh = fopen(argv[1], "r");
	//First line indicates how many pairs of matrices there are and the matrix size
	fscanf(fh, "%d %d\n", &nmats, &matrixSize);

    FILE *outputFile = fopen("output.txt", "w");
    if (!outputFile) {
        perror("Error opening output file");
    }
    // Inicia un temporizador para medir el tiempo de ejecución
    clock_t start_time, end_time;
    double cpu_time_used;
    start_time = clock();
	//Dynamically create matrices of the size needed
	a = allocateMatrix();
	b = allocateMatrix();
	c = allocateMatrix();

	//printf("Loading %d pairs of square matrices of size %d from %s...\n", nmats, matrixSize, fname);
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

		//printf("Multiplying %d matrices...\n", matrixSize); //Remove this line for performance tests
		mm();
		//printResult(outputFile); //Remove this line for performance tests
	}
	fclose(fh);

	// Free memory
	free(*a);
	free(a);
	free(*b);
	free(b);
	free(*c);
	free(c);
    // Calcula el tiempo total de ejecución y lo muestra
    end_time = clock();
    cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    printf("%f", cpu_time_used);
	//printf("Done.\n");
	return 0;
}

