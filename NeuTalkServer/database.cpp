#include "database.h"
#include <QDebug>
#include <QSqlQuery>
#include <QCryptographicHash>
#include <QList>

DataBase::DataBase(QObject *parent) : QObject(parent)
{
}

void DataBase::GetConnectToDatabase()
{
    db = QSqlDatabase::addDatabase(db_name);
    db.setDatabaseName(db_path);
    if (!db.open())
    {
       emit ConnectRes(false);
    }
    emit ConnectRes(true);
}

void DataBase::SetDatabase(const QString &database_name, const QString& database_path)
{
    db_name = database_name;
    db_path = database_path;
    GetConnectToDatabase();
}

bool DataBase::RegisterQuery(const QString& mail, const QString& passward, const QString& username)
{
    QString exec_str = "insert into users(mail,passward,user_name) values('" + mail+ "','" + passward + "','" + username +"')";
    QSqlQuery query;
    qDebug()<<exec_str;
    if(!query.exec(exec_str))
        return false;
    return true;
}

bool DataBase::LoginQuery(const QString& mail, const QString& passward)
{
    QString exec_str = "select passward from users where mail = '"+mail+"'";
    QSqlQuery query;
    if(!query.exec(exec_str))
        return false;
    query.next();
    if(query.value(0).toString() != passward)
        return false;
    return true;
}

void DataBase::RegisterExec(QTcpSocket *socket, const QString &message)
{
    QString mail="", passward="", username="";
    QList <QString> list = message.split('$');
    mail = list[0];
    passward = list[1];
    username = list[2];

    QString hashmail = QCryptographicHash::hash(mail.toUtf8(),QCryptographicHash::Md5).toHex();
    QString hashpassward = QCryptographicHash::hash(passward.toUtf8(),QCryptographicHash::Md5).toHex();

    if(!RegisterQuery(hashmail, hashpassward, username))
    {
        emit RegisterRes(socket, false);
        return;
    }
    emit RegisterRes(socket, true);
    return;
}

void DataBase::LoginExec(QTcpSocket *socket, const QString &message)
{

    QString mail="", passward="";
    QList <QString> list = message.split('$');
    mail = list[0];
    passward = list[1];

    QString hashmail = QCryptographicHash::hash(mail.toUtf8(),QCryptographicHash::Md5).toHex();
    QString hashpassward = QCryptographicHash::hash(passward.toUtf8(),QCryptographicHash::Md5).toHex();

    if(!LoginQuery(hashmail,hashpassward))
    {
        emit LoginRes(socket, false);
        return;
    }
    emit LoginRes(socket, true);
    return;
}

void DataBase::GetUserExec(QTcpSocket *socket, const QString &user_id)
{
    QString user_name="", exec_str, hashmail;
    QSqlQuery query;
    hashmail = QCryptographicHash::hash(user_id.toUtf8(),QCryptographicHash::Md5).toHex();

    QList<UserInfo>friendList = GetFriendList(hashmail);

    exec_str = "select user_name from users where mail = '"+hashmail+"'";
    query.exec(exec_str);
    query.next();
    user_name = query.value(0).toString();

    emit GetUserRes(socket, user_name, friendList);
}

void DataBase::FindExec(QTcpSocket *socket, const QString &send_user_id, const QString &recv_user_id)
{
    QString send_hashmail = QCryptographicHash::hash(send_user_id.toUtf8(),QCryptographicHash::Md5).toHex();
    QString recv_hashmail = QCryptographicHash::hash(recv_user_id.toUtf8(),QCryptographicHash::Md5).toHex();

    QSqlQuery query;
    QString user_name;
    QString exec_str = "select user_name from users where mail = '"+recv_hashmail+"'";
    query.exec(exec_str);
    query.next();
    if(query.value(0).toString().length() == 0)
    {
        emit findRes(socket, "", false);
        return;
    };
    user_name = query.value(0).toString();
    emit findRes(socket, user_name, true);
}

void DataBase::AddExec(QTcpSocket *socket, const QString &send_user_id, const QString &recv_user_id)
{
    QString send_hashmail = QCryptographicHash::hash(send_user_id.toUtf8(),QCryptographicHash::Md5).toHex();
    QString recv_hashmail = QCryptographicHash::hash(recv_user_id.toUtf8(),QCryptographicHash::Md5).toHex();

    QSqlQuery query;
    QString friendstr;
    QList<QString> friendlist;
    QString exec_str = "select friend from users where mail = '"+send_hashmail+"'";
    query.exec(exec_str);
    query.next();
    friendstr = query.value(0).toString();
    friendlist = friendstr.split('#');

    if(friendlist.length()>1)
    {
        for(int i=1;i<friendlist.length();i++)
        {
            if(recv_hashmail == friendlist.at(i))
            {
                emit addRes(socket, 1);
                return;
            }
        }
    }

    friendstr = friendstr + "#" + recv_hashmail;
    exec_str = "update users set friend = '" + friendstr + "' where mail = '" + send_hashmail + "'";
    qDebug()<<exec_str;
    if(!query.exec(exec_str))
    {
        emit addRes(socket, 2);
        return;
    };
    emit addRes(socket, 0);
    socket->waitForBytesWritten(1000);
    QList<UserInfo> list = GetFriendList(send_hashmail);
    emit updateList(socket, list);

    //给对方也添加自己为好友
    exec_str = "select friend from users where mail = '"+recv_hashmail+"'";
    query.exec(exec_str);
    query.next();
    friendstr = query.value(0).toString();
    friendstr = friendstr + '#' + send_hashmail;
    exec_str = "update users set friend = '" + friendstr + "' where mail = '" + recv_hashmail + "'";
    query.exec(exec_str);
}

void DataBase::DeleteExec(QTcpSocket *socket, const QString &send_user_id, const QString &recv_user_id)
{
    QString send_hashmail = QCryptographicHash::hash(send_user_id.toUtf8(),QCryptographicHash::Md5).toHex();
    QString recv_hashmail = recv_user_id;
    QSqlQuery query1,query2;
    QString friendstr1, friendstr2;
    QList<QString> friendlist1, friendlist2;
    QString exec_str1 = "select friend from users where mail = '"+send_hashmail+"'";
    QString exec_str2 = "select friend from users where mail = '"+recv_hashmail+"'";
    query1.exec(exec_str1);
    query1.next();
    friendstr1 = query1.value(0).toString();
    query2.exec(exec_str2);
    query2.next();
    friendstr2 = query2.value(0).toString();
    friendlist1 = friendstr1.split('#');
    friendlist2 = friendstr2.split('#');
    int flag1=0, flag2=0;
    if(friendlist1.length()>1)
    {
        for(int i=1;i<friendlist1.length();i++)
        {
            if(recv_hashmail == friendlist1.at(i))
            {
                friendlist1.removeAt(i);
                flag1=1;
                break;
            }
        }
        for(int i=1;i<friendlist2.length();i++)
        {
            if(send_hashmail == friendlist2.at(i))
            {
                friendlist2.removeAt(i);
                flag2=1;
                break;
            }
        }
        if(flag1)
        {
            friendstr1 = "head";
            for(int i=1; i<friendlist1.length(); i++)
            {
                friendstr1 = friendstr1 + '#' + friendlist1.at(i);
            }
            exec_str1 = "update users set friend = '" + friendstr1 + "' where mail = '" + send_hashmail + "'";
            query1.exec(exec_str1);
        }
        if(flag2)
        {
            friendstr2 = "head";
            for(int i=1; i<friendlist2.length(); i++)
            {
                friendstr2 = friendstr2 + '#' + friendlist2.at(i);
            }
            exec_str2 = "update users set friend = '" + friendstr2 + "' where mail = '" + recv_hashmail + "'";
            qDebug()<<exec_str2;
            query2.exec(exec_str2);
        }
    }
    QList<UserInfo> list = GetFriendList(send_hashmail);
    emit updateList(socket,list);
}

QList<UserInfo> DataBase::GetFriendList(QString hashmail)
{
    QSqlQuery query;
    QString exec_str = "select friend from users where mail = '"+hashmail+"'";
    query.exec(exec_str);
    query.next();
    QString friendlist = query.value(0).toString();

    QList <UserInfo> friendList;
    QList <QString> tmpList = friendlist.split('#');

    if(tmpList.length()>1)
    {
        for(int i=1; i<tmpList.length(); i++)
        {
            QString tmp_mail = tmpList.at(i), tmp_name;
            exec_str = "select user_name from users where mail = '"+tmp_mail+"'";
            query.exec(exec_str);
            query.next();
            tmp_name = query.value(0).toString();
            UserInfo friendInfo;
            friendInfo.mail = tmp_mail;
            friendInfo.user_name = tmp_name;
            friendList.append(friendInfo);
        }
    }
    return friendList;
}
