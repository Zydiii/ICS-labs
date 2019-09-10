/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 *
 * Student Name: Wang Xinwei
 * Student ID: 516030910041
 *
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
	/* 32x32 64x64 61x67*/
	int i, j, tmp, index;
	int row_Block, col_Block;
	int num1, num2, num3, num4, num5, num6;

	if (M == 32)
    {
		for (row_Block = 0; row_Block < N; row_Block += 8)
		{
			for (col_Block = 0; col_Block < M; col_Block += 8)
			{
				for (i = row_Block; i < row_Block+8; i++)
				{
					for (j = col_Block; j < col_Block+8; j++)
					{
						if (i != j)
							B[j][i] = A[i][j];
						else{
							tmp = A[i][j]; //diagonal
							index = i;
						}				
					}
					if (row_Block == col_Block)
					{
						B[index][index] = tmp;
					}
				}
			}
		}
	}

	else if (M == 61)
	{
		for (row_Block = 0; row_Block < N; row_Block += 16)
        {
            for (col_Block = 0; col_Block < M; col_Block += 16)
            {
                for (i = row_Block; (i < row_Block+16) && (i < N); i++)
                {
                    for (j = col_Block; (j < col_Block+16) && (j < M); j++)
                    {
                        if (i != j)
                            B[j][i] = A[i][j];
                        else{
                            tmp = A[i][j]; //diagonal
                            index = i;
                        }
                    }
                    if (row_Block == col_Block)
					{
                        B[index][index] = tmp;
                    }
                }
            }
        }

	}

	else 
	{
		for (row_Block = 0; row_Block < N; row_Block += 8)
		{
			for (col_Block = 0; col_Block < M; col_Block += 8)
			{
				
				//diagonal block
				if (row_Block == col_Block)
				{
					//block 1
					for (i = row_Block; i < row_Block+4; i++){
                		for (j = col_Block; j < col_Block+4; j++){
							B[63-j][63-i] = A[i][j];
						}	
					}
					//block 2
					for (i = row_Block+4; i < row_Block+8; i++){
                    	for (j = col_Block; j < col_Block+4; j++){
                       	 	B[63-j][63-i] = A[i][j];
                    	}
               		}
					//block 3
					for (i = row_Block+4; i < row_Block+8; i++){
                 		for (j = col_Block+4; j < col_Block+8; j++){
							B[63-j][63-i] = A[i][j];
						}
					}
					//block 4
					for (i = row_Block; i < row_Block+4; i++){
                    	for (j = col_Block+4; j < col_Block+8; j++){
							B[63-j][63-i] = A[i][j];
						}
					}			
					continue;
				}


				//block 1
		    	for (i = row_Block; i < row_Block+4; i++)
                {
                    for (j = col_Block; j < col_Block+4; j++)
                    {
                        if (i != j)
                            B[j][i] = A[i][j];
                        else{
                            tmp = A[i][j]; //diagonal
                            index = i;
                        }
                    }
					if (i == row_Block){
						num1 = A[i][j];
						num2 = A[i][j+1];
						num3 = A[i][j+2];
						num4 = A[i][j+3];
					}
					if (i == row_Block+1){
						num5 = A[i][j];
                        num6 = A[i][j+1];
					}
                    if (row_Block == col_Block){
                        B[index][index] = tmp;
                    }
                }
				
				//block 2
				for (i = row_Block+4; i < row_Block+8; i++)
                {
                    for (j = col_Block; j < col_Block+4; j++)
                    {
                        B[j][i] = A[i][j];
					}
                }

				//block 3
				for (i = row_Block+4; i < row_Block+8; i++)
                {
                    for (j = col_Block+4; j < col_Block+8; j++)
                    {
                        if (i != j)
                            B[j][i] = A[i][j];
                        else{
                            tmp = A[i][j]; //diagonal
                            index = i;
                        }
                    }
                    if (row_Block == col_Block){
                        B[index][index] = tmp;
                   }
                }

				//block 4
				for (i = row_Block; i < row_Block+4; i++)
                {					
                    for (j = col_Block+4; j < col_Block+8; j++)
                    {
					    if (i == row_Block){
							B[j][i] = num1;
							B[j+1][i] = num2;
							B[j+2][i] = num3;
							B[j+3][i] = num4;
							break;
						}
						else if ((i == row_Block+1) && (j == col_Block+4)){
							B[j][i] = num5;
                            B[j+1][i] = num6;
							j = col_Block+5;
						}
						else {
							B[j][i] = A[i][j];
						}
                    }
                }
			}	
		}

		for (row_Block = 0; row_Block < N/2; row_Block += 8)
        {
            for (i = row_Block; i < row_Block+8; i++)
            {
            	for (j = row_Block; j < row_Block+8; j++)
                {
					tmp = B[63-i][63-j];
					B[63-i][63-j] = B[i][j];
					B[i][j] = tmp;
				}
			}
		}
	}
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}
