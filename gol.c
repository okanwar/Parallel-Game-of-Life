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
void clearBoard(int *board, int size);
void updateBoard( Board *board, int mode );
int getRow(int index, int num_col);
int getCol(int index, int num_col);

int main(int argc, char *argv[]) {

	int ret;
	int verbose = 0;
	char *config_file = NULL;
	Board board;
	int iterations = 0;
	struct timeval begin_time, end_time, result_time;

	
	while (( ret = getopt( argc, argv, "vcl:")) != -1){

		switch(ret){
		
			case 'c':
				config_file = optarg;
				break;
			case 'v':
				verbose = 1;
				break;
			case 'l':
				//Get files from server
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
	for( int i = 1; i <= iterations; i++ ){
		
	//Log time 

		//Print if verbose
		if( verbose ){
			printf("Time step:%d/%d \n", i, iterations);
			printBoard(&board);
			usleep(100000);
			if( i != iterations ){ 
				system( "clear" );
			}

		}

		//Update board
		update(&board);
		
	}
	
	//Log time
	gettimeofday(&end_time, NULL);

	//Compute total time
	timeval_subtract( &result_time , &end_time, &begin_time );
	printf("Total time:%ld.%.6ld\n", result_time.tv_sec, result_time.tv_usec);

	//Free memory

	return 0;
}

int mod(int val, int mod){
	int ret = val % mod;
	if(ret < 0){
		ret = (ret+mod) % mod;
	}
	return ret;
}

int getRow(int index, int num_col){
	return index / num_col;
}

int getCol(int index, int num_col){
	int row = index / num_col;
	return ( index - (row*num_col));
}

/*
 *
 */
void update(Board *board){
	int neighbors = 0;
	//iterate over all spots
	for( int i = 0; i < board->size; i++ ){
		//Check rules
		neighbors = search(board, i, 1);
		switch( board->arr[i] ){
			
			case 0:  //Cell is dead
				
				//Check if it can be revived (Rule 3)
				if( neighbors == 3 ){
					//Update revive board with cell
					board->revive[i] = 1;
				}
				break;
			case 1:  //Cell is alive
				//Check if it dies to loneliness or overpopulation (Rule 1&2)
				if( neighbors < 2 ){
					//Update death board with cell death
					board->die[i] = 1;
				} else if (neighbors > 3){
					//Update death board with cell death
					board->die[i] = 1;
				}
				break;
			default:
				printf("Error: Cell is neither alive nor dead\n");
				break;
		};
	}
	
	//Update actual board with alive and dead
 	updateBoard( board, 0);
	updateBoard( board, 1);	
	//Clear alive and dead boards
	clearBoard( board->die, board->size );
	clearBoard( board->revive, board->size );
}

/*
 * A method used to update the board given a kill or revive command
 */
void updateBoard( Board *board, int mode ){
	int *read_board;
	
	//Check mode and set board to read from
	if( mode == 0 ){
		read_board = board->die;
	} else if( mode == 1){
		read_board = board->revive;
	} else {
		printf("Invalid mode, mode must be either 0 (kill) or 1 (revive)\n");
	}
	
	//Update board according to read board
	for( int i = 0; i < board->size; i++ ){
		if( read_board[i] == 1 ){
			
			switch( mode ){
				case 0: //Kill Cell
					board->arr[i] = 0;
					break;
				case 1: //Revive Cell
					board->arr[i] = 1;
					break;
				default:
					printf("Error: Case is invalid mode\n");
					break;
			};
		}
	}
}

/*
 * A method used to reset a board to all 0's
 */
void clearBoard(int *board, int size){

	for(int i = 0; i < size; i++){
		board[i] = 0;
	}
}

/*
 * A method to search in a 3x3 grid around a point
 */
int search(Board *board, int index, int search_val){
	
	int start_c, start_r;
	int count = 0;

	//Convert index to start_c, start_r
	start_c = mod( (getCol( index, board->cols) - 1), board->cols);	
	start_r = mod( (getRow( index, board->cols) - 1), board->rows);
	
	int r = start_r;
	int c = start_c;
	int curr_index = 0;
	for( int i = 0; i < 3; i++ ){ //Iterate over cols
		c = mod(start_c+i, board->cols);
		for( int j = 0; j < 3; j++ ){
			r= mod(start_r+j, board->rows);
			curr_index = convertOneD( r, c, board->cols);
			if( curr_index == index){	//Index we are searching around 
				continue; 
			} else if ( board->arr[curr_index] == search_val ){
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
	int x,y;

	//Read in from config file
	FILE *cfg = NULL;
	if( (cfg = fopen( config_file, "r")) == NULL ){
		printf("Could not open file: %s\n", config_file);
		return;
	}

	while( (ret = fscanf( cfg, "%s", scan)) > 0){
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





