    #include <stdio.h>
    #include <winsock2.h>
    #include <pthread.h>
    #pragma comment(lib,"ws2_32.lib")
    char buffer[4096] = {0};
    int iRecvLen = 0;
    int iSnedLen = 0;
    char name[20];
    void  THRE_RECV(SOCKET ClientSocket)
    {
        char buffer[50]={0};
        while(1)
        {
            memset(buffer, 0, sizeof(buffer));///接收消息

            iRecvLen = recv(ClientSocket, buffer, sizeof(buffer), 0);
            if (SOCKET_ERROR == iRecvLen)
            {
                printf("send failed with error code: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return -1;
            }
            ///printf("recv %d bytes from %s: ", iRecvLen, nameOther);//为了美观最好不要打印这个了
            //strcat(buffer, "\0");
            buffer[iRecvLen] = 0;
            if(strlen(buffer) != 0 ) {
                fputs(buffer, stdout);
            }
        }
    }

    int main()
    {
         WSADATA wsaData = { 0 };///存放套接字信息,WSADATA结构被用来保存AfxSocketInit函数返回的WindowsSockets初始化信息。
        SOCKET ClientSocket = INVALID_SOCKET;///客户端套接字
        ///printf("%d\n",INVALID_SOCKET);
        SOCKADDR_IN ServerAddr = { 0 };///服务端地址
        USHORT uPort = 18000;///服务端端口

        ///初始化套接字
        if(WSAStartup(MAKEWORD(2,2), &wsaData))///该函数的第一个参数指明程序请求使用的Socket版本，其中高位字节指明副版本、低位字节指明主版本
        ///第二个参数返回请求的socket版本信息
        {
            printf("WSAStartup failed with error code: %d\n", WSAGetLastError());
            return -1;
        }
        ///判断套接字版本
        if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
        {
            printf("wVersion was not 2.2\n");
            return -1;
        }
        ///创建套接字
        ClientSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);///AF_INET代表一个地址族,SOCK_STREAM表示为TCP协议的流服务,IPPROTO_TCP的值为6
        ///printf("%d\n",IPPROTO_TCP);
        if (ClientSocket == INVALID_SOCKET)
        {
            printf("socket初始化失败并返回错误代码: %d\n", WSAGetLastError());
            return -1;
        }
        ///输入服务器IP
        printf("Please input the server's IP:");
        char IP[32] = { 0 };
        gets(IP);
        ///输入聊天的用户名
        printf("Please input the Client's username:");
        char name[32] = {0};
        memset(name,0,sizeof(name));
        gets(name);
        ///设置服务器地址，填充服务器地址的结构
        ServerAddr.sin_family = AF_INET;
        ServerAddr.sin_port = htons(uPort);///服务器端口
        ServerAddr.sin_addr.S_un.S_addr = inet_addr(IP);///服务器地址

        printf("Connecting...........\n");
        ///连接服务器
        if(SOCKET_ERROR == connect(ClientSocket, (SOCKADDR*)&ServerAddr, sizeof(ServerAddr)))
        {
            printf("Connect failed with error code: %d \n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return -1;
        }
        printf("成功加入迷你聊天室~ 服务器 IP:%s Port:%d\n",inet_ntoa(ServerAddr.sin_addr),htons(ServerAddr.sin_port));
        _beginthreadex(NULL,0,&THRE_RECV,ClientSocket,NULL,0);
        iSnedLen = send(ClientSocket,name,strlen(name),0);
        if(SOCKET_ERROR == iSnedLen)
        {
            printf("send failed with error code: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return -1;
        }
        while(1)
        {
            fgets(buffer, sizeof(buffer), stdin);
            if(strcmp(buffer,"bye") == 0) break;
            iSnedLen = send(ClientSocket, buffer, strlen(buffer), 0);
            if (SOCKET_ERROR == iSnedLen)
            {
                printf("send failed with error code: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return -1;
            }
        }
        closesocket(ClientSocket);
        WSACleanup();
        system("pause");
        return 0;
    }
