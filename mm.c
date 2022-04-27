#include <stdio.h>
#include <stdlib.h>
#include "timer.h"
#define getName(var)#var // for printing unique matrix names

double **malloc2D(int rows, int cols) {

    // allocate 2d array space with pointer for each row and pointer to index each row
    double **x = (double **)malloc(rows*sizeof(double *) + rows*cols*sizeof(double));
    x[0] = (double *)(x + rows); // assign pointer for 2d array starting index
    // assign pointers for each row location
    for (int j = 1; j < rows; j++) {
      x[j] = x[j-1] + cols;  
   }
   return x;   // return 2d array
}

void MatrixMultiply(double ** A,  double ** B , double ** C, int mA, int nAB, int mB) {

    // loop matrices in row major order
    for (int x = 0;  x < mA; x ++) {   
        for (int z = 0;  z < mB; z++) {   
            double sum = 0.0; // set sum to 0 (avoids calculating memory data fragments)
            // loops size of nAB = nA(cols of A) = nB(rows of B) 
            for (int y = 0;  y < nAB; y++) {  
                // calc summation of temp = temp + (A[m][n] x B[n][m])
                sum += A[x][y] * B[y][z];  
                // set curent row*col in C to temp
                C[x][z] = sum;
            }
        }
    }
}

int readMatrix (char * iFile, double *** M, int * rows, int * cols, char * mVar) {
    
    FILE * fp = NULL;  
    // check if inputFile is readable for binary values
    if ((fp = fopen(iFile, "rb")) != NULL) { 

        // read values for row and col sizes
        fread(&(*rows), sizeof(int), 1, fp);  
        fread(&(*cols), sizeof(int), 1, fp); 

        // allocate 2d array space for inputFile matrix 
        *M = malloc2D((*rows),(*cols));  
        // read 8 byte binary values from file as doubles into 2d array 
        fread(*M[0], sizeof(double), (*rows)*(*cols), fp);  

        // close file and return ok
        fclose(fp);  
        return 0; 
    }  
    // if here, print error msg for outFile open/write and return error
    printf("error: cannot open and/or read from file '%s'\n", iFile);
    return 1;  
}

int writeMatrix(double ** M, int rows, int cols, char * oFile)    {

    FILE * fp = NULL;
    // check if inputFile is writable for binary values
    if ((fp = fopen(oFile, "wb")) != NULL) {
        
        // write values for result matrix row and col sizes
        fwrite(&rows, sizeof(int), 1, fp);  
        fwrite(&cols, sizeof(int), 1, fp);  
        // write each double in result matrix C as binary into file 
        fwrite(M[0], sizeof(double), rows*cols, fp); 

        // close file and return ok
        fclose(fp);  
        return 0; 
    } 
    // if here, print error msg for outFile open/write and return error
    printf("error: cannot open and/or write to file '%s'\n", oFile);
    return 1;  
}

int main(int argn, char **argv) {

    int retCode = 1; // set return code to 1 for error initially 
    // check if 4 args are entered 
    if (argn == 4) {  

        double startOA=0.0, finishOA=0.0, elapsedOA=0.0; // overall program time values
        GET_TIME(startOA); // get overall programs start time

        char * inFileA = argv[1], * inFileB = argv[2], * outFile = argv[3]; // assign pointers to file args
        double ** A, ** B,** C;  // assign pointers for 2d arrays
        int rA = 0, cA = 0, rB = 0, cB = 0;  // values for matrix rows/cols
        
        double startMM=0.0, finishMM=0.0, elapsedMM = 0.0;  // mm function time values
        double overallGF=0.0, mmGF =0.0, gfOP = 0.0; // gigaflop per second values
      
        // check if reading files for matrix A AND matrix B returned success codes
        if(readMatrix(inFileA, &A, &rA, &cA, getName(A)) == 0 && readMatrix(inFileB, &B, &rB, &cB, getName(B)) == 0) {

            // check if matrix A cols equal matrix B rows
            if(cA == rB) {

                C = malloc2D(rA, cB); // allocate space for result matrix C

                GET_TIME(startMM);   // get mm function start time
                MatrixMultiply(A, B, C, rA, rB, cB);  // call to perform matrix multiplication
                GET_TIME(finishMM); // get mm function finish time

                elapsedMM=finishMM-startMM; // mm elapsed time
                gfOP = (2*((double)rA*(double)cB*(double)rB)) * 0.000000001; // calc gigaflops GFop = [2(n^3) * (1 * 10^-9)]
                mmGF = gfOP/elapsedMM;  // mm function gigaflops per second 

                retCode = writeMatrix(C, rA, cB, outFile); // call to write result matrix C into file
                free(C); // free C pointer
                
            } // else, print msg for error multiplying inputFileA and  inputFileB matrices  
            else printf("error: cannot multiply matrices '%s': A(#rows x #cols) and '%s': B(#rows x #cols), unless A(#cols='%d') == B(#rows='%d')\n", inFileA, inFileB, cA, rB);  

            free(B); // free B pointer
            free(A); // free A pointer

        } // otherwise, file I/O error occurred
        
        GET_TIME(finishOA); // get overall program finish time
        elapsedOA=finishOA-startOA; // calc overall program elapsed time
        overallGF = gfOP/elapsedOA;  // calc overall program gigaflops per second

        // return mm and overall results (if any) for elapsed time and gigaflops for second 
        printf("C(#rows) = %d, C(#cols) = %d, OverallElaspedTime = %f sec, MMElaspedTime = %f sec, OverallGF = %f GF/sec, MMGF = %f GF/sec \n", rA, cB, elapsedOA, elapsedMM, overallGF, mmGF);
        
    } // else, print usage stmt for correct args
    else printf("usage: %s <input_fileA.dat> <input_fileB.dat> <outputFile.dat>\n", argv[0]);

    exit(retCode); // exit with current return code status
}
