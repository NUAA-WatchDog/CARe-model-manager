#pragma once

#include"CPlusPlus.h"

class sClient
{
protected:
	SOCKET sclient;
public:
	sClient();
	~sClient();
	int		WDrecv(string &recvbuf, int recv_size);		//recv client request
	int		WDsend(const char *sendbuf, int send_size);	//send data to client
	int		WDclose();									//close connectrion
	void	WDassign(SOCKET client);					//sClient <- (SOCKET)client
	SOCKET	WDvalid();									//sClient -> (SOCKET)client
private:

};

class WDSocket:public sClient
{
private:
	int server_port;
	SOCKET sockfd;
	SOCKADDR_IN server_addr;
	SOCKADDR_IN client_addr;
public:
	WDSocket();
	WDSocket(int port);					//init with port
	~WDSocket();
	void	WDsa_init();				//init server addr
	void	WDsocket();					//socket
	int		WDbind();					//bind
	int		WDlisten(int listen_size);	//listen without listen_size
	void	WDaccept();					//accept a connection
	sClient WDgetclient();				//return sClient
	SOCKET	WDgetfd();					//return sockfd
};

struct sockparam {

	sClient *client;

};