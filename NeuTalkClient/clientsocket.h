#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <QObject>
#include <QTcpSocket>

/*
 * status = 0, login succeed
 * status = 1, login failed
 * status = 2, register succeed
 * status = 3, register failed
 * status = 4, connect succeed
 * status = 5, connect failed
 * status = 6, message send succeed
 * status = 7, message send failed
 * status = 8, get friendlist succeed
 * status = 9, get friendlist failed
*/

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

class ClientSocket : public QObject
{
    Q_OBJECT
public:

    int GetStatus();
    void ConnectToServer();
    void waitForReadyRead(int sec);
    void AddFriendMsg(const QString &send_user_mail, const QString &recv_user_mail, int type);
    void DeleteFriendMsg(const QString &send_user_mail, const QString &recv_user_mail);
    explicit ClientSocket(QObject *parent = nullptr);
    ~ClientSocket();

    QString mail = "";

signals:
    void RecvMsg(Message message);
    void RecvInitMsg(const QString &user_name, const QString &friend_list);
    void RecvAddFriend(const QString &Msg);
    void RecvFriendList(QList<UserInfo>list);
    void StatusChange();

public slots:
    void Login(const QString &mail, const QString &passward);
    void Register(const QString &mail, const QString &passward, const QString &username);
    void SendMsg(Message message);
    void InitUser(const QString &user_mail);

private slots:
    void ClientRecvData();

private:
    int status = -1;
    QTcpSocket *clientSocket;
};

#endif // CLIENTSOCKET_H
