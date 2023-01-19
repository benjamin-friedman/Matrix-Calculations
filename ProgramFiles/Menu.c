/*
  Author: Benjamin G. Friedman
  Date: 05/20/2021
  File: Menu.c
  Description:
	  - Implementation file for the menu interface.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Menu.h"


/***** Helper functions used only in this file *****/
/*
PRECONDITION
  - line is a string holding the user's input for the choice from the menu.
  - rangeStart/rangeEnd are the valid start and endpoints of the range.
POSTCONDITION
  - Returns TRUE, else FALSE for any invalid input.
  - Invalid input - Anything other than a single integer in that range of [0, 7]
*/
static Boolean menuChoiceIsValid(const char* line, int rangeStart, int rangeEnd);




/***** Helper functions defined in Matrix.c *****/
void numberAppender(int n, char* append);
Boolean inputIsValidPositiveInt(const char* line, int expectedNumbers);
Boolean inputIsValidUnsignedInt(const char* line, int expectedNumbers);




/***** Functions declared in Menu.h *****/
void menu_displayMenu(void) {
	printf("------ MENU ------\n");
	for (int i = 0; i < operationsSize; ++i)
		printf("%d) %s\n", i + 1, operations[i]);
	printf("0) quit\n\n");
	printf("Enter the number of the operation to perform or enter 0 to quit.\nChoice: ");
}



int menu_getUserChoice(void) {
	int userChoice;
	Boolean validChoice;
	char line[500];

	do {
		fgets(line, 500, stdin);
		line[strlen(line) - 1] = '\0';
		validChoice = menuChoiceIsValid(line, 0, operationsSize);
		if (!validChoice) {
			printf("Input error. Re-enter input.\nChoice: ");
		}
	} while (!validChoice);
	sscanf(line, "%d", &userChoice);
	printf("\n");

	return userChoice;
}



Status menu_matrixMultiplication(void) {
	int rows1, columns1, rows2, columns2;        // dimensions of the matrices to be multiplied
	Status m1ValidInput, m2ValidInput;           // checks if the input for the entries of the matrices is valid
	Status memoryAllocation;                     // checks for memory allocation failure
	Boolean canBeMultiplied;                     // checks if matrix1/matrix2 can be multiplied
	MATRIX hMatrix1 = NULL;                      // matrix 1 being multiplied
	MATRIX hMatrix2 = NULL;                      // matrix 2 being multiplied
	MATRIX hResult = NULL;                       // result of the multiplication

	// get the dimensions and validate input
	printf("For matrix multiplication, the columns of the first matrix must equal the rows of the second matrix.\n");
	do {
		do { // matrix 1
			m1ValidInput = matrix_getDimensions(&rows1, &columns1, 1, operations[0]);
			if (!m1ValidInput) {
				printf("Input error. Re-enter input.\n");
			}
		} while (!m1ValidInput);

		do { // matrix2
			m2ValidInput = matrix_getDimensions(&rows2, &columns2, 2, operations[0]);
			if (!m2ValidInput) {
				printf("Input error. Re-enter input.\n");
			}
		} while (!m2ValidInput);

		canBeMultiplied = matrix_canBeMultipliedD(columns1, rows2);
		if (!canBeMultiplied) {
			printf("Input error. The columns of matrix 1 must equal the rows of matrix 2 in order "
				"for two matrices to be multiplied.\nRe-enter the dimensions starting with the first matrix.\n");
		}
	} while (!canBeMultiplied);

	// create the matrices
	if (!(hMatrix1 = matrix_init(rows1, columns1)))
		return FAILURE;
	if (!(hMatrix2 = matrix_init(rows2, columns2))) {
		matrix_destroy(&hMatrix1);
		return FAILURE;
	}

	// fill up the entries of the matrices
	do {
		matrix_fillPrompt(hMatrix1, 1);
		m1ValidInput = matrix_fillInput(hMatrix1, &memoryAllocation);
		if (!m1ValidInput) {
			if (!memoryAllocation) {
				printf("\n\nMemory allocation failure during matrix multiplication.\n\n");
				matrix_destroy(&hMatrix1);
				matrix_destroy(&hMatrix2);
				return FAILURE;
			}
			else
				printf("Input error. Re-enter input.\n");
		}
	} while (!m1ValidInput);

	do {
		matrix_fillPrompt(hMatrix2, 2);
		m2ValidInput = matrix_fillInput(hMatrix2, &memoryAllocation);
		if (!m2ValidInput) {
			if (!memoryAllocation) {
				printf("\n\nMemory allocation failure during matrix multiplication.\n\n");
				matrix_destroy(&hMatrix1);
				matrix_destroy(&hMatrix2);
				return FAILURE;
			}
			else
				printf("Input error. Re-enter input.\n");
		}

	} while (!m2ValidInput);

	// implement matrix multiplication
	if (!matrix_multiply(hMatrix1, hMatrix2, &hResult)) {
		matrix_destroy(&hMatrix1);
		matrix_destroy(&hMatrix2);
		matrix_destroy(&hResult);
		return FAILURE;
	}

	// display results and clean up memory
	printf("\nThe two matrices being multiplied are:\n");
	matrix_print(hMatrix1);
	matrix_print(hMatrix2);
	printf("The resulting matrix after multiplication is:\n");
	matrix_print(hResult);

	matrix_destroy(&hMatrix1);
	matrix_destroy(&hMatrix2);
	matrix_destroy(&hResult);

	return SUCCESS;
}



Status menu_matrixAddition(void) {
	int rows, columns;                // dimensions of the matrices
	int numMatrices;                  // number of matrices
	Status validInput;                // valid input for the number of matrices, matrix dimensions, and matrix entries
	Status memoryAllocation;          // checks for memory allocation failure
	MATRIX hResult = NULL;            // result of the addition
	MATRIX* hMatrices = NULL;         // array of matrices
	char append[3] = { '\0' };        // used to append, for example, "st" to 21st

	// get the number of matrices to add
	do {
		validInput = matrix_getNumMatrices("add", &numMatrices);
		if (!validInput)
			printf("Input error. Re-enter input.\n");
	} while (!validInput);

	// get the dimensions of each matrix (all are the same)
	do {
		validInput = matrix_getDimensions(&rows, &columns, 0, operations[1]);
		if (!validInput)
			printf("Input error. Re-enter input.\n");
	} while (!validInput);

	// create the result matrix and the matrix array
	if (!(hResult = matrix_init(rows, columns)))
		return FAILURE;
	if (!(hMatrices = calloc(numMatrices, sizeof(*hMatrices)))) {
		matrix_destroy(&hResult);
		return FAILURE;
	}
	for (int i = 0; i < numMatrices; ++i) {
		if (!(hMatrices[i] = matrix_init(rows, columns))) {
			if (i > 0) {
				for (; i >= 0; i--)
					matrix_destroy(&hMatrices[i]);
			}
			free(hMatrices);
			matrix_destroy(&hResult);
			return FAILURE;
		}
	}

	// fill up the entries of the matrix
	for (int i = 0; i < numMatrices; ++i) {
		numberAppender(i + 1, append);
		do {
			matrix_fillPrompt(hMatrices[i], i + 1);
			validInput = matrix_fillInput(hMatrices[i], &memoryAllocation);
			if (!validInput) {
				if (!memoryAllocation) {
					printf("\n\nMemory allocation failure during matrix addition.\n\n");
					for (int i = 0; i < numMatrices; ++i)
						matrix_destroy(&hMatrices[i]);
					free(hMatrices);
					matrix_destroy(&hResult);
					return FAILURE;
				}
				else
					printf("Input error. Re-enter input for the %d%s matrix.\n", i + 1, append);
			}
		} while (!validInput);
	}

	// implement matrix addition
	if (!matrix_add(hMatrices, numMatrices, &hResult)) {
		for (int i = 0; i < numMatrices; ++i)
			matrix_destroy(&hMatrices[i]);
		free(hMatrices);
		matrix_destroy(&hResult);
		return FAILURE;
	}

	// display results and clean up memory
	printf("The %d matrices being added are:\n", numMatrices);
	for (int i = 0; i < numMatrices; ++i) {
		matrix_print(hMatrices[i]);
	}
	printf("The resulting matrix after addition is:\n");
	matrix_print(hResult);

	for (int i = 0; i < numMatrices; ++i)
		matrix_destroy(&hMatrices[i]);
	free(hMatrices);
	matrix_destroy(&hResult);

	return SUCCESS;
}



Status menu_matrixSubtraction(void) {
	int rows, columns;                // dimensions of the matrices
	int numMatrices;                  // number of matrices
	Status validInput;                // valid input for the number of matrices, matrix dimensions, and matrix entries
	Status memoryAllocation;          // checks for memory allocation failure
	MATRIX hResult = NULL;            // result of the subtraction
	MATRIX* hMatrices = NULL;         // array of matrices
	char append[3] = { '\0' };        // used to append, for example, "st" to 21st

	// get the number of matrices to subtract
	do {
		validInput = matrix_getNumMatrices("subtract", &numMatrices);
		if (!validInput)
			printf("Input error.. Re-enter input.\n");
	} while (!validInput);

	// get the dimensions of each matrix (all are the same)
	do {
		validInput = matrix_getDimensions(&rows, &columns, 0, operations[2]);
		if (!validInput)
			printf("Input error. Re-enter input.\n");
	} while (!validInput);

	// create the result matrix and the matrix array
	if (!(hResult = matrix_init(rows, columns)))
		return FAILURE;
	if (!(hMatrices = calloc(numMatrices, sizeof(*hMatrices)))) {
		matrix_destroy(&hResult);
		return FAILURE;
	}
	for (int i = 0; i < numMatrices; ++i) {
		if (!(hMatrices[i] = matrix_init(rows, columns))) {
			if (i > 0) {
				for (; i >= 0; i--)
					matrix_destroy(&hMatrices[i]);
			}
			free(hMatrices);
			matrix_destroy(&hResult);
			return FAILURE;
		}
	}

	// fill up the entries of the matrices
	for (int i = 0; i < numMatrices; ++i) {
		numberAppender(i + 1, append);
		do {
			matrix_fillPrompt(hMatrices[i], i + 1);
			validInput = matrix_fillInput(hMatrices[i], &memoryAllocation);
			if (!validInput) {
				if (!memoryAllocation) {
					printf("\n\nMemory allocation failure during matrix subtraction.\n\n");
					for (int i = 0; i < numMatrices; ++i)
						matrix_destroy(&hMatrices[i]);
					matrix_destroy(&hResult);
					free(hMatrices);
					return FAILURE;
				}
				else
					printf("Input error. Re-enter input for the %d%s matrix.\n", i + 1, append);
			}
		} while (!validInput);
	}

	// implement matrix subtraction
	if (!matrix_subtract(hMatrices, numMatrices, &hResult)) {
		for (int i = 0; i < numMatrices; ++i)
			matrix_destroy(&hMatrices[i]);
		matrix_destroy(&hResult);
		free(hMatrices);
		return FAILURE;
	}

	// display results and clean up memory
	printf("The %d matrices being subtracted are:\n", numMatrices);
	for (int i = 0; i < numMatrices; ++i) {
		matrix_print(hMatrices[i]);
	}
	printf("The resulting matrix after subtraction is:\n");
	matrix_print(hResult);

	matrix_destroy(&hResult);
	for (int i = 0; i < numMatrices; ++i)
		matrix_destroy(&hMatrices[i]);
	free(hMatrices);

	return SUCCESS;
}



Status menu_matrixPower(void) {
	int rows, columns, power;        // power and dimensions of the matrix
	char line[500];                  // buffer to get line of user input
	Boolean validInputPower;         // valid input for the matrix power - inputIsValidPositiveInt returns a Boolean
	Status validInput;               // valid input for the matrix dimensions and entries
	Status memoryAllocation;         // checks for memory allocation failure
	MATRIX hMatrix = NULL;           // matrix to perform the power operation on
	MATRIX hResult = NULL;           // result of the power operation

	// get the matrix power and the dimensions
	do {
		printf("Enter the matrix power. It must be an integer that is at least 1.\n");
		fgets(line, 500, stdin);
		line[strlen(line) - 1] = '\0';
		validInputPower = inputIsValidPositiveInt(line, 1);
		if (!validInputPower)
			printf("Input error. Re-enter input.\n");
	} while (!validInputPower);
	sscanf(line, "%d", &power);

	do {
		validInput = matrix_getDimensions(&rows, &columns, -1, operations[3]);
		if (!validInput || rows != columns)
			printf("Input error. Re-enter input.\n");
	} while (!validInput || rows != columns);

	// create the matrix
	if (!(hMatrix = matrix_init(rows, columns)))
		return FAILURE;

	// fill up the entries of the matrix
	do {
		matrix_fillPrompt(hMatrix, 0);
		validInput = matrix_fillInput(hMatrix, &memoryAllocation);
		if (!validInput) {
			if (!memoryAllocation) {
				printf("\n\nMemory allocation failure during the matrix power operation.\n\n");
				matrix_destroy(&hMatrix);
				return FAILURE;
			}
			else
				printf("Input error. Re-enter input.\n");
		}
	} while (!validInput);

	// implement the power operation
	if (!matrix_power(hMatrix, power, &hResult)) {
		matrix_destroy(&hMatrix);
		return FAILURE;
	}

	// display results and clean up memory
	printf("The original matrix in the power operation is:\n");
	matrix_print(hMatrix);
	printf("The result of the matrix raised to the power of %d is:\n", power);
	matrix_print(hResult);

	matrix_destroy(&hResult);
	matrix_destroy(&hMatrix);

	return SUCCESS;
}



Status menu_matrixTranspose(void) {
	int rows, columns;              // dimensions of the matrix
	Status validInput;              // valid input for the matrix dimensions and entries
	Status memoryAllocation;        // checks for memory allocation failure
	MATRIX hMatrix = NULL;          // matrix to be transposed
	MATRIX hResult = NULL;          // result of the the transpose operation

	// get the dimensions of the matrix to transpose
	do {
		validInput = matrix_getDimensions(&rows, &columns, -1, operations[4]);
		if (!validInput)
			printf("Input error. Re-enter input.\n");
	} while (!validInput);

	// create the matrix
	if (!(hMatrix = matrix_init(rows, columns)))
		return FAILURE;

	// fill up the entries of the matrix
	do {
		matrix_fillPrompt(hMatrix, 0);
		validInput = matrix_fillInput(hMatrix, &memoryAllocation);
		if (!validInput) {
			if (!memoryAllocation) {
				printf("\n\nMemory allocation failure during the matrix transpose operation.\n\n");
				matrix_destroy(&hMatrix);
				return FAILURE;
			}
			else
				printf("Input error. Re-enter input.\n");
		}
	} while (!validInput);

	// implement the matrix transpose operation
	if (!matrix_transpose(hMatrix, &hResult)) {
		matrix_destroy(&hMatrix);
		return FAILURE;
	}

	// display results and clean up memory
	printf("The matrix being transposed is:\n");
	matrix_print(hMatrix);
	printf("The resulting transposed matrix is:\n");
	matrix_print(hResult);

	matrix_destroy(&hResult);
	matrix_destroy(&hMatrix);

	return SUCCESS;
}



Status menu_matrixDeterminant(void) {
	int rows, columns;              // dimensions of the matrix
	long double determinant;        // the result of the determinant operation
	Status validInput;              // valid input for the number of matrices/dimensions
	Status memoryAllocation;        // checks for memory allocation failure
	MATRIX hMatrix = NULL;          // matrix to perform the determinant operation on

	// get the dimensions
	do {
		validInput = matrix_getDimensions(&rows, &columns, -1, operations[5]);
		if (!validInput || rows != columns) {
			printf("Input error. Re-enter input.\n");
		}
	} while (!validInput || rows != columns);

	// create the matrix
	if (!(hMatrix = matrix_init(rows, columns)))
		return FAILURE;

	// fill up the entries of the matrix
	do {
		matrix_fillPrompt(hMatrix, 0);
		validInput = matrix_fillInput(hMatrix, &memoryAllocation);
		if (!validInput) {
			if (!memoryAllocation) {
				printf("\n\nMemory allocation failure during the matrix determinant operation.\n\n");
				matrix_destroy(&hMatrix);
				return FAILURE;
			}
			else
				printf("Input error. Re-enter input.\n");
		}
	} while (!validInput);

	// implement the determinant operation
	determinant = matrix_determinant(hMatrix, &memoryAllocation);
	if (!memoryAllocation) {
		matrix_destroy(&hMatrix);
		return FAILURE;
	}

	// display results and clean up memory
	printf("The determinant of the following matrix is %Lg\n", determinant);
	matrix_print(hMatrix);

	matrix_destroy(&hMatrix);

	return SUCCESS;
}



Status menu_matrixInverse(void) {
	int rows, columns;               // dimensions of the matrix
	Status validInput;               // valid input for the matrix dimensions and entries
	Status memoryAllocation;         // checks for memory allocation failure
	Boolean matrixIsVertible;        // checks if the matrix is vertible during the inverse operation
	MATRIX hMatrix = NULL;           // matrix to perform the inverse operation on
	MATRIX hResult = NULL;           // the inverse matrix

	// get the dimensions
	do {
		validInput = matrix_getDimensions(&rows, &columns, -1, operations[6]);
		if (!validInput || rows != columns) {
			printf("Input error. Re-enter input.\n");
		}
	} while (!validInput || rows != columns);

	// create the matrix
	if (!(hMatrix = matrix_init(rows, columns)))
		return FAILURE;

	// fill up the entries of the matrix
	do {
		matrix_fillPrompt(hMatrix, 0);
		validInput = matrix_fillInput(hMatrix, &memoryAllocation);
		if (!validInput) {
			if (!memoryAllocation) {
				printf("\n\nMemory allocation failure during the matrix inverse operation.\n\n");
				matrix_destroy(&hMatrix);
				return FAILURE;
			}
			else
				printf("Input error. Re-enter input.\n");
		}
	} while (!validInput);

	// implement the inverse operation
	if (!matrix_inverse(hMatrix, &hResult, &matrixIsVertible)) {
		// memory allocation failure
		if (matrixIsVertible) {
			printf("\n\nMemory allocation failure during the matrix inverse operation.\n\n");
			matrix_destroy(&hMatrix);
			matrix_destroy(&hResult);
			return FAILURE;
		}
		// matrix is invertible
		else {
			printf("The determinant of the following matrix is 0. Therefore, it has no inverse.\n");
			matrix_print(hMatrix);
			matrix_destroy(&hMatrix);
			matrix_destroy(&hResult);
			return SUCCESS;
		}
	}

	// display results and cleanup memory
	printf("The matrix is:\n");
	matrix_print(hMatrix);
	printf("Its inverse is:\n");
	matrix_print(hResult);

	matrix_destroy(&hMatrix);
	matrix_destroy(&hResult);

	return SUCCESS;
}




/***** Helper functions used only in this file *****/
static Boolean menuChoiceIsValid(const char* line, int rangeStart, int rangeEnd) {
	return inputIsValidUnsignedInt(line, 1) && atoi(line) >= rangeStart && atoi(line) <= rangeEnd;
}
