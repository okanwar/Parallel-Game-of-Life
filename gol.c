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
int ruleOne(Board *board, int x);
int search(Board *board, int index, int search_val);
int mod(int val, int mod);
int convertOneD( int r, int c, int cols);


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
	//Log time
	gettimeofday(&begin_time, NULL);
	//Simulate GOL
	for( int i = 0; i < iterations; i++ ){
		
	//Log time 

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
	printf("-2 mod 10 = %d\n", mod(-2, 10));
	printf(" 50 mod 18 = %d\n", mod(50, 18));

	return 0;
}

int mod(int val, int mod){
	return ((val % mod) + mod) % mod;
}

/*
 *
 */
void update(Board *board){
	
	//iterate over all spots
	for( int i = 0; i < board->size; i++ ){
		//Check rules

		switch( board->arr[i] ){
			
			case 0:  //Cell is dead
				
				//Check if it can be revived (Rule 3)
				if( search( board, i, 1) == 3 ){ 	//Cell can be revived
					board->revive[i] = 1;
				}
				break;
			case 1:  //Cell is alive

				//Check if it dies to loneliness or overpopulation (Rule 1&2)
				if( search( board, i, 1) <= 1 ){	//Dies from loneliness
					board->die[i] = 1;
				} else if( search( board, i, 1) >= 4 ){    //Dies from overpopulation
					board->die[i] = 1;
				}
				break;
			default:
				printf("Error: Cell is neither alive nor dead\n");
				break;
		};
	}
	
	//Update actual board with alive and dead
	
	//Clear alive and dead boards
}



/*
 * A method to search in a 3x3 grid around a point
 */
int search(Board *board, int index, int search_val){
	
	int start_c, start_r, d_index, dd_index;
	int count = 0;

	//Convert index to start_c, start_r
	
	//Row
	if( index < board->cols ){ //start_r == 0
		start_r = 0;
	} else {
		start_r = index / board->rows;
	}	

	//Col
	if( mod(index, board->rows) == 0 ){ //Col 0
		start_c = 0;
	} else if( index < board->cols){ //Index in row 0
		start_c = index;
	} else { 
		start_c = index % (start_r * board->cols);
	}

	// 2 X X
	// X 1 X
	// X X X
	//start_c and start_r are at 1, move them to 2 and wrap if necessary
	start_c = mod( start_c-1, board->cols );
	start_r = mod( start_r-1, board->cols );

	//Search for specified val
	int r = start_r;
	int c = start_c;
	for( int i = 0; i < 3; i++ ){ //Iterate over rows 3 times
		for( int j = 0; j < 3; j++ ){ //Iterate over cols 3 times

			//Get 2d index of where to look and convert to 1d index, then
			//check val
			if( i == 1 && j == 1) { continue; } // The point we are searching around
			r += i;
			c += j;
			if( board->arr[convertOneD( r, c, board->cols )] == search_val ){
				count++;
			}
		}		
	}

	return count;
}


/*
 * Converts a given row and col number in 2d to its 1d index
 */
int convertOneD( int r, int c, int cols){
	return (r * cols) + c;
}


/*
 *
 */
void initBoard( Board *board){

	board->size = board->rows * board->cols;
	board->arr = calloc( board->size, sizeof(int) );
	board->die = calloc( board->size, sizeof(int) );
	board->revive = calloc( board->size, sizeof(int) );
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
					printf("adding a live spot at (%d,%d)\n", x, y);
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
		if( ( (i+1) % (board->cols) == 0)  && i!=0){
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





