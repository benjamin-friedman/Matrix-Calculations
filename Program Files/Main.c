/*
  Author: Benjamin G. Friedman
  Date: 05/20/2021
  File: Main.c
  Description:
      - The main function for the matrix calculations program.
*/


#include <stdio.h>
#include <stdlib.h>
#include "Menu.h"




int main(void) {
	
	int userChoice;
	do {
		menu_displayMenu();
		userChoice = menu_getUserChoice();
		switch (userChoice) {
		case 1:
			if (!menu_matrixMultiplication()) {
				printf("Error - memory allocation failure. Exiting the program.\n");
				exit(1);
			}
			break;
		case 2:
			if (!menu_matrixAddition()) {
				printf("Error - memory allocation failure. Exiting the program.\n");
				exit(2);
			}
			break;
		case 3:
			if (!menu_matrixSubtraction()) {
				printf("Error - memory allocation failure. Exiting the program.\n");
				exit(3);
			}
			break;
		case 4:
			if (!menu_matrixPower()) {
				printf("Error - memory allocation failure. Exiting the program.\n");
				exit(4);
			}

			break;
		case 5:
			if (!menu_matrixTranspose()) {
				printf("Error - memory allocation failure. Exiting the program.\n");
				exit(5);
			}
			break;
		case 6:
			if (!menu_matrixDeterminant()) {
				printf("Error - memory allocation failure. Exiting the program.\n");
				exit(6);
			}
			break;
		case 7:
			if (!menu_matrixInverse()) {
				printf("Error - memory allocation failure. Exiting the program.\n");
				exit(7);
			}
		default:
			break;
		}
	} while (userChoice);

	
	return 0;
}
