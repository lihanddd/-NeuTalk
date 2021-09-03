#include "menudlg.h"
#include "ui_menudlg.h"
#include <QtDebug>

MenuDlg::MenuDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MenuDlg)
{
    ui->setupUi(this);
}

MenuDlg::~MenuDlg()
{
    delete ui;
}

void MenuDlg::on_listWidget_itemClicked(QListWidgetItem *item)
{
    if(item->text()=="取消")
    {
        qDebug()<<"cancel";
        this->close();
    }
    else if(item->text()=="删除好友")
    {
        emit deleteFriend(item->text());
        qDebug()<<"delete friend";
        this->close();
    }
}

