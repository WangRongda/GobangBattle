/*
 * =============================================================================
 *
 *       Filename:  serv.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年06月07日 16时08分20秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  StandAt (Wangrd), 123899696@qq.com
 *   Organization:  
 *
 * =============================================================================
 */

#include "Gobang_serv.h"

void error_handling(const char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

Serv::Serv()
{
}

void Serv::startListen(const char *port)
{
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (-1 == serv_sock) 
	{
		error_handling("socket() error!");
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(port));

	if (-1 == bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) 
	{
		error_handling("bind() error!");
	}

	if (-1 == listen(serv_sock, 15)) 
	{
		error_handling("listen() error!");
	}
}

int  Serv::acce(char *ip)
{
//	memset(&clnt_addr, 0, sizeof(clnt_addr));
	socklen_t clnt_addr_size = sizeof(clnt_addr);
	clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
	if (-1 == clnt_sock) 
	{
		error_handling("accept() error!");
	}
	strcpy(ip, inet_ntoa(clnt_addr.sin_addr));
	printf("Connected from %s, client fd %d\n ", inet_ntoa(clnt_addr.sin_addr), clnt_sock);
	return clnt_sock;
}

int Serv::sendMsg(int clnt_sock, char *Msg_send, int len)
{
	int returnLen;
	if (-1 == (returnLen = write(clnt_sock, Msg_send, len)))
	{
		puts("write() error!");
		return -1;
	}
	return returnLen;
}

int Serv::acepMsg(int clnt_sock, char *Msg_acep, int len)
{
	int returnLen;
	if (-1 == (returnLen = read(clnt_sock, Msg_acep, len)))
	{
		puts("read() error!");
		return -1;
	}
	return returnLen;
}

void Serv::end(int clnt_sock)
{
	close(clnt_sock);
}

void Serv::endListen()
{
	close(serv_sock);
}

Serv::~Serv()
{
	close(serv_sock);
}


