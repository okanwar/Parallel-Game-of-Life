/**
 * File: gol.c
 *
 * This file contains code to run the game of life with multiple threads
 * partitioning the game board.
 *
 * This file is part of COMP 280, Project 09.
 *
 * Authors:
 * 1. Om Kanwar (okanwar@sandiego.edu)
 * 2. Michael Dana (mdana@sandiego.edu)
 */

#define _XOPEN_SOURCE 600

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>

struct ThreadIndex {
	int start_twod;
	int end_twod;
};

typedef struct ThreadIndex ThreadIndex;

struct Board {
	pthread_barrier_t barrier;
	pthread_barrier_t exit_barrier;
	pthread_barrier_t print_barrier;
	int num_threads;
	int iteration_num;
	int iterations_total;
	int rows;
	int cols;
	int size;
	int live_spots;
	int *arr;
	int *die;
	int *revive;
	int *partitions;
	pthread_t *tidAr;
	pthread_t print_thread;
	int print;
	int part_info;
	ThreadIndex *threadIndices;
};

typedef struct Board Board;

/*
 * Forward declerations
 */
void readConfig( char *config_file, int *iterations, Board *board);
void initBoard( Board *board);
void printBoard( Board *board );
void update( Board *board, pthread_t tid);
void timeval_subtract (struct timeval *result, struct timeval *end, struct timeval *start);
int ruleOne(Board *board, int x);
int search(Board *board, int index, int search_val);
int mod(int val, int mod);
int convertOneD( int r, int c, int cols);
void clearBoard(int *board, int size);
void updateBoard( Board *board, int mode );
int getRow(int index, int num_col);
int getCol(int index, int num_col);
void freeBoard(Board *board);
int open_clientfd( char *hostname, char *port);
char* getConfig( char *config );
void getList();
char *concat( char *s1, char *s2);
void setConfig( char *config_file, Board *board, int *iterations);
void* threadFunction (void *arg);
void initPartitions(Board *board);
void* printThread(void *arg);
void createThreads(Board *board);
int findTid(pthread_t tid, Board *board);
void waitForThreads(Board *board);
void getBounds( int *start_index, int *end_index, int index, Board *board);
void initStruct(Board *board);

int main(int argc, char *argv[]) {
	int ret;
	char *config_file = NULL;
	Board board;
	int iterations = 0;
	struct timeval begin_time, end_time, result_time;
	int n_flag = 0;
	int c_flag = 0;
	int l_flag = 0;
	initStruct(&board);
	
	while (( ret = getopt( argc, argv, "pvc:ln:t:")) != -1){

		switch(ret){
		
			case 'c':
				config_file = optarg;
				c_flag = 1;
				break;
			case 'v':
				board.print = 1;
				break;
			case 'l':
				//Get files from server
				l_flag = 1;
				break;
			case 'n':
				//Run from server file
				n_flag = 1;
				config_file = getConfig( optarg );
				printf("running from server file\n");
				break;
			case 't':
				if( strtol(optarg, NULL, 10) <= 0 ){
					printf("Error: Invalid number of threads\n");
					exit(1);
				} else {
					board.num_threads = strtol( optarg, NULL, 10);
				}
				break;
			case 'p':
				board.part_info = 1;
				break;
			default:
				printf("Invalid Use!\n");
				exit(0);

		}
	}
	
	//Make sure only -n or only -c options are set
	if( n_flag == 1 && c_flag == 1 ){
		printf("Error: -c and -n options specified!\n");
		exit(1);
	}

	// -l option specified
	if( l_flag ){
		getList();
		exit(0);
	}

	//Read config file
	if( config_file == NULL ){
		printf("Error: No config file!\n");
		exit(1);
	}
	if( c_flag ){
		readConfig( config_file, &iterations, &board);
	} else {
		setConfig(config_file, &board, &iterations);
	}

	//Create threads
	createThreads(&board);

	//Log time
	gettimeofday(&begin_time, NULL);

	//Simulate GOL
	
	//Wait to finish
	while( board.iteration_num < board.iterations_total){
	}
	
	//Log time
	gettimeofday(&end_time, NULL);


	//Compute total time
	timeval_subtract( &result_time , &end_time, &begin_time );
	printf("Total time:%ld.%.6ld\n", result_time.tv_sec, result_time.tv_usec);

	//Wait for threads to exit
	waitForThreads(&board);

	//Free memory
	freeBoard(&board);
	if(n_flag){
		free(config_file);
	}


	return 0;
}

/*
 * A function to initialize values in struct to defaults
 *
 * @param board A pointer to the struct where the board infromation is kept
 */
void initStruct(Board *board){
	board->num_threads = 4;
	board->print =  0;
	board->part_info = 0;
}

/*
 * A function to join worker threads to main thread
 *
 * @param board A pointer to the struct where all the board infromation is
 * 	kept.
 */
void waitForThreads(Board *board){

	for( int i = 0; i < board->num_threads;i++){
		pthread_join( board->tidAr[i],NULL);
	}

	pthread_join( board->print_thread, NULL);

}

/*
 * A function to search the array of thread ids for its index which is the
 * threads logical id
 *
 * @param tid The threads actual id
 * @param board A pointer to the struct containing the boards infromation
 */
int findTid(pthread_t tid, Board *board){
	for(int i = 0; i < board->num_threads; i++){
		if( board->tidAr[i] == tid ){
			return i;
		}
	}

	printf("ERROR: TID not found!\n");
	return -1;
}

/*
 * A function to create all the worker threads
 *
 * @param board A pointer to a struct containing  information about the board
 *
 */
void createThreads(Board *board){

	//Create update thread
	pthread_create( &board->print_thread, NULL, printThread, board);
	
	//set up count for indices
	int last = 0;
	for( int i = 0; i < board->num_threads; i++){
		pthread_create ( &board->tidAr[i], NULL, threadFunction, board);

		//Start index is last + 1
		board->threadIndices[i].start_twod = last;

		//Last will be set the same for now
		board->threadIndices[i].end_twod = last;

		//increment last index given partition count
		int count = 0;
		while( count < board->partitions[i]){
			board->threadIndices[i].end_twod++;
			last++;
			count++;
		}
		board->threadIndices[i].end_twod--;
	}
}

/*
 * A function for a worker thread to execute, the thread will simulate a given
 * 	number of rows
 *
 * @param arg A pointer to the struct containing information about the board
 *
 * @return Returns a null value, no need to return just ends after it has
 * 	finished its iterations
 */
void* threadFunction(void *arg) {
	struct Board *board = (struct Board*)arg;
	int index = findTid( pthread_self(), board);

	
	while( board->iteration_num < board->iterations_total){

		//Wait for all worker threads and print thread
		pthread_barrier_wait( &board->barrier);

		//Wait for print thread to finish before updating again
		pthread_barrier_wait( &board->print_barrier );

		//Update board
		update(board, pthread_self());

	}
	//Print partition info
	if( board->part_info ){
		fprintf( stdout, "tid  %d: rows:  %d:%d  (%d)\n", index, board->threadIndices[index].start_twod,
			 board->threadIndices[index].end_twod, board->partitions[index]);
		fflush(stdout);	
	}
	return (void*) NULL;
}

/*
 * A function for the print/updating worker thread to run, prints the game
 * board and update the game board given updates by worker threads
 *
 * @param arg A pointer to a struct containing information about the board
 *
 * @return Returns a null value, no need to return just prints and updates the
 * 	board
 *
 */
void* printThread(void *arg) {
	struct Board *board = (struct Board*)arg;
	while ( board-> iteration_num < board->iterations_total ){
		
		//Wait for all barrier to finish before printing
		pthread_barrier_wait( &board->barrier);
		
		//Print if possible
		if(board->print ) {
			printf("Time step:%d/%d \n", board->iteration_num+1, board->iterations_total);
			printBoard(board);
			usleep(100000);
			if ( board->iteration_num != board->iterations_total) {
				system( "clear" );
			}	
		} 
		board->iteration_num++;

		//Update board
		updateBoard( board, 0);
		updateBoard( board, 1);
		//clear alive and dead boards
		clearBoard( board->die, board->size);
		clearBoard( board->revive, board->size);
		
		//Print has finished so wait
		pthread_barrier_wait( &board->print_barrier );
	}

	return (void*) NULL;
}

/*
 * A function to partition the rows for a given number of threads
 *
 * @param board A pointer to a struct containing information about the board
 *
 */
void initPartitions(Board *board){
	int row_num = board->rows / board->num_threads;
	int remainder = board->rows % board->num_threads;
	int i = 0;
	while( remainder != 0  && i < board->num_threads){
		board->partitions[i] = row_num + 1;
		remainder--;
		i++;
	}
	while( i < board->num_threads){
		board->partitions[i] =  row_num;
		i++;
	}


}

/*
 * A function to perform modular arithmetic
 * 
 * @param val The value to do modular arithmetic with
 * @param mod The value val with be modded by
 */
int mod(int val, int mod){
	int ret = val % mod;
	if(ret < 0){
		ret = (ret+mod) % mod;
	}
	return ret;
}

/*
 * A function to get the row value of a 1D index
 *
 * @param index The one dimensional array index value
 * @param num_col The number of columns in the 2d array
 */
int getRow(int index, int num_col){
	return index / num_col;
}

/*
 * A function to get the column value of a 1D index
 *
 * @param index The one dimensional array index value
 * @param num_col The number of columns in the 2D array
 */
int getCol(int index, int num_col){
	int row = index / num_col;
	return ( index - (row*num_col));
}

void getBounds( int *start_index, int *end_index, int index, Board *board){

	//1D start index is cols * index of start in 2d
	*start_index = (board->cols) * (board->threadIndices[index].start_twod);
	
	//1d end index is (cols * index of end in 2d) + (cols - 1)
	*end_index = (board->cols *(board->threadIndices[index].end_twod)) + ((board->cols) - 1); 

}

/*
 * A function used to update the board, this is where the rules of the GOL will
 * be checked for
 *
 * @param board A reference to the board where the game simulation is taking
 * place
 */
void update(Board *board, pthread_t tid){
	int neighbors = 0;

	//Calculate starting and ending indexes
	int index = -1;
	if( (index = findTid(tid, board)) == -1 ){
		printf("Error: tid not found\n");
	}
	int start_index, end_index;
	getBounds(&start_index, &end_index, index, board);

	//iterate over all spots
	for( int i = start_index; i <= end_index; i++ ){
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
	
}

/*
 * A method used to update the board given a kill or revive command, this
 * function will kill and revive cells
 *
 * @param board The board where the simulation is taking place
 * @param mode The mode, either 0 for kill or 1 for revive
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
 *
 * @param board A reference to an integer array representing the board
 * @param size The size of the array
 */
void clearBoard(int *board, int size){

	for(int i = 0; i < size; i++){
		board[i] = 0;
	}
}

/*
 * A method to search in a 3x3 grid around a point for a specified value
 *
 * @param board A reference to the board where the simulation is taking place
 * @param index The index where the search will be centered around
 * @param search_val The value that is going to be searched for
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
 *
 * @param r The row number from a 2D array
 * @param c The column number from a 2D array
 * @param cols The number of columns in the 2D array
 */
int convertOneD( int r, int c, int cols){
	return (r * cols) + c;
}


/*
 * A function to allocate space for the board where the simulation is taking
 * place
 *
 * @param board A reference to the board where the simulation is taking place
 */
void initBoard( Board *board){
	board->size = board->rows * board->cols;
	board->arr = calloc( board->size, sizeof(int) );
	board->die = calloc( board->size, sizeof(int) );
	board->revive = calloc( board->size, sizeof(int) );
	board->partitions = calloc( board->num_threads, sizeof(int) );
	board->tidAr = calloc( board->num_threads, sizeof(pthread_t));
	board->threadIndices = calloc( board->num_threads, sizeof(ThreadIndex) );
	initPartitions(board);
	pthread_barrier_init(&(board->barrier), NULL, board->num_threads+1);
	pthread_barrier_init(&(board->print_barrier), NULL, board->num_threads+1);
	pthread_barrier_init(&(board->exit_barrier), NULL, board->num_threads+2);
}


 /*
  * A function the takes in a config file and sets up the simulation
  *
  * @param config_file The name of the config file containing the simulations
  * setup
  * @param iterations A reference to a variable where the number of iterations
  * is going to be stored
  * @param board A reference to the board where the simulation is going to
  * take place
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
		exit(1);
	}

	while( (ret = fscanf( cfg, "%s", scan)) > 0){
		//Convert string to int
		num = strtol(scan, NULL, 10);
		//Use input from config
		switch( count ){
			
			case 1: 		//Set first input to rows
				board->rows = num;
				count++;
				break;
			case 2:			//Set second input to cols
				board->cols = num;
				count++;
				if( board->num_threads > board->cols ){
					printf("ERROR: Number of threads greater than number of columns\n");
					exit(1);
				}
				break;
			case 3:			//Set third input to iterations
				*iterations = num;
				board->iterations_total = num;
				board->iteration_num = 0;

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
					board->arr[ (y * board->cols) + x ] = 1;
					index++;
				}
				count++;
				break;
		}
	}	
}

/*
 * A function used to free the allocated memory of a board
 *
 * @param board A reference to the board that memory is going to be freed from
 */
void freeBoard(Board *board){
	free( board->arr );
	free( board->revive );
	free( board->die );
	free( board->partitions);
	free( board->tidAr);
	free( board->threadIndices);
}

/*
 * A function used to print out the board
 *
 * @param board A reference to the board to print
 */
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

/*
 * Subtracts two timevals, storing the result in a third timeval
 *
 * @param result The result of the subtraction
 * @param end The end time
 * @param start The start time
 */
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

/*
 * A function to eastablish connection to the server
 *
 * @param hostname The hostname of the server
 * @param port The port of the server
 */
int open_clientfd( char *hostname, char *port){
	int clientfd;
	struct addrinfo hints, *listp, *p;

	// Gets a list of potential server adresses
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_socktype = SOCK_STREAM; 	//Open a connection
	hints.ai_flags = AI_NUMERICSERV; 	//using a numeric port arg
	hints.ai_flags |= AI_ADDRCONFIG; 	
	getaddrinfo(hostname, port, &hints, &listp);

	//Walk list for a successful connection
	for( p = listp; p; p = p->ai_next ){
		//Create socket descriptor
		if((clientfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0 )
			continue; //Socket failed try next

		//Connecting to server
		if( connect(clientfd, p->ai_addr, p->ai_addrlen) != -1 )
		   break; 	//Success
		close(clientfd);	//Connect failed try next
	}
	//Clean up
	freeaddrinfo(listp);
	if(!p){	//All connects failed
		return -1;
	} else {
	   return clientfd;
	}	   
}
/*
 * A function used to get the configuration settings from the server
 *
 * @param config The config file to read from server
 */
char* getConfig( char *config ){
	char *hostname = "comp280.sandiego.edu";
	char *port = "9181";
	char *result = malloc( 100 * sizeof(int));
	int sockfd;
	int ret;
	char *msg;
	char *str1;

	//Make msg
	str1 = "get ";
	char *str3 = malloc( strlen(str1) + strlen(config) + 4);
	strcpy(str3 ,str1);
	strcat(str3 ,config);
	msg = str3;
	
	if( (sockfd = open_clientfd(hostname, port)) == -1 ){
		printf("Error: could not connect\n");
		return NULL;
	} else {
		//Send msg
		ret = send( sockfd, msg, strlen(msg), 0 );
		if( ret == -1 ){
			printf("Error msg not sent\n");
			return NULL;
		} else if ( ret != (int)strlen(msg)){
			printf("Full msg not sent\n");
		} else {
			//Recieve msg
			if( (recv( sockfd, result, 6000, 0)) > 0 ){
				close(sockfd);
				free(msg);
				return result;
			} else {
				printf("Error: message not recieved!\n");
				return NULL;
			}
		}
	}
	printf("Error config not found\n");
	return NULL;
}

/*
 * A function used to get the list of configuration files on the server
 * comp280.sandiego.edu
 */
void getList(){
	char *hostname = "comp280.sandiego.edu";
	char *port = "9181";
	char *msg = "list";
	char result[1000];
	int sockfd;

	if( (sockfd = open_clientfd(hostname, port)) == -1 ){
		printf("Error: could not connect\n");
		return;
	} else {
		//Send msg
		if( (send( sockfd, msg, sizeof(msg), 0)) != sizeof(msg) ){
			printf("msg not fully sent\n");
		} else {
			//Recieve msg
			if( (recv( sockfd, result, 1000, 0)) > 0 ){
				close(sockfd);
			} else {
				printf("Error: message not recieved!\n");
				return;
			}
		}
	}
}

/*
 * A function used to begin simulation with a string of config settings
 *
 * @param config_file The srtin containing the config
 * @param board The board were the simulation takes place
 * @param iterations Pointer to the number of iterations
 */
void setConfig( char *config_file , Board *board, int *iterations){
	int val, x ,y;
	int count = 1;
	char *tok;
	char *del;
	del = "\n";
	tok = strtok(config_file,del);
	while( tok != NULL ){
		//printf("%s,%d\n", tok, count);
		val = strtol(tok, NULL, 10);

		switch( count ){
			case 1:
				board->rows = val;
				count++;
				break;
			case 2:
				board->cols = val;
				count++;
				if( board->num_threads > board->cols ){
					printf("ERROR: Number of threads greater than number of columns\n");
					exit(1);
				}
				break;
			case 3:
				*iterations = val;
				board->iterations_total = val;
				board->iteration_num = 0;
				initBoard(board);
				count++;
				break;
			case 4:
			//Initialize live spots
				board->live_spots = val;
				count++;
				break;
			default:
				x = strtol(&tok[0], NULL, 10);
				y = strtol(&tok[2], NULL, 10);
				board->arr[ (y*board->cols) + x ] = 1;
				count++;
				break;
		};	
		tok = strtok(NULL,del);	
	}
}		
