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

/*
 * Forward declerations
 */
void readConfig(char *config_file, int *iterations, int *live_spots);
void initBoard( int *board, int *live_spots);
void printArr( int *arr, int size );


int main(int argc, char *argv[]) {

	int ret;
	int verbose = 0;
	char *config_file = NULL;
	int *live_spots;
	int *board;
	int iterations = 0;
	
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
	readConfig( config_file, &iterations, live_spots);
	//Initialize board
	initBoard( board, live_spots);
	//Simulate GOL
	//


	return 0;
}


/*
 *
 */
void initBoard( int *board, int *live_spots){

}


 /*
 *
 */
void readConfig(char *config_file, int *iterations, int *live_spots){
	
	int rows = 0;
	int cols = 0;
	int ret;
	char scan[50];
	int num = 0;
	int count = 1;
	int num_live_spots;

	//Read in from config file
	FILE *cfg = NULL;
	if( (cfg = fopen( config_file, "r")) == NULL ){
		printf("Could not open file: %s\n", config_file);
		return;
	}

	while( ret = fscanf( cfg, "%s", scan) > 0){
		//Convert string to int
		num = atoi(scan);
		//Use input from config
		switch( count ){
			
			case 1: 		//Set first input to rows
				rows = num;
				count++;
				break;
			case 2:			//Set second input to cols
				cols = num;
				count++;
				break;
			case 3:			//Set third input to iterations
				*iterations = num;
				count++;
				break;
			case 4:			//Fourth input is for number of live spots
				//Initialize live_spots
				num_live_spots = num;
				live_spots = calloc ( num_live_spots*2, sizeof(int) );
				count++;
				break;
			default: 		//Rest of the arguments are live spots so store them
				live_spots[count-5] = num;
				count++;
				break;
		}
	}	

}

void printArr( int *arr, int size ){
	
	for( int i = 0; i < size; i++ ){
		printf("%d\n", arr[i] );
	}
}



