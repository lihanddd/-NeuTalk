#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <clientsocket.h>
#include <logindlg.h>
#include <QDateTime>
#include <QLineEdit>
#include <QTimer>
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    my_socket = new ClientSocket;
    my_logindlg = new LoginDlg(my_socket);
    connect(my_socket, &ClientSocket::StatusChange, this, &MainWindow::FitStatus);
    connect(my_socket, &ClientSocket::RecvInitMsg, this, &MainWindow::InitUser);
    connect(this, &MainWindow::StatusChange, my_logindlg, &LoginDlg::FitStatus);
    connect(my_logindlg, &LoginDlg::LoginSuccess, this, &MainWindow::SetupUser);
    connect(my_logindlg, &LoginDlg::accepted, this, &MainWindow::show);
    connect(my_socket, &ClientSocket::RecvFriendList, this, &MainWindow::RecvFriendList);
    my_socket->ConnectToServer();
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::run()
{
    if(my_logindlg->exec()==QDialog::Accepted){
        delete my_logindlg;
        return true;
    }
    return false;
}

void MainWindow::SetupUser(const QString &user_mail)
{
    my_socket->ConnectToServer();
    this->user_mail = user_mail;
    my_socket->InitUser(user_mail);
}

void MainWindow::InitUser(const QString &user_name, const QString &friend_list)
{   QList<UserInfo> tmpUserInfo;
    QList<QString>tmplist = friend_list.split('#');
    for(int i=0; i<tmplist.length(); i++)
    {
        QList<QString> tmpUser = tmplist.at(i).split('|');
        if(tmpUser.length()!=2)
        {
            break;
        }
        UserInfo userInfo;
        userInfo.mail = tmpUser[0];
        userInfo.user_name = tmpUser[1];
        tmpUserInfo.append(userInfo);
    }
    this->user_name = user_name;
    this->friendList = tmpUserInfo;
    qDebug()<<user_name;
    for(int i=0; i<friendList.length(); i++)
    {
        qDebug()<<friendList[i].mail;
        qDebug()<<friendList[i].user_name;
    }
    updateFriend();
    ui->user_mail->setText(this->user_mail);
    ui->user_name->setText(this->user_name);
}

void MainWindow::FitStatus()
{
    int status = my_socket->GetStatus();
    this->status = status;
    if(my_logindlg != NULL)
        emit StatusChange(status);
    if(status==8)
    {
        qDebug()<<"????????????????????????";
    }
    else if(status==9)
    {
        qDebug()<<"????????????????????????";
    }
}

//void MainWindow::receiveData(QJsonObject(data)){
//    int type = data.value("type").toInt();
//    QString name = data.value("name").toString();
//    if(name == NAME) //????????????????????????
//    {
//        switch (type) {
//        case TYPE_UPDATE_FRIEND: {
//            updateFriend(data);
//            break;
//        }
//        case TYPE_ADD_FRIEND : addFriend(data); break;
//        case TYPE_UPDATE_HEAD : gethead(data.value("head").toString());
//        case TYPE_DELETE : updateFriend(data);
//        }
//    }


//}
void MainWindow::updateFriend()
{
    int num = friendList.length();
    int n = ui->listWidget->count();
    for(int i=0;i<n;i++)
    {
        QListWidgetItem *item = ui->listWidget->takeItem(0);
        delete item;
    }
    for(int i=0; i<num; i++){
        QString name = friendList[i].user_name; //????????????
//        QString mail = friendList[i].mail; //????????????
        QString head = "";  //????????????
        QListWidgetItem *item = new QListWidgetItem;
        item->setText(name);
//        item->setText(mail);
        item->setIcon(QIcon(tr(":/img/img/Customer.png").arg(head)));
        item->setBackgroundColor(QColor(255,245,251));
        ui->listWidget->addItem(item);
    }
}

void MainWindow::RecvFriendList(QList<UserInfo>list)
{
    friendList = list;
    updateFriend();
}

void MainWindow::on_pushButton_clicked()
{
    AddFriend *w = new AddFriend(my_socket, user_mail);
    w->show();
}

//void MainWindow::on_pushButton_delete_clicked()
//{
//    int count = ui->listWidget->currentRow();//??????????????????ListWidget??????????????????????????????
//    friendname = ui->listWidget->item(count)->text();
//    QMessageBox::StandardButton rb = QMessageBox::information(NULL, tr("????????????"),
//                                 tr("??????????????????%1").arg(friendname),
//                                 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
//    if(rb == QMessageBox::Yes)
//    {
//        QJsonObject data_send;
//        data_send.insert("type",TYPE_DELETE);
//        data_send.insert("name",NAME);
//        data_send.insert("friendname",friendname);
//        tcpsocket->send_json(data_send);
//        //???????????? ?????????????????????????????? TYPE???delete

//    }
//}

//void MainWindow::on_pushButton_clicked()
//{
//    my_chat = new ChatWindow(user_mail, "All");
//    connect(my_chat, &ChatWindow::sendMsg, my_socket, &ClientSocket::SendMsg);
//    connect(my_socket, &ClientSocket::RecvMsg, my_chat, &ChatWindow::recvMsg);
//    my_chat->show();
//}

//void MainWindow::InitFriendList(QString friendlist)
//{
//    friendList = friendlist.split('$');
//    qDebug()<<friendList;
//}


void MainWindow::on_listWidget_customContextMenuRequested(const QPoint &pos)
{
    QMenu *cmenu = new QMenu(ui->listWidget);
    cmenu->setStyleSheet("QMenu{background-color:rgb(255,255,255);color:rgb(0, 0, 0);font:10pt ""??????"";}"
"QMenu::item:selected{background-color:#CCDAE7;}");
    QAction *delete_friend_action = cmenu->addAction("????????????");
    QAction *cancel_action = cmenu->addAction("??????");
    connect(delete_friend_action, SIGNAL(triggered(bool)), this, SLOT(delete_friend(bool)));
    connect(cancel_action, SIGNAL(triggered(bool)), this, SLOT(cancel(bool)));
    cmenu->exec(QCursor::pos());
}

void MainWindow::delete_friend(bool checked)
{
    int count = ui->listWidget->currentRow();//??????????????????ListWidget??????????????????????????????
    QString friendname = friendList.at(count).mail;
    QMessageBox::StandardButton rb = QMessageBox::information(NULL, tr("????????????"),
                                 tr("??????????????????%1").arg(friendname),
                                 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if(rb == QMessageBox::Yes)
    {
        my_socket->DeleteFriendMsg(user_mail, friendname);
    }
}

void MainWindow::cancel(bool checked)
{
    return;
}

void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    int count = ui->listWidget->currentRow();//??????????????????ListWidget??????????????????????????????
    QString friendname = ui->listWidget->item(count)->text();
   //????????????
    ChatWindow *my_chat = new ChatWindow(user_name,friendname);
    connect(my_chat, &ChatWindow::sendMsg, my_socket, &ClientSocket::SendMsg);
    connect(my_socket, &ClientSocket::RecvMsg, my_chat, &ChatWindow::recvMsg);
    my_chat->show();
}

