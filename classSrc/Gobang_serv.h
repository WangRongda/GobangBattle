/*
 * =====================================================================================
 *
 *       Filename:  serv.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年06月07日 14时27分46秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  StandAt (Wangrd), 123899696@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

class Serv
{
public:
	Serv();
	~Serv();
    /* 创建用于接受请求的服务器套接字,开始监听*/
	void startListen(const char *port = "5150");
	void endListen();
	/* 受理一个连接，返回创建的套接字描述符 */
	int acce(char *ip); 
	/* 用法和write()一样 */
	int sendMsg(int clnt_sock, char *Msg_send, int len); 
	/* 用法和read()一样 */
	int acepMsg(int clnt_sock, char *Msg_acep, int len); 
	void end(int clnt_sock); //关闭套接字n

private:
	int serv_sock;
	int clnt_sock;
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
};

