#pragma once
#ifndef TCPSOCKET_H
#define TCPSOCKET_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>

using std::string;
using namespace std;

class TCPServer
{
public:
     TCPServer();
     ~TCPServer();
     //bind, listen, accept
     int tcpbind(unsigned short port);
     int tcplisten();
     int tcpaccept();

     // can recv and send after accept
     int recvbuf(char *recvbuf, int buflen);
     int sendbuf(char *sendbuf, int buflen);
     int tcpclose();

public:
     int server_sockfd;
     int client_sockfd;
     socklen_t cliaddr_len;
     struct sockaddr_in my_addr;//server network struct
     struct sockaddr_in remote_addr; //client network struct

};

class TCPClient{
public:
     TCPClient();
     ~TCPClient();
     int tcpconnect(string dstip, unsigned int dstport);
     int sendbuf(char *sendbuf, int buflen);
     int recvbuf(char *recvbuf, int buflen);
     int tcpclose();

public:
     int client_fd;
     struct sockaddr_in remote_addr;

};
#endif
