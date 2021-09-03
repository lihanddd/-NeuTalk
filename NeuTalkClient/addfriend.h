#ifndef ADDFRIEND_H
#define ADDFRIEND_H

#include <QDialog>
#include <clientsocket.h>
namespace Ui {
class AddFriend;
}

class AddFriend : public QDialog
{
    Q_OBJECT

public:
    explicit AddFriend(ClientSocket *clientsocket, QString user_id, QWidget *parent = nullptr);
    void Init();
    void addinformation(QString Msg); //表示查找后有这个人
    ~AddFriend();

private slots:
    void on_pushButton_find_clicked();
    void RecvAddFriendMsg(QString Msg);

    void on_pushButton_add_clicked();

private:
    Ui::AddFriend *ui;
    QString my_id;
    ClientSocket *my_socket;
};

#endif // ADDFRIEND_H
