#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TOLERANCE 1e-5

int matricesAreEqual(char* file1, char* file2) {
    FILE *fp1 = fopen(file1, "r");
    FILE *fp2 = fopen(file2, "r");
    double val1, val2;
    int equal = 1;

    if (!fp1 || !fp2) {
        fprintf(stderr, "Error opening files.\n");
        exit(EXIT_FAILURE);
    }

    while (fscanf(fp1, "%lf", &val1) == 1 && fscanf(fp2, "%lf", &val2) == 1) {
        if (fabs(val1 - val2) > TOLERANCE) {
            equal = 0;
            break;
        }
    }

    fclose(fp1);
    fclose(fp2);
    return equal;
}

int main() {
    if (matricesAreEqual("output.txt", "output_threadings.txt")) {
        printf("Matrices are equal within the defined tolerance.\n");
    } else {
        printf("Matrices are not equal.\n");
    }
    return 0;
}