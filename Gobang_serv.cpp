/*
 * =============================================================================
 *
 *       Filename:  hello_serv.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年06月07日 16时22分41秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  StandAt (Wangrd), 123899696@qq.com
 *   Organization:  
 *
 * =============================================================================
 */
#include "Gobang_serv.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

int wait = -1; // 和 等待对手的客户端 连接的套接字文件描述符
sem_t mutex, writeFile, fd;
Serv servSock;
FILE *fp;

void printTime()
{
	time_t timep;
	time (&timep);
	sem_wait(&writeFile);
	fprintf(fp, "%s", asctime(localtime(&timep)));
}

void disconnet(int n)
{
	servSock.end(n);
	printTime();
	fprintf(fp, "disconnect with %d\n", n);
	fflush(fp);
	sem_post(&writeFile);
}


/* 处理套接字 n(fd) 的线程 */
void *clnt_thread(void *arg)
{
	int matchId = -1; //用于放对手的fd
	int n = *((int *)arg); 
	sem_wait(&mutex);
	 //当前客户端为奇数客户端(没有对手),等待下一个客户端连上
	if (-1 == wait) 
	{
		wait = n;
	}
	else  //偶数客户端，将去和那个等待的人开始游戏
	{
		/* wait 要重置给下一个奇数客户端用
		 * 记下对手，和他对弈的时候才找的到人*/
		matchId = wait;  
		wait = -1;
	}
	sem_post(&mutex);

	char msg[10] = { 0 };
	/* 后来连上的人告诉对手/自己开始游戏,后来连上的先下
	 * 第一个连上的并不知道对方fd,发给他 */
	if (-1 != matchId) 
	{
		strcpy(msg, "myfd is");
		sprintf(&msg[7], "%d", n);
		if (-1 == servSock.sendMsg(matchId, msg, 10)) //发送给对手客户端
		{
			disconnet(matchId);
			return 0;
		}
		if (-1 == servSock.sendMsg(n, "startGame", 10))
		{
			disconnet(n);
			return 0;
		}
	}
	else  //第一个连上的客户端发来 对手id
	{
		if (servSock.acepMsg(n, msg, 10) <= 0)
		{
			/* 还没有对手退出 */
			disconnet(n);
			sem_wait(&mutex);
			wait = -1;
			sem_post(&mutex);
			return 0;
		}
		if (!strstr(msg, "myfd is"))
		{
			printTime();
			fprintf(fp, "receve message %s from bad man\n\n", msg);
			sem_post(&writeFile);
			disconnet(n);
			sem_wait(&mutex);
			wait = -1;
			sem_post(&mutex);
			return 0;
		}
		else
		sscanf(msg, "myfd is%d", &matchId);
	}
	/* 双方线程都知道对手的fd了，开始对弈
	 * 在接收自己受理的客户端发来的下棋的消息，
	 * 转发给对手，让对手知道自己下的棋，
	 * n表示自己，matchId表示对手 */
	while (1)
	{
		if (servSock.acepMsg(n, msg, 10) <= 0)
		{
			disconnet(n);
			servSock.sendMsg(matchId, "he  exit!", 10);
			return 0;
		}
		printTime();
		fprintf(fp, "people%d put in %s\n\n", n, msg);
		sem_post(&writeFile);
		if (strlen(msg) != 9) continue;
		if (matchId >= 4 && -1 == servSock.sendMsg(matchId, msg, 10))
		{
			disconnet(matchId);
			servSock.sendMsg(n, "he  exit!", 10);
			return 0;
		}
	}	
	servSock.end(n);
}/* clnt_thread */

void *acceptThread(void *arg)
{
	pthread_t clnt;
	servSock.startListen();
	while (1)
	{
		char ip[20] = { 0 };
		sem_wait(&fd);
		int n = servSock.acce(ip);
		sem_post(&fd);
		printTime();
		fprintf(fp, "connected from %s, client %d\n", ip, n);
		sem_post(&writeFile);
		/* 每accept()一个客户端，创建一个线程处理,继续accept */
		if (-1 == pthread_create(&clnt, NULL, clnt_thread, &n))
		{
			printf("create thread failed!\n");
		}
		sleep(1);
	}

	pthread_join(clnt, NULL);
}

int main(int argc, char *argv[])
{
	pthread_t serv;
	sem_init(&mutex, 0, 1);
	sem_init(&writeFile, 0, 1);
	sem_init(&fd, 0, 1);
	sem_wait(&fd);
	if (!(fp = fopen("/home/socketer/GobangBattle/log","w")))
	{
		printf("open file log failed\n");
		exit(1);
	}
	sem_post(&fd);
	
	/* 创建用于accept()的线程 */
	if (-1 == pthread_create(&serv, NULL, acceptThread, NULL))
	{
		printf("create thread failed!\n");
	}

	pthread_join(serv, NULL);
	return 0;
}	
