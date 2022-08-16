/*
  Author: Benjamin G. Friedman
  Date: 05/20/2021
  File: Menu.h
  Description:
      - Header file for the menu interface.
*/


#ifndef MENU_H
#define MENU_H


#include "Matrix.h"




/***** Functions defined in Menu.c *****/
/*
PRECONDITION
  - None.
POSTCONDITION
  - Displays the menu.
*/
void menu_displayMenu(void);


/*
PRECONDITION
  - None.
POSTCONDITION
  - Returns the user's choice from the menu. User choice is a number in the range of 0 - 7 inclusive.
*/
int menu_getUserChoice(void);


/*
PRECONDITION
  - None.
POSTCONDITION
  - Implements matrix multiplication and returns SUCCESS, else FAILURE for any memory allocation failure.
*/
Status menu_matrixMultiplication(void);


/*
PRECONDITION
  - None.
POSTCONDITION
  - Implements matrix addition and returns SUCCESS, else FAILURE for any memory allocation failure.
  - For example, A + B + C.
*/
Status menu_matrixAddition(void);


/*
PRECONDITION
  - None.
POSTCONDITION
  - Implements matrix subtraction and returns SUCCESS, else FAILURE for any memory allocation failure.
  - For example, A - B - C.
*/
Status menu_matrixSubtraction(void);


/*
PRECONDITION
  - None.
POSTCONDITION
  - Implements the matrix power operation and returns SUCCESS, else FAILURE for any memory allocation failure.
  - For example, A^4.
*/
Status menu_matrixPower(void);


/*
PRECONDITION
  - None.
POSTCONDITION
  - Implements the matrix transpose operation and returns SUCCESS, else FAILURE for any memory allocation failure.
  - For example, A^T.
*/
Status menu_matrixTranspose(void);


/*
PRECONDITION
  - None.
POSTCONDITION
  - Implements the matrix inverse operation and returns SUCCESS, else FAILURE for any memory allocation failure.
  - For example, A^-1.
*/
Status menu_matrixInverse(void);


/*
PRECONDITION
  - None.
POSTCONDITION
  - Implements the matrix determinant operation and returns SUCCESS, else FAILURE for any memory allocation failure.
  - For example, det(A).
*/
Status menu_matrixDeterminant(void);


#endif
