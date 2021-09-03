//smtp.h 头文件
#ifndef SMTP_H
#define SMTP_H

#include <QByteArray>
#include <QString>
#include <QTcpSocket>

class Smtp
{
public:
    Smtp(QByteArray username,QByteArray password);
    void send(QByteArray recvaddr,QString subject,QString content);
    ~Smtp();
private:
    QTcpSocket * clientsocket;
    QByteArray username;//发送方邮箱
    QByteArray password;//授权码
    QByteArray mailfrom = "mail from:<";
    QByteArray rcptto = "rcpt to:<";
    QByteArray prefrom = "from:";
    QByteArray preto = "to:";
    QByteArray presubject ="subject:";
    QByteArray recvaddr;//接收方邮箱
    QString subject;//邮件标题
    QString content; //发送内容
    QByteArray recvdata;//接收到的数据
};

#endif // SMTP_H

