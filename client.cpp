// Joshua Bean, jab1896

#include <iostream>
#include <sys/types.h>   // defines types (like size_t)
#include <sys/socket.h>  // defines socket class
#include <netinet/in.h>  // defines port numbers for (internet) sockets, some address structures, and constants
#include <netdb.h> 
#include <iostream>
#include <fstream>
#include <arpa/inet.h>   // if you want to use inet_addr() function
#include <string.h>
#include <unistd.h>

using namespace std;


int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "Usage: ./client <server_address> <port> <filename>" << endl;
        exit(-1);
    }

    char server_addr = argv[1];
    char n_port = argv[2];
    int port = atoi(n_port);
    char filename = argv[3];

    struct hostent *s; 
    s = gethostbyname(server_addr);

    struct sockaddr_in server;
    int handshake_socket = 0;
    socklen_t slen = sizeof(server);

    if ((handshake_socket=socket(AF_INET, SOCK_DGRAM, 0))==-1) cout << "Error creating handshake socket." << endl;

    memset((char *) &server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    bcopy((char *)s->h_addr, (char *)&server.sin_addr.s_addr, s->h_length);

    char handshake[32] = "1248";  // test this
    if (sendto(handshake_socket, handshake, 32, 0, (struct sockaddr *)&server, slen)==-1) cout << "Error sending handshake." << endl;  // test buffer

    recvfrom(handshake_socket, handshake, 32, 0, (struct sockaddr *)&server, &slen);

    int r_port = atoi(handshake);  // probably won't work

    close(handshake_socket);

    int main_socket = 0;
    if ((main_socket=socket(AF_INET, SOCK_DGRAM, 0))==-1) cout << "Error creating new socket." << endl;
    server.sin_port = htons(r_port);
    bcopy((char *)s->h_addr, (char *)&server.sin_addr.s_addr, s->h_length);  // test this

    ifstream file;
    file.open(filename);

    char payload[3] = "";
    while(!file.eof())
    {
        file.read(payload, sizeof(payload));
        if (sendto(main_socket, payload, 4, 0, (struct sockaddr *)&server, slen)==-1) cout << "Error in sendto function for file." << endl;
    }
    payload = EOF;
    if (sendto(main_socket, payload, 4, 0, (struct sockaddr *)&server, slen)==-1) cout << "Error in sendto function for file." << endl;
    
    char ack[512] = "";
    recvfrom(main_socket, ack, 512, 0, (struct sockaddr *)&server, &slen);

    cout << ack << endl;
    close(main_socket);

    return 0;
}
