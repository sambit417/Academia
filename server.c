#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "admin.h"
#include "faculty.h"
#include "student.h"
#define PORT 5555
#define BUFFER_SIZE 1024

void handleClient(int client_socket);

int main() {
    int server_socket, client_socket;
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t clientAddrLen = sizeof(clientAddress);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(1);
    }

	printf("Socket created successfully..\n");


    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("Socket binding failed");
        exit(1);
    }



    if (listen(server_socket, 5) == -1) {
        perror("Listening failed");
        exit(1);
    }

    printf("Server is active\n", PORT);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&clientAddress, &clientAddrLen);
        if (client_socket == -1) {
            perror("Accepting connection failed");
            continue;
        }

        pid_t childPID = fork();
        if (childPID < 0) {
            perror("Fork failed");
        } else if (childPID == 0) {
            // This is the child process
            close(server_socket); // Close the server socket in the child process
            handleClient(client_socket);
            close(client_socket);
            exit(0); // Terminate the child process
        } else {
            // This is the parent process
            close(client_socket);    // Close the client socket in the parent process
            waitpid(-1, NULL, WNOHANG); // Cleanup zombie child processes
        }
    }

    // Close the server socket (never reached in this example)
    close(server_socket);
    return 0;
}

void handleClient(int client_socket) {
    char buffer[BUFFER_SIZE];
    int bytesRead;

    // Prompt the client to choose a role
    char role_Menu[] = ".......Welcome to Academia.......\nSelect your role:\n{1. Admin 2. Faculty 3. Student 4. Exit}\n";
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        send(client_socket, role_Menu, strlen(role_Menu), 0);

        // Receive the client's role choice
        bytesRead = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            perror("Error receiving role choice");
            close(client_socket);
            return;
        }
		int ch = atoi(buffer);
        

        // Process the client's role choice using a switch statement
        switch (ch) {
            case 1:
                // Handle Admin role
                // Implement the admin-related functionality here
				char *auth1 = admin_Authentication(client_socket);
                if (auth1 != NULL){
                    send(client_socket, "Authentication successful\n", strlen("Authentication successful\n"), 0);
                    admin_Fun(client_socket);
                }else{
                
                    send(client_socket, "Authentication failed. Exiting.\n", strlen("Authentication failed. Exiting.\n"), 0);
                    close(client_socket);
                }
                break;
            case 2:
                char *auth2 = faculty_Authentication(client_socket);
                if (auth2 != NULL){
                    send(client_socket, "Authentication successful\n", strlen("Authentication successful\n"), 0);
                    faculty_Fun(client_socket,auth2);
                }else{

                    send(client_socket, "Authentication failed. Exiting.\n", strlen("Authentication failed. Exiting.\n"), 0);
                    close(client_socket);
                }
                
                break;
            case 3:
                // Handle Student role
                // Implement the student-related functionality here
                char *auth = student_Authentication(client_socket);
                if (auth != NULL){
                    send(client_socket, "Authentication successful\n", strlen("Authentication successful\n"), 0);
                    student_Fun(client_socket,auth);
                }else{
                    
                    send(client_socket, "Authentication failed. Exiting.\n", strlen("Authentication failed. Exiting.\n"), 0);
                    close(client_socket);
                }
                break;
            case 4:
                
                char response[] = "You are Exiting Now !!.\n";
			    send(client_socket, response, strlen(response), 0);
			    printf("Client gets Exited");
			    close(client_socket);
			return;
            
            default:
                send(client_socket, "Invalid choice. Try again.\n", strlen("Invalid choice. Try again.\n"), 0);
                break;
        }
    }
    // Close the client socket
    close(client_socket);
}

