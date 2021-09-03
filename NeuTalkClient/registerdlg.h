#ifndef REGISTERDLG_H
#define REGISTERDLG_H

#include <QDialog>
#include <clientsocket.h>

namespace Ui {
class RegisterDlg;
}

class RegisterDlg : public QDialog
{
    Q_OBJECT

signals:

public:
    explicit RegisterDlg(ClientSocket *socket, QWidget *parent = nullptr);
    ~RegisterDlg();
    void RegisterMod(bool isRegister);
    bool isRegistered(QString mail);
    void register_wait1s();
    QString mail;
    ClientSocket *clientsocket;

public slots:

private slots:
    void on_register_button_clicked();

    void on_pushButton_send_verification_code_clicked();

private:
    Ui::RegisterDlg *ui;
};

#endif // REGISTERDLG_H
