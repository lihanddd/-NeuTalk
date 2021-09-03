#include "neuserver.h"
#include <QDataStream>

NeuServer::NeuServer(QObject *parent) : QObject(parent)
{

}

void NeuServer::Close()
{
    while(!socket_list->isEmpty())
    {
        tcpSocket = socket_list->last();
        tcpSocket->disconnectFromHost();
    }
    tcpServer->close();
}

void NeuServer::Open(QHostAddress ip, int port)
{
    tcpServer->listen(ip, port);
}

void NeuServer::SetNeuServer(QHostAddress ip, int port)
{
    tcpServer = new QTcpServer;
    socket_list= new QList<QTcpSocket*>;
    if(!tcpServer->listen(QHostAddress::Any, port))
    {
        emit ShowMessage(NULL, "<font style='color:red;'>服务器端监听失败！</font>");
        return;
    }
    else
    {
        emit ShowMessage(NULL, "<font style='color:green;'>服务器监听成功！</font>");
        connect(tcpServer, SIGNAL(newConnection()), this, SLOT(ServerNewConnection()));
    }
}

void NeuServer::ServerNewConnection()
{
    //获取客户端连接
    tcpSocket = tcpServer->nextPendingConnection();
    if(!tcpSocket)
    {
        emit ShowMessage(tcpSocket, "<font style='color:red;'>获取客户端连接失败</font>");
        return;
    }
    else
    {
        socket_list->append(tcpSocket);
        emit ShowMessage(tcpSocket, "<font style='color:green;'>获取客户端连接成功</font>");
        connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(ServerReadData()), Qt::DirectConnection);
        connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(ServerDisConnection()));
    }
}

void NeuServer::ServerReadData()
{
    tcpSocket = (QTcpSocket *)this->sender();//得到发出信号的客户端
    char buffer[1024] = {0};
    tcpSocket->read(buffer, 1024);
    if( strlen(buffer) > 0)
    {
        QString showMsg = buffer;
        emit ShowMessage(tcpSocket, showMsg);
        if(showMsg.startsWith("register message:"))
        {
            QString message = showMsg.mid(17,showMsg.length()-1);
            emit RegisterToDb(tcpSocket, message);
        }
        else if(showMsg.startsWith("login message:"))
        {
            QString message = showMsg.mid(14,showMsg.length()-1);
            emit LoginToDb(tcpSocket, message);
        }
        else if(showMsg.startsWith("message:"))
        {
            QString message = showMsg.mid(8,showMsg.length()-1);
            QList<QString>list = message.split('$');
            Message Msg;
            Msg.datetime = list[0];
            Msg.send_user_id = list[1];
            Msg.recv_user_id = list[2];
            Msg.content = list[3];
            if(Msg.recv_user_id == "All")
            {
                SendMsgToAllUsers(Msg);
                return;
            }
            else
            {
                SendMsgToAllUsers(Msg);
                return;
            }
        }
        else if(showMsg.startsWith("init user:"))
        {
            QString user_id = showMsg.mid(10);
            emit GetUserToDb(tcpSocket, user_id);
        }
        else if(showMsg.startsWith("find:"))
        {
            QString Msg = showMsg.mid(5);
            QList <QString> list = Msg.split('$');
            QString send_user_mail = list[0];
            QString recv_user_mail = list[1];
            emit FindToDb(tcpSocket, send_user_mail, recv_user_mail);
        }
        else if(showMsg.startsWith("add:"))
        {
            QString Msg = showMsg.mid(4);
            QList <QString> list = Msg.split('$');
            QString send_user_mail = list[0];
            QString recv_user_mail = list[1];
            emit AddToDb(tcpSocket, send_user_mail, recv_user_mail);
        }
        else if(showMsg.startsWith("delete:"))
        {
            QString Msg = showMsg.mid(7);
            QList <QString> list = Msg.split('$');
            QString send_user_mail = list[0];
            QString recv_user_mail = list[1];
            emit DeleteToDb(tcpSocket, send_user_mail, recv_user_mail);
        }
    }
    else
    {
        emit ShowMessage(tcpSocket, "<font style='color:red;'>未正确接收数据</font>");
        return;
    }
}

void NeuServer::SendStatus(QTcpSocket *socket, int status)
{
    QString sendMsg = QString::number(status);
    if(socket->isValid())
    {
        int sendRe = socket->write(sendMsg.toUtf8());
        socket->waitForBytesWritten(1000);
        if( -1 == sendRe)
        {
            emit ShowMessage(socket, "<font style='color:red;'>服务端发送客户端状态失败！</font>");
        }
    }
    else
    {
        emit ShowMessage(socket, "<font style='color:red;'>套接字无效！</font>");
    }
}

void NeuServer::SendUser(QTcpSocket *socket, const QString &user_name, QList<UserInfo> friend_list)
{
    QString friendstr = "";
    for(int i=0; i<friend_list.length(); i++)
    {
        if(i==0)
        {
            friendstr = friendstr + friend_list.at(i).mail + '|' + friend_list.at(i).user_name;
            continue;
        }
        friendstr = friendstr + '#' + friend_list.at(i).mail + '|' + friend_list.at(i).user_name;
    }
    if(socket->isValid())
    {
        QString Msg = "user:" + user_name + '$' + friendstr;
        qDebug()<<Msg;
        int sendRe = socket->write(Msg.toUtf8());
        socket->waitForBytesWritten(1000);
        if( -1 == sendRe)
        {
            emit ShowMessage(socket, "<font style='color:red;'>服务端发送用户数据失败！</font>");
        }
    }
    else
    {
        emit ShowMessage(socket, "<font style='color:red;'>套接字无效！</font>");
    }
}

void NeuServer::SendMsg(QTcpSocket *tcpSocket_sendfile, QString message)
{
    if(tcpSocket_sendfile->isValid())
    {
        message = "message:" + message;
        int sendRe = tcpSocket_sendfile->write(message.toUtf8());
        tcpSocket_sendfile->waitForBytesWritten(1000);
        if( -1 == sendRe)
        {
            emit ShowMessage(tcpSocket_sendfile, "<font style='color:red;'>服务端发送信息失败！</font>");
        }
    }
    else
    {
        emit ShowMessage(tcpSocket_sendfile, "<font style='color:red;'>套接字无效！</font>");
    }
}

void NeuServer::SendUpdateList(QTcpSocket *socket, QList<UserInfo> list)
{
    QString friendstr = "";
    for(int i=0; i<list.length(); i++)
    {
        if(i==0)
        {
            friendstr = friendstr + list.at(i).mail + '|' + list.at(i).user_name;
            continue;
        }
        friendstr = friendstr + '#' + list.at(i).mail + '|' + list.at(i).user_name;
    }
    if(socket->isValid())
    {
        friendstr = "update list:" + friendstr;
        int sendRe = socket->write(friendstr.toUtf8());
        socket->waitForBytesWritten(1000);
        if( -1 == sendRe)
        {
            emit ShowMessage(socket, "<font style='color:red;'>服务端发送信息失败！</font>");
        }
    }
    else
    {
        emit ShowMessage(socket, "<font style='color:red;'>套接字无效！</font>");
    }
}

void NeuServer::SendFind(QTcpSocket *socket, const QString &user_name, bool res)
{
    if(socket->isValid())
    {
        QString Msg;
        if(res)
        {
            Msg = "find succeed:" + user_name;
        }
        else
        {
            Msg = "find failed";
        }
        int sendRe = socket->write(Msg.toUtf8());
        socket->waitForBytesWritten(1000);
        if( -1 == sendRe)
        {
            emit ShowMessage(socket, "<font style='color:red;'>服务端发送查询用户状态失败！</font>");
        }
    }
    else
    {
        emit ShowMessage(socket, "<font style='color:red;'>套接字无效！</font>");
    }
}

void NeuServer::SendAdd(QTcpSocket *socket, int status)
{
    if(socket->isValid())
    {
        QString Msg = "add message:";
        switch(status)
        {
            case 0:
            {
                Msg = Msg + "add succeed";
                break;
            }
            case 1:
            {
                Msg = Msg + "alreadyadd";
                break;
            }
            case 2:
            {
                Msg = Msg + "add failed";
                break;
            }
        }
        int sendRe = socket->write(Msg.toUtf8());
        socket->waitForBytesWritten(1000);
        if( -1 == sendRe)
        {
            emit ShowMessage(socket, "<font style='color:red;'>服务端发送添加用户信息失败！</font>");
        }
    }
    else
    {
        emit ShowMessage(socket, "<font style='color:red;'>套接字无效！</font>");
    }
}

void NeuServer::SendMsgDirect(Message message)
{

}

void NeuServer::SendMsgToAllUsers(Message message)
{
    QString Msg = message.datetime + "$" + message.send_user_id + "$" + message.recv_user_id + "$" + message.content;
    for(int i = 0;i < socket_list->length();i ++)
    {
        SendMsg(socket_list->at(i), Msg);
    }
}

void NeuServer::ServerDisConnection()
{
    tcpSocket = (QTcpSocket *)this->sender();//得到发出信号的客户端
    QString temp = QString("[%1:%2]:退出").arg(tcpSocket->peerAddress().toString()).arg(tcpSocket->peerPort());
    emit ShowMessage(tcpSocket, temp);
    //从链表中删除
    for(int i=0;i<socket_list->length();i++)
    {
        if(socket_list->at(i)->peerAddress() == tcpSocket->peerAddress())
        {
            if(socket_list->at(i)->peerPort() == tcpSocket->peerPort())
            {
                socket_list->removeAt(i);
                break;
            }
        }
    }

    return;
}
