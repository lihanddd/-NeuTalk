#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QDateTime>
#include <QDebug>
#include <QApplication>
#include <cstring>
#include <QHostAddress>
#include <QHostInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lineEdit_port->setText("8888");
    SetTimeLabel();
    QHostInfo info = QHostInfo::fromName(QHostInfo::localHostName());
    QList<QHostAddress> addlist = info.addresses();
    for(int i = 0; i < addlist.count(); i++)
    {
        qDebug() << addlist.at(i).toString();
        ui->choose_box_ip->addItem(addlist.at(i).toString());
    }
    InitDatabase();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_start_server_button_clicked()
{
    if(ui->start_server_button->text()=="start server")
    {
        ui->choose_box_ip->setDisabled(true);
        ui->lineEdit_port->setDisabled(true);
        ui->start_server_button->setText("stop server");
        if(NULL == my_server)
        {
            InitSocket();
            ShowMessage(NULL, "<font style='color:green;'>start server</font>");
            return;
        }
        const QString host_ip = ui->choose_box_ip->currentText();
        QHostAddress host_address;
        host_address.setAddress(host_ip);
        int port = ui->lineEdit_port->text().toInt();
        my_server->Open(host_address, port);
        qDebug()<<host_address;
        qDebug()<<port;
        ShowMessage(NULL, "<font style='color:green;'>reopen server</font>");
    }
    else
    {
        ui->choose_box_ip->setDisabled(false);
        ui->lineEdit_port->setDisabled(false);
        ui->start_server_button->setText("start server");
        my_server->Close();
        ShowMessage(NULL, "<font style='color:red;'>stop server</font>");
    }
}

void MainWindow::LoginRes(QTcpSocket *socket, bool res)
{
    if(res)
    {
        my_server->SendStatus(socket, 0);
    }
    else
    {
        my_server->SendStatus(socket, 1);
    }
}

void MainWindow::RegisterRes(QTcpSocket *socket, bool res)
{
    if(res)
    {
        my_server->SendStatus(socket, 2);
    }
    else
    {
        my_server->SendStatus(socket, 3);
    }

}

void MainWindow::GetUserRes(QTcpSocket *socket, const QString &user_name, QList<UserInfo> friend_list)
{
    my_server->SendUser(socket, user_name, friend_list);
}

void MainWindow::SetTimeLabel()
{
    QPalette palette;
    palette.setColor(QPalette::WindowText,Qt::black);
    ui->label_current_time->setPalette(palette);
    QTimer *CurrentTime = new QTimer(this);
    CurrentTime->start(0);
    connect(CurrentTime,&QTimer::timeout,[=](){
        QDateTime current_time = QDateTime::currentDateTime();
        QString StrCurrentTime = current_time.toString("yyyy-MM-dd hh:mm:ss ddd");
        ui->label_current_time->setText(StrCurrentTime);
    });
}

void MainWindow::ConnectDbRes(bool res)
{
    if(res)
    {
        ShowMessage(NULL, "<font style='color:green;'>connect to database</font>");
    }
    else
    {
        ShowMessage(NULL, "<font style='color:red;'>connect failed</font>");
        ui->start_server_button->setDisabled(true);
    }
}

void MainWindow::findRes(QTcpSocket *socket, const QString &user_name, bool res)
{
    my_server->SendFind(socket, user_name, res);
}

void MainWindow::addRes(QTcpSocket *socket, int status)
{
    my_server->SendAdd(socket, status);
}

void MainWindow::updateList(QTcpSocket *socket, QList<UserInfo> list)
{
    my_server->SendUpdateList(socket, list);
}

void MainWindow::ShowMessage(QTcpSocket *tcpSocket, const QString& str)
{
    QString temp = "";
    if(tcpSocket != NULL)
    {
        temp = QString("[%1:%2]:").arg(tcpSocket->peerAddress().toString()).arg(tcpSocket->peerPort());
    }
    QDateTime dateTime(QDateTime::currentDateTime());
    QString qStr = dateTime.toString("yyyy-MM-dd hh:mm:ss ddd");
    QString message = "<font style='font-size:16px; background-color:white; color:#ff69b4;'>["+qStr+"]</font>" + temp + str;
    ui->message->append(message);
}

void MainWindow::InitDatabase()
{
    my_db = new DataBase();
    connect(my_db, &DataBase::ConnectRes, this, &MainWindow::ConnectDbRes);
    my_db->SetDatabase("QSQLITE","../NeuTalkServer/my.db");
    connect(my_db, &DataBase::LoginRes, this, &MainWindow::LoginRes);
    connect(my_db, &DataBase::RegisterRes, this, &MainWindow::RegisterRes);
    connect(my_db, &DataBase::GetUserRes, this, &MainWindow::GetUserRes);
    connect(my_db, &DataBase::findRes, this, &MainWindow::findRes);
    connect(my_db, &DataBase::addRes, this, &MainWindow::addRes);
    connect(my_db, &DataBase::updateList, this, &MainWindow::updateList);
}

void MainWindow::InitSocket()
{
    my_server = new NeuServer();
    connect(my_server, &NeuServer::ShowMessage, this, &MainWindow::ShowMessage);
    connect(my_server, &NeuServer::LoginToDb, my_db, &DataBase::LoginExec);
    connect(my_server, &NeuServer::RegisterToDb, my_db, &DataBase::RegisterExec);
    connect(my_server, &NeuServer::GetUserToDb, my_db, &DataBase::GetUserExec);
    connect(my_server, &NeuServer::FindToDb, my_db, &DataBase::FindExec);
    connect(my_server, &NeuServer::AddToDb, my_db, &DataBase::AddExec);
    connect(my_server, &NeuServer::DeleteToDb, my_db, &DataBase::DeleteExec);
    const QString host_ip = ui->choose_box_ip->currentText();
    int port = ui->lineEdit_port->text().toInt();
    QHostAddress host_address;
    host_address.setAddress(host_ip);
    qDebug()<<host_address;
    qDebug()<<port;
    my_server->SetNeuServer(host_address, port);
}

