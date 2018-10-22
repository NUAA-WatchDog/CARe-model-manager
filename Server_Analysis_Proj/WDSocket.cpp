#include "stdafx.h"
#include "WDSocket.h"

/*WDSocket function*/

WDSocket::WDSocket()
{
}

WDSocket::WDSocket(int port) {

	server_port = port;

}

WDSocket::~WDSocket()
{
}

void WDSocket::WDsa_init() {

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(server_port);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(&(server_addr.sin_zero), 0, 8);

}

void WDSocket::WDsocket() {

	try
	{
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
	}
	catch (const std::exception& e)
	{
		throw(e);
	}
}

SOCKET WDSocket::WDgetfd() {

	return sockfd;

}

int WDSocket::WDbind() {

	int ret;

	try
	{
		ret = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr));
	}
	catch (const std::exception& e)
	{
		throw(e);
	}
	return ret;
}

int WDSocket::WDlisten(int listen_size) {

	int ret;

	try
	{
		ret = listen(sockfd, listen_size);
	}
	catch (const std::exception& e)
	{
		throw(e);
	}
	return ret;
}

void WDSocket::WDaccept() {

	int clientlen = sizeof(client_addr);
	try
	{
		sclient = accept(sockfd, (sockaddr *)&client_addr, &clientlen);
	}
	catch (const std::exception& e)
	{
		throw(e);
	}
}

sClient WDSocket::WDgetclient() {

	sClient client;

	client.WDassign(sclient);

	return client;
}

/*sClient function*/

sClient::sClient()
{
}

sClient::~sClient()
{
}

int sClient::WDrecv(string &recvbuf , int recv_size) {

	int ret;
	char recvbuf1[1024];

	try
	{
		ret = recv(sclient, recvbuf1, recv_size, 0);
		recvbuf.assign(recvbuf1);
	}
	catch (const std::exception& e)
	{
		throw(e);
	}
	return ret;
}

int sClient::WDsend(const char *sendbuf, int send_size) {

	int ret;

	try
	{
		ret = send(sclient, sendbuf, send_size, 0);
	}
	catch (const std::exception& e)
	{
		throw(e);
	}
	return ret;
}

int sClient::WDclose() {

	int ret;

	try
	{
		ret = closesocket(sclient);
	}
	catch (const std::exception& e)
	{
		throw(e);
	}
	return 0;
}

void sClient::WDassign(SOCKET client) {

	sclient = client;
}

SOCKET sClient::WDvalid() {

	return sclient;

}
