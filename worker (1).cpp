#include <stdio.h>   // For printf and fprintf
#include <stdlib.h>  // For atoi (convert string to integer)
#include <math.h>    // For sqrt (square root function)

int main(int argc, char *argv[]) {

	// Check if the user provided exactly two command line arguments
	if (argc != 3) {
		// Print error message showing correct usage format
		fprintf(stderr, "Usage: %s <lower_bound> <upper_bound>\n", argv[0]);
		return 1;  // Exit the program
	}

	// Convert the command line arguments (strings) to integers
	int lowerBound = atoi(argv[1]);
	int upperBound = atoi(argv[2]);


	char primes[] = "Primes : ";
	char newline[] = " ";


	printf("%s", primes);

	// Loop through all numbers in the given range
	for (int i = lowerBound; i <= upperBound; i++) {

		// Skip numbers less than or equal to 1 (not prime)
		if (i <= 1) continue;

		int isPrime = 1;  // Assume the number is prime initially

		// Check if 'i' has any divisor other than 1 and itself
		for (int j = 2; j <= sqrt(i); j++) {
			if (i % j == 0) {  // If divisible, it's not a prime
				isPrime = 0;
				break;  // No need to check further
			}
		}

		// If still prime, print the number
		if (isPrime) {
			printf("%d ", i);
		}
	}

	// Print a space (or newline) after listing primes
	printf("%s\n", newline);

	return 0;  // Program finished successfully
}
