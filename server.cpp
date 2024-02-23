// Joshua Bean, jab1896

#include <iostream>
#include <sys/types.h>   
#include <sys/socket.h> 
#include <netinet/in.h>  
#include <time.h>
#include <string.h>
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

    // get first port from args
    int n_port = atoi(argv[1]);
    // generate r_port
    srand(time(NULL));
    int r_port = rand() % 64512 + 1024;
    string port = to_string(r_port);
    const char * port_payload = port.c_str();  // convert port number to cstring for sending to client

    // create handshake socket
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
    if (bind(handshake_socket, (struct sockaddr *)&server, sizeof(server)) == -1) cout << "Error binding handshake socket." << endl;
    
    if (recvfrom(handshake_socket, payload, 32, 0, (struct sockaddr *)&client, &clen)==-1) cout << "Failed to receive handshake." << endl;

    if (sendto(handshake_socket, port_payload, 64, 0, (struct sockaddr *)&client, clen)==-1) cout << "Error sending port." << endl;
    cout << endl << "My random port is: " << r_port << endl << endl;
    close(handshake_socket);

    // create second socket
    int main_socket = 0;
    server.sin_port = htons(r_port);
    if ((main_socket=socket(AF_INET, SOCK_DGRAM,0)) == -1) cout << "Error in main socket creation." << endl;
    
    if (bind(main_socket, (struct sockaddr *)&server, sizeof(server)) == -1) cout << "Error binding main socket." << endl;

    // create upload file
    ofstream upload ("upload.txt");
    char packet_count[512];
    int packets;
    char file_chunk[5];
    // receive packet count
    if (recvfrom(main_socket, packet_count, 32, 0, (struct sockaddr *)&client, &clen)==-1) cout << "Fail to receive packet count from client" << endl;
    packets = atoi(packet_count);
    if (sendto(main_socket, packet_count, 32, 0, (struct sockaddr *)&client, clen)==-1) cout << "Error in sendto function for packet count acknowledgement." << endl;
    // receive data for each expected packet
    for (int i = 0; i <= packets; i++)
    {
        if (recvfrom(main_socket, file_chunk, 4, 0, (struct sockaddr *)&client, &clen)==-1) cout << "Fail to receive data from client" << endl;
        // write data to file stream
        upload << file_chunk;
        // convert each data chunk to uppercase
        for (int i = 0; i < sizeof(file_chunk); i++)
        {
            file_chunk[i] = toupper(file_chunk[i]);
        }
        // send data back as ack
        if (sendto(main_socket, file_chunk, 4, 0, (struct sockaddr *)&client, clen)==-1) cout << "Error sending ack." << endl;
    }

    upload.close();
    close(main_socket);

    return 0;
}
