#ifndef _CLIENT_LINK_LIST_H_
#define _CLIENT_LINK_LIST_H_

#include <WinSock2.h>

#include <stdio.h>

//�ͻ�����Ϣ�ṹ��
typedef struct Client
{
    SOCKET sClient;         //�ͻ����׽���
    char buf[128];          //���ݻ�����
    char userName[16];      //�ͻ����û���
    char IP[20];            //�ͻ���IP
    unsigned short Port;    //�ͻ��˶˿�
    UINT_PTR flag;          //��ǿͻ��ˣ��������ֲ�ͬ�Ŀͻ���
    struct Client* next;          //ָ����һ�����
}Client, *pClient;

#define N sizeof(Client)
/*
* function  ��ʼ������
* return    �޷���ֵ
*/
void Init();

/*
* function  ��ȡͷ�ڵ�
* return    ����ͷ�ڵ�
*/
pClient GetHeadNode();

/*
* function  ���һ���ͻ���
* param     client��ʾһ���ͻ��˶���
* return    �޷���ֵ
*/
void AddClient(pClient client);

/*
* function  ɾ��һ���ͻ���
* param     flag��ʶһ���ͻ��˶���
* return    ����true��ʾɾ���ɹ���false��ʾʧ��
*/
boolean RemoveClient(UINT_PTR flag);

/*
* function  ����name����ָ���ͻ���
* param     name��ָ���ͻ��˵��û���
* return    ����һ��client��ʾ���ҳɹ�������INVALID_SOCKET��ʾ�޴��û�
*/
SOCKET findSocket(char* name);

/*
* function  ����SOCKET����ָ���ͻ���
* param     client��ָ���ͻ��˵��׽���
* return    ����һ��pClient��ʾ���ҳɹ�������NULL��ʾ�޴��û�
*/
pClient findClient(SOCKET client);

/*
* function  ����ͻ���������
* param     client��ʾһ���ͻ��˶���
* return    ����������
*/
int CountCon();

/*
* function  �������
* return    �޷���ֵ
*/
void ClearClient();

/*
* function  �������״̬���ر�һ������
* return ����ֵ
*/
void CheckConnection();

/*
* function  ָ�����͸��ĸ��ͻ���
* param     FromName��������
* param     ToName,   ������
* param     data,     ���͵���Ϣ
*/
void SendData(char* FromName,  char* data);


#endif //_CLIENT_LINK_LIST_H_
