/**************************************************
  HAROKOPIO UNIVERSITY OF ATHENS
  DEPARTMENT OF INFORMATICS AND TELEMATICS
  OPERATING SYSTEMS - 2ND PROJECT
  GKOULIS DIMITRIOS (21204)
  GKOULD@GMAIL.COM
  
  CLIENT
**************************************************/

// Includes
#include <stdio.h>      // printf
#include <string.h>     // strlen
#include <sys/socket.h> // socket
#include <arpa/inet.h>  // inet_addr

#define IPADDRESS "127.0.0.1" // Do not touch this if you are using client at localhost
                              // Change this if you want to run client NOT at localhost

// Main Function
int main(int argc , char *argv[]) {
	
	int userInput;
    int sock;
    struct sockaddr_in server;
    char message[1000], server_reply[2000]; 
	
    sock = socket(AF_INET , SOCK_STREAM , 0); // Create Socket (Stream Socket)
	
    if (sock == -1) {
        printf("Could not create socket!"); // Connection Failed Message
    }
    puts("Socket created!");                // Connection Succeed Message
     
    server.sin_addr.s_addr = inet_addr(IPADDRESS); // Sending, a socket which binds to the IP Address: (see defines)
    server.sin_family = AF_INET;                   // Address family = Internet | AF_INET for socket programming over a network is the safest option
    server.sin_port = htons( 8888 );               // Set port number, using htons function to use proper byte order
  
    // Connect the socket to the server using the address struct
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0) {
        perror("ERROR! Connection Failed!"); // Show error if the previous line not succeed
        return 1;                            // End program
    }
     
    puts("Connected to Server!\n");
     
	while(1) { // while true
	
	    printf("Rock-Paper-Scissors Game\n"); // User options (client program)
		printf("1. Rock\n");
		printf("2. Paper\n");
		printf("3. Scissors\n");
		printf("4. Quit\n");
		
		printf("Your Hand Sign: ");
		scanf("%d", &userInput);
		
		if(userInput == 1) { // Rock
			if( send(sock, "1", 1, 0) < 0) { //Send some data to server
				puts("Send failed");
				return 1;
			}
		}
		else if(userInput == 2) { // Paper
			if( send(sock, "2", 1, 0) < 0) { //Send some data to server
				puts("Send failed");
				return 1;
			}
		}
		else if(userInput == 3) { // Scissors
			if( send(sock, "3", 1, 0) < 0) { //Send some data to server
				puts("Send failed");
				return 1;
			}
		}
		else if(userInput == 4) { // Quit client
		    break;                // End loop & end program
		}
		else {
		    printf("Invalid Option!\n");
		}
		
		//Receive a reply from the server
        if( recv(sock, server_reply, 2000, 0) < 0) {
            puts("recv Failed!"); // Error message
            break;                // Exit loop & end program
        }
        
		printf("\n\n");
        puts(server_reply); // Print Server Reply | Server Reply = Game Result
		
	} // End while
	
    close(sock); // End connection (using socket)
	
    return 0;    // Terminate (succesfully) the program
	
} // End main
