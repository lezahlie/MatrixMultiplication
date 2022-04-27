#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

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

void init2D(double ** M, int rows, int cols){

    int count  = 1; // set count to start at 0
    // loop in row major order
    for(int a = 0; a < rows; a++)   
        for(int b = 0; b < cols; b++)  
            M[a][b] = count++; // store incrementing count in each index
         
}   

int writeMatrix(double ** M, int rows, int cols, char* oFile) {

    FILE * fp = NULL; 
    // check if inputFile is writable for binary values
    if ((fp = fopen(oFile, "wb")) != NULL) { 

        // write values for result matrix row and col sizes
        fwrite(&rows, sizeof(int), 1, fp); 
        fwrite(&cols, sizeof(int), 1, fp); 
        // write each double in result matrix C as binary into file 
        fwrite(M[0], sizeof(double), rows*cols, fp); 

        // close file, print success, and return ok
        fclose(fp); 
        printf("success: wrote matrix(%d x %d) into file '%s' \n", rows, cols, oFile);
        return 0; // return success
    }
    // if here, print msg for outFile open/write error
    printf("error: cannot open and/or write to file '%s' \n", oFile); 
    return 1; // return error code
}

int main(int argn, char **argv) {

    int retCode = 1; // set return code as error initially

    // check if 4 args are entered
    if (argn == 4) { 

        char * rp,* cp; // pointers for row and col value locations
        // checks each char in row and col args, then converts to base 10 ints
        int  rows = strtol((char *)argv[1], &rp, 10); 
        int  cols = strtol((char *)argv[2], &cp, 10); 
        char * outFile = argv[3]; // assign pointer to outputFile arg
        
        // check if row arg OR col arg is either: not a valid int OR less than 0
        if( (*rp != '\0' ||  rows < 1) || (*cp != '\0' || cols < 1)) {
            // print error message, then exit all nested statements 
            printf("error: <num_rows> and <num_cols> must be valid integers greater than zero\n"); 
        } else  {

            double** M = malloc2D(rows, cols); // allocate 2d array for matrix
            init2D(M, rows, cols);  // initialize 2d array with incrementing values
            retCode = writeMatrix(M, rows, cols, outFile);  // call to write initlized matrix to output file
            free(M); // free M pointer 
        }

    } // else, print usage statement for correct args
    else printf("usage: %s <num_rows> <num_cols> <output data file>\n", argv[0]);

    exit(retCode); // exit with current return value
}