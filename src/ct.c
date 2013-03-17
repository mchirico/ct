/*
    Copyright (C) GPL 2005, Mike Chirico mchirico@gmail.com
    https://github.com/mchirico/ct

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


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
    char ip[MAXSUB +1];
    char time_buffer[TIMEBUF_SIZE+1];

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
    getTime(data->ip);

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

    printf(",%s", port);
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
    strcpy(data->ip,hptr->h_name);
    strcpy(data->port, port);


}





int main(int argc, char **argv)
{
    pthread_t thread[50];	/* thread variables */
    thdata data[50];		/* structs to be passed to threads */


    char hname[MAXSUB + 1];
    char port[MAXSUB + 1];
    char cmd[MAXSUB + 1];

    manageInput(argc, argv, hname, port);
    setupConnection(hname, port, &data[0]);

    pthread_create(&thread[0], NULL, (void *)&quickConnect, (void *)&data[0]);
    sleep(1);



    // Future stuff
    //process(sockfd, cmd);

    /* It appears that pthread_cancel will kill the thread if it is still
       active. You could try to do a join to confirm, and it shouldn't
       block. 
    */
    int sig;
    sig = pthread_cancel(thread[0]);
    if (sig != 0)
	fprintf(stderr, "Error\n");

    close(data[0].sockfd);

    if (data[0].status == -1)
	printf(",No Connection,timeout\n");
    exit(data[0].status);

}
