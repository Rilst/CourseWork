#ifndef MESSAGESYSTEM_H
#define MESSAGESYSTEM_H

#include <QWidget>
#include "msg.h"
#include <QBuffer>
#include <QFileDialog>
#include <QMessageBox>

namespace Ui {
class messagesystem;
}

class messagesystem : public QWidget
{
    Q_OBJECT

public:
    explicit messagesystem(QWidget *parent = nullptr);
    ~messagesystem();
    bool DataReady = false;
public slots:
    void getData(Msg*);
private slots:
    void ConnectionError();
    void userLeft(const QString &username);
    void ReceiveMessage(const QString &text);
    void ReceiveImage(const QByteArray &img);
    void on_InputFileButtn_clicked();
    void on_SendButtn_clicked();

signals:
    void backToInv();
    void sendmsg(const QString &text);
    void sendimg(const QString &hex);
private:
    Ui::messagesystem *ui;
    Msg *msg;
};

#endif // MESSAGESYSTEM_H
