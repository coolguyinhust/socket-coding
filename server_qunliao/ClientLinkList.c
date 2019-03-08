#include "ClientLinkList.h"

pClient head ; //����һ��ͷ���

/*
* function  ��ʼ������
* return    �޷���ֵ
*/
void Init()
{
    head = (pClient)malloc(sizeof(Client));
    head->next = NULL;
}

/*
* function  ��ȡͷ�ڵ�
* return    ����ͷ�ڵ�
*/
pClient GetHeadNode()
{
    return head;
}

/*
* function  ���һ���ͻ���
* param     client��ʾһ���ͻ��˶���
* return    �޷���ֵ
*/
void AddClient(pClient client)
{
    client->next = head->next;  //���磺head->1->2,Ȼ�����һ��3��������
    head->next = client;        //3->1->2,head->3->1->2
}

/*
* function  ɾ��һ���ͻ���
* param     flag��ʶһ���ͻ��˶���
* return    ����true��ʾɾ���ɹ���false��ʾʧ��
*/
boolean RemoveClient(UINT_PTR flag)
{
    //��ͷ������һ�����Ƚ�
    pClient pCur = head->next;//pCurָ���һ�����
    pClient pPre = head;      //pPreָ��head
    while (pCur)
    {
        // head->1->2->3->4,Ҫɾ��2����ֱ����1->3
        if (pCur->flag == flag)
        {
            pPre->next = pCur->next;
            closesocket(pCur->sClient);  //�ر��׽���
            free(pCur);   //�ͷŸý��
            return TRUE;
        }
        pPre = pCur;
        pCur = pCur->next;
    }
    return FALSE;
}

/*
* function  ����ָ���ͻ���
* param     name��ָ���ͻ��˵��û���
* return    ����socket��ʾ���ҳɹ�������INVALID_SOCKET��ʾ�޴��û�
*/
SOCKET findSocket(char* name)
{
    //��ͷ������һ�����Ƚ�
    pClient pCur = head;
    while (pCur = pCur->next)
    {
        if (strcmp(pCur->userName, name) == 0)
            return pCur->sClient;
    }
    return INVALID_SOCKET;
}

/*
* function  ����SOCKET����ָ���ͻ���
* param     client��ָ���ͻ��˵��׽���
* return    ����һ��pClient��ʾ���ҳɹ�������NULL��ʾ�޴��û�
*/
pClient findClient(SOCKET client)
{
    //��ͷ������һ�����Ƚ�
    pClient pCur = head;
    while (pCur = pCur->next)
    {
        if (pCur->sClient == client)
            return pCur;
    }
    return NULL;
}

/*
* function  ����ͻ���������
* param     client��ʾһ���ͻ��˶���
* return    ����������
*/
int CountCon()
{
    int iCount = 0;
    pClient pCur = head;
    while (pCur = pCur->next)
        iCount++;
    return iCount;
}

/*
* function  �������
* return    �޷���ֵ
*/
void ClearClient()
{
    pClient pCur = head->next;
    pClient pPre = head;
    while (pCur)
    {
        //head->1->2->3->4,��ɾ��1��head->2,Ȼ��free 1
        pClient p = pCur;
        pPre->next = p->next;
        free(p);
        pCur = pPre->next;
    }
}

/*
* function �������״̬���ر�һ������
* return ����ֵ
*/
void CheckConnection()
{
    pClient pclient = GetHeadNode();
    while (pclient = pclient->next)
    {
        if (send(pclient->sClient, "", sizeof(""), 0) == SOCKET_ERROR)
        {
            if (pclient->sClient != 0)
            {
                printf("Disconnect from IP: %s,UserName: %s\n", pclient->IP, pclient->userName);
                char error[128] = { 0 };   //����������Ϣ������Ϣ����
                sprintf(error, "The %s was downline.\n", pclient->userName);
                pClient temp=GetHeadNode()->next;
                while(temp!=NULL){
                    if(temp!=pclient)   send(temp->sClient, error, sizeof(error), 0);
                    temp=temp->next;
                }

                closesocket(pclient->sClient);   //����򵥵��жϣ���������Ϣʧ�ܣ�����Ϊ�����ж�(��ԭ���ж���)���رո��׽���
                RemoveClient(pclient->flag);
                break;
            }
        }
    }
}

/*
* function  ָ�����͸��ĸ��ͻ���
* param     FromName��������
* param     ToName,   ������
* param     data,     ���͵���Ϣ
*/
void SendData(char* FromName,  char* data)
{
    char error[128] = { 0 };
    int ret = 0;
    SOCKET socket;
    pClient sending_client= findClient( findSocket(FromName));
    pClient temp=GetHeadNode()->next;
    while(temp!=NULL){
        if(temp!=sending_client){
            socket=temp->sClient;
            if (socket != INVALID_SOCKET && strlen(data) != 0)
            {
                char buf[128] = { 0 };
                sprintf(buf, "%s: %s", FromName, data);   //��ӷ�����Ϣ���û���
                ret = send(socket, buf, sizeof(buf), 0);
            }
            else//���ʹ�����Ϣ������Ϣ����
            {
                if(socket == INVALID_SOCKET)
                    sprintf(error, "The %s was downline.\n", temp->userName);
                else
                    sprintf(error, "Send to %s message not allow empty, Please try again!\n", temp->userName);
                send(sending_client->sClient, error, sizeof(error), 0);
            }
            if (ret == SOCKET_ERROR)//����������Ϣ������Ϣ����
            {
                sprintf(error, "The %s was downline.\n", temp->userName);
                send(sending_client->sClient, error, sizeof(error), 0);
            }

        }

        temp=temp->next;
    }



}
