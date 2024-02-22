// Joshua Bean, jab1896

#include <iostream>
#include <sys/types.h>   // defines types (like size_t)
#include <sys/socket.h>  // defines socket class
#include <netinet/in.h>  // defines port numbers for (internet) sockets, some address structures, and constants
#include <time.h>        // used for random number generation
#include <string.h> // using this to convert random port integer to string
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fstream>
#include <stdio.h>


using namespace std;


int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        cout << "Usage: ./server <port>" << endl;
        exit(-1);
    } 

    int n_port = atoi(argv[1]);

    struct sockaddr_in server;
    struct sockaddr_in client;
    int handshake_socket = 0;
    socklen_t clen = sizeof(client);
    char payload[32];
  
    if ((handshake_socket=socket(AF_INET, SOCK_DGRAM, 0))==-1) cout << "Error in handshake socket creation." << endl;
  
    memset((char *) &server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(n_port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(handshake_socket, (struct sockaddr *)&server, sizeof(server)) == -1) cout << "Error in binding for handshake." << endl;

    srand(time(NULL));
    int r_port = rand() % 64512 + 1024;
    string port = to_string(r_port);
    const char * port_payload = port.c_str();
    
    if (recvfrom(handshake_socket, payload, 32, 0, (struct sockaddr *)&client, &clen)==-1) cout << "Failed to receive handshake." << endl; 
    cout << "Received data: " << payload << endl;


    if (sendto(handshake_socket, port_payload, 64, 0, (struct sockaddr *)&client, clen)==-1) cout << "Error in sendto function." << endl;
  
    close(handshake_socket);


    int main_socket = 0;
    memset((char *) &server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(r_port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    if ((main_socket=socket(AF_INET, SOCK_DGRAM,0)) == -1) cout << "Error in main socket creation." << endl;
    
    if (bind(main_socket, (struct sockaddr *)&server, sizeof(server)) == -1) cout << "Error in binding for main socket." << endl;

    ofstream upload ("upload.txt");
    char file_chunk[512];
    int count = 0;
    
    while (count < 3)
    {
        cout << "testing socket" << endl;
        if (recvfrom(main_socket, file_chunk, 32, 0, (struct sockaddr *)&client, &clen)==-1);
        upload << file_chunk;
        cout << file_chunk;
        count++;
        for (int i = 0; i < sizeof(file_chunk); i++)
        {
            file_chunk[i] = toupper(file_chunk[i]);
        }
        if (sendto(main_socket, file_chunk, 512, 0, (struct sockaddr *)&client, clen)==-1) cout << "Error in sendto function for ack." << endl;
    }
    upload.close();
    
    
    close(main_socket);

    return 0;
}
