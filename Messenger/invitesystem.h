#ifndef INVITESYSTEM_H
#define INVITESYSTEM_H

#include <QWidget>
#include <QMessageBox>
#include <QRegExpValidator>
#include "msg.h"
#include "messagesystem.h"

namespace Ui {
class InviteSystem;
}

class InviteSystem : public QWidget
{
    Q_OBJECT

public:
    explicit InviteSystem(QWidget *parent = nullptr);
    ~InviteSystem();

signals:
    void AcceptInvite();
    void DeclineInvite(const QString &User);
    void SendDataMess(Msg*);
public slots:
    void getData(Msg* data);
    void getInvite(const QString &user, const QString &encrypt_key);
    void getAccept();
    void getDecline(const QString &user, const QString &reason);

private slots:
    void ConnectionError();

    void on_InvButtn_clicked();

private:
    Ui::InviteSystem *ui;
    Msg *msg;
    messagesystem *messsys;
};

#endif // INVITESYSTEM_H
