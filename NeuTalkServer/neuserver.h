#ifndef NEUSERVER_H
#define NEUSERVER_H

#include <QObject>
#include <QList>
#include <QTcpServer>
#include <QTcpSocket>

typedef struct UserInfo
{
    QString mail;
    QString user_name;
}UserInfo;

typedef struct Message
{
    QString send_user_id;      //消息发送者名称
    QString recv_user_id;      //消息接受者名称
    QString datetime;          //消息时间
    QString type;              //消息类型 image,file,message
    QString content;            //消息内容   消息内容，文件全路径
    QString filename;           //文件名称
    QString filesize;           //文件大小
}Message;

class NeuServer : public QObject
{
    Q_OBJECT
public:
    explicit NeuServer(QObject *parent = nullptr);
    void SetNeuServer(QHostAddress address, int port);
    void SendStatus(QTcpSocket *socket, int status);
    void SendUser(QTcpSocket *socket, const QString &user_name, QList<UserInfo> friend_list);
    void SendFind(QTcpSocket *socket, const QString &user_name, bool res);
    void SendAdd(QTcpSocket *socket, int status);
    void SendUpdateList(QTcpSocket *socket, QList<UserInfo> list);
    void Close();
    void Open(QHostAddress ip, int port);

signals:
    void ShowMessage(QTcpSocket *socket, const QString &message);
    void LoginToDb(QTcpSocket *socket, const QString &message);
    void RegisterToDb(QTcpSocket *socket, const QString &message);
    void GetUserToDb(QTcpSocket *socket, const QString &user_id);
    void FindToDb(QTcpSocket *socket, const QString &send_user_name, const QString &recv_user_name);
    void AddToDb(QTcpSocket *socket, const QString &send_user_name, const QString &recv_user_name);
    void DeleteToDb(QTcpSocket *socket, const QString &send_user_name, const QString &recv_user_name);

private slots:
    void ServerReadData();
    void ServerDisConnection();
    void ServerNewConnection();

private:
    QTcpServer *tcpServer = NULL;
    QTcpSocket *tcpSocket = NULL;
    QList<QTcpSocket*> *socket_list;

    void SendMsgToAllUsers(Message message);
    void SendMsgDirect(Message message);
    void SendMsg(QTcpSocket *tcpSocket_sendfile, QString message);
};

#endif // NEUSERVER_H
