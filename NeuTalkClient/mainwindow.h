#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QMenu>
#include <clientsocket.h>
#include <logindlg.h>
#include <addfriend.h>
#include <chatwindow.h>
#include <menudlg.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void updateFriend(); //从数据库中获得当前好友
    void Init();
    void gethead(const QString &head);
    void addFriend(QJsonObject data);
    bool run();

signals:
    void StatusChange(int status);

private slots:
    void FitStatus();
    void SetupUser(const QString &user_mail);
    void on_pushButton_clicked();
    void InitUser(const QString &user_name, const QString &friend_list);
    void RecvFriendList(QList<UserInfo>list);
    void on_listWidget_customContextMenuRequested(const QPoint &pos);
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
    void delete_friend(bool check);
    void cancel(bool check);

private:
    Ui::MainWindow *ui;
    ChatWindow *my_chat = NULL;
    LoginDlg *my_logindlg = NULL;
    ClientSocket *my_socket;
    QMenu *menu;
    int status = -1;
    QString user_mail;
    QString user_name;
    QList<UserInfo> friendList;

    void SetTimeLabel();
    void createActions();
};
#endif // MAINWINDOW_H
