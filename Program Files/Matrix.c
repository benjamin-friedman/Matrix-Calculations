/*
  Copyright (C) 2021 Benjamin G. Friedman
  File Description:
      - Name: Matrix.c
      - Description: Implementation file for the matrix interface.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "Matrix.h"


typedef struct matrix {
	long double* matrix;        // 2D array
	int rows;                   // total rows
	int columns;                // total columns
	int maxLength;              // max width of a number out of the entire array i.e -425.73 has a width of 7 (5 numbers, '.', and '-')
} Matrix;


// the various matrix operations that can be performed
const char* operations[] = { "multiplication", "addition", "subtraction", "power", "transpose", "determinant",  "inverse" };
const int numOperations = sizeof(operations) / sizeof(*operations);


/***** helper functions defined in this file *****/
/*
PRECONDITION
  - n is any integer
  - append is the character array to hold the append string. It has a capacity of 3 with a null terminator
    at index 2.
POSTCONDITION
  - Based on what n is, append now holds the append string.
    For example, if n is 21 the append string is "st" for 21st.
*/
void numberAppender(const int n, char* append);


/*
PRECONDITION
  - n is any double
POSTCONDITION
  - Calculates the total number of characters comprising the number.
    If it's an integer this will just be the length of the integer (i.e. 1024 = length 4).
    If it's floating point, then it will not include trailing zeros.
*/
int calcNumLength(const long double n);


/*
PRECONDITION
  - numString is a string containing the string representation of a number.
POSTCONDITION
  - Removes trailing zeroes from the string representation of a number.
    For example, the string "24.1720000" becomes "24.172".
*/
void removeTrailingZeroes(char* numString);


/*
PRECONDITION
  - None
POSTCONDITION
  - Returns the derminant of a 2 x 2 matrix. In a 2 x 2 matrix, the entries
    a11, a12, a21, and a22 are at indices 00, 01, 10, and 11 respectively.
*/
long double calculate2x2determinate(const long double a11, const long double a12, const long double a21, const long double a22);


/*
PRECONDITION
  - pMatrix is a pointer to a valid square matrix object.
  - pMemoryAllocation is a pointer to a Status to check for memory allocation failure.
  - The function is a helper function for the matrix_determinant function.
POSTCONDITION
  - Returns the determinant of pMatrix and sets the Status pointed to by pMemoryAllocation to SUCCESS.
  - Returns 0 and sets the Status pointed to by pStatus to FAILURE for any memory allocation failure.
  - The mathematical definition of the determinant is recursive so this function implements the recursion.
    It calls calculate2x2determinate in its base case which returns the determinant of each individual 2 x 2 submatrix.
*/
long double calculateDeterminate(Matrix* pMatrix, Status* pMemoryAllocation);


/*
PRECONDITION
  - pMatrix is a pointer to a valid square matrix object.
  - ppMatrix is a pointer to a pointer to a valid matrix object or a pointer that's NULL.
POSTCONDITION
  - Stores the adjugate of pMatrix in the pointer pointed to by ppMatrix.
    For a given non-2 x 2 matrix A, the adjugate matrix is the transpose of the matrix of cofactors of A.
    For a 2 x 2 matrix, the adjugate matrix does not involve the transpose. It is a special case handled separately.
  - Returns SUCCESS, else FAILURE for any memory allocation failure.
*/
Status calculateAdjugateMatrix(Matrix* pMatrix, Matrix** ppResult);


/*
PRECONDITION
  - line is a string containing any amount of potentially valid floating point numbers.
  - expectedNumbers is the expected amount of floating point numbers to be found in the line.
POSTCONDITION
  - Returns TRUE if the input is valid, else FALSE.
  - Valid input is all valid floating point numbers, and the amount of numbers entered matches expectedNumbers.
*/
Boolean inputIsValidDouble(const char* line, const int expectedNumbers);


/*
PRECONDITION
  - line is a string containing any amount of potentially valid positive integers.
  - expectedNumbers is the expected amount of positive integers to be found in the line.
POSTCONDITION
  - Returns TRUE if the input is valid, else FALSE.
  - Valid input is integers > 0, and the amount of integers entered matches expectedNumbers.
*/
Boolean inputIsValidPositiveInt(const char* line, const int expectedNumbers);


/*
PRECONDITION
  - line is a string containing any amount of potentially valid unsigned integers.
  - expectedNumbers is the expected amount of unsigned integers to be found in the line.
POSTCONDITION
  - Returns TRUE if the input is valid, else FALSE.
  - Valid input is integers >= 0, and the amount of integers entered matches expectedNumbers.
*/
Boolean inputIsValidUnsignedInt(const char* line, const int expectedNumbers);


/*
PRECONDITION
  - line is a string containing only floating point numbers.
  - a is an array of doubles.
  - size is the size of the array a.
POSTCONDITION
  - The numbers in line are stored as doubles in a and returns SUCCESS.
  - Returns FAILURE if the index of the array goes out of bounds in the case where the amount
    of numbers in line is greater than size.
*/
Status linestringToArray(const char* line, long double* a, const int size);


/*
PRECONDITION
  - hMatrix is a handle to a valid matrix object.
  - row/column is the entry to look at
  - pOutOfBounds is a pointer to a Boolean to check for out of bounds in the array or is NULL.
POSTCONDITION
  - Since the 2D matrix is implemented as a 1D array, returns the actual index in the 1D array using the row-column coordinates
    of the conceptual 2D array.
  - In bounds - Returns the index and sets the Boolean pointed to by pOutOfBounds to FALSE.
  - Out of bounds - Returns OUT_OF_BOUNDS and sets the Boolean pointed to by pOutOfBounds to TRUE.
  - If pOutOfBounds is NULL, then it is ignored. This is for cases where it's known it won't be out of bounds which happens
    consistently in this program. The checking for out of bounds is just an additional optional feature.
*/
int at(MATRIX hMatrix, const int row, const int column, Boolean* pOutOfBounds);


/* 
PRECONDITION
  - ppMatrix is a pointer to a pointer to a valid matrix object or a pointer that's NULL.
  - rows/columns are the new dimensions the matrix should be adjusted to.
POSTCONDITION:
  - If the matrix object does not exist, a new matrix object with the correct dimensions is created.
  - If the matrix exists, its dimensions are checked. If its dimensions are incorrect, a new matrix array
    is created to adjust it to the proper dimensions.
  - Returns SUCCESS, else FAILURE for any memory allocation failure.
*/
Status adjustMatrixDimensions(Matrix** ppMatrix, const int rows, const int columns);


/***** functions declared in Matrix.h *****/
MATRIX matrix_init(const int rows, const int columns) {
	Matrix* pMatrix = (Matrix*)malloc(sizeof(*pMatrix));
	if (pMatrix) {
		pMatrix->rows = rows;
		pMatrix->columns = columns;
		pMatrix->maxLength = 1;
		if (!(pMatrix->matrix = calloc(rows * columns, sizeof(*(pMatrix->matrix))))) {
			free(pMatrix);
			return NULL;
		}
	}

	return (MATRIX)pMatrix;
}


Status matrix_getDimensions(int* pRows, int* pColumns, const int n, const char* operation) {
	char append[3] = { '\0' };        // holds "st" for 1st, "nd" for 2nd etc.
	char line[500];                   // buffer for line of user input

	// prompt the user to enter the dimensions for the correct matrix
	printf("Enter the desired rows and columns for the ");
	if (n == -1)
		printf("matrix ");
	else if (n == 0)
		printf("matrices ");
	else {
		numberAppender(n, append);
		printf("%d%s matrix ", n, append);
	}
	printf("separated by a space.\n");

	if (!strcmp(operation, operations[3])) {
		printf("For the matrix %s operation, the rows must equal the columns.\n"
			"For example, enter \"3 3\" to create a 3 x 3 matrix.\n", operations[3]);
	}
	else if (!strcmp(operation, operations[5])) {
		printf("For the matrix %s operation, the rows must equal the columns.\n"
			"For example, enter \"3 3\" to create a 3 x 3 matrix.\n", operations[5]);
	}
	else if (!strcmp(operation, operations[6])) {
		printf("For the matrix %s operation, the rows must equal the columns.\n"
			"For example, enter \"3 3\" to create a 3 x 3 matrix.\n", operations[6]);
	}
	else {
		printf("For example, enter \"3 5\" to create a 3 x 5 matrix.\n");
	}

	// get and validate user input for the dimensions
	fgets(line, 500, stdin);
	line[strlen(line) - 1] = '\0';
	if (!inputIsValidPositiveInt(line, 2)) {
		*pRows = -1;
		*pColumns = -1;
		return FAILURE;
	}
	sscanf(line, "%d%d", pRows, pColumns);

	return SUCCESS;
}


Status matrix_getNumMatrices(const char* operation, int* pNumMatrices) {
	char line[500];        // holds line of user input

	// get and validate user input for the number of matrices
	printf("Enter the number of matrices to %s. Must be an integer that is at least 2.\n", operation);
	fgets(line, 500, stdin);
	line[strlen(line) - 1] = '\0';
	if (!inputIsValidPositiveInt(line, 1) || atoi(line) < 2) {
		*pNumMatrices = 0;
		return FAILURE;
	}
	sscanf(line, "%d", pNumMatrices);

	return SUCCESS;
}


Boolean matrix_canBeMultipliedD(const int rows1, const int columns1, const int rows2, const int columns2) {
	return columns1 == rows2;
}


Boolean matrix_canBeMultipliedM(MATRIX hMatrix1, MATRIX hMatrix2) {
	Matrix* pMatrix1 = (Matrix*)hMatrix1;
	Matrix* pMatrix2 = (Matrix*)hMatrix2;
	return pMatrix1->columns == pMatrix2->rows;
}


Status matrix_fillInput(MATRIX hMatrix, Status* pMemoryAllocation) {
	Matrix* pMatrix = (Matrix*)hMatrix;			
	long double* inputRow;               // array to hold the numbers the user enters for any given row
	long double* oldArrayCopy;           // stores a copy of the old matrix entries
	int maxLength = 1;                   // max length of the numbers found
	int numLength;                       // length of a single number
	char line[500];                      // buffer to hold user input
	*pMemoryAllocation = SUCCESS;        // no memory allocation failure yet

	// create the array for a single row
	if (!(inputRow = malloc(pMatrix->columns * sizeof(*inputRow)))) {
		*pMemoryAllocation = FAILURE;
		return FAILURE;
	}

	// copy the old array so the matrix can be preserved if input validation fails
	if (!(oldArrayCopy = malloc(pMatrix->rows * pMatrix->columns * sizeof(*oldArrayCopy)))) {
		*pMemoryAllocation = FAILURE;
		free(inputRow);
		return FAILURE;
	}
	for (int i = 0; i < pMatrix->rows * pMatrix->columns; i++)
		oldArrayCopy[i] = pMatrix->matrix[i];

	// get and validate user input for the entries of the matrix
	for (int i = 0; i < pMatrix->rows; i++) {
		fgets(line, 500, stdin);
		line[strlen(line) - 1] = '\0';
		if (!inputIsValidDouble(line, pMatrix->columns)) {
			free(pMatrix->matrix);
			pMatrix->matrix = oldArrayCopy;
			free(inputRow);
			return FAILURE;
		}
		linestringToArray(line, inputRow, pMatrix->columns);
		for (int j = 0; j < pMatrix->columns; j++) {
			pMatrix->matrix[at(hMatrix, i, j, NULL)] = inputRow[j];
			numLength = calcNumLength(inputRow[j]);
			if (i == 0 && j == 0)
				maxLength = numLength;
			else if (numLength > maxLength)
				maxLength = numLength;
		}
	}
	pMatrix->maxLength = maxLength;

	free(inputRow);
	free(oldArrayCopy);
	printf("\n");

	return SUCCESS;
}


void matrix_fillPrompt(MATRIX hMatrix, const int matrixNumber) {
	Matrix* pMatrix = (Matrix*)hMatrix;
	char append[3] = { '\0' };

	printf("Enter values for the ");
	if (matrixNumber) {
		numberAppender(matrixNumber, append);
		printf("%d%s ", matrixNumber, append);
	}
	printf("%d x %d matrix with each row separated by a newline.\n", pMatrix->rows, pMatrix->columns);
}


Status matrix_multiply(MATRIX hMatrix1, MATRIX hMatrix2, MATRIX* phResult) {
	Matrix* pMatrix1 = (Matrix*)hMatrix1;        // matrix 1 being multiplied
	Matrix* pMatrix2 = (Matrix*)hMatrix2;        // matrix 2 being multiplied

	// recreate the result matrix if its dimensions aren't appropriate for the multiplication or it's NULL
	if (!adjustMatrixDimensions((Matrix**)phResult, pMatrix1->rows, pMatrix2->columns))
		return FAILURE;
	Matrix* pResult = (Matrix*)*phResult;        // result of multiplication
	
	long double sum = 0;               // the sum for each new entry in the result matrix
	int newRow = 0;                    // index tracker of the new matrix's row
	int newColumn = 0;                 // index tracker of the new matrix's column
	int maxLength = 1;                 // max length of the new matrix (same as in the matrix structure).
	int numLength;                     // gets the length of each number to be compared to max length
	Boolean firstNewNum = TRUE;        // TRUE = first number being calculated in the whole result matrix

	// perform the multiplication
	for (int m1row = 0; m1row < pMatrix1->rows; m1row++) {
		newColumn = 0;
		for (int m2column = 0; m2column < pMatrix2->columns; m2column++) {
			sum = 0;
			for (int m1column = 0, m2row = 0; m1column < pMatrix1->columns; m1column++, m2row++) {
				sum += pMatrix1->matrix[at(hMatrix1, m1row, m1column, NULL)] * pMatrix2->matrix[at(hMatrix2, m2row, m2column, NULL)];
			}
			numLength = calcNumLength(sum);
			if (firstNewNum) {
				maxLength = numLength;
				firstNewNum = FALSE;
			}
			else if (numLength > maxLength)
				maxLength = numLength;
			pResult->matrix[at((MATRIX)pResult, newRow, newColumn, NULL)] = sum;
			newColumn++;
		}
		newRow++;
	}
	pResult->maxLength = maxLength;

	return SUCCESS;
}


Status matrix_add(MATRIX* hMatrices, const int numMatrices, MATRIX* phResult) {
	Matrix* pMatrixToAdd = (Matrix*)hMatrices[0];        // first matrix being added

	// recreate the result matrix if its dimensions aren't appropriate for the addition or it's NULL
	if (!adjustMatrixDimensions((Matrix**)phResult, pMatrixToAdd->rows, pMatrixToAdd->columns))
		return FAILURE;
	Matrix* pResult = (Matrix*)*phResult;        // result of addition

	long double sum = 0;               // sum for each new individual term
	int maxLength = 1;                 // max length of the new matrix (same as in the matrix structure).
	int numLength;                     // gets the length of each number to be compared to max length
	Boolean firstNewNum = TRUE;        // TRUE = first number being calculated in the whole result matrix
	
	// perform the addition
	// outer 2 loops for result matrix
	for (int resultRow = 0; resultRow < pMatrixToAdd->rows; resultRow++) {
		for (int resultColumn = 0; resultColumn < pMatrixToAdd->columns; resultColumn++) {
			// inner loop to get the sum
			sum = 0;
			for (int i = 0; i < numMatrices; i++) {
				pMatrixToAdd = (Matrix*)hMatrices[i];
				sum += pMatrixToAdd->matrix[at(hMatrices[i], resultRow, resultColumn, NULL)];
			}
			numLength = calcNumLength(sum);
			if (firstNewNum) {
				maxLength = numLength;
				firstNewNum = FALSE;
			}
			else if (numLength > maxLength)
				maxLength = numLength;
			pResult->matrix[at((MATRIX)pResult, resultRow, resultColumn, NULL)] = sum;
		}
	}
	pResult->maxLength = maxLength;

	return SUCCESS;
}


Status matrix_subtract(MATRIX* hMatrices, const int numMatrices, MATRIX* phResult) {
	Matrix* pMatrixToSubtract = (Matrix*)hMatrices[0];        // 1st matrix, the matrix being subtracted from

	// recreate the result matrix if its dimensions aren't appropriate for the subtraction or it's NULL
	if (!adjustMatrixDimensions((Matrix**)phResult, pMatrixToSubtract->rows, pMatrixToSubtract->columns))
		return FAILURE;
	Matrix* pResult = (Matrix*)*phResult;        // result of subtraction

	long double sum = 0;               // sum for each new individual term
	int maxLength = 1;                 // max length of the new matrix (same as in the matrix structure).
	int numLength;                     // gets the length of each number to be compared to max length
	Boolean firstNewNum = TRUE;        // TRUE = first number being calculated in the whole result matrix

	// perform the subtraction
	// outer 2 loops for result matrix
	for (int resultRow = 0; resultRow < pMatrixToSubtract->rows; resultRow++) {
		for (int resultColumn = 0; resultColumn < pMatrixToSubtract->columns; resultColumn++) {
			// inner loop to get the sum
			sum = 0;
			for (int i = 0; i < numMatrices; i++) {
				pMatrixToSubtract = (Matrix*)hMatrices[i];
				if (i == 0)
					sum += pMatrixToSubtract->matrix[at(hMatrices[i], resultRow, resultColumn, NULL)];
				else
					sum -= pMatrixToSubtract->matrix[at(hMatrices[i], resultRow, resultColumn, NULL)];
			}
			numLength = calcNumLength(sum);
			if (firstNewNum) {
				maxLength = numLength;
				firstNewNum = FALSE;
			}
			else if (numLength > maxLength)
				maxLength = numLength;
			pResult->matrix[at((MATRIX)pResult, resultRow, resultColumn, NULL)] = sum;
		}
	}
	pResult->maxLength = maxLength;

	return SUCCESS;
}


Status matrix_power(MATRIX hMatrix, const int power, MATRIX* phResult) {
	MATRIX hToMultiply = NULL;        // handles used for power operation
	MATRIX hTempResult = NULL;

	// case power = 1
	if (power == 1) {
		if (!matrix_assignment(hMatrix, phResult))
			return FAILURE;
		return SUCCESS;
	}
	// case power = 2
	else if (power == 2) {
		if (!matrix_multiply(hMatrix, hMatrix, phResult))
			return FAILURE;
		return SUCCESS;
	}

	// all other cases
	// do the first multiplication in the power operation
	if (!matrix_multiply(hMatrix, hMatrix, &hToMultiply))
		return FAILURE;
	// do all other multiplications thereafter
	for (int i = 3; i <= power; i++) {
		if (!matrix_multiply(hMatrix, hToMultiply, &hTempResult)) {
			matrix_destroy(&hToMultiply);
			return FAILURE;
		}
		matrix_destroy(&hToMultiply);
		hToMultiply = hTempResult;
		hTempResult = NULL;
	}

	Matrix* pResult = (Matrix*)*phResult;
	if (pResult) {
		free(pResult->matrix);
		free(pResult);
	}
	*phResult = hToMultiply;

	return SUCCESS;
}


Status matrix_transpose(MATRIX hMatrix, MATRIX* phResult) {
	Matrix* pMatrix = (Matrix*)hMatrix;        // the matrix being transposed

	// recreate the result matrix if its dimensions aren't appropriate for the transpose or it's NULL
	if (!adjustMatrixDimensions((Matrix**)phResult, pMatrix->columns, pMatrix->rows))
		return FAILURE;
	Matrix* pResult = (Matrix*)*phResult;        // result of the transpose operation

	// calculate the transpose
	for (int i = 0; i < pMatrix->rows; i++) {
		for (int j = 0; j < pMatrix->columns; j++) {
			pResult->matrix[at((MATRIX)pResult, j, i, NULL)] = pMatrix->matrix[at((MATRIX)pMatrix, i, j, NULL)];
		}
	}
	pResult->maxLength = pMatrix->maxLength;

	return SUCCESS;
}


long double matrix_determinant(MATRIX hMatrix, Status* pMemoryAllocation) {
	Matrix* pMatrix = (Matrix*)hMatrix;
	*pMemoryAllocation = SUCCESS;

	// the determinant of a 1 x 1 matrix is just the single number in the matrix
	if (pMatrix->rows == 1 && pMatrix->columns == 1)
		return pMatrix->matrix[0];

	// all other matrices - 2 x 2, 3 x 3 etc.
	return calculateDeterminate(pMatrix, pMemoryAllocation);
}


Status matrix_inverse(MATRIX hMatrix, MATRIX* phResult, Boolean* pMatrixIsVertible) {
	Matrix* pMatrix = (Matrix*)hMatrix;        // the matrix to be inverted
	Status memoryAllocation;                   // checks for memory allocation failure
	long double determinant;                   // the result of the determinant operation
	*pMatrixIsVertible = TRUE;                 // assume the matrix is vertible

	// recreate the result matrix if its dimensions aren't appropriate for the inverse operation or it's NULL
	if (!adjustMatrixDimensions((Matrix**)phResult, pMatrix->rows, pMatrix->columns))
		return FAILURE;
	Matrix* pResult = (Matrix*)*phResult;        // result of the inverse operation

	// implement the determinant operation and check for memory allocation failure/determinant doesn't exist
	determinant = matrix_determinant(hMatrix, &memoryAllocation);
	if (!memoryAllocation)
		return FAILURE;
	else if (determinant == 0) {
		*pMatrixIsVertible = FALSE;
		return FAILURE;
	}

	if (!calculateAdjugateMatrix(pMatrix, &pResult))
		return FAILURE;
	
	// multiply every term in the adjugate matrix by 1 / determinant - the result of this is the inverse.
	Boolean firstNewNum = TRUE;        // TRUE = first number being calculated in the whole result matrix
	long double newTerm;               // each new term after multiply by 1 / determinant
	int maxLength = 1;                 // max length of the new matrix (same as in the matrix structure).
	int numLength;                     // gets the length of each number to be compared to max length

	for (int i = 0; i < pResult->rows * pResult->columns; i++) {
		newTerm = pResult->matrix[i] / determinant;
		numLength = calcNumLength(newTerm);
		if (firstNewNum) {
			maxLength = numLength;
			firstNewNum = FALSE;
		}
		else if (numLength > maxLength)
			maxLength = numLength;
		pResult->matrix[i] = newTerm;
	}
	pResult->maxLength = maxLength;

	return SUCCESS;
}


Boolean matrix_canBeAdded(MATRIX hMatrix1, MATRIX hMatrix2) {
	Matrix* pMatrix1 = (Matrix*)hMatrix1;
	Matrix* pMatrix2 = (Matrix*)hMatrix2;
	return pMatrix1->rows == pMatrix2->rows && pMatrix1->columns == pMatrix2->columns;
}


void matrix_print(MATRIX hMatrix) {
	Matrix* pMatrix = (Matrix*)hMatrix;
	char numString[500];                              // buffer to hold each number as a string
	int extraSpaces;                                  // will count how many extra spaces to print for each number
	int spacesPerNum = pMatrix->maxLength + 2;        // each number occupies the same fixed space

	// the total spaces horizontally the matrix takes up so it's known how many dashes to print
	int totalSpaces = spacesPerNum * pMatrix->columns + pMatrix->columns + 1;

	// print the matrix
	for (int i = 0; i < totalSpaces; i++)
		printf("-");
	printf("\n");

	for (int i = 0; i < pMatrix->rows; i++) {
		for (int j = 0; j < pMatrix->columns; j++) {
			long double num = pMatrix->matrix[at(hMatrix, i, j, NULL)];
			sprintf(numString, "%Lf", num);
			removeTrailingZeroes(numString);
			printf("|");
			printf("%s", numString);
			extraSpaces = pMatrix->maxLength - strlen(numString);

			while (extraSpaces > 0) {
				printf(" ");
				extraSpaces--;
			}
			printf("  ");
			if (j == pMatrix->columns - 1)
				printf("|");
		}
		printf("\n");
		for (int i = 0; i < totalSpaces; i++)
			printf("-");
		printf("\n");
	}
	printf("\n\n");
}


long double matrix_getEntry(MATRIX hMatrix, const int row, const int column, Boolean* pOutOfBounds) {
	Matrix* pMatrix = (Matrix*)hMatrix;

	long double entry = pMatrix->matrix[at(hMatrix, row, column, pOutOfBounds)];

	return (*pOutOfBounds) ? OUT_OF_BOUNDS : entry;
}


Status matrix_setEntry(MATRIX hMatrix, const long double newEntry, const int row, const int column) {
	Matrix* pMatrix = (Matrix*)hMatrix;

	// out of bounds
	if (row >= pMatrix->rows || column >= pMatrix->columns)
		return FAILURE;

	// in bounds
	pMatrix->matrix[at(hMatrix, row, column, NULL)] = newEntry;

	return SUCCESS;
}


Status matrix_assignment(MATRIX hMatrix, MATRIX* phResult) {
	Matrix* pMatrix = (Matrix*)hMatrix;
	Matrix* pResult = (Matrix*)*phResult;
	MATRIX hNewMatrix = NULL;
	long double* matrix;

	// the matrix object doesn't exist
	if (!pResult) {
		if (!(hNewMatrix = matrix_init(pMatrix->rows, pMatrix->columns)))
			return FAILURE;
		*phResult = hNewMatrix;
		pResult = (Matrix*)*phResult;
	}
	// the matrix object exists but its dimensions are incorrect
	else if (pResult->rows != pMatrix->rows || pResult->columns != pMatrix->columns) {
		if (!(matrix = calloc(pMatrix->rows * pMatrix->columns, sizeof(*matrix))))
			return FAILURE;
		free(pResult->matrix);
		pResult->matrix = matrix;
		pResult->rows = pMatrix->rows;
		pResult->columns = pMatrix->columns;
	}

	// copy the matrix entries
	int matrixSize = pMatrix->rows * pMatrix->columns;
	for (int i = 0; i < matrixSize; i++)
		pResult->matrix[i] = pMatrix->matrix[i];
	pResult->maxLength = pMatrix->maxLength;

	return SUCCESS;
}


void matrix_destroy(MATRIX* phMatrix) {
	Matrix* pMatrix = (Matrix*)*phMatrix;
	if (pMatrix) {
		free(pMatrix->matrix);
		free(pMatrix);
		*phMatrix = NULL;
	}
}




/***** helper functions declared in this file. *****/
void numberAppender(const int n, char* append) {
	// special case
	if (n >= 4 && n <= 20) {
		append[0] = 't';
		append[1] = 'h';
	}
	// st i.e. 21st
	else if ((n - 1) % 10 == 0) {
		append[0] = 's';
		append[1] = 't';
	}
	// nd i.e. 22nd
	else if ((n - 2) % 10 == 0) {
		append[0] = 'n';
		append[1] = 'd';
	}
	// rd i.e 23rd
	else if ((n - 3) % 10 == 0) {
		append[0] = 'r';
		append[1] = 'd';
	}
	// th i.e. 20th, 24th, 25th, 26th etc.
	else {
		append[0] = 't';
		append[1] = 'h';
	}
}


int calcNumLength(const long double n) {
	char numString[20];
	int nInt = (int)floor(n);

	if (nInt == n) {
		sprintf(numString, "%d", nInt);
		return strlen(numString);
	}
	else
		sprintf(numString, "%Lf", n);
	int totalChars = strlen(numString);
	for (int i = strlen(numString) - 1; numString[i] == '0' || numString[i] == '.'; i--) {
		totalChars--;
	}

	return totalChars;
}


void removeTrailingZeroes(char* numString) {
	int i;
	Boolean reachedDecimalPoint = FALSE;

	for (i = strlen(numString) - 1; i >= 0 && (numString[i] == '0' || numString[i] == '.'); i--) {
		if (reachedDecimalPoint == TRUE)
			break;
		if (numString[i] == '.')
			reachedDecimalPoint = TRUE;
		numString[i] = '\0';
	}
}


long double calculate2x2determinate(const long double a11, const long double a12, const long double a21, const long double a22) {
	return a11 * a22 - a21 * a12;
}


long double calculateDeterminate(Matrix* pMatrix, Status* pMemoryAllocation) {
	// base case: 2 x 2 matrix
	if (pMatrix->rows == 2 && pMatrix->columns == 2) {
		long double a11 = pMatrix->matrix[at((MATRIX)pMatrix, 0, 0, NULL)];
		long double a21 = pMatrix->matrix[at((MATRIX)pMatrix, 0, 1, NULL)];
		long double a12 = pMatrix->matrix[at((MATRIX)pMatrix, 1, 0, NULL)];
		long double a22 = pMatrix->matrix[at((MATRIX)pMatrix, 1, 1, NULL)];
		return calculate2x2determinate(a11, a12, a21, a22);
	}

	// all other cases: recursive calculation
	long double sum = 0;
	for (int column = 0; column < pMatrix->columns; column++) {
		// create the recursive submatrix
		MATRIX hSubMatrix = matrix_init(pMatrix->rows - 1, pMatrix->columns - 1);
		if (!hSubMatrix) {
			*pMemoryAllocation = FAILURE;
			return 0;
		}
		Matrix* pSubMatrix = (Matrix*)hSubMatrix;

		// get one part of the recursive sum
		// get the a_1_jth entry
		long double entry = pMatrix->matrix[at((MATRIX)pMatrix, 0, column, NULL)];
		int subMatrix_row = 0;
		int subMatrix_column = 0;
		// get the recursive submatrix
		for (int row = 1; row < pMatrix->rows; row++) {
			subMatrix_column = 0;
			for (int _column = 0; _column < pMatrix->columns; _column++) {
				if (_column == column)
					continue;
				pSubMatrix->matrix[at(hSubMatrix, subMatrix_row, subMatrix_column, NULL)] = pMatrix->matrix[at((MATRIX)pMatrix, row, _column, NULL)];
				subMatrix_column++;
			}
			subMatrix_row++;
		}

		if (column % 2 == 0) {
			sum += entry * calculateDeterminate(pSubMatrix, pMemoryAllocation);
		}
		else {
			sum -= entry * calculateDeterminate(pSubMatrix, pMemoryAllocation);
		}
		matrix_destroy((MATRIX*)(&pSubMatrix));
	}

	return sum;
}


Status calculateAdjugateMatrix(Matrix* pMatrix, Matrix** ppResult) {
	MATRIX hMatrixOfCofactors = NULL;        // the adjugate is the transpose of the matrix of cofactors

	// recreate the result matrix if its dimensions aren't appropriate for the adjugate or it's NULL
	if (!adjustMatrixDimensions(ppResult, pMatrix->rows, pMatrix->columns))
		return FAILURE;	
	Matrix* pResult = *ppResult;
	
	// special case for 1 x 1 matrix
	if (pResult->rows == 1 && pResult->columns == 1) {
		pResult->matrix[0] = (pMatrix->matrix[0] != 0) ? 1 : 0;
		return SUCCESS;
	}
	// special case for a 2 x 2 matrix
	else if (pResult->rows == 2 && pResult->columns == 2) {
		for (int i = 0; i < pResult->rows * pResult->columns; i++)
			pResult->matrix[i] = pMatrix->matrix[i];
		pResult->matrix[at((MATRIX)pResult, 0, 1, NULL)] *= -1;
		pResult->matrix[at((MATRIX)pResult, 1, 0, NULL)] *= -1;
		long double temp = pResult->matrix[at((MATRIX)pResult, 0, 0, NULL)];
		pResult->matrix[at((MATRIX)pResult, 0, 0, NULL)] = pResult->matrix[at((MATRIX)pResult, 1, 1, NULL)];
		pResult->matrix[at((MATRIX)pResult, 1, 1, NULL)] = temp;
		int length1 = calcNumLength(pResult->matrix[at((MATRIX)pResult, 0, 0, NULL)]);
		int length2 = calcNumLength(pResult->matrix[at((MATRIX)pResult, 0, 1, NULL)]);
		int length3 = calcNumLength(pResult->matrix[at((MATRIX)pResult, 1, 0, NULL)]);
		int length4 = calcNumLength(pResult->matrix[at((MATRIX)pResult, 1, 1, NULL)]);
		int maxLength = length1;
		if (length2 > maxLength)
			maxLength = length2;
		if (length3 > maxLength)
			maxLength = length3;
		if (length4 > maxLength)
			maxLength = length4;
		pResult->maxLength = maxLength;
		return SUCCESS;
	}

	// all other matrices - 3 x 3, 4 x 4 etc.
	// create the matrix of cofactors
	if (!(hMatrixOfCofactors = matrix_init(pMatrix->columns, pMatrix->rows))) {
		return FAILURE;
	}
	Matrix* pMatrixOfCofactors = (Matrix*)hMatrixOfCofactors;
	// calculate each entry for the matrix of cofactors
	Status memoryAllocation;           // checks for memory allocation failure
	int maxLength = 1;                 // max length of the new matrix (same as in the matrix structure).
	int numLength;                     // gets the length of each number to be compared to max length
	Boolean firstNewNum = TRUE;        // TRUE = first number being calculated in the whole result matrix
	long double newTerm;               // each new term after multiplying by 1 / determinant

	for (int row = 0; row < pMatrix->rows; row++) {
		for (int column = 0; column < pMatrix->columns; column++) {
			// create the recursive submatrix
			MATRIX hSubMatrix = matrix_init(pMatrix->rows - 1, pMatrix->columns - 1);
			if (!hSubMatrix) {
				matrix_destroy(&hMatrixOfCofactors);
				return FAILURE;
			}
			Matrix* pSubMatrix = (Matrix*)hSubMatrix;

			// get the submatrix
			int subMatrix_row = 0;
			int subMatrix_column = 0;
			for (int _row = 0; _row < pMatrix->rows; _row++) {
				if (_row == row)
					continue;
				subMatrix_column = 0;
				for (int _column = 0; _column < pMatrix->columns; _column++) {
					if (_column == column)
						continue;
					pSubMatrix->matrix[at(hSubMatrix, subMatrix_row, subMatrix_column, NULL)] = pMatrix->matrix[at((MATRIX)pMatrix, _row, _column, NULL)];
					subMatrix_column++;
				}
				subMatrix_row++;
			}

			// get the i, jth entry in the matrix of cofactors
			memoryAllocation = SUCCESS;
			newTerm = calculateDeterminate(pSubMatrix, &memoryAllocation);
			// memory allocation failure
			if (!memoryAllocation) {
				matrix_destroy(&hMatrixOfCofactors);
				return FAILURE;
			}
			// multiply by -1 or not
			if (newTerm != 0 && (row + column) % 2 != 0) {
				newTerm *= -1;
			}
			numLength = calcNumLength(newTerm);
			if (firstNewNum) {
				maxLength = numLength;
				firstNewNum = FALSE;
			}
			else if (numLength > maxLength)
				maxLength = numLength;
			pMatrixOfCofactors->matrix[at(hMatrixOfCofactors, row, column, NULL)] = newTerm;

			matrix_destroy(&hSubMatrix);
		}
	}
	pMatrixOfCofactors->maxLength = maxLength;
		

	// calculate the adjugate and clean up memory
	if (!matrix_transpose(hMatrixOfCofactors, (MATRIX*)ppResult)) {
		matrix_destroy(&hMatrixOfCofactors);
		return FAILURE;
	}

	matrix_destroy(&hMatrixOfCofactors);

	return SUCCESS;
}


Boolean inputIsValidDouble(const char* line, const int expectedNumbers) {
	Boolean negativeAlreadyExists = FALSE;
	Boolean decimalPointAlreadyExists = FALSE;

	// user only pressed "enter" or whitespace followed by "enter" - space is the only valid whitespace character
	int i = 0;
	while (isspace(line[i])) {
		if (line[i] != ' ')
			return FALSE;
		i++;
	}
	if (line[i] == '\0')
		return FALSE;

	// validate it's all numbers in the correct format
	while (line[i] != '\0') {
		// something other than a digit, negative sign, decimal point, or space
		if (!isdigit(line[i]) && line[i] != ' ' && line[i] != '-' && line[i] != '.')
			return FALSE;

		// repeat decimals points/negative signs are ok if they're in separate numbers
		else if (line[i] == ' ') {
			negativeAlreadyExists = FALSE;
			decimalPointAlreadyExists = FALSE;
		}

		// negative character
		else if (line[i] == '-') {
			// Multiple negatives are invalid
			if (negativeAlreadyExists)
				return FALSE;
			else
				negativeAlreadyExists = TRUE;

			// negative is the first character
			if (i == 0) {
				// invalid if the next character isn't a digit or decimal point
				if (!isdigit(line[i + 1]) && line[i + 1] != '.')
					return FALSE;
			}
			// negative that's not the first character
			else {
				// invalid if the previous character isn't a space, or the next character isn't a digit or decimal point.
				if (line[i - 1] != ' ' || (!isdigit(line[i + 1]) && line[i + 1] != '.'))
					return FALSE;
			}
		}

		// decimal point
		else if (line[i] == '.') {
			// Multiple decimal points are invalid.
			if (decimalPointAlreadyExists)
				return FALSE;
			else
				decimalPointAlreadyExists = TRUE;

			// decimal is the first character
			if (i == 0) {
				// invalid if next character isn't a digit
				if (!isdigit(line[i + 1]))
					return FALSE;
			}
			// decimal is not the first character
			else {
				// invalid if the previous character isn't a space, negative sign, or number, or if the next character isn't a number
				if ((!isdigit(line[i - 1]) && line[i - 1] != ' ' && line[i - 1] != '-') || !isdigit(line[i + 1]))
					return FALSE;
			}
		}

		i++;
	}


	// input has been validated - now verify an appropriate amount of numbers has been entered
	i = 0;
	int totalNumbers = 0;
	while (line[i] != '\0') {
		while (line[i] != ' ' && line[i] != '\0') {
			i++;
		}
		totalNumbers++;
		if (line[i] != '\0')
			i++;
	}

	return totalNumbers == expectedNumbers;
}


Boolean inputIsValidPositiveInt(const char* line, const int expectedNumbers) {
	// user only pressed "enter" or whitespace followed by "enter" - space is the only valid whitespace character
	int i = 0;
	while (isspace(line[i])) {
		if (line[i] != ' ')
			return FALSE;
		i++;
	}
	if (line[i] == '\0')
		return FALSE;

	// validate it's all numbers or spaces
	while (line[i] != '\0') {
		// something other than a digit or space
		if (!isdigit(line[i]) && line[i] != ' ')
			return FALSE;
		i++;
	}


	// input has been validated - now verify an appropriate amount of numbers has been entered and that none of the numbers are 0.
	char singleNumStr[50];
	int singleNumInt;
	int idx = 0;
	int totalNumbers = 0;
	i = 0;
	while (line[i] != '\0') {
		while (line[i] != ' ' && line[i] != '\0') {
			singleNumStr[idx++] = line[i++];
		}
		singleNumStr[idx] = '\0';
		idx = 0;
		singleNumInt = atoi(singleNumStr);
		if (singleNumInt == 0)
			return FALSE;
		totalNumbers++;
		if (line[i] != '\0')
			i++;
	}

	return totalNumbers == expectedNumbers;
}


Boolean inputIsValidUnsignedInt(const char* line, const int expectedNumbers) {
	// user only pressed "enter" or whitespace followed by "enter" - space is the only valid whitespace character
	int i = 0;
	while (isspace(line[i])) {
		if (line[i] != ' ')
			return FALSE;
		i++;
	}
	if (line[i] == '\0')
		return FALSE;

	// validate it's all numbers in the correct format
	while (line[i] != '\0') {
		// something other than a digit or space
		if (!isdigit(line[i]) && line[i] != ' ')
			return FALSE;
		i++;
	}

	// input has been validated - now verify an appropriate amount of numbers has been entered
	int totalNumbers = 0;
	i = 0;
	while (line[i] != '\0') {
		while (line[i] != ' ' && line[i] != '\0') {
			i++;
		}
		totalNumbers++;
		if (line[i] != '\0')
			i++;
	}

	return totalNumbers == expectedNumbers;
}


Status linestringToArray(const char* line, long double* a, const int size) {
	char singleNum[50];        // buffer to hold a single number from the line
	int i = 0;                 // index for line
	int j = 0;                 // index for singleNum
	int k = 0;                 // index for a

	while (line[i] != '\0') {
		while (line[i] != ' ' && line[i] != '\0') {
			singleNum[j++] = line[i++];
		}
		singleNum[j] = '\0';
		if (k >= size)
			return FAILURE;
		a[k++] = (long double)strtod(singleNum, NULL);
		if (line[i] != '\0') {
			i++;
			j = 0;
		}
	}
	return SUCCESS;
}


int at(MATRIX hMatrix, const int row, const int column, Boolean* pOutOfBounds) {
	Matrix* pMatrix = (Matrix*)hMatrix;

	// out of bounds
	if (row >= pMatrix->rows || column >= pMatrix->columns) {
		if (pOutOfBounds)
			*pOutOfBounds = TRUE;
		return OUT_OF_BOUNDS;
	}

	// in bounds
	if (pOutOfBounds)
		*pOutOfBounds = FALSE;

	return row * pMatrix->columns + column;
}


Status adjustMatrixDimensions(Matrix** ppMatrix, const int rows, const int columns) {
	Matrix* pMatrix = (Matrix*)*ppMatrix;
	MATRIX hNewMatrix;
	long double* matrix;

	// the matrix object doesn't exist
	if (!pMatrix) {
		if (!(hNewMatrix = matrix_init(rows, columns)))
			return FAILURE;
		*ppMatrix = (Matrix*)hNewMatrix;
	}
	// the matrix object exists but its dimensions are incorrect
	else if (pMatrix->rows != rows || pMatrix->columns != columns) {
		if (!(matrix = calloc(rows * columns, sizeof(*matrix))))
			return FAILURE;
		free(pMatrix->matrix);
		pMatrix->matrix = matrix;
		pMatrix->rows = rows;
		pMatrix->columns = columns;
		pMatrix->maxLength = 1;
	}

	return SUCCESS;
}
