/*
 * Copyright (C) GPL 2005, Mike Chirico mchirico@gmail.com
 *
 *   gcc ct.c -o ct -lpthread
 *
 *   Create a program that will quickly test a connection
 *   without blocking.
 */


#include <arpa/inet.h>
#include <time.h>
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <pthread.h>		/* POSIX Threads */
#include <unistd.h>

#define SA      struct sockaddr
#define MAXLINE 4096
#define MAXSUB  200


#define LISTENQ         1024

extern int h_errno;


typedef struct str_thdata {
    int thread_no;
    int sockfd;
    int status;
    struct sockaddr_in servaddr;
    char hname[MAXSUB + 1];
    char port[MAXSUB + 1];

} thdata;



FILE *
 Popen(const char *command, const char *mode)
{
    FILE *fp;

    if ((fp = popen(command, mode)) == NULL)
	fprintf(stderr, "popen error");
    return (fp);
}

int Pclose(FILE * fp)
{
    int n;

    if ((n = pclose(fp)) == -1)
	fprintf(stderr, "pclose error");
    return (n);
}

char *
 Fgets(char *ptr, int n, FILE * stream)
{
    char *rptr;

    if ((rptr = fgets(ptr, n, stream)) == NULL && ferror(stream))
	fprintf(stderr, "fgets error");

    return (rptr);
}

void Fputs(const char *ptr, FILE * stream)
{
    if (fputs(ptr, stream) == EOF)
	fprintf(stderr, "fputs error");
}



extern int errno;
ssize_t

process(int sockfd, char *cmd)
{
    char sendline[MAXLINE + 1], recvline[MAXLINE + 1];
    ssize_t n;
    FILE *fp;
    char buf[MAXLINE + 1];

    //fp = Popen("date", "r");
    fp = Popen(cmd, "r");

    while (Fgets(buf, MAXLINE, fp) != NULL) {
	Fputs(buf, stdout);
	snprintf(sendline, MAXSUB,
		 "%s\n", buf);
	write(sockfd, sendline, strlen(sendline));
	n = read(sockfd, recvline, MAXLINE);
	recvline[n] = '\0';
	printf("%s", recvline);
    }

    Pclose(fp);
    return n;

}



void prTime() {
  char buffer[30];
  struct timeval tv;

  time_t curtime;



  gettimeofday(&tv, NULL); 
  curtime=tv.tv_sec;

  strftime(buffer,30,"%m-%d-%Y %T.",localtime(&curtime));
  printf("%s%d",buffer,tv.tv_usec);
  return;

}



void *
 quickConnect(void *ptr)
{
    thdata *data;
    data = (thdata *) ptr;

    if (connect(data->sockfd, (SA *) & (data->servaddr), sizeof(data->servaddr)) == 0) {
	data->status = 1;
	printf(",Connected");
	printf(",***   Good  ***\n");
    } else {
	data->status = 0;
	printf(",No Connection\n");
	//exit(1);
    }

}



int main(int argc, char **argv)
{
    pthread_t thread1, thread2;	/* thread variables */
    thdata data1, data2;	/* structs to be passed to threads */


    int sockfd;
    struct sockaddr_in servaddr;

    char **pptr;
    char hname[MAXSUB + 1];
    char port[MAXSUB + 1];
    char cmd[MAXSUB + 1];


    if (argc < 3) {
	printf("Need hostname  port\n");
	printf("./client 127.0.0.1  10001\n");
	exit(0);
    }
    snprintf(hname, MAXSUB, "%s", (char *)argv[1]);
    snprintf(port, MAXSUB, "%s", (char *)argv[2]);

    if (argc == 3) {
	snprintf(cmd, MAXSUB, "%s", (char *)argv[2]);
    } else {
	snprintf(cmd, MAXSUB, "date");
    }


    char str[50];
    struct hostent *hptr;
    if ((hptr = gethostbyname(hname)) == NULL) {
	fprintf(stderr, " gethostbyname error for host: %s: %s",
		hname, hstrerror(h_errno));
	exit(1);
    }

    prTime();
    printf(",%s", hptr->h_name);
    if (hptr->h_addrtype == AF_INET
	&& (pptr = hptr->h_addr_list) != NULL) {
	printf(",%s",
	       inet_ntop(hptr->h_addrtype, *pptr, str,
			 sizeof(str)));
    } else {
	fprintf(stderr, "Error call inet_ntop \n");
    }

    printf(",%s",port);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(port));
    inet_pton(AF_INET, str, &servaddr.sin_addr);

    data1.thread_no = 1;
    data1.sockfd = sockfd;
    data1.servaddr = servaddr;
    data1.status = -1;
    strcpy(data1.hname, hname);
    strcpy(data1.port, port);

    pthread_create(&thread1, NULL, (void *)&quickConnect, (void *)&data1);
    sleep(1);

    //process(sockfd, cmd);
    close(sockfd);
    if(data1.status == -1)
      printf(",No Connection,timeout\n");
    exit(data1.status);

}
