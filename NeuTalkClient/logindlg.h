#ifndef LOGINDLG_H
#define LOGINDLG_H

#include <QDialog>
#include <clientsocket.h>
#include <registerdlg.h>
#include <registerdlgreal.h>

namespace Ui {
class LoginDlg;
}

class LoginDlg : public QDialog
{
    Q_OBJECT

public:
    RegisterDlg *registerdlg;
    RegisterDlgReal *registerdlgreal;
    ClientSocket *clientsocket;
    explicit LoginDlg(ClientSocket *socket = NULL, QWidget *parent = nullptr);
    ~LoginDlg();

signals:
    void Login(const QString &mail, const QString &passward);
    void Register(const QString &mail, const QString &passward, const QString &username);
    void LoginSuccess(const QString &mail);

public slots:
    void FitStatus(int status);
    void SendRegisterMsg(const QString &mail, const QString &passward, QString username = "");

private slots:
    void on_show_passward_toggled(bool checked);
    void on_login_button_clicked();
    void on_register_button_clicked();

private:
    Ui::LoginDlg *ui;

    void ConnectMod(bool isConnect);
    void LoginMod(bool isLogin);
};

#endif // LOGINDLG_H
