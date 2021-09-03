#include "registerdlg.h"
#include "ui_registerdlg.h"
#include <QTimer>
#include <QTime>
#include <QMessageBox>
#include <smtp.h>

int code;

RegisterDlg::RegisterDlg(ClientSocket *socket, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDlg)
{
    clientsocket = socket;
    ui->setupUi(this);
}

RegisterDlg::~RegisterDlg()
{
    delete ui;
}

void RegisterDlg::register_wait1s(){
    QEventLoop eventloop;
    QTimer::singleShot(1000, &eventloop, SLOT(quit()));
    eventloop.exec();
}

void RegisterDlg::on_register_button_clicked()
{
    QString mail = ui->lineEdit_id_enter->text();
    if(mail.length()==0)
    {
        ui->register_status->setText("<font style='color:red;'>empty mail</font>");
        register_wait1s();
        ui->register_status->clear();
        return;
    }
    if(QString::number(code)!=ui->lineEdit_verification_enter->text())
    {
        ui->register_status->setText("<font style='color:red;'>wrong verification code</font>");
        register_wait1s();
        ui->register_status->clear();
        ui->lineEdit_verification_enter->clear();
        return;
    }
    QRegExp re("^[A-Za-z0-9]+([_\\.][A-Za-z0-9]+)*@([A-Za-z0-9\\-]+\\.)+[A-Za-z]{2,6}$");
    if(re.exactMatch(mail))
    {
        this->mail = mail;
        accept();
    }
    else
    {
        QMessageBox::warning(NULL,"提示","邮箱格式不正确,请重新输入。");
        ui->lineEdit_id_enter->clear();
    }
}

void RegisterDlg::on_pushButton_send_verification_code_clicked()
{
    QString recvaddr0=ui->lineEdit_id_enter->text();
    QRegExp re("^[A-Za-z0-9]+([_\\.][A-Za-z0-9]+)*@([A-Za-z0-9\\-]+\\.)+[A-Za-z]{2,6}$");
    if(re.exactMatch(recvaddr0)){
        QMessageBox::information(NULL,"提示","验证码将在30s内发送至您的邮箱，请耐心等待。");
        QTime time= QTime::currentTime();
        qsrand(time.msec()+time.second()*1000);
        code = 1000+qrand() % (9999-1000);
        qDebug()<<code;
        Smtp smtp("neutalk_lhd@163.com","OUCCHGAPILVZXFWZ");
        QByteArray recvaddr=recvaddr0.toUtf8();
        QString subject="NeuTalk注册验证码";
        QString content="欢迎注册NeuTalk，本次操作的验证码为："+QString::number(code);
        smtp.send(recvaddr,subject,content);
    }
    else{
        QMessageBox::warning(NULL,"提示","邮箱格式不正确,请重新输入。");
    }
}

