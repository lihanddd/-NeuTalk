#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QPushButton>
#include "qnchatmessage.h"
#include "clientsocket.h"

namespace Ui {
class ChatWindow;
}

class ChatWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ChatWindow(QString send_user_id, QString recv_user_id, QWidget *parent = nullptr);
    ~ChatWindow();
    void dealMessage(QNChatMessage *messageW, QListWidgetItem *item, QString text, QString time, QNChatMessage::User_Type type);
    void dealMessageTime(QString curMsgTime);
protected:
    bool eventFilter(QObject *object, QEvent *event)override;
    void resizeEvent(QResizeEvent *event)override;

signals:
    void sendMsg(Message message);

public slots:
    void recvMsg(Message message);

private slots:

    void on_pushButton_clicked();

private:
    Ui::ChatWindow *ui;
    QPushButton *send;
    QString send_user_id;
    QString recv_user_id;
    void addMessage(Message msg, bool isSelf);
    void sendMessage();
};


#endif // CHATWINDOW_H
