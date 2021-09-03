#include "addfriend.h"
#include "ui_addfriend.h"
AddFriend::AddFriend(ClientSocket *clientsocket, QString user_id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddFriend)
{
    ui->setupUi(this);
    my_socket = clientsocket;
    my_id = user_id;
    connect(my_socket,&ClientSocket::RecvAddFriend,this,&AddFriend::RecvAddFriendMsg);
}

AddFriend::~AddFriend()
{
    delete ui;
}

void AddFriend::RecvAddFriendMsg(QString Msg){
    if(Msg.startsWith("find succeed:"))
    {
        QString sendMsg = Msg.mid(13);
        addinformation(sendMsg);
    }
    else if(Msg == "find failed")
    {
        ui->pushButton_add->setDisabled(true);
        ui->label->setText(tr("没有找到该用户，请检查用户名是否正确"));
    }
    else if(Msg == "alreadyadd")
    {
        ui->label->setText(tr("该用户已经是您的好友，请勿重复添加"));
    }
    else if(Msg == "add succeed")
    {
        ui->label->setText(tr("添加成功"));
        accept();
    }
}
void AddFriend::addinformation(QString Msg){
    QList<QString> list = Msg.split('$');
    QString name = list[0];
//    QString head = data.value("head").toString();
    //image: url(:/head/images/A.jpg)
//    head = tr("image: url(:/head/images/") + head + tr(".png)");
    ui->label_head->setStyleSheet("border-image: url(:/img/img/Customer.png);");
    ui->label->setText(name);
    ui->pushButton_add->setDisabled(false);
}

void AddFriend::on_pushButton_find_clicked()//查找
{
    QString user_id = ui->lineEdit->text();
    my_socket->AddFriendMsg(my_id, user_id, 0);
}


void AddFriend::on_pushButton_add_clicked()//添加
{
    QString user_id = ui->lineEdit->text();
    if(user_id == my_id)
    {
        ui->label->setText("不可以添加自己");
        return;
    }
    my_socket->AddFriendMsg(my_id, user_id, 1);
}

