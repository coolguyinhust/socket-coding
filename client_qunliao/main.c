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
            memset(buffer, 0, sizeof(buffer));///������Ϣ

            iRecvLen = recv(ClientSocket, buffer, sizeof(buffer), 0);
            if (SOCKET_ERROR == iRecvLen)
            {
                printf("send failed with error code: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return -1;
            }
            ///printf("recv %d bytes from %s: ", iRecvLen, nameOther);//Ϊ��������ò�Ҫ��ӡ�����
            //strcat(buffer, "\0");
            buffer[iRecvLen] = 0;
            if(strlen(buffer) != 0 ) {
                fputs(buffer, stdout);
            }
        }
    }

    int main()
    {
         WSADATA wsaData = { 0 };///����׽�����Ϣ,WSADATA�ṹ����������AfxSocketInit�������ص�WindowsSockets��ʼ����Ϣ��
        SOCKET ClientSocket = INVALID_SOCKET;///�ͻ����׽���
        ///printf("%d\n",INVALID_SOCKET);
        SOCKADDR_IN ServerAddr = { 0 };///����˵�ַ
        USHORT uPort = 18000;///����˶˿�

        ///��ʼ���׽���
        if(WSAStartup(MAKEWORD(2,2), &wsaData))///�ú����ĵ�һ������ָ����������ʹ�õ�Socket�汾�����и�λ�ֽ�ָ�����汾����λ�ֽ�ָ�����汾
        ///�ڶ����������������socket�汾��Ϣ
        {
            printf("WSAStartup failed with error code: %d\n", WSAGetLastError());
            return -1;
        }
        ///�ж��׽��ְ汾
        if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
        {
            printf("wVersion was not 2.2\n");
            return -1;
        }
        ///�����׽���
        ClientSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);///AF_INET����һ����ַ��,SOCK_STREAM��ʾΪTCPЭ���������,IPPROTO_TCP��ֵΪ6
        ///printf("%d\n",IPPROTO_TCP);
        if (ClientSocket == INVALID_SOCKET)
        {
            printf("socket��ʼ��ʧ�ܲ����ش������: %d\n", WSAGetLastError());
            return -1;
        }
        ///���������IP
        printf("Please input the server's IP:");
        char IP[32] = { 0 };
        gets(IP);
        ///����������û���
        printf("Please input the Client's username:");
        char name[32] = {0};
        memset(name,0,sizeof(name));
        gets(name);
        ///���÷�������ַ������������ַ�Ľṹ
        ServerAddr.sin_family = AF_INET;
        ServerAddr.sin_port = htons(uPort);///�������˿�
        ServerAddr.sin_addr.S_un.S_addr = inet_addr(IP);///��������ַ

        printf("Connecting...........\n");
        ///���ӷ�����
        if(SOCKET_ERROR == connect(ClientSocket, (SOCKADDR*)&ServerAddr, sizeof(ServerAddr)))
        {
            printf("Connect failed with error code: %d \n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return -1;
        }
        printf("�ɹ���������������~ ������ IP:%s Port:%d\n",inet_ntoa(ServerAddr.sin_addr),htons(ServerAddr.sin_port));
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
