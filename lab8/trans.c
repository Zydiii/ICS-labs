/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
/*
 * name: Zhang Yuandi
 * loginID: ics517030910230
 */
#include "cachelab.h"
#include <stdio.h>

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
    int i, j, k, l, a0, a1, a2, a3, a4, a5, a6, a7;
    /*32 * 32 case*/
    if (M == 32)
    {
        for (i = 0; i < 32; i += 8)
        {
            for (j = 0; j < 32; j += 8)
            {
                /*Not diagonal, we just need use 8*8 block to read A and write B.*/
                if (i != j)
                {
                    for (k = i; k < i + 8; k++)
                    {
                        for (l = j; l < j + 8; l++)
                        {
                            B[k][l] = A[l][k];
                        }
                    }
                }
                /*Diagonal, we read 8 A together and keep them in local variables, then write into B together.*/
                else
                {
                    for (k = i; k < i + 8; k++)
                    {
                        a0 = A[k][j];
                        a1 = A[k][j + 1];
                        a2 = A[k][j + 2];
                        a3 = A[k][j + 3];
                        a4 = A[k][j + 4];
                        a5 = A[k][j + 5];
                        a6 = A[k][j + 6];
                        a7 = A[k][j + 7];
                        B[j][k] = a0;
                        B[j + 1][k] = a1;
                        B[j + 2][k] = a2;
                        B[j + 3][k] = a3;
                        B[j + 4][k] = a4;
                        B[j + 5][k] = a5;
                        B[j + 6][k] = a6;
                        B[j + 7][k] = a7;
                    }
                }
            }
        }
    }
    /*64 * 64 case*/
    else if (M == 64)
    {
        /*First, use 8 * 8 block, then use 4 * 4 for each block.*/
        for (i = 0; i < N; i += 8)
        {
            for (j = 0; j < M; j += 8)
            {
                for (k = i; k < i + 4; k++)
                {
                    /*Store A left-up part into B lef-up part, A right-up part into B right-up part.*/
                    a0 = A[k][j];
                    a1 = A[k][j + 1];
                    a2 = A[k][j + 2];
                    a3 = A[k][j + 3];
                    a4 = A[k][j + 4];
                    a5 = A[k][j + 5];
                    a6 = A[k][j + 6];
                    a7 = A[k][j + 7];
                    B[j][k] = a0;
                    B[j + 1][k] = a1;
                    B[j + 2][k] = a2;
                    B[j + 3][k] = a3;
                    B[j][k + 4] = a4;
                    B[j + 1][k + 4] = a5;
                    B[j + 2][k + 4] = a6;
                    B[j + 3][k + 4] = a7;
                }
                for (k = j + 4; k < j + 8; k++)
                {
                    /*A left-down part.*/
                    a0 = A[i + 4][k - 4];
                    a1 = A[i + 5][k - 4];
                    a2 = A[i + 6][k - 4];
                    a3 = A[i + 7][k - 4];
                    /*B right-up part.*/
                    a4 = B[k - 4][i + 4];
                    a5 = B[k - 4][i + 5];
                    a6 = B[k - 4][i + 6];
                    a7 = B[k - 4][i + 7];
                    /*Set B right-up part.*/
                    B[k - 4][i + 4] = a0; 
                    B[k - 4][i + 5] = a1;
                    B[k - 4][i + 6] = a2;
                    B[k - 4][i + 7] = a3;
                    /*Set B left-down part.*/
                    B[k][i] = a4;
                    B[k][i + 1] = a5;
                    B[k][i + 2] = a6;
                    B[k][i + 3] = a7;
                    /*Set B right-down part.*/
                    B[k][i + 4] = A[i + 4][k];
                    B[k][i + 5] = A[i + 5][k];
                    B[k][i + 6] = A[i + 6][k];
                    B[k][i + 7] = A[i + 7][k];
                }
            }
        }
    }
    /*61 * 67case*/
    else
    {
        /*Use 16 * 16 block to transpose every time.*/
        for (i = 0; i < 67; i += 16)
        {
            for (j = 0; j < 61; j += 16)
            {
                for (k = i; k < i + 16 && k < N; k++)
                {
                    for (l = j; l < j + 16 && l < M; l++)
                    {
                        B[l][k] = A[k][l];
                    }
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

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; j++)
        {
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

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; ++j)
        {
            if (A[i][j] != B[j][i])
            {
                return 0;
            }
        }
    }
    return 1;
}
