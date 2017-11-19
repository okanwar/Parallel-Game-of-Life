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


struct Board {
	int rows;
	int cols;
	int size;
	int live_spots;
	int *arr;
	int *die;
	int *revive;
};

typedef struct Board Board;

/*
 * Forward declerations
 */
void readConfig( char *config_file, int *iterations, Board *board);
void  initBoard( Board *board);
void printBoard( Board *board );
void update( Board *board);
void timeval_subtract (struct timeval *result, struct timeval *end, struct timeval *start);


int main(int argc, char *argv[]) {

	int ret;
	int rows, cols;
	int verbose = 0;
	char *config_file = NULL;
	Board board;
	int iterations = 0;
	struct timeval begin_time, end_time, result_time;

	
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
		exit(1);
	}

	//Read config file
	readConfig( config_file, &iterations, &board);

	//Simulate GOL
	for( int i = 0; i < iterations; i++ ){
		
	//Log time 
	gettimeofday(&begin_time, NULL);

		//Print if verbose
		if( verbose ){
			printf("Time step:%d/%d \n", i, iterations);
			printBoard(&board);
			usleep(100000);
			system( "clear" );

		}

		//Update board
		update(&board);

		//
	}
	
	//Log time
	gettimeofday(&end_time, NULL);

	//Compute total time
	timeval_subtract( &result_time , &end_time, &begin_time );
	printf("Total time:%ld.%0.6ld\n", result_time.tv_sec, result_time.tv_usec);

	//Free memory

	return 0;
}

/*
 *
 */
void update(Board *board){
	
	//iterate over all spots
	for( int i = 0; i < board->size; i++ ){
		
		//Check rules

		//Rule 1

	}
}

int rule_one(Board *board, int x){
	int neighbor_count = 0;

	//Check neighbors
	for( int i = x; i < x+3; i++ ){
		
	//TODO: Finish rules, then updates the actual board	
	}
}

/*
 *
 */
void initBoard( Board *board){

	board->size = board->rows * board->cols;
	board->arr = calloc( board->size, sizeof(int) );

}


 /*
 *
 */
void readConfig(char *config_file, int *iterations, Board *board){
	
	int ret;
	char scan[50];
	int num = 0;
	int count = 1;
	int index = 0;
	int num_live_spots;
	int x,y;

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
				board->rows = num;
				count++;
				break;
			case 2:			//Set second input to cols
				board->cols = num;
				count++;
				break;
			case 3:			//Set third input to iterations
				*iterations = num;

				//Initialize board now that enough information is given
				initBoard(board);
				count++;
				break;
			case 4:			//Fourth input is for number of live spots
				//Initialize live_spots
				board->live_spots = num;
				count++;
				break;
			default: 		//Rest are live spots		
				
				//Alternate storing reads in x and y based on even or odd
				if( count % 2 != 0 ){ //Odd count
					x = num;
				} else {
					y = num;
					//once we have a full coord put it in, index++
					board->arr[ (x * board->cols) + y ] = 1;
					index++;
				}
				count++;
				break;
		}
	}	


}



void printBoard( Board *board ){

	for(int i = 0; i < board->size; i++){
		printf(" %d ", board->arr[i] );

		//New line if row has been filled
		if( ( (i+1) % board->cols == 0)  && i!=0){
			printf("\n");
		}
	}	

	printf("\n");
}

void timeval_subtract (struct timeval *result, struct timeval *end, struct timeval *start) {
	
	// Perform the carry for the later subtraction by updating start.
	if (end->tv_usec < start->tv_usec) {
        int nsec = (start->tv_usec - end->tv_usec) / 1000000 + 1;
	    start->tv_usec -= 1000000 * nsec;
		start->tv_sec += nsec;
	}	
	if (end->tv_usec - start->tv_usec > 1000000) {
	    int nsec = (end->tv_usec - start->tv_usec) / 1000000;
        start->tv_usec += 1000000 * nsec;
        start->tv_sec -= nsec;
	}

	// Compute the time remaining to wait.tv_usec is certainly positive.
	result->tv_sec = end->tv_sec - start->tv_sec;
    result->tv_usec = end->tv_usec - start->tv_usec;
	
}





