#include <stdio.h>
#include <stdlib.h>

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

int readMatrix(char* iFile) {
    
    FILE * fp = NULL; 
    // check if inputFile is readable for binary value
    if ((fp = fopen(iFile, "rb")) != NULL) { 

        int rows=0, cols=0; // row and col values
         // read values for row and col sizes
        fread(&rows, sizeof(int), 1, fp); 
        fread(&cols, sizeof(int), 1, fp); 

        // allocate 2d array for inputFile matrix
        double ** M = malloc2D(rows, cols); 
        // read 8 byte binary values from file as doubles into 2d array 
        fread(M[0], sizeof(double), rows*cols, fp);

        // close file, then print row/col valuesk
        fclose(fp); // close file pointer 
        printf("#rows = %d, #cols = %d\n", rows, cols); // print each element in matrix

        // loops in row major order
        for(int a=0; a < rows; a++)  { 
            for(int b=0; b < cols; b++)  { 
                // prints values for each row
                printf("%f ", M[a][b]); 
            }printf("\n"); 
        }
        // free M pointer, return ok
        free(M); 
        return 0; 
    }
    // if here print infile open/read error, return error
    printf("error: cannot read binary from file '%s'\n", iFile); 
    return 1; 
}

int main(int argn, char **argv) {

    int retCode = 1; // set return code as 1 initially 
    // check if 2 arguments are passed 
    if (argn == 2) { 

        char * inFile = argv[1]; // set inFile pointer
        // call read matrix function with inFile param, returns error/success value
        retCode = readMatrix(inFile); 

    } // if here print usage stmt for incorrect params
    else printf("usage: %s <inputFile.dat>\n", argv[0]);
    
    exit(retCode); // exit with current return 
}