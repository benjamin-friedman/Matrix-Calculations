/*
  Author: Benjamin G. Friedman
  Date: 05/20/2021
  File: Matrix.h
  Description:
      - Header file for the matrix interface.
*/


#ifndef MATRIX_H
#define MATRIX_H


#include "Status.h"


/***** Global variables, macros, and opaque object handle *****/
typedef void* MATRIX;                // opaque object handle for matrix objects
#define OUT_OF_BOUNDS -909090        // error code for going out of bounds of a matrix object's array

extern const char* operations[];    // the various matrix operations that can be performed
extern const int operationsSize;




/*
PRECONDITION
  - rows/columns are the desired dimensions of the new matrix and are >= 1.
POSTCONDITION
  - Returns a handle to a matrix object with the given amount of rows and columns, else NULL for any
    memory allocation failure.
*/
MATRIX matrix_init(int rows, int columns);


/*
PRECONDITION
  - pRows/pColumns are pointers to the integers to store the dimenions.
  - n dicates what the prompt looks like (described in the POSTCONDITION) and is in range [-1, ...).
  - operation is a string that will be printed indicating which matrix operation is being performed.
POSTCONDITION
  - Prompts the user to enter the dimensions of the matrix. If n == -1, prints 'matrix' in the prompt.
    If n == 0, prints 'matrices' in the prompt. If n is anything else like 1 for example, it would print
    "1st" matrix.
  - Valid input - Stores the desired rows and columns in the variables pointed to by pRows and pColumns
    and returns SUCCESS.
  - Invalid input - Stores -1 in the variables pointed to by pRows/pColumns and returns FAILURE.
  - Valid input is two integers >= 1.
*/
Status matrix_getDimensions(int* pRows, int* pColumns, int n, const char* operation);


/*
PRECONDITION
  - operation is a string that will print indicating whether the function call is for addition or subtraction.
  - pNumMatrices is a pointer to the integer that the number of matrices will be stored in.
POSTCONDITION
  - Prompts the user to enter the number of matrices to be added or subtracted.
  - Valid input - Stores the desired number of matrices to add/subtract in the integer pointed to by
    pNumMatrices and returns SUCCESS.
  - Invalid input - Stores 0 in the variable pointed to by pNumMatrices and returns FAILURE.
  - Valid input is a single integer >= 2.
*/
Status matrix_getNumMatrices(const char* operation, int* pNumMatrices);


/*
PRECONDITION
  - columns1/rows2 are the partial dimensions of two matrices respectively
    (this is all that is needed to see if the two matrices can be multiplied)
POSTCONDITION
  - Returns TRUE if the two matrices can be multiplied, else FALSE.
  - In order for them to be capable of multiplying, columns1 must equal rows2 i.e. 2 x 3 and 3 x 4.
*/
Boolean matrix_canBeMultipliedD(int columns1, int rows2);


/*
PRECONDITION
  - hMatrix1 and hMatrix2 are handles to valid matrix objects.
POSTCONDITION
  - Returns TRUE if two matrices can be multiplied, else FALSE.
  - In order for them to be capable of multiplying, the columns of hMatrix1 must equal the rows of hMatrix2
    i.e. 2 x 3 and 3 x 4.
*/
Boolean matrix_canBeMultipliedM(MATRIX hMatrix1, MATRIX hMatrix2);


/*
PRECONDITION
  - hMatrix is a handle to a valid matrix object.
  - pMemoryAllocation is a pointer to a status to check for memory allocation failure.
POSTCONDITION
  - Fills up the rows of the matrix by prompting the user for input.
  - Returns SUCCESS, else FAILURE for any invalid input or memory allocation failure.
  - The status pointed to by pMemoryAllocationFailure is used to differentiate between the two situations.
    If it is input failure, the variable pointed to by pMemoryAllocationFailure is set to SUCCESS.
    If it is memory allocation failure, it is set to FAILURE.
*/
Status matrix_fillInput(MATRIX hMatrix, Status* pMemoryAllocation);


/*
PRECONDITION
  - hMatrix is a handle to a valid matrix object.
  - matrixNumber indicates the matrix for which numbers are being inputted (for example, the 4th matrix in a series
    of 5 matrices being added). It is in range [0, ...).
POSTCONDITION
  - Prompts the user to enter values for the matrix based on the dimensions of the matrix.
  - If matrixNumber is 0, it is ignored. If it is 1, 2... it will print "1st" matrix, "2nd" matrix etc.
*/
void matrix_fillPrompt(MATRIX hMatrix, int matrixNumber);


/*
PRECONDITION
  - hMatrix1 and hMatrix2 are handles to valid matrix objects whose dimensions are appropriate for multiplication.
  - phResult is a pointer to a handle to a valid matrix object or a NULL handle.
POSTCONDITION
  - The resulting matrix from the multiplication is stored in the handle pointed to by phResult.
  - Returns SUCCESS, else FAILURE for any memory allocation failure.
*/
Status matrix_multiply(MATRIX hMatrix1, MATRIX hMatrix2, MATRIX* phResult);


/*
PRECONDITION
  - hMatrices is an array of handles to valid matrix objects all with the same dimensions.
  - hMatricesSize is the number of matrices in the array.
  - phResult is a pointer to a handle to a valid matrix object or a NULL handle.
POSTCONDITION
  - The resulting matrix from the addition is stored in the handle pointed to by phResult.
  - Returns SUCCESS, else FAILURE for any memory allocation failure.
*/
Status matrix_add(MATRIX* hMatrices, int hMatricesSize, MATRIX* phResult);


/*
PRECONDITION
  - hMatrices is an array of handles to valid matrix objects all with the same dimensions.
  - hMatricesSize is the number of matrices in the array.
  - phResult is a pointer to a handle to a valid matrix object or a NULL handle.
POSTCONDITION
  - The resulting matrix from the subtraction is stored in the handle pointed to by phResult.
  - Returns SUCCESS, else FAILURE for any memory allocation failure.
*/
Status matrix_subtract(MATRIX* hMatrices, int hMatricesSize, MATRIX* phResult);


/*
PRECONDITION
  - hMatrix is a handle to valid matrix object.
  - phResult is a pointer to a handle to a valid matrix object or a NULL handle.
  - power is the power to which hMatrix will be raised to.
POSTCONDITION
  - The resulting matrix from the power operation is stored in the handle pointed to by phResult.
  - Returns SUCCESS, else FAILURE for any memory allocation failure.
*/
Status matrix_power(MATRIX hMatrix, int power, MATRIX* phResult);


/*
PRECONDITION
  - hMatrix is a handle to a valid matrix object.
  - phResult is a pointer to a handle to a valid matrix object or a NULL handle.
POSTCONDITION
  - The resulting matrix from the transpose operation is stored in the handle pointed to by phResult.
  - Returns SUCCESS, else FAILURE for any memory allocation failure.
*/
Status matrix_transpose(MATRIX hMatrix, MATRIX* phResult);


/*
PRECONDITION
  - hMatrix is a handle to a valid matrix object that is a square matrix (i.e. dimensions are n x n).
  - pMemoryAllocation is a pointer to a Status.
POSTCONDITION
  - Returns the determinant of the matrix and sets the Status pointed to by pMemoryAllocation to SUCCESS.
  - Returns 0 and sets the Status pointed to by pMemoryAllocation to FAILURE for any memory allocation failure.
    It does not directly set pMemoryAllocation to FAILURE. This is done by a helper function.
*/
long double matrix_determinant(MATRIX hMatrix, Status* pMemoryAllocation);


/*
PRECONDITION
  - hMatrix is a handle to a valid matrix object that is a square matrix (i.e. dimensions are n x n).
  - phResult is a pointer to a handle to a valid matrix object or a NULL handle.
  - pMatrixIsVertible is a pointer to a Boolean to indicate if the matrix is vertible or not.
POSTCONDITION
  - Vertible Matrix - Stores the inverse of hMatrix in the handle pointed to by phResult and sets the Boolean pointed to
    by pMatrixIsVertible to TRUE. Returns SUCCESS.
  - Invertible Matrix (determinant = 0) - Sets the Boolean pointed to by pMatrixIsVertible to FALSE. Returns FAILURE.
  - Memory allocation failure - Sets the Boolean pointed to by pMatrixIsVertible to TRUE. Returns FAILURE.
*/
Status matrix_inverse(MATRIX hMatrix, MATRIX* phResult, Boolean* pMatrixIsVertible);


/*
PRECONDITION
  - hMatrix1 and hMatrix2 are handles to valid matrix objects.
POSTCONDITION
  - Returns TRUE if the matrices can be added, else FALSE. For two matrices to be added they must be the same dimensions.
*/
Boolean matrix_canBeAdded(MATRIX hMatrix1, MATRIX hMatrix2);


/*
PRECONDITION
  - hMatrix1 is a handle to a valid matrix object.
POSTCONDITION
  - Prints out the matrix to stdout.
*/
void matrix_print(MATRIX hMatrix);


/*
PRECONDITION
  - hMatrix is a handle to a valid matrix object.
  - row/column is the location of the entry.
  - pOutOfBounds is a pointer to a Boolean to check for out of bounds in the array.
POSTCONDITION
  - In bounds - Returns the entry stored at the given row/column and sets the Boolean pointed to by pOutOfBounds to FALSE.
  - Out of bounds - Returns OUT_OF_BOUNDS and sets the variable pointed to by pOutOfBounds to TRUE.
*/
long double matrix_getEntry(MATRIX hMatrix, int row, int column, Boolean* pOutOfBounds);


/*
PRECONDITION
  - hMatrix is a handle to a valid matrix object.
  - newEntry is the new entry to be added
  - row/column are the location where the new entry should go in the matrix.
POSTCONDITION
  - In bounds - sets the entry stored at the given row/column and returns SUCCESS.
  - Out of bounds - does nothing with the entry and returns FAILURE.
*/
Status matrix_setEntry(MATRIX hMatrix, long double newEntry, int row, int column);


/*
PRECONDITION
  - hMatrix is a handle to a valid matrix object.
  - phResult is a pointer to a handle to a valid matrix object or a NULL handle.
POSTCONDITION
  - Stores a deep copy of hMatrix in the handle pointed to by phResult and returns SUCCESS.
  - Returns FAILURE for any memory allocation failure.
*/
Status matrix_assignment(MATRIX hMatrix, MATRIX* phResult);


/*
PRECONDITION
  - phResult is a pointer to a handle to a valid matrix object or a NULL handle.
POSTCONDITION
  - Frees all memory associated with the matrix handle and sets the handle to NULL.
    If the handle were NULL before the function call, it would be ignored.
*/
void matrix_destroy(MATRIX* phMatrix);


#endif
