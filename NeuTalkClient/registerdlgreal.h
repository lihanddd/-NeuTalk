#ifndef REGISTERDLGREAL_H
#define REGISTERDLGREAL_H

#include <QDialog>

namespace Ui {
class RegisterDlgReal;
}

class RegisterDlgReal : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDlgReal(QString mail, QWidget *parent = nullptr);
    ~RegisterDlgReal();

signals:
    void Register(const QString &mail, const QString &passward, const QString &username);

public slots:
    void RegisterMod(bool isRegister);

private slots:
    void on_register_button_clicked();

private:
    QString mail;
    Ui::RegisterDlgReal *ui;
};

#endif // REGISTERDLGREAL_H
