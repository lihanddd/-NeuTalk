#include "clientsocket.h"
#include <QCryptographicHash>

ClientSocket::ClientSocket(QObject *parent) : QObject(parent)
{

}

ClientSocket::~ClientSocket()
{
    delete clientSocket;
}

int ClientSocket::GetStatus()
{
    return status;
}

void ClientSocket::ConnectToServer()
{
    clientSocket = new QTcpSocket();
    QString ip = "127.0.0.1";
    int port = 8888;
    clientSocket->connectToHost(ip, port);
    if(!clientSocket->waitForConnected(3000))
    {
        status = 5;//connect failed
        emit StatusChange();
        return;
    }
    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(ClientRecvData()));
}

void ClientSocket::ClientRecvData()
{
    QByteArray buf = clientSocket->readAll();
    qDebug()<<"ClientReader: "<<buf;
    if(buf[0]>='0'&&buf[0]<='9')
    {
        status = buf.toInt();
        emit StatusChange();
        return;
    }
    else if(buf.startsWith("message:"))
    {
        QString message = buf.mid(8);
        QList<QString>list = message.split("$");
        Message Msg;
        Msg.datetime = list[0];
        Msg.send_user_id = list[1];
        Msg.recv_user_id = list[2];
        Msg.content = list[3];
        emit RecvMsg(Msg);
    }
    else if(buf.startsWith("user:"))
    {
        QString message = buf.mid(5);
        QList<QString>list = message.split("$");
        emit RecvInitMsg(list[0], list[1]);
    }
    else if(buf.startsWith("update list:"))
    {
        QString message = buf.mid(12);
        QList<UserInfo> tmpUserInfo;
        QList<QString> tmplist = message.split('#');
        for(int i=0; i<tmplist.length(); i++)
        {
            QList<QString> tmpUser = tmplist.at(i).split('|');
            if(tmpUser.length()!=2)
            {
                continue;
            }
            UserInfo userInfo;
            userInfo.mail = tmpUser[0];
            userInfo.user_name = tmpUser[1];
            tmpUserInfo.append(userInfo);
        }
        emit RecvFriendList(tmpUserInfo);
    }
    else if(buf.startsWith("add message:"))
    {
        QString message = buf.mid(12);
        emit RecvAddFriend(message);
    }
    else if(buf.startsWith("find succeed:"))
    {
        emit RecvAddFriend(buf);
    }
    else if(buf.startsWith("find failed"))
    {
        emit RecvAddFriend(buf);
    }
}

void ClientSocket::Login(const QString &mail, const QString &passward)
{
    QString hashmail = QCryptographicHash::hash(mail.toUtf8(),QCryptographicHash::Md5).toHex();
    QString hashpassward = QCryptographicHash::hash(passward.toUtf8(),QCryptographicHash::Md5).toHex();
    QString sendMsg = "login message:" + hashmail + "$" +hashpassward;

    int sendRe = clientSocket->write(sendMsg.toUtf8());
    clientSocket->waitForReadyRead(1000);
    if(sendRe == -1)
    {
        status = 1;
        emit StatusChange();
        return;
    }
}

void ClientSocket::Register(const QString &mail, const QString &passward, const QString &username)
{
    QString hashmail = QCryptographicHash::hash(mail.toUtf8(),QCryptographicHash::Md5).toHex();
    QString hashpassward = QCryptographicHash::hash(passward.toUtf8(),QCryptographicHash::Md5).toHex();
    QString sendMsg = "register message:" + hashmail + "$" +hashpassward + "$" + username;

    int sendRe = clientSocket->write(sendMsg.toUtf8());
    clientSocket->waitForReadyRead(1000);
    if(sendRe == -1)
    {
        status = 2;
        emit StatusChange();
        return;
    }
}

void ClientSocket::InitUser(const QString &user_mail)
{
    QString hashmail = QCryptographicHash::hash(user_mail.toUtf8(),QCryptographicHash::Md5).toHex();
    QString sendMsg = "init user:" + hashmail;
    int sendRe = clientSocket->write(sendMsg.toUtf8());
    clientSocket->waitForReadyRead(1000);
    if(sendRe == -1)
    {
        status = 9;
        emit StatusChange();
        return;
    }
}

void ClientSocket::AddFriendMsg(const QString &send_user_mail, const QString &recv_user_mail, int type)
{
    QString send_hashmail = QCryptographicHash::hash(send_user_mail.toUtf8(),QCryptographicHash::Md5).toHex();
    QString recv_hashmail = QCryptographicHash::hash(recv_user_mail.toUtf8(),QCryptographicHash::Md5).toHex();
    switch(type)
    {
        case 0:
        {
            QString Msg = "find:" + send_hashmail + '$' + recv_hashmail;
            int sendRe = clientSocket->write(Msg.toUtf8());
            clientSocket->waitForReadyRead(1000);
            if(sendRe == -1)
            {
                qDebug()<<"发送查找用户请求失败";
                return;
            }
            qDebug()<<"发送查找用户请求成功";
            break;
        }
        case 1:
        {
            QString Msg = "add:" + send_hashmail + '$' + recv_hashmail;
            int sendRe = clientSocket->write(Msg.toUtf8());
            clientSocket->waitForReadyRead(1000);
            if(sendRe == -1)
            {
                qDebug()<<"发送添加用户请求失败";
                return;
            }
            qDebug()<<"发送添加用户请求成功";
            break;
        }
        case 2:
        {
            break;
        }
    }
}

void ClientSocket::DeleteFriendMsg(const QString &send_user_mail, const QString &recv_user_mail)
{
    QString send_hashmail = QCryptographicHash::hash(send_user_mail.toUtf8(),QCryptographicHash::Md5).toHex();
    QString recv_hashmail = recv_user_mail;
    QString sendMsg = "delete:" + send_hashmail + '$' + recv_hashmail;
    int sendRe = clientSocket->write(sendMsg.toUtf8());
    clientSocket->waitForReadyRead(1000);
    if(sendRe == -1)
    {
        qDebug()<<"删除好友失败";
        return;
    }
}

void ClientSocket::SendMsg(Message message)
{
//    typedef struct Message
//    {
//        QString send_user_id;      //消息发送者名称
//        QString recv_user_id;      //消息接受者名称
//        QString datetime;          //消息时间
//        QString type;              //消息类型 image,file,message
//        QString content;            //消息内容   消息内容，文件全路径
//        QString filename;           //文件名称
//        QString filesize;           //文件大小
//    }Message;
    QString send_user_id = message.send_user_id;
    QString recv_user_id = message.recv_user_id;
    QString datetime = message.datetime;
    QString type = message.type;
    QString content = message.content;
    QString filename = message.filename;
    QString filesize = message.filesize;

    if(type == "message")
    {
        QString sendMsg = "message:" + datetime + "$" + send_user_id + "$" + recv_user_id + "$" + content;
        int sendRe = clientSocket->write(sendMsg.toUtf8());
        clientSocket->waitForReadyRead(1000);
        if(sendRe == -1)
        {
            status = 7;
            emit StatusChange();
            return;
        }
    }
}

void ClientSocket::waitForReadyRead(int sec)
{
    this->clientSocket->waitForReadyRead(sec);
}
