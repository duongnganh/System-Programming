//NAME: DUONG NGUYEN A.
//pID: p20150003
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
	int i, j, row, col, block_len;
	int diag = 0, temp = 0;
	int arg0, arg1, arg2, arg3;

	if (N == 32 || N == 67) {
		if (N == 32) block_len = 8;
		if (N == 67) block_len = 16;
		//using blocking
		for (col = 0; col < M; col += block_len){
			for (row = 0; row < N; row += block_len){
				for (i = row; (i < row + block_len) && (i < N); i ++){
					for (j = col; (j < col + block_len) && (j < M); j ++) 
						if (i != j)
							B[j][i] = A[i][j];
						else {
							 temp = A[i][j];
							 diag = i;
						}
					if (row == col) {
						B[diag][diag] = temp;
					}
				}
			}
		}
	}
	if (N == 64){
		//divide each 8*8 block into 4 subblocks of size 4x4
		for (col = 0; col < M; col += 8){ 
			for (row = 0; row < N; row += 8){
				//transpose A top left to B top left
				//move the transpose of A bottom left to B top right
				for (i = row; i < row + 4; i++){
					for (j = col; j < col + 4; j++){
						if (i != j)
							B[j][i] = A[i][j];
						else {
							temp = A[i][j];
							diag = i;
						}
					}
					for (j = col + 4; j < col + 8; j++){
						B[j-4][i+4] = A[i][j];
					}
					if (row == col) {
						B[diag][diag] = temp;
					}
				}
				//move B top right to B bottom left
				//transpose A top right to B top right
				for (j = col; j < col + 4; j++){
					arg0=B[j][row+4];
					arg1=B[j][row+5];
					arg2=B[j][row+6];
					arg3=B[j][row+7];

					for (i = row + 4; i < row + 8; i++){
						B[j][i] = A[i][j];
					}

					B[j+4][row]=arg0;
					B[j+4][row+1]=arg1;
					B[j+4][row+2]=arg2;
					B[j+4][row+3]=arg3;
				}
				//transpose A bottom right to B bottom right
				for (i = row + 4; i < row + 8; i++){
					for (j = col + 4; j < col + 8; j++){
						if (i != j)
							B[j][i] = A[i][j];
						else {
							temp = A[i][j];
							diag = i;
						}
					}
					if (row == col) {
						B[diag][diag] = temp;
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

