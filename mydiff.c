#include <stdio.h>
#include <stdlib.h>
#define getName(name) #name

double *malloc1D(int rows, int cols) {

   // allocates  array set to matrix pointer
   double *M = (double*)malloc(rows*cols*sizeof(double));
   return M; 
}

double TSSE(double *A, double * B, int rows, int cols) {

    double sumSE = 0.0;
    // compare input matrices A versus B
    for (int a = 0;  a < rows; a++) {   
        for (int b = 0;  b < cols; b++) {   
           double mDiff = (A[a*cols+b] - B[a*cols+b]);  // mDiff = (A[a][b]-B[a][b])
           sumSE += (mDiff * mDiff);  // sum = sum + (A[a][b]-B[a][b])^2
        }
    }
    return sumSE;  // returns the total squared sum of error
}

double AVGPRE(double *A, double * B, int rows, int cols) {

    double sumRE = 0.0, msize=rows*cols; 
    // compare input matrices A versus B
    for (int a = 0;  a < rows; a ++) {   
        for (int b = 0;  b < cols; b++) {
            double avgDiff = (A[a*cols+b]-B[a*cols+b])/A[a*cols+b];
            if (avgDiff < 0) avgDiff = avgDiff *(-1);   // avgDiff = |(A[a][b]-B[a][b])/A[a][b]|
            sumRE += avgDiff; // sumRE = sumRE + |(A[a][b]-B[a][b])/A[a][b]|
        } 
    }
    return (1/msize)*sumRE;  // returns average sum of relative error
}

int readMatrix (char * iFile, double **M, int * rows, int * cols, char *mName) {
    
    FILE * fp = NULL;  
    // check if inputFileA is readable for binary values
    if ((fp = fopen(iFile, "rb")) != NULL) { 

        // read values for row and col sizes
        fread(&(*rows), sizeof(int), 1, fp);  
        fread(&(*cols), sizeof(int), 1, fp);  
        
        *M = malloc1D((*rows),(*cols)); // allocate space for matrix 
        fread(M[0], sizeof(double), (*rows)*(*cols), fp); // read binary values from file into matrix

        // close file, return ok
        fclose(fp);     
        return 0; 

    } 
    // if here, print error msg for outFile open/write and retunr error
    printf("error: cannot open and/or read from file '%s'\n", iFile);
    return 1; 
}

int main(int argn, char **argv) {

    int retCode = 1; // set return code as error initially 
    // check if 3 arguments are passed 
    if (argn == 3) { 

        char * inFileA = argv[1], * inFileB = argv[2];  // assign pointers to inputFile args
        double *A, *B;  // assign pointers for 1d arrays
        int rA = 0, cA = 0, rB = 0, cB = 0;  // values for matrix rows/cols
        double tsse = 0.0, avgpre= 0.0; // values for total sum of squared error and average percent of relative error

        // call read matrix function with inFile param, returns error/success value
        if(readMatrix(inFileA, &A, &rA, &cA, getName(A)) == 0 && readMatrix(inFileB, &B, &rB, &cB, getName(B)) == 0) {

            // check if #rows and #cols for both input matrices are equal
            if  ((rA != rB) || (cA != cB))  {
                printf("error: '%s'= A(%d x %d) and '%s'= B(%d x %d) both must have the same #rows and #cols\n", inFileA, rA, cA, inFileB, rB, cB);
            } else{

                tsse = TSSE(A, B, rA, cB); // get Total Sum of Squared Error result 
                avgpre = AVGPRE(A, B, rA, cB); // get Avg Percent Relative Error result 
                // print results and set return code to ok
                printf("A(%d x %d) vs. B(%d x %d): TSSE(Total Sum of Squared Error) = %f, AVGPRE(Avg Percent Relative Error) = %f%% \n", rA, cA, rB, cB, tsse, avgpre);
                retCode = 0; 
            }
        }
    } // if here print usage stmt for incorrect params
    else printf("usage: %s <inputFileA.dat> <inputFileB.dat>\n", argv[0]);

    exit(retCode); // exit with current return code status
}