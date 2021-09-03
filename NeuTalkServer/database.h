#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QTcpSocket>
#include <neuserver.h>

class DataBase : public QObject
{
    Q_OBJECT
public:
    explicit DataBase(QObject *parent = nullptr);
    void SetDatabase(const QString &database_name, const QString& database_path);
    bool RegisterQuery(const QString& mail, const QString& passward, const QString &username);
    bool LoginQuery(const QString& mail, const QString& passward);

signals:
    void ConnectRes(bool res);
    void LoginRes(QTcpSocket *socket, bool res);
    void RegisterRes(QTcpSocket *socket, bool res);
    void GetUserRes(QTcpSocket *socket, const QString &user_name, QList<UserInfo> friend_list);
    void findRes(QTcpSocket *socket, const QString &user_name, bool res);
    void addRes(QTcpSocket *socket, int status);
    void updateList(QTcpSocket *socket, QList<UserInfo> list);

public slots:
    void RegisterExec(QTcpSocket *socket, const QString &message);
    void LoginExec(QTcpSocket *socket, const QString &message);
    void GetUserExec(QTcpSocket *socket, const QString &user_id);
    void FindExec(QTcpSocket *socket, const QString &send_user_id, const QString &recv_user_id);
    void AddExec(QTcpSocket *socket, const QString &send_user_id, const QString &recv_user_id);
    void DeleteExec(QTcpSocket *socket, const QString &send_user_name, const QString &recv_user_name);

private:
    QString db_name;
    QString db_path;
    QSqlDatabase db;
    void GetConnectToDatabase();
    QList<UserInfo> GetFriendList(QString user_id);
};

#endif // DATABASE_H
