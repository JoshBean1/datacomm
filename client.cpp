// Joshua Bean, jab1896
// Much of the socket code for both my client and server is based on the example code.
#include <iostream>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <iostream>
#include <fstream>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <vector>
#include <array>


using namespace std;


int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        cout << "Usage: ./client <server_address> <port> <filename>" << endl;
        exit(-1);
    }

    char* server_addr = argv[1];
    char* n_port = argv[2];
    int port = atoi(n_port);
    char* filename = argv[3];

    struct hostent *s; 
    s = gethostbyname(server_addr);

    struct sockaddr_in server;
    int handshake_socket = 0;
    socklen_t slen = sizeof(server);
    
    // create first socket
    if ((handshake_socket=socket(AF_INET, SOCK_DGRAM, 0))==-1) cout << "Error creating handshake socket." << endl;

    memset((char *) &server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    bcopy((char *)s->h_addr, (char *)&server.sin_addr.s_addr, s->h_length);

    // send handshake data
    char handshake[32] = "1248";
    if (sendto(handshake_socket, handshake, 32, 0, (struct sockaddr *)&server, slen)==-1) cout << "Error sending handshake." << endl;  // test buffer

    // receive random port into same handshake variable
    recvfrom(handshake_socket, handshake, 64, 0, (struct sockaddr *)&server, &slen);

    // convert port number to int
    int r_port = atoi(handshake);
    
    close(handshake_socket);
    sleep(1);  // delay to give server time to establish second socket

    // creat second socket
    int main_socket = 0;
    if ((main_socket=socket(AF_INET, SOCK_DGRAM, 0))==-1) cout << "Error creating new socket." << endl;
    // specify port as random port received from server
    server.sin_port = htons(r_port);
    bcopy((char *)s->h_addr, (char *)&server.sin_addr.s_addr, s->h_length);

    // open file specified
    ifstream file;
    file.open(filename);

    char ack[512] = "";  // variable for receiving acknowledgement from server

    char packets[10000][5];  // file stored here

    int count = 0;  // count packets needed
    
    while(true)
    {
        file.read(packets[count], 4);
        if (file.eof()) break;
        count++;
    }
    
    // convert number of packets to c string to send to server
    // server knows how many to expect based on this number
    int packet_count = count;
    string packet_count_str = to_string(packet_count);
    const char * send_packet_count = packet_count_str.c_str();

    if (sendto(main_socket, send_packet_count, 32, 0, (struct sockaddr *)&server, slen)==-1) cout << "Error in sendto function for packet count." << endl;
    recvfrom(main_socket, ack, 32, 0, (struct sockaddr *)&server, &slen);  // server sends number back when received

    // loop through data in 4 character segments
    for (int i = 0; i <= count; ++i)
    {
        if (sendto(main_socket, packets[i], 4, 0, (struct sockaddr *)&server, slen)==-1) cout << "Error in sendto function for file." << endl;
        recvfrom(main_socket, ack, 4, 0, (struct sockaddr *)&server, &slen);
        cout << ack << endl;  // print out response
    }
    
    close(main_socket);
    file.close();
    return 0;
}
