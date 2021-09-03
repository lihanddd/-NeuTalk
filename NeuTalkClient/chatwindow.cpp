#include "chatwindow.h"
#include "ui_chatwindow.h"
#include <QDateTime>
#include <QKeyEvent>
#include <QDebug>

ChatWindow::ChatWindow(QString send_user_id, QString recv_user_id, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ChatWindow)
{
    this->send_user_id = send_user_id;
    this->recv_user_id = recv_user_id;
    ui->setupUi(this);

    ui->textEdit->installEventFilter(this);
    resize(800, 600);
}

ChatWindow::~ChatWindow()
{
    delete ui;
}
bool ChatWindow::eventFilter(QObject *object, QEvent *event)
{
    if(object == ui->textEdit)
    {
         if(event->type() == QEvent::KeyPress)
         {
             QKeyEvent* peve = dynamic_cast<QKeyEvent*>(event);
             if((peve->modifiers() & Qt::ControlModifier) && (peve->key() == Qt::Key_Return || peve->key() == Qt::Key_Enter)) //Ctrl + Enter　换行
             {
                 ui->textEdit->insertPlainText(QString(" ") += '\n');
                 return true;
             }
             if(peve->key() == Qt::Key_Return || peve->key() == Qt::Key_Enter) //回车键盘
             {
                 on_pushButton_clicked();
                 return true;
             }
         }
    }
    return QMainWindow::eventFilter(object, event);
}

void ChatWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    ui->title->resize(this->width(), 31);
    ui->textEdit->resize(this->width(), ui->widget->height());
    ui->pushButton->move(ui->textEdit->width()+ui->textEdit->x() - ui->pushButton->width() - 10,
                         ui->textEdit->height()+ui->textEdit->y() - ui->pushButton->height() - 10);


    for(int i = 0; i < ui->listWidget->count(); i++) {
        QNChatMessage* messageW = (QNChatMessage*)ui->listWidget->itemWidget(ui->listWidget->item(i));
        QListWidgetItem* item = ui->listWidget->item(i);

        dealMessage(messageW, item, messageW->text(), messageW->time(), messageW->userType());
    }
}

void ChatWindow::sendMessage()
{
    QString msg = ui->textEdit->toPlainText();

    // 把最后一个回车换行符删掉
    while (msg.endsWith("\n")) {
        msg.remove(msg.length() - 2, 2);
    }
    if(msg.isEmpty())
    {
        return;
    }
    // 清除输入框
    ui->textEdit->clear();

    msg = msg+' ';

    QString time = QString::number(QDateTime::currentDateTime().toTime_t());
    Message Msg;
    Msg.datetime = time;
    Msg.send_user_id = send_user_id;
    Msg.recv_user_id = recv_user_id;
    Msg.content = msg;
    Msg.type = "message";

    dealMessageTime(time);
    QNChatMessage* messageW = new QNChatMessage(ui->listWidget->parentWidget());
    QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
    dealMessage(messageW, item, msg, time, QNChatMessage::User_Me);
    messageW->setTextSuccess();

    emit sendMsg(Msg);
}

void ChatWindow::recvMsg(Message Msg)
{
    if(Msg.recv_user_id == send_user_id)
    {
        QString time = QString::number(QDateTime::currentDateTime().toTime_t());
        dealMessageTime(time);
        QNChatMessage* messageW = new QNChatMessage(ui->listWidget->parentWidget());
        QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
        dealMessage(messageW, item, Msg.content, time, QNChatMessage::User_She);
        messageW->setTextSuccess();
        ui->listWidget->scrollToBottom();
    }
    else
    {
        return;
    }
}

//void MainWindow::on_pushButton_clicked()
//{
//    QString msg = ui->textEdit->toPlainText();
//    ui->textEdit->setText("");
//    QString time = QString::number(QDateTime::currentDateTime().toTime_t()); //时间戳

//    bool isSending = true; // 发送中

//    qDebug()<<"addMessage" << msg << time << ui->listWidget->count();
//    if(ui->listWidget->count()%2) {
//        if(isSending) {
//            dealMessageTime(time);

//            QNChatMessage* messageW = new QNChatMessage(ui->listWidget->parentWidget());
//            QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
//            dealMessage(messageW, item, msg, time, QNChatMessage::User_Me);
//        } else {
//            bool isOver = true;
//            for(int i = ui->listWidget->count() - 1; i > 0; i--) {
//                QNChatMessage* messageW = (QNChatMessage*)ui->listWidget->itemWidget(ui->listWidget->item(i));
//                if(messageW->text() == msg) {
//                    isOver = false;
//                    messageW->setTextSuccess();
//                }
//            }
//            if(isOver) {
//                dealMessageTime(time);

//                QNChatMessage* messageW = new QNChatMessage(ui->listWidget->parentWidget());
//                QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
//                dealMessage(messageW, item, msg, time, QNChatMessage::User_Me);
//                messageW->setTextSuccess();
//            }
//        }
//    } else {
//        if(msg != "") {
//            dealMessageTime(time);

//            QNChatMessage* messageW = new QNChatMessage(ui->listWidget->parentWidget());
//            QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
//            dealMessage(messageW, item, msg, time, QNChatMessage::User_She);
//        }
//    }
//    ui->listWidget->setCurrentRow(ui->listWidget->count()-1);
//}

void ChatWindow::dealMessage(QNChatMessage *messageW, QListWidgetItem *item, QString text, QString time,  QNChatMessage::User_Type type)
{
    messageW->setFixedWidth(this->width());
    QSize size = messageW->fontRect(text);
    item->setSizeHint(size);
    messageW->setText(text, time, size, type);
    ui->listWidget->setItemWidget(item, messageW);
}

void ChatWindow::dealMessageTime(QString curMsgTime)
{
    bool isShowTime = false;
    if(ui->listWidget->count() > 0) {
        QListWidgetItem* lastItem = ui->listWidget->item(ui->listWidget->count() - 1);
        QNChatMessage* messageW = (QNChatMessage*)ui->listWidget->itemWidget(lastItem);
        int lastTime = messageW->time().toInt();
        int curTime = curMsgTime.toInt();
        qDebug() << "curTime lastTime:" << curTime - lastTime;
        isShowTime = ((curTime - lastTime) > 60); // 两个消息相差一分钟
//        isShowTime = true;
    } else {
        isShowTime = true;
    }
    if(isShowTime) {
        QNChatMessage* messageTime = new QNChatMessage(ui->listWidget->parentWidget());
        QListWidgetItem* itemTime = new QListWidgetItem(ui->listWidget);

        QSize size = QSize(this->width(), 40);
        messageTime->resize(size);
        itemTime->setSizeHint(size);
        messageTime->setText(curMsgTime, curMsgTime, size, QNChatMessage::User_Time);
        ui->listWidget->setItemWidget(itemTime, messageTime);
    }
}

void ChatWindow::on_pushButton_clicked()
{
    sendMessage();
}
