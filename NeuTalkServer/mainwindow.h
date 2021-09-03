#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <neuserver.h>
#include <database.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:

private slots:
    void on_start_server_button_clicked();
    void ConnectDbRes(bool res);
    void LoginRes(QTcpSocket *socket, bool res);
    void RegisterRes(QTcpSocket *socket, bool res);
    void GetUserRes(QTcpSocket *socket, const QString &user_name, QList<UserInfo> friend_list);
    void findRes(QTcpSocket *socket, const QString &user_name, bool res);
    void addRes(QTcpSocket *socket, int status);
    void updateList(QTcpSocket *socket, QList<UserInfo> list);

private:
    Ui::MainWindow *ui;
    NeuServer *my_server = NULL;
    DataBase *my_db = NULL;

    void SetTimeLabel();
    void ShowMessage(QTcpSocket *tcpSocket, const QString& str);
    void InitSocket();
    void InitDatabase();
};

#endif // MAINWINDOW_H
