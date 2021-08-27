/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
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
    // M = 32, N = 32
    // block size = 8 * 8 (ints)
    // total 16 blocks of each matrics
    if (M == 32) {
        for (int ibase = 0; ibase < N; ibase += 8) {
            for (int jbase = 0; jbase < M; jbase += 8) {
                // a block
                for (int i = ibase; i < ibase + 8; i++) {
                    // if we do like:
                    // for j=jbase -> jbase + 8 do
                    //     B[j][i] = A[i][j]
                    // there will be conflict miss on diagnose
                    // ie. B[n][n] and A[n][n] are mapped to the same cache set
                     
                    // so use 8 local temp variable to sequentially load A's row
                    // then store in B, whitch can avoide conflict

                    int j = jbase;
                    
                    int t0 = A[i][j]; // temp
                    int t1 = A[i][j+1];
                    int t2 = A[i][j+2];
                    int t3 = A[i][j+3];
                    int t4 = A[i][j+4];
                    int t5 = A[i][j+5];
                    int t6 = A[i][j+6];
                    int t7 = A[i][j+7];
                    
                    B[j][i] = t0;
                    B[j+1][i] = t1;
                    B[j+2][i] = t2;
                    B[j+3][i] = t3;
                    B[j+4][i] = t4;
                    B[j+5][i] = t5;
                    B[j+6][i] = t6;
                    B[j+7][i] = t7;
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

