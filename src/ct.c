/*

  The MIT License (MIT)
  Copyright (c) 2005 Mike Chirico mchirico@gmail.com
  https://github.com/mchirico/ct


  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the
  "Software"), to deal in the Software without restriction, including
  without limitation the rights to use, copy, modify, merge, publish,
  distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to
  the following conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.



 Description:

    Simple program for testing a tcp connection, without blocking.


 Example Usage:

    $ ./ct gmail.com 80
    03-16-2013 12:59:32.20282,gmail.com,74.125.226.213,80,Connected,***   Good  ***

    $ ./ct gmail.com 81
    03-16-2013 13:00:10.79637,gmail.com,74.125.226.213,81,No Connection

  Or, you can but this in a quick script. It will never block, so your script
  can go about it's business.

    $ for i in $(seq 79 84); do ./ct gmail.com ${i}; done
    03-16-2013 13:15:35.20628,gmail.com,74.125.226.245,79,No Connection,timeout
    03-16-2013 13:15:36.24639,gmail.com,74.125.226.246,80,Connected,***   Good  ***
    03-16-2013 13:15:37.28678,gmail.com,74.125.226.245,81,No Connection,timeout
    03-16-2013 13:15:38.32927,gmail.com,74.125.226.246,82,No Connection,timeout
    03-16-2013 13:15:39.37058,gmail.com,74.125.226.245,83,No Connection,timeout
    03-16-2013 13:15:40.41139,gmail.com,74.125.226.246,84,No Connection,timeout



 Getting and Compiling the Program:

    wget https://raw.github.com/mchirico/ct/master/src/ct.c
    gcc ct.c -o ct -lpthread




TODO:

    ./ct gmail.com,google.com   80,81,82,83,84,85




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
#define MAX_WORKER_THREADS 20
#define MAX_NUM_THREAD_DATABASE 500
#define TIMEBUF_SIZE 30
#define LISTENQ         1024

extern int h_errno;


typedef struct str_thdata {
    int thread_no;
    int sockfd;
    int status;
    struct sockaddr_in servaddr;
    char hname[MAXSUB + 1];
    char port[MAXSUB + 1];
    char ip[MAXSUB + 1];
    char time_buffer[TIMEBUF_SIZE + 1];
    char comment[MAXSUB + 1];

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


//03 - 16 - 2013 16: 10:43.203604
int getTime(char *buffer)
{
    char tbuff[30];
    struct timeval tv;

    time_t curtime;


    gettimeofday(&tv, NULL);
    curtime = tv.tv_sec;
    strftime(tbuff, 30, "%m-%d-%Y %T.", localtime(&curtime));
    snprintf(buffer, 30, "%s%d", tbuff, tv.tv_usec);

    return strlen(buffer);

}



void prTime()
{
    char buffer[30];
    struct timeval tv;

    time_t curtime;

    gettimeofday(&tv, NULL);
    curtime = tv.tv_sec;

    strftime(buffer, 30, "%m-%d-%Y %T.", localtime(&curtime));
    printf("%s%d", buffer, tv.tv_usec);
    return;

}



void *
 quickConnect(void *ptr)
{
    thdata *data;
    data = (thdata *) ptr;
    getTime(data->time_buffer);

    if (connect(data->sockfd, (SA *) & (data->servaddr), sizeof(data->servaddr)) == 0) {
	data->status = 1;
    } else {
	data->status = 0;
	//exit(1);
    }

}


void manageInput(int argc, char **argv, char *hname, char *port)
{
    if (argc < 3) {
	printf("Need hostname  port\n");
	printf("./client 127.0.0.1  10001\n");
	exit(0);
    }
    snprintf(hname, MAXSUB, "%s", (char *)argv[1]);
    snprintf(port, MAXSUB, "%s", (char *)argv[2]);

    return;
}




int setupConnection(char *hname, char *port, thdata * data)
{
    int sockfd;
    struct sockaddr_in servaddr;

    char **pptr;
    char str[50];
    char ip[50];

    ip[0] = '\0';
    struct hostent *hptr;
    if ((hptr = gethostbyname(hname)) == NULL) {
	fprintf(stderr, " gethostbyname error for host: %s: %s",
		hname, hstrerror(h_errno));
	exit(1);
    }
    //prTime();

    if (hptr->h_addrtype == AF_INET
	&& (pptr = hptr->h_addr_list) != NULL) {
	snprintf(ip, 50, "%s", inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)));

    } else {
	fprintf(stderr, "Error call inet_ntop \n");
    }


    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(port));
    inet_pton(AF_INET, str, &servaddr.sin_addr);


    data->thread_no = 1;
    data->sockfd = sockfd;
    data->servaddr = servaddr;
    data->status = -1;
    strcpy(data->hname, hname);
    strcpy(data->ip, ip);
    strcpy(data->port, port);


}

void prData(thdata * data)
{

    if (data->status == -1) {
	printf("%s,%s,%s,%s,%d,No Connection,timeout\n",
	       data->time_buffer, data->hname, data->ip, data->port, data->status);
    }
    if (data->status == 0) {
	printf("%s,%s,%s,%s,%d,No Connection\n",
	       data->time_buffer, data->hname, data->ip, data->port, data->status);
    }
    if (data->status == 1) {
	printf("%s,%s,%s,%s,%d,Connected,*** GOOD ***\n",
	       data->time_buffer, data->hname, data->ip, data->port, data->status);
    }
}



int main(int argc, char **argv)
{
    pthread_t thread[MAX_WORKER_THREADS];	/* thread variables */
    thdata data[MAX_NUM_THREAD_DATABASE];	/* structs to be passed to
						   threads */


    char hname[MAXSUB + 1];
    char port[MAXSUB + 1];
    char cmd[MAXSUB + 1];

    manageInput(argc, argv, hname, port);
    setupConnection(hname, port, &data[0]);

    pthread_create(&thread[0], NULL, (void *)&quickConnect, (void *)&data[0]);
    sleep(1);



    //Future stuff
	// process(sockfd, cmd);

    /* It appears that pthread_cancel will kill the thread if it is still
       active. You could try to do a join to confirm, and it shouldn't
       block. */
    int sig;
    sig = pthread_cancel(thread[0]);
    if (sig != 0) {
      //fprintf(stderr, "Error thread may have been terminated\n");

    }

    close(data[0].sockfd);
    prData(&data[0]);




    exit(data[0].status);

}
