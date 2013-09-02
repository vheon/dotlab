#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void getMatrix(const char *path, double *mat);
void printMatrix(double *mat, unsigned int row, unsigned int col);

double* getMatrixA(unsigned int row, unsigned int col);
double* getMatrixB(unsigned int row, unsigned int col);
void getMatrixC(double *mat, unsigned int row, unsigned int col);

int main(int argc, char *argv[])
{
	const unsigned int kRowA = 2;
	const unsigned int kColA = 2;
	const unsigned int kRowB = 2;
	const unsigned int kColB = 2;
	const unsigned int kRowC = kRowA * kRowB;
	const unsigned int kColC = kColA * kColB;

	double *matA = getMatrixA(kRowA, kColA);
	double *matB = getMatrixB(kRowB, kColB);
	double *matC = malloc(kRowC * kColC * sizeof(double));

	printMatrix(matA, kRowA, kColA);
	printf("\n");
	printMatrix(matB, kRowB, kColB);
	printf("\n");

	unsigned int iA;
	unsigned int iB;
	unsigned int jA;
	unsigned int jB;
	int iC;
	int jC;

	double aij;
	double bij;

#pragma omp parallel for private(iA,iB,jA,jB,jC,aij,bij)
	for (iC = 0; iC < kRowC; ++iC) {
		iA = iC / kColB;
		iB = iC % kColB;
		for (jC = 0; jC < kColC; ++jC) {
			jA = jC / kColB;
			jB = jC % kColB;

			aij = matA[iA * kColA + jA];
			bij = matB[iB * kColB + jB];

			matC[iC * kColC + jC] = aij * bij;
		}
	}
	printf("\n");
	printMatrix(matC, kRowC, kColC);
	return 0;
}

double* getMatrixA(unsigned int row, unsigned int col) {
	double *matA = malloc(row * col * sizeof(double));

	matA[0] = 1;
	matA[1] = 2;
	matA[2] = 3;
	matA[3] = 4;

	return matA;
}

double* getMatrixB(unsigned int row, unsigned int col) {
	double *matB = malloc(row * col * sizeof(double));

	matB[0] = 0;
	matB[1] = 5;
	matB[2] = 6;
	matB[3] = 7;

	return matB;
}

void printMatrix(double *mat, unsigned int row, unsigned int col) {
	unsigned int i;
	unsigned int j;
	for (i = 0; i < row; ++i) {
		for (j = 0; j < col; ++j) {
			printf("%.2f ", mat[i * col + j]);
		}
		printf("\n");
	}
}
