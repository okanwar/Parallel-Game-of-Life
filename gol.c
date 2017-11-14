/**
 * File: gol.c
 *
 * Starter code for COMP280 Project 6 ("Game of Life")
 *
 * Replace this comment with your top-level comment with your name and a
 * description of this program.
 */

#define _XOPEN_SOURCE 600

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>

int main(int argc, char *argv[]) {

	int ret;
	int verbose = 0;
	char *config_file = NULL;
	int *live_spots;
	int *board;
	
	// Step 1: Parse command line args (I recommend using getopt again).
	// You need to support the "-c" and "-v" options for the basic requirements.
	// The advanced requirements require you to add "-l" and "-n" options.
	
	// Step 2: Read in the configuration file and use it to initialize your game
	// board. Write a function to do this for you.
	
	// Step 3: Start your timer
	
	// Step 4: Simulate for the required number of steps.
	// Again, you should put this in its own function
	
	// Step 5: Stop your timer, calculate amount of time simulation ran for and
	// then print that out.
	
	//Get arguments from command line
	
	while (( ret = getopt( argc, argv, "vc:")) != -1){

		switch(ret){
		
			case 'c':
				config_file = optarg;
				break;
			case 'v':
				verbose = 1;
				break;
			default:
				printf("Default\n");
				exit(0);

		}
	}

	//Check if config_file has been set
	if( config_file == NULL ){
		printf("Error: No config file!\n");
		exit(0);
	}

	//Read config file

	//Simulate GOL
	//


	return 0;
}

/*
 *
 */
void readConfig(char *config_file, int iterations, int *board, int *live_spots){
	
	int rows = 0;
	int cols = 0;

	//Read in from config file

}





