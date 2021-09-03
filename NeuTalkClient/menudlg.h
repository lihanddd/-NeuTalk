#ifndef MENUDLG_H
#define MENUDLG_H

#include <QDialog>
#include <QListWidget>

namespace Ui {
class MenuDlg;
}

class MenuDlg : public QDialog
{
    Q_OBJECT

public:
    explicit MenuDlg(QWidget *parent = nullptr);
    ~MenuDlg();

signals:
    void deleteFriend(QString friend_name);

private slots:
    void on_listWidget_itemClicked(QListWidgetItem *item);

private:
    Ui::MenuDlg *ui;
};

#endif // MENUDLG_H
