#include "TCPSocket.h"
#include <iostream>

//tcp server
TCPServer::TCPServer(){

}
TCPServer::~TCPServer(){

}
int TCPServer::tcpbind(unsigned short port){
	memset(&my_addr, 0,sizeof(my_addr));
	
	my_addr.sin_family=AF_INET; //set as IP communication
	my_addr.sin_addr.s_addr=INADDR_ANY;//allow every ip connected to local tcp
	my_addr.sin_port= htons(port); //服务器端口号
	
	/*create socket fd --IPv4协议*/
	if((server_sockfd = socket(PF_INET,SOCK_STREAM,0)) < 0)
	{  
		cout << "socket error!" <<endl;
		return 1;
	}
 
	/*bind socket fd to server internet addr*/
	if( bind(server_sockfd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr)) < 0)
	{
		cout << "bind error!" << endl;
		return 1;
	}
	cliaddr_len = sizeof(sockaddr_in);
	return 0; 
}

int TCPServer::tcplisten(){
	if(listen(server_sockfd, 10) < 0){
		cout << "listen error!" <<endl;
		return 1;
	}
	return 0;
}

int TCPServer::tcpaccept(){
	if((client_sockfd = accept(server_sockfd,(struct sockaddr *)&remote_addr,&cliaddr_len)) < 0){
		cout << "accept error" << endl;
		return 1;
	}
	return 0;
}

//can only send and recv after tcpbind(), tcplisten(),  tcpaccept()
int TCPServer::sendbuf(char *sendbuf, int buflen){
	int sendlen = send(client_sockfd, sendbuf, buflen, 0);
	if(sendlen < 0){
		cout << "send error" << endl;
		return -1;
	}
	return sendlen;
}

int TCPServer::recvbuf(char *recvbuf, int buflen){
	int recvlen = recv(client_sockfd, recvbuf, buflen, 0);
	if (recvlen < 0){
		cout << "recv error" << endl;
		return -1;
	}
	return recvlen;
}

int TCPServer::tcpclose(){
	close(server_sockfd);
	close(client_sockfd);
	return 0;
}

//tcp client
TCPClient::TCPClient(){

}
TCPClient::~TCPClient(){

}

//tcp客户端不需要自己绑定端口，会系统分配
int TCPClient::tcpconnect(string dstip, unsigned int dstport){
	memset(&remote_addr, 0, sizeof(remote_addr));
	remote_addr.sin_family = AF_INET;
	remote_addr.sin_addr.s_addr = inet_addr(dstip.c_str());
	remote_addr.sin_port = htons(dstport);

	//create socket fd
	if( (client_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0){
		cout << "socket error" << endl;
		return 1;
	}

	//connect to server
	if( connect(client_fd, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) < 0){
		cout << "connect error" << endl;
		return 1;
	}
	return 0;
}

int TCPClient::sendbuf(char *sendbuf, int buflen){
	int sendlen = send(client_fd, sendbuf, buflen, 0);
	if (sendlen < 0){
		cout << "send error!" << endl;
		return -1;
	}
	return sendlen;
}

int TCPClient::recvbuf(char *recvbuf, int buflen){
	int recvlen = recv(client_fd, recvbuf, buflen, 0);
	if (recvlen < 0){
		cout << "recv error!" << endl;
		return -1;
	}
	return recvlen;
}

int TCPClient::tcpclose(){
	close(client_fd);
	return 0;
}
