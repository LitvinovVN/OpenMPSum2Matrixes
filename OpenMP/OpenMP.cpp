#include "stdafx.h"
#include "stdlib.h"
#include "time.h"
#include "omp.h"

#define NUM_ROWS 5000
#define NUM_COLS 10000

#define SERIAL false
#define PARALLEL true

double MatrixA[NUM_ROWS][NUM_COLS];
double MatrixB[NUM_ROWS][NUM_COLS];
double MatrixResultSerial[NUM_ROWS][NUM_COLS];
double MatrixResultParallel[NUM_ROWS][NUM_COLS];

// Инициализация двумерного массива
// псевдослучайными числами с указанным seed
// isParallel = true - параллельная реализация
// isParallel = false - последовательная реализация
void InitMatrix(double Matrix[NUM_ROWS][NUM_COLS], int seed, bool isParallel)
{	
	srand(seed);
	#pragma omp parallel for if(isParallel)
	for (int i = 0; i < NUM_ROWS; i++)
	{			
		for (int j = 0; j < NUM_COLS; j++)
		{
			Matrix[i][j] = 0.1*(rand() % 2001 - 1000);
		}
	}	
}

// Вывод двумерного массива
void PrintMatrix(double Matrix[NUM_ROWS][NUM_COLS])
{	
	for (int i = 0; i < NUM_ROWS; i++)
	{
		for (int j = 0; j < NUM_COLS; j++)
		{			
			printf("%f\t", Matrix[i][j]);
		}
		printf("\n");
	}
}

// Суммирование матриц
// isParallel = false - последовательный алгоритм
// isParallel = true - параллельный алгоритм
void SumMatrixes(double MatrixA[NUM_ROWS][NUM_COLS], double MatrixB[NUM_ROWS][NUM_COLS], double MatrixResult[NUM_ROWS][NUM_COLS],bool isParallel)
{	
	#pragma omp parallel for if(isParallel)
	for (int i = 0; i < NUM_ROWS; i++)
	{
		for (int j = 0; j < NUM_COLS; j++)
		{
			MatrixResult[i][j] = MatrixA[i][j] + MatrixB[i][j];
		}
	}
}

// Тест поддержки OpenMP
void OpenMPSupportTest()
{
	#ifdef _OPENMP
	printf("OpenMP is supported\n");
	#endif
}

void InitMatrixes()
{
	printf("--- Initialization --- \n");

	printf("--- MatrixA --- SERIAL ---\n");
	double MatrixAInitStart = omp_get_wtime();
	InitMatrix(MatrixA, 1, SERIAL);
	double MatrixAInitEnd = omp_get_wtime();
	double MatrixAInitTime = MatrixAInitEnd - MatrixAInitStart;
	printf("MatrixAInit Time = %lf\n", MatrixAInitTime);

	printf("--- MatrixB --- PARALLEL ---\n");
	double MatrixBInitStart = omp_get_wtime();
	InitMatrix(MatrixB, 54, PARALLEL);
	double MatrixBInitEnd = omp_get_wtime();
	double MatrixBInitTime = MatrixBInitEnd - MatrixBInitStart;
	printf("MatrixBInit Time = %f\n", MatrixBInitTime);
	
	printf("Acceleration = %.2f\n", MatrixAInitTime/MatrixBInitTime);

	printf("--- End Initialization --- \n");
}

void ParallelRegionTest()
{
	#pragma omp parallel
	{
		#pragma omp master
		{
			printf("Parallel region, %i threads\n", omp_get_num_threads());
		}		
	}
}

int main()
{
	omp_set_num_threads(4);	
	omp_set_dynamic(0);
	omp_set_nested(0);

	InitMatrixes();
	
	printf("\n--- Start Matrix Sum ---\n");
	double tstart = omp_get_wtime();
	SumMatrixes(MatrixA, MatrixB, MatrixResultSerial, SERIAL);
	double tend = omp_get_wtime();
	double timeSumSerial = tend - tstart;
	printf("Matrix Sum SERIAL Time = %f\n", timeSumSerial);	
	double tStartParallel = omp_get_wtime();
	SumMatrixes(MatrixA, MatrixB, MatrixResultParallel, PARALLEL);
	double tEndParallel = omp_get_wtime();
	double timeSumParallel = tEndParallel - tStartParallel;
	printf("Matrix Sum PARALLEL Time = %f\n", timeSumParallel);
	printf("Acceleration = %.2f\n", timeSumSerial / timeSumParallel);
	printf("--- End Matrix Sum ---\n");
	
	printf("\n---------------\n");
	OpenMPSupportTest();	
	printf("Timer accuracy = %.3e\n", omp_get_wtick());
	printf("omp_get_dynamic() = %i\n", omp_get_dynamic());
	printf("omp_get_max_threads() = %i\n", omp_get_max_threads());
	printf("omp_get_num_procs() = %i\n", omp_get_num_procs());
	printf("omp_get_nested() = %i\n", omp_get_nested());
	//printf("\n--- MatrixResult ---\n");
	//PrintMatrix(MatrixResult);
	
	ParallelRegionTest();

	system("pause");
    return 0;
}

