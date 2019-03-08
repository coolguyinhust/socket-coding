/* wliu comments: required for windows socket programming */
#include <winsock.h>
#pragma comment(lib, "wsock32.lib")

#include <process.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#define SERVER_PORT 5432
#define MAX_BUFSIZE 256
void * re(void*);
void * sd(void*);

int WSAreturn;

/* wliu comments: useless pointer */
//FILE *fp;

struct hostent *hp;
struct sockaddr_in sin;
char *host;
char buf[MAX_BUFSIZE];
int s;
int len;
static int Interrupt=0;
static int ACK=0;

int main(int argc, char * argv[])
{
	/* wliu comments: required for windows socket programming */
	WSADATA WSAData;


	if (argc==2) {
		host = argv[1];
	}
	else {
		fprintf(stderr, "usage: duplex-talk host\n");
		exit(1);
	}

	/* wliu comments: modified for windows socket programming */
	WSAreturn = WSAStartup(0x101,&WSAData);
	if(WSAreturn)
	{
		fprintf(stderr, "duplex-talk: WSA error.\n");
		exit(1);
	}

	/* translate host name into peer's IP address */
	hp = gethostbyname(host);
	if (!hp) {
		fprintf(stderr, "duplex-talk: unknown host: %s\n", host);
		exit(1);
	}

	/* wliu comments: modified for string memory operation in windows */
	//bzero((char *)&sin, sizeof(sin));
	//bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);

	/* build address data structure */
	memset((char *)&sin, 0, sizeof(sin));
	memcpy((char *)&sin.sin_addr, hp->h_addr, hp->h_length);

	sin.sin_family = AF_INET;
	sin.sin_port = htons(SERVER_PORT);

	/* active open */
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("duplex-talk: socket failed.");
		exit(1);
	}

	if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
		perror("duplex-talk: connect failed.");
		/* wliu comments: modified for windows socket programming */
		//close(s);
		closesocket(s);
		exit(1);
	}

	/* wliu comments: displaying current status */
	printf("[duplex-talk client] connection to %s is ready\n", host);
	printf("[duplex-talk client] please input your message (empty input to halt):\n");

	/* wliu comments: modification to support connection termination */

	/* main loop: get and send lines of text */
	/*while (Interrupt!=1&&ACK!=1) {
            ���ǻ������̣߳����Բ�ֹ�������̣߳���Ϊ���߳��ֻᴴ�����߳�
        printf("again");
    _beginthread(sd,0,NULL);
    _beginthread(re,0,NULL);
	} */
	/**
	*   һ��ʼʹ�õ���<process.h>�µ�_beginthread�����̣߳��������������ʱ�޷������˳��������޷����ƣ�
	*   �ʻ���<pthread.h>��
	**/
	pthread_t thread1,thread2;    //���������̱߳�ʶ��
	pthread_create(&thread1 ,NULL,(void*)sd, NULL);
    pthread_create(&thread2 ,NULL,(void*)re, NULL);
    pthread_join(thread2,NULL);
    pthread_join(thread1,NULL);
	if(ACK==1){
        printf("[duplex-talk] empty message is received\n");
	}
	if(Interrupt==1){
        printf("[duplex-talk] empty message is sent to server\n");
	}
	printf("[duplex-talk] connection is terminated\n");

	/* wliu comments: modified for windows socket programming */
	WSACleanup();
	return 1;
}

void * sd(void * p){
    while(1){
        //������������ȵ����̵�����
        fgets(buf, sizeof(buf), stdin);	//client˵��
         //wliu comments: modified to stop sending
        if ( strlen(buf) == 1 )  {//client���Ϳ���Ϣ��ζ��ͨ������
            // wliu comments: user input empty message with '\n'
            buf[0] = '\0';
            send(s, buf, 1, 0);
            Interrupt=1;
            break;
        } else {//client���ͷǿ���Ϣ
            buf[MAX_BUFSIZE-1] = '\0';
            len = strlen(buf) + 1;
            send(s, buf, len, 0);
        }
    }


}

void * re(void* p){
    while(1){
        len = recv(s, buf, sizeof(buf), 0);//client������Ϣ
        if ( strlen(buf) == 0 )  //client���տ���Ϣ��ζ��ͨ������
        {
            /* wliu comments: received empty message */
            ACK=1;
            break;
        }
        else //client���շǿ���Ϣ���ӡ֮
        {
            printf("[duplex-talk] server: ");
            fputs(buf, stdout);
        }
    }
}

