#include "logindlg.h"
#include "ui_logindlg.h"
#include <clientsocket.h>
#include <smtp.h>
#include <QHostAddress>
#include <QTimer>
#include <time.h>
#include <QCryptographicHash>

LoginDlg::LoginDlg(ClientSocket *socket, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDlg)
{
    clientsocket = socket;
    connect(this, &LoginDlg::Login, socket, &ClientSocket::Login);
    connect(this, &LoginDlg::Register, socket, &ClientSocket::Register);
    ui->setupUi(this);
}

void wait1s(){
    QEventLoop eventloop;
    QTimer::singleShot(1000, &eventloop, SLOT(quit()));
    eventloop.exec();
}

LoginDlg::~LoginDlg()
{
    delete ui;
}

void LoginDlg::FitStatus(int status)
{
    if(status == 0)
        LoginMod(true);
    else if(status == 1)
        LoginMod(false);
    else if(status == 2)
        registerdlgreal->RegisterMod(true);
    else if(status == 3)
        registerdlgreal->RegisterMod(false);
    else if(status == 4)
        ConnectMod(true);
    else if(status == 5)
        ConnectMod(false);
}

void LoginDlg::on_show_passward_toggled(bool checked)
{
    if(checked==true)
    {
        ui->lineEdit_passward_enter->setEchoMode(QLineEdit::Normal);
    }
    else
    {
        ui->lineEdit_passward_enter->setEchoMode(QLineEdit::Password);
    }
}

void LoginDlg::ConnectMod(bool isConnect)
{
    if(isConnect)
    {
        ui->status->setText("<font style='color:green;'>connected to server!</font>");
    }
    else
    {
        ui->status->setText("<font style='color:red;'>connect failed =.=</font>");
        ui->login_button->setDisabled(true);
        ui->lineEdit_id_enter->setDisabled(true);
        ui->register_button->setDisabled(true);
        ui->lineEdit_passward_enter->setDisabled(true);
    }
}

void LoginDlg::LoginMod(bool isLogin)
{
    if(isLogin)
    {
        ui->status->setText("<font style='color:green;'>login success!</font>");
        emit LoginSuccess(ui->lineEdit_id_enter->text());
        wait1s();
        accept();
    }
    else
    {
        ui->status->setText("<font style='color:red;'>login failed =.=</font>");
        ui->lineEdit_id_enter->clear();
        ui->lineEdit_passward_enter->clear();
        ui->lineEdit_id_enter->setFocus();
        wait1s();
        ui->status->clear();
        ui->login_button->setText("login??");
    }
}

void LoginDlg::on_login_button_clicked()
{
    QString mail = ui->lineEdit_id_enter->text(), passward = ui->lineEdit_passward_enter->text();
    if(mail.length()==0||passward.length()==0)
    {
        ui->status->setText("<font style='color:red;'>empty mail/passward</font>");
        wait1s();
        ui->status->clear();
        return;
    }
    emit Login(mail, passward);
}

void LoginDlg::SendRegisterMsg(const QString &mail, const QString &passward, QString username)
{
    emit Register(mail, passward, username);
}

void LoginDlg::on_register_button_clicked()
{
    registerdlg = new RegisterDlg(clientsocket);
    if(registerdlg->exec()==QDialog::Accepted)
    {
        registerdlgreal = new RegisterDlgReal(registerdlg->mail);
        registerdlg->close();
        connect(registerdlgreal, &RegisterDlgReal::Register, this, &LoginDlg::SendRegisterMsg);
        if(registerdlgreal->exec()==QDialog::Accepted)
        {
            registerdlgreal->close();
            return;
        }
    }
}
