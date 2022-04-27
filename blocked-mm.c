#include <stdio.h>
#include <stdlib.h>
#include "timer.h"
#define getName(name) #name

double *malloc1D(int rows, int cols) {
    
    // allocates 1d array for entire matrix
   double *M = (double*)malloc(rows*cols*sizeof(double));
   // initializes each index to zero (prevents calculating memory data fragments)
   for(int a=0; a< rows; a++)
      for(int b=0; b < cols; b++)
            M[a*cols+b] = 0.0;

   return M; // return pointer to matrix
}

void BlockMM(int len, int block, int start_x, int start_z, int start_y, double *A, double*B, double *C) {

    // loops each block in A's rows, then each column in B's column 
    for (int x = start_x; x < (start_x + block); ++x) {
        for (int z = start_z; z < (start_z + block); ++z) {
            // set sum value to starting value in current block
            double sum = C[x*len+z]; 
            // loops starting index of y to last index in current block
            for( int y = start_y; y < (start_y + block); y++)   {
                sum += A[x*len+y] * B[y*len+z]; // sum = sum + (A[m][n] x B[n][m])                 
                C[x*len+z] = sum; // set C[x][y] as sum value        
            }     
        }
    }
}

void DGEMM(int len, int block, double *A, double * B , double * C) {

    // loops to scope out each blocks starting index
    for ( int z = 0; z < len; z += block ) 
        for ( int x = 0; x < len; x += block) 
            // loop each y = (cols in A == rows in B)
            for ( int y = 0; y < len; y += block ) 
                BlockMM(len, block, x, z, y, A, B, C); // call to perform mm by blocks
}


int readMatrix (char * iFile, double **M, int * rows, int * cols, char *mName) {
    
    FILE * fp = NULL;  

    // check if inputFile is readable for binary values
    if ((fp = fopen(iFile, "rb")) != NULL) { 

        // read values for row and col sizes
        fread(&(*rows), sizeof(int), 1, fp);  
        fread(&(*cols), sizeof(int), 1, fp);  

        // allocate 1d array space for inputFile matrix 
        *M = malloc1D((*rows),(*cols)); 
        // read in 8 byte binary values from file into 1d array 
        fread(M[0], sizeof(double), (*rows)*(*cols), fp);  

        // close file and return ok
        fclose(fp); 
        return 0; 

    } 
    // if here, print error msg for outFile open/write and return error
    printf("error: cannot open and/or read from file '%s'\n", iFile);
    return 1; 
}

int writeMatrix(double *M, int len, char * oFile)    {

    FILE * fp = NULL; 
    // check if inputFile is readable for binary values
    if ((fp = fopen(oFile, "wb")) != NULL) {
        
        // write values for row and col sizes
        fwrite(&len, sizeof(int), 1, fp);  
        fwrite(&len, sizeof(int), 1, fp);  

        // write each double in result matrix C as binary into file 
        fwrite(M, sizeof(double), len*len, fp); 
        
        // close file and return ok
        fclose(fp); 
        return 0;  
    } 
    // if here, print error msg for outFile open/write 
    printf("error: cannot open and/or write to file '%s'\n", oFile);
    return 1;  // return code 1 for error
}

int main(int argn, char **argv) {

    int retCode = 1; // set return code to 1 for error initially 
    // check if 4 args are entered 
    if (argn == 5) {  

        double startOA=0.0, finishOA=0.0, elapsedOA=0.0; // overall program time values
        GET_TIME(startOA); // get overall programs start time

        char * inFileA = argv[1], * inFileB = argv[2], * outFile = argv[3], *bp;  // set each arg file to a pointer
        double *A, *B,* C;   // assign pointers for 1d arrays
        int rA = 0, cA = 0, rB = 0, cB = 0, len = 0, block = 0;  // values for matrix rows/cols
        
        double startMM=0.0, finishMM=0.0, elapsedMM=0.0;  // mm function time values
        double overallGF=0.0, mmGF =0.0, gfOP = 0.0; // gigaflop per second values

        // check if reading files for matrix A AND matrix B returned success
        if(readMatrix(inFileA, &A, &rA, &cA, getName(A)) == 0 && readMatrix(inFileB, &B, &rB, &cB, getName(B)) == 0) {

            // checks each char in block size arg and converts to base 10 int
            block = strtol((char *)argv[4], &bp, 10); 
            
            // check if #rows and #cols both input matrices are equal
            if  ((rA != rB) || (cA != cB) || (rB != cA))  {
                printf("error: '%s'= A(%d x %d) and '%s'= B(%d x %d) both must have equal #rows and #cols\n", inFileA, rA, cA, inFileB, rB, cB);
            } // check if block size arg pointer parsed entired string as int
            else if (*bp != '\0')   {
                printf("error: <int Block_Size> = '%s' is not a valid integer\n", argv[4]);
            }  // check if matrix size for C is diviable by block size
            else if (rA % block != 0) {
                printf("error: matrix size = %dx%d is not divisable by blocksize = %d \n", rA, cB, block);
            } // if here, all error checks passed
            else {

                len = rA = cA = rB = cB; // set len as square matrix side length
                C = malloc1D(len, len); // allocate space for result matrix C 

                GET_TIME(startMM);  // get dgemm functions start time
                DGEMM(len, block, A, B, C);  // call to DGEMM for blocked matrix multiplication 
                GET_TIME(finishMM); //get dgemm functions finish time
    
                elapsedMM = finishMM-startMM; // calc elapsed time for blocked dgemm
                gfOP = (2*((double)len*(double)len*(double)len)) * 0.000000001; // calc gigaflops GFop = [2(n^3) * (1 * 10^-9)]
                mmGF = gfOP/elapsedMM; // calc blocked dgemm gigaflops per second 

                retCode = writeMatrix(C, len, outFile); // call to write result matrix C into file
                free(C); // free C pointer
            }
            free(B); // free B pointer
            free(A); // free A pointer

        } // otherwise, file I/O error occured
        
        GET_TIME(finishOA); // get overall programs finish time 
        elapsedOA= finishOA-startOA; // calc overall programs elapsed time 
        overallGF = gfOP/elapsedOA; // calc overall programs gigaflops per second
        
        // return dgemm and overall results for elapsed time and gigaflops for second 
        printf("C(#rows) = %d, C(#cols) = %d, OverallElapsedTime = %f sec, MMElaspsedTime = %f sec, OverallGF = %f GF/sec, MMGF = %f GF/sec, Blocksize = %d \n", len, len, elapsedOA, elapsedMM, overallGF, mmGF, block);  
                 
    } // else, print usage stmt for incorrect args
    else printf("usage: %s <input_fileA.dat> <input_fileB.dat> <outputFile.dat> <int BlockSize>\n", argv[0]);

    exit(retCode); // exit with current return code value 
}