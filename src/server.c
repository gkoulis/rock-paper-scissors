/**************************************************
  HAROKOPIO UNIVERSITY OF ATHENS
  DEPARTMENT OF INFORMATICS AND TELEMATICS
  OPERATING SYSTEMS - 2ND PROJECT
  GKOULIS DIMITRIOS (21204)
  GKOULD@GMAIL.COM
  
  SERVER
**************************************************/

// Includes
#include <stdio.h>
#include <string.h>      
#include <stdlib.h>      
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>   // inet_addr
#include <unistd.h>      // write
#include <pthread.h>     // threads, link with lpthread

// Global Variables
int total_games = 0;   // Total games
int clients_sum = 0;   // Clients connected at least once to server
int clients_wins = 0;  // Total clients' wins number
int clients_loses = 0; // Total clients' loses number
int cpu_wins = 0;      // Total cpu's wins number
int cpu_loses = 0;     // Total cpu's loses number
int tot_draws = 0;     // Total draws number

// Functions Declaration
void *connection_handler(void *); // Thread function.           This function handle connection for each client
int random_num();                 // Random num function.       This function return a random integer number between 0 and 2, 
								  //							add 1 and finally get CPU hand sign
void print_stats();               // Print statistics function. This function calculates statistics and print them to server screen. 
								  //                            (stats for all games, and all players-clients)

// Main Function
int main(int argc , char *argv[]) {
	
    int socket_desc, 
		client_sock, 
		c, 
		*new_sock;
		
    struct sockaddr_in server, client;
	
    socket_desc = socket(AF_INET , SOCK_STREAM , 0); // Create Socket (Stream Socket)
	
    if (socket_desc == -1) {               // Socket Creation Failed (Message)
        printf("ERROR! Could not create socket!");
    }
    puts("Socket Created!");                // Socket Successfully Created (Message)
     
    // Configure settings of the server address struct
    server.sin_family = AF_INET;         // Address family = Internet | AF_INET for socket programming over a network is the safest option
    server.sin_addr.s_addr = INADDR_ANY; // When sending, a socket bound with INADDR_ANY binds to the default IP Address 
    server.sin_port = htons( 8888 );     // Set port number, using htons function to use proper byte order
     
    // Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0) { // Bind the address struct to the socket
        perror("ERROR! Bind Failed!"); // Print error message   (Bind)
        return 1;
    }
    puts("Bind Done!");                // Print success message (Bind)
     
    // Listen (on the socket) with 3 max connection requests queued
	if(listen(socket_desc, 3) == 0) {
		puts("Waiting for incoming connections...");
	}
	else {
		puts("ERROR!");
	}
	
    c = sizeof(struct sockaddr_in);
	
	// Accept call creates a new socket for the incoming connection
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) ) {
		
		printf("\n----------------------------------------\n");
        puts("Connection Accepted!");                                    // Connection accepted message
		printf("Client's IP Address: %s\n", inet_ntoa(client.sin_addr)); // Print connected Client's IP
		
        pthread_t sniffer_thread; // set pthread
        new_sock = malloc(1);     // allocate memory to new_sock
        *new_sock = client_sock;  // assign client_sock to new_sock pointer to send to connection handler
        
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0) { // Create thread
            perror("ERROR! Could Not Create Thread!");                                              // Error message if could not create thread
			printf("----------------------------------------\n\n");
            return 1;                                                                               // End program
        }
         
        // Now join the thread , so that we dont terminate before the thread
        // pthread_join( sniffer_thread , NULL);
        puts("Handler Assigned!");
		
		printf("----------------------------------------\n\n");
		
		clients_sum++; // add one more client to clients' sum variable
		
    }
     
    if (client_sock < 0) {
        perror("Socket's Acception Failed!"); // Socket's Acception Failed message
        return 1;                             // End program
    }
	
    return 0; // Successfully end program
	
} // End main


// Function which handle connection for each client
void *connection_handler(void *socket_desc) {

    int sock = *(int*)socket_desc;       // Get the Socket descriptor
    int read_size;						 // Size of client's message. When 0 end connection.
	int server_result;                   // Server's result - random number (each number shows to specific hand sign)
    char *message , client_message[1];   // Client's message
	char opts[4] = {'1', '2', '3', '4'}; // 1 - Rock 
										 // 2 - Paper
										 // 3 - Scissors
										 // 4 - Quit
	
    //Receive a message from client
    while( (read_size = recv(sock , client_message , 1 , 0)) > 0 ) {
		
		server_result = random_num();      // CPU's random choice (hand sign)
		
		if(client_message[0] == opts[0]) { // Client: Rock
			
			puts("> Client's Hand Sign: Rock"); // Print at server's screen, the client's hand sign
			
			if(server_result == 1) { // CPU: Rock
				write(sock, " You: Rock \n CPU: Rock \n Draw! \n\n", 50);        // Send the message back to client	
				tot_draws++; // one more draw
			}
			else if(server_result == 2) { // CPU: Paper
				write(sock, " You: Rock \n CPU: Paper \n CPU Wins! \n\n", 50);   // Send the message back to client
				clients_loses++; // one more lose for client
 				cpu_wins++;      // one more win for cpu
			}
			else if(server_result == 3) { // CPU: Scissors
				write(sock, " You: Rock \n CPU: Scissors \n You Win! \n\n", 50); // Send the message back to client
				clients_wins++; // one more win for client
				cpu_loses++;    // one more lose for cpu
			}
			
			total_games++; // add one more to total games
			
			print_stats(); // print stats after client's action
			
		}
		else if(client_message[0] == opts[1]) { // Client: Paper
		
			puts("> Client's Hand Sign: Paper"); // Print at server's screen, the client's hand sign
			
			if(server_result == 1) { // CPU: Rock
				write(sock, " You: Paper \n CPU: Rock \n You Win! \n\n", 50);      // Send the message back to client
				clients_wins++; // one more win for client
				cpu_loses++;    // one more lose for cpu
			}
			else if(server_result == 2) { // CPU: Paper
				write(sock, " You Paper \n CPU: Paper \n Draw! \n\n", 50);         // Send the message back to client
				tot_draws++; // one more draw
			}
			else if(server_result == 3) { // CPU: Scissors
				write(sock, " You: Paper \n CPU: Scissors \n CPU Wins! \n\n", 50); // Send the message back to client
				clients_loses++; // one more lose for client
				cpu_wins++;      // one more win for cpu
			}
			
			total_games++; // add one more to total games
			
			print_stats(); // print stats after client's action
		
		}
		else if(client_message[0] == opts[2]) { // Client: Scissors
			
			puts("> Client's Hand Sign: Scissors"); // Print at server's screen, the client's hand sign
			
			if(server_result == 1) { // CPU: Rock
				write(sock, " You: Scissors \n CPU: Rock \n CPU Wins! \n\n", 50); // Send the message back to client
				clients_loses++; // one more lose for client
				cpu_wins++;      // one more win for cpu
			}
			else if(server_result == 2) { // CPU: Paper
				write(sock, " You: Scissors \n CPU: Paper \n You Win! \n\n", 50); // Send the message back to client
				clients_wins++; // one more win for client
				cpu_loses++;    // one more lose for cpu
			}
			else if(server_result == 3) { // CPU: Scissors
				write(sock, " You: Scissors \n CPU: Scissors \n Draw! \n\n", 50); // Send the message back to client
				tot_draws++; // one more draw
			}
			
			total_games++; // add one more to total games
			
			print_stats(); // print stats after client's action
			
		}
		
    }
     
    if(read_size == 0) {                 // If client don't send message
        puts("> Client Disconnected!"); 
        fflush(stdout);                  // Flush the output buffer of the stream
    }
    else if(read_size == -1) {           // Error between server and client communication, if client's program for some reason send -1(!)
        perror("recv Failed!");
    }
	
    free(socket_desc); // Free the Socket Pointer
     
    return 0;          // End function, (terminate thread)
	
} // connection_handler


// Return random number between 0 and 2, add 1 and finally get server result (for the game)
int random_num() {

	int limit = 2;                    // Limit
    int divisor = RAND_MAX/(limit+1); // Divisor of operation - determines the width of possible results (0-2 in this case)
    int retval;                       // Returning Value

    do { 
        retval = rand() / divisor;
    } while(retval > limit);

	retval++;      // add 1, because 1-rock, 2-paper, 3-scissors
	
    return retval; // Return result
	
} // End random_num


// Calculate statistics and print them to server
void print_stats() {

	float clients_wins_per;  // Clients Wins Percentage  (total games, all clients)
	float clients_loses_per; // Clients Loses Percentage (total games, all clients)
	float cpu_wins_per;      // CPU Wins Percentage      (total games, all clients)
	float cpu_loses_per;     // CPU Loses Percentage     (total games, all clients)
	float draws_per;         // Draw Percentage          (total games, all clients)
	
	clients_wins_per = (clients_wins*100.0f)/total_games;   // Calculate clients' win percentage
	clients_loses_per = (clients_loses*100.0f)/total_games; // Calculate clients' lose percentage
	cpu_wins_per = (cpu_wins*100.0f)/total_games;           // Calculate cpu's win percentage
	cpu_loses_per = (cpu_loses*100.0f)/total_games;         // Calculate cpu's lose percentage
	draws_per = (tot_draws*100.0f)/total_games; 			// Calculate draw percentage
	
	printf("--- Statistics ---\n"); // Print Stats at server screen
	printf("Total Games        : %d     |  Total Players        : %d \n", total_games, clients_sum);
	printf("Clients' Win Rate  : %.2f  |  Total Clients' Wins  : %d \n", clients_wins_per, clients_wins);
	printf("Clients' Lose Rate : %.2f  |  Total Clients' Loses : %d \n", clients_loses_per, clients_loses);
	printf("CPU's Win Rate     : %.2f  |  Total CPU's Wins     : %d \n", cpu_wins_per, cpu_wins);
	printf("CPU's Lose Rate    : %.2f  |  Total CPU's Loses    : %d \n", cpu_loses_per, cpu_loses);
	printf("Draw Rate          : %.2f  |  Total Draws          : %d \n", draws_per, tot_draws);
	printf("\n");
	
	return;
	
} // End print_stats
