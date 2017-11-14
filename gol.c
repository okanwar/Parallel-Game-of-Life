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

struct Coord {
	int x;
	int y;
};

typedef struct Coord Coord;

struct LiveSpots {
	int num_live_spots;
	Coord *coords;
};

typedef struct LiveSpots LiveSpots;



/*
 * Forward declerations
 */
void readConfig( char *config_file, int *iterations, LiveSpots *live_spots, int *rows, int *cols);
char * initBoard( LiveSpots *live_spots, int rows, int cols);
void printArr( int *arr, int size );


int main(int argc, char *argv[]) {

	int ret;
	int rows, cols;
	int verbose = 0;
	char *config_file = NULL;
	char *board;
	int iterations = 0;

	LiveSpots live_spots;
	
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
	readConfig( config_file, &iterations, &live_spots, &rows, &cols);
	//Initialize board
	board = initBoard( &live_spots, rows, cols);
	//Simulate GOL
	//


	return 0;
}


/*
 *
 */
char * initBoard( LiveSpots *live_spots, int rows, int cols){
	char *board;
	board = calloc( rows*cols, sizeof(char) );

	//Fill in live spots
}


 /*
 *
 */
void readConfig(char *config_file, int *iterations, LiveSpots *live_spots, int *rows, int *cols){
	
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
				*rows = num;
				count++;
				break;
			case 2:			//Set second input to cols
				*cols = num;
				count++;
				break;
			case 3:			//Set third input to iterations
				*iterations = num;
				count++;
				break;
			case 4:			//Fourth input is for number of live spots
				//Initialize live_spots
				live_spots->num_live_spots = num;
				live_spots->coords = calloc ( num_live_spots, sizeof(Coord) );
				count++;
				break;
			default: 		//Rest of the arguments are live spots so store them
//TODO: Fix to fill in x and y
				live_spots->coords[count-5].x= num;
				count++;
				break;
		}
	}	

}

void printLiveSpots( LiveSpots *ls){
	
	for( int i = 0; i < ls->num_live_spots; i++ ){
		printf("%d,%d\n", ls->coords[i].x, ls->coords[i].y );
	}
}



