#include "registerdlgreal.h"
#include "ui_registerdlgreal.h"
#include <QMessageBox>

RegisterDlgReal::RegisterDlgReal(QString mail, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDlgReal)
{
    this->mail = mail;
    ui->setupUi(this);
}

RegisterDlgReal::~RegisterDlgReal()
{
    delete ui;
}

void RegisterDlgReal::on_register_button_clicked()
{
    QString username = ui->lineEdit_username_enter->text();
    QString passward = ui->lineEdit_passward_enter->text();
    QString passward_confirm = ui->lineEdit_passward_confirm->text();
    if(username.length()==0 || passward.length()==0 || passward_confirm.length()==0)
    {
        QMessageBox::warning(NULL,"提示","昵称、密码不能为空");
        return;
    }
    if(passward != passward_confirm)
    {
        QMessageBox::warning(NULL,"提示","密码输入两次不同");
        ui->lineEdit_passward_confirm->clear();
        ui->lineEdit_passward_enter->clear();
        return;
    }
    emit Register(mail, passward, username);
}


void RegisterDlgReal::RegisterMod(bool isRegister)
{
    if(isRegister)
    {
        QMessageBox::warning(NULL,"提示","注册成功");
        accept();
    }
    else
    {
        QMessageBox::warning(NULL,"提示","注册失败");
        ui->lineEdit_passward_confirm->clear();
        ui->lineEdit_passward_enter->clear();
        ui->lineEdit_username_enter->setFocus();
    }
}
