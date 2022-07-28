#include <time.h>
#include "lexer.h"

clock_t start_time, end_time;
double total_CPU_time, total_CPU_time_in_seconds;
int choice;

int main(int argc, char *argv[])
{

	if (argc < 2)
	{
		printf("Add filenames as a command line argument");
		exit(1);
	}

	do
	{
		printf("\n0. Exit\n1. Remove comments\n2. Print tokens\n3. Print parse tree to file\n4. Time taken to generate parse tree\n");
		printf("Enter a number from 0-4: ");
		scanf("%d", &choice);
		switch (choice)
		{
		case 0:
			break;

		case 1:
			removeComments(argv[1], "cleanfile.txt");
			break;

		case 2:
			printLexer(argv[1]);
			break;

		case 3:
			//  use runLexer() to generate tokenfile
			//  parser should read from tokenfile
			//  print to tree file
			//  print all errors on console
			break;

		case 4:
			start_time = clock();

			runLexer(argv[1]);

			end_time = clock();
			total_CPU_time = (double)(end_time - start_time);
			total_CPU_time_in_seconds = total_CPU_time / CLOCKS_PER_SEC;
			printf("Total CPU time: %lf\n",total_CPU_time);
			printf("Seconds: %lf\n",total_CPU_time_in_seconds);
			break;

		default:
			break;
		}
	} while (choice);
}