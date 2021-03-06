#include <WinSock2.h>
#include <process.h>
#include <stdlib.h>
#include "ClientLinkList.h"
#pragma comment(lib,"ws2_32.lib")

SOCKET g_ServerSocket = INVALID_SOCKET;      //服务端套接字
SOCKADDR_IN g_ClientAddr = { 0 };            //客户端地址
int g_iClientAddrLen = sizeof(g_ClientAddr);

typedef struct _Send
{
    char FromName[16];
    char data[128];
}Send,*pSend;

//发送数据线程
unsigned __stdcall ThreadSend(void* param)
{
    pSend psend = (pSend)param;  //转换为Send类型
    SendData(psend->FromName,  psend->data); //发送数据
    return 0;
}


//接受数据
unsigned __stdcall ThreadRecv(void* param)
{
    int ret = 0;
    while (1)
    {
        pClient pclient = (pClient)param;
        if (!pclient)
            return 1;
        ret = recv(pclient->sClient, pclient->buf, sizeof(pclient->buf), 0);
        if (ret == SOCKET_ERROR)
            return 1;
        if(strlen(pclient->buf)!=0){
            pSend psend = (pSend)malloc(sizeof(Send));
            //把发送人的用户名和接收消息的用户和消息赋值给结构体，然后当作参数传进发送消息进程中
            memcpy(psend->FromName, pclient->userName, sizeof(psend->FromName));
            memcpy(psend->data, pclient->buf, sizeof(psend->data));
            _beginthreadex(NULL, 0, ThreadSend, psend, 0, NULL);
            Sleep(200);
        }
    }

    return 0;
}

//开启接收消息线程
void StartRecv()
{
    pClient pclient = GetHeadNode();
    while (pclient = pclient->next)
        _beginthreadex(NULL, 0, ThreadRecv, pclient, 0, NULL);
}

//管理连接
unsigned __stdcall ThreadManager(void* param)
{
    while (1)
    {
        CheckConnection();  //检查连接状况
        Sleep(2000);        //2s检查一次
    }

    return 0;
}

//接受请求
unsigned __stdcall ThreadAccept(void* param)
{
    _beginthreadex(NULL, 0, ThreadManager, NULL, 0, NULL);
    Init(); //初始化一定不要再while里面做，否则head会一直为NULL！！！
    while (1)
    {
        //创建一个新的客户端对象
        pClient pclient = (pClient)malloc(sizeof(Client));

        //如果有客户端申请连接就接受连接
        if ((pclient->sClient = accept(g_ServerSocket, (SOCKADDR*)&g_ClientAddr, &g_iClientAddrLen)) == INVALID_SOCKET)
        {
            printf("accept failed with error code: %d\n", WSAGetLastError());
            closesocket(g_ServerSocket);
            WSACleanup();
            return -1;
        }
        recv(pclient->sClient, pclient->userName, sizeof(pclient->userName), 0); //接收用户名和指定聊天对象的用户名

        memcpy(pclient->IP, inet_ntoa(g_ClientAddr.sin_addr), sizeof(pclient->IP)); //记录客户端IP
        pclient->flag = pclient->sClient; //不同的socke有不同UINT_PTR类型的数字来标识
        pclient->Port = htons(g_ClientAddr.sin_port);
        AddClient(pclient);  //把新的客户端加入链表中

        printf("成功连接到IP:%s ,端口: %d,群昵称: %s\n",
            pclient->IP, pclient->Port, pclient->userName);

        if (CountCon() >= 2)                     //当至少两个用户都连接上服务器后才进行消息转发
            StartRecv();

        Sleep(2000);
    }
    return 0;
}

//启动服务器
int StartServer()
{
    //存放套接字信息的结构
    WSADATA wsaData = { 0 };
    SOCKADDR_IN ServerAddr = { 0 };             //服务端地址
    USHORT uPort = 18000;                       //服务器监听端口

    //初始化套接字
    if (WSAStartup(MAKEWORD(2, 2), &wsaData))
    {
        printf("WSAStartup failed with error code: %d\n", WSAGetLastError());
        return -1;
    }
    //判断版本
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
    {
        printf("wVersion was not 2.2\n");
        return -1;
    }
    //创建套接字
    g_ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (g_ServerSocket == INVALID_SOCKET)
    {
        printf("socket failed with error code: %d\n", WSAGetLastError());
        return -1;
    }

    //设置服务器地址
    ServerAddr.sin_family = AF_INET;//连接方式
    ServerAddr.sin_port = htons(uPort);//服务器监听端口
    ServerAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//任何客户端都能连接这个服务器

    //绑定服务器
    if (SOCKET_ERROR == bind(g_ServerSocket, (SOCKADDR*)&ServerAddr, sizeof(ServerAddr)))
    {
        printf("bind failed with error code: %d\n", WSAGetLastError());
        closesocket(g_ServerSocket);
        return -1;
    }
    //设置监听客户端连接数
    if (SOCKET_ERROR == listen(g_ServerSocket, 20000))
    {
        printf("listen failed with error code: %d\n", WSAGetLastError());
        closesocket(g_ServerSocket);
        WSACleanup();
        return -1;
    }
    printf("迷你多人聊天服务器已启动~\n");
    _beginthreadex(NULL, 0, ThreadAccept, NULL, 0, 0);
    for (int k = 0;k < 100;k++) //让主线程休眠，不让它关闭TCP连接.
        Sleep(10000000);

    //关闭套接字
    ClearClient();
    closesocket(g_ServerSocket);
    WSACleanup();
    return 0;
}

int main()
{
    StartServer(); //启动服务器
    return 0;
}
