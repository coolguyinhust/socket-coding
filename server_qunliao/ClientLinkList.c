#include "ClientLinkList.h"

pClient head ; //创建一个头结点

/*
* function  初始化链表
* return    无返回值
*/
void Init()
{
    head = (pClient)malloc(sizeof(Client));
    head->next = NULL;
}

/*
* function  获取头节点
* return    返回头节点
*/
pClient GetHeadNode()
{
    return head;
}

/*
* function  添加一个客户端
* param     client表示一个客户端对象
* return    无返回值
*/
void AddClient(pClient client)
{
    client->next = head->next;  //比如：head->1->2,然后添加一个3进来后是
    head->next = client;        //3->1->2,head->3->1->2
}

/*
* function  删除一个客户端
* param     flag标识一个客户端对象
* return    返回true表示删除成功，false表示失败
*/
boolean RemoveClient(UINT_PTR flag)
{
    //从头遍历，一个个比较
    pClient pCur = head->next;//pCur指向第一个结点
    pClient pPre = head;      //pPre指向head
    while (pCur)
    {
        // head->1->2->3->4,要删除2，则直接让1->3
        if (pCur->flag == flag)
        {
            pPre->next = pCur->next;
            closesocket(pCur->sClient);  //关闭套接字
            free(pCur);   //释放该结点
            return TRUE;
        }
        pPre = pCur;
        pCur = pCur->next;
    }
    return FALSE;
}

/*
* function  查找指定客户端
* param     name是指定客户端的用户名
* return    返回socket表示查找成功，返回INVALID_SOCKET表示无此用户
*/
SOCKET findSocket(char* name)
{
    //从头遍历，一个个比较
    pClient pCur = head;
    while (pCur = pCur->next)
    {
        if (strcmp(pCur->userName, name) == 0)
            return pCur->sClient;
    }
    return INVALID_SOCKET;
}

/*
* function  根据SOCKET查找指定客户端
* param     client是指定客户端的套接字
* return    返回一个pClient表示查找成功，返回NULL表示无此用户
*/
pClient findClient(SOCKET client)
{
    //从头遍历，一个个比较
    pClient pCur = head;
    while (pCur = pCur->next)
    {
        if (pCur->sClient == client)
            return pCur;
    }
    return NULL;
}

/*
* function  计算客户端连接数
* param     client表示一个客户端对象
* return    返回连接数
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
* function  清空链表
* return    无返回值
*/
void ClearClient()
{
    pClient pCur = head->next;
    pClient pPre = head;
    while (pCur)
    {
        //head->1->2->3->4,先删除1，head->2,然后free 1
        pClient p = pCur;
        pPre->next = p->next;
        free(p);
        pCur = pPre->next;
    }
}

/*
* function 检查连接状态并关闭一个连接
* return 返回值
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
                char error[128] = { 0 };   //发送下线消息给发消息的人
                sprintf(error, "The %s was downline.\n", pclient->userName);
                pClient temp=GetHeadNode()->next;
                while(temp!=NULL){
                    if(temp!=pclient)   send(temp->sClient, error, sizeof(error), 0);
                    temp=temp->next;
                }

                closesocket(pclient->sClient);   //这里简单的判断：若发送消息失败，则认为连接中断(其原因有多种)，关闭该套接字
                RemoveClient(pclient->flag);
                break;
            }
        }
    }
}

/*
* function  指定发送给哪个客户端
* param     FromName，发信人
* param     ToName,   收信人
* param     data,     发送的消息
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
                sprintf(buf, "%s: %s", FromName, data);   //添加发送消息的用户名
                ret = send(socket, buf, sizeof(buf), 0);
            }
            else//发送错误消息给发消息的人
            {
                if(socket == INVALID_SOCKET)
                    sprintf(error, "The %s was downline.\n", temp->userName);
                else
                    sprintf(error, "Send to %s message not allow empty, Please try again!\n", temp->userName);
                send(sending_client->sClient, error, sizeof(error), 0);
            }
            if (ret == SOCKET_ERROR)//发送下线消息给发消息的人
            {
                sprintf(error, "The %s was downline.\n", temp->userName);
                send(sending_client->sClient, error, sizeof(error), 0);
            }

        }

        temp=temp->next;
    }



}
