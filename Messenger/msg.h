#ifndef MSG_H
#define MSG_H

#include <QWidget>
#include <QTcpSocket>
#include <QHostAddress>
#include <QJsonObject>
#include <QMessageBox>
#include <QJsonParseError>
#include <QElapsedTimer>
#include "E:\CFU\Qt-Secret\src\Qt-RSA\qrsaencryption.h"

class Msg : public QObject
{
    Q_OBJECT
public:
    explicit Msg(QObject *parent = nullptr);
    QByteArray encrypt_key;
    QString activeUser;
    QRSAEncryption encrypt;

private slots:
    void slotReadyRead();

public slots:
    void login(const QString &Login, const QString &Pass);
    void reg(const QString &Login, const QString &Pass);
    void StatusChanged(QAbstractSocket::SocketState socketState);
    void InviteUser(const QString &User);
    void AcceptInvite();
    void DeclineInvite(const QString &User);
    void SendMessage(const QString &message);
    void SendImage(const QString &hex);

private :
    QElapsedTimer time;
    QTcpSocket* socket;
    void jsonReceived(const QJsonObject &docObj);
    void WriteToSocket(const QByteArray &data);
    QByteArray Data;
    QString session_id;
    QByteArray private_key, open_key;

signals:
    void ConnectionError();
    void loggedIn();
    void regOK();
    void regError();
    void InvFromUser(const QString &user, const QString &encrypt_key);
    void AcceptedInv();
    void DeclinedInv(const QString &user, const QString &reason);
    void loginError(const QString &reason);
    void messageReceived(const QString &sender, const QString &text);
    void userLeft(const QString &username);
    void receivemsg(const QString &text);
    void receiveimg(const QByteArray &img);
};

#endif // MSG_H
