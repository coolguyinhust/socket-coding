/* wliu comments: required for windows socket programming */
#include <winsock.h>
#pragma comment(lib, "wsock32.lib")

#include <stdio.h>
#include <string.h>
#include <process.h>
#include <pthread.h>
#define SERVER_PORT 5432
#define MAX_PENDING 5
#define MAX_BUFSIZE 256

#define MAX_CONNECTION 3

void * re(void*);
void * sd(void*);
int WSAreturn;

struct sockaddr_in sin;
char buf[MAX_BUFSIZE];
int len;
int s, new_s;
static int Interrupt=0;
/* wliu comments: connections */
int conn;
static int ACK=0;

int main()
{
	/* wliu comments: required for windows socket programming */
	WSADATA WSAData;

	/* wliu comments: required for windows socket programming */
	WSAreturn = WSAStartup(0x101,&WSAData);
	if(WSAreturn)
	{
		fprintf(stderr, "duplex-talk: WSA error.\n");
		exit(1);
	}

	/* wliu comments: modified for string memory operation in windows */
	//bzero((char *)&sin, sizeof(sin));

	/* build address data structure */
	memset((char *)&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(SERVER_PORT);

	/* setup passive open */
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("duplex-talk: socket failed.");
		exit(1);
	}
	if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
		perror("duplex-talk: bind failed.");
		exit(1);
	}

	listen(s, MAX_PENDING);

	/* wliu comments: displaying current status */
	printf("[duplex-talk server] server is ready in listening ...\n");


	/* wait for connection, then receive and print text */
	conn = 0;
	while( conn < MAX_CONNECTION ) {
		/* wliu comments: correction for variable len */
		len = sizeof(struct sockaddr_in);

		if ((new_s = accept(s, (struct sockaddr *)&sin, &len)) < 0){
			perror("duplex-talk: accept failed.");
			exit(1);
		}

		/* wliu comments: displaying current status */
		printf("[duplex-talk server] received a connection from %s : \n", inet_ntoa(sin.sin_addr));

		/* wliu comments: modification to support connection termination */
		//while (len = recv(new_s, buf, sizeof(buf), 0))  {


        /*这种方式开销较大，创建了无数线程，且_beginthread不好控制
            while (Interrupt!=1&&ACK!=1) {
            printf("[duplex-talk] client: ");
            _beginthread(re,0,NULL);
            _beginthread(sd,0,NULL);
           }
        */
        pthread_t thread1,thread2;    //定义两个线程标识符
        pthread_create(&thread1 ,NULL,(void*)sd, NULL);
        pthread_create(&thread2 ,NULL,(void*)re, NULL);
        pthread_join(thread2,NULL);
        pthread_join(thread1,NULL);


        if(ACK==1){
            printf("[duplex-server] empty message is received\n");
        }

        if(Interrupt==1){
            printf("[duplex-server] empty message is sent to server\n");
        }

		printf("[duplex-talk] connection from %s is terminated\n", inet_ntoa(sin.sin_addr));

		/* wliu comments: modified for windows socket programming */
		//close(new_s);
		closesocket(new_s);

		conn ++;
	}

	printf("[duplex-talk server] max_connections achieved, server halt\n");

	/* wliu comments: required for windows socket programming */
	WSACleanup();
	return 1;
}

void * re(void* p){
    while(1){
        len = recv(new_s, buf, sizeof(buf), 0);//server接收信息
        /* wliu comments: modified to stop sending */
        if ( strlen(buf) == 0 )  { //server接收空信息意味着通信结束
            /* wliu comments: received empty message */
            ACK=1;
            break;
        } else {//server接收非空信息则打印之
            printf("[duplex-talk] client: ");
            fputs(buf, stdout);
        }
    }
}

void * sd(void * p){
    while(1){
        fgets(buf, sizeof(buf), stdin);	//server说话
        /* wliu comments: modified to stop sending */
        if ( strlen(buf) == 1 )  //server发送空消息意味着通信结束
        {
            /* wliu comments: user input empty message with '\n' */
            buf[0] = '\0';
            send(new_s, buf, 1, 0);
            //printf("[duplex-talk] empty message is send to client\n");
            Interrupt=1;
            break;
        }
        else //server发送非空消息
        {
            buf[MAX_BUFSIZE-1] = '\0';
            len = strlen(buf) + 1;
            send(new_s, buf, len, 0);
           // printf("[duplex-talk] server: %s", buf);
        }
    }
}
