#include "msg.h"

Msg::Msg(QObject *parent) : QObject(parent), socket(new QTcpSocket(this))
{
     socket->connectToHost("192.168.1.105", 8080); //Server ip and port
     connect(socket, &QTcpSocket::readyRead, this, &Msg::slotReadyRead);
     connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
     this, SLOT(StatusChanged(QAbstractSocket::SocketState)));
     time.start();

     encrypt.generatePairKey(open_key, private_key, QRSAEncryption::RSA_128);

}

void Msg::StatusChanged(QAbstractSocket::SocketState socketState){
    if (socketState == QAbstractSocket::UnconnectedState){
        emit ConnectionError();
    }
}

void Msg::jsonReceived(const QJsonObject &docObj)
{
    time.restart();
    const QJsonValue typeVal = docObj.value(QLatin1String("type"));
    if (typeVal.isNull() || !typeVal.isString()){
        return;
    }
    if (typeVal.toString().compare(QLatin1String("check"), Qt::CaseInsensitive) == 0){
        QJsonObject message;
        message["type"] = QStringLiteral("check");
        WriteToSocket(QJsonDocument(message).toJson());
    }
    else if (typeVal.toString().compare(QLatin1String("auth"), Qt::CaseInsensitive) == 0) {
        const QJsonValue resultVal = docObj.value(QLatin1String("success"));
        if (resultVal.isNull() || !resultVal.isBool())
            return;
        const bool loginSuccess = resultVal.toBool();
        if (loginSuccess) {
            const QJsonValue Session = docObj.value(QLatin1String("session_id"));
            session_id = Session.toString();
            emit loggedIn();
            return;
        }
        else{
            const QJsonValue reason = docObj.value(QLatin1String("reason"));
            emit loginError(reason.toString());
        }

    }
    else if (typeVal.toString().compare(QLatin1String("reg"), Qt::CaseInsensitive) == 0) {
        const QJsonValue resultVal = docObj.value(QLatin1String("success"));
        if (resultVal.isNull() || !resultVal.isBool())
            return;
        const bool regSuccess = resultVal.toBool();
        if (regSuccess) {
            emit regOK();
            return;
        }
        else emit regError();

    }
    else if (typeVal.toString().compare(QLatin1String("Invite"), Qt::CaseInsensitive) == 0) {
        const QJsonValue usernameVal = docObj.value(QLatin1String("user"));
        const QJsonValue encryptKeyVal = docObj.value(QLatin1String("key"));
        if (usernameVal.isNull() || !usernameVal.isString())
            return;
        if (encryptKeyVal.isNull() || !encryptKeyVal.isString())
            return;
        emit InvFromUser(usernameVal.toString(), encryptKeyVal.toString());
    }

    else if (typeVal.toString().compare(QLatin1String("AcceptInvite"), Qt::CaseInsensitive) == 0) {
        const QJsonValue usernameVal = docObj.value(QLatin1String("user"));
        const QJsonValue encryptKeyVal = docObj.value(QLatin1String("key"));
        if (usernameVal.isNull() || !usernameVal.isString())
            return;
        if (encryptKeyVal.isNull() || !encryptKeyVal.isString())
            return;

        activeUser = usernameVal.toString();
        encrypt_key = QByteArray::fromHex(encryptKeyVal.toString().toUtf8());

        emit AcceptedInv();
    }

    else if (typeVal.toString().compare(QLatin1String("DeclineInvite"), Qt::CaseInsensitive) == 0) {
        const QJsonValue usernameVal = docObj.value(QLatin1String("user"));
        const QJsonValue reasonVal = docObj.value(QLatin1String("reason"));
        if (usernameVal.isNull() || !usernameVal.isString())
            return;
        if (reasonVal.isNull() || !reasonVal.isString())
            return;

        emit DeclinedInv(usernameVal.toString(), reasonVal.toString());
    }
    else if (typeVal.toString().compare(QLatin1String("message"), Qt::CaseInsensitive) == 0) {
        const QJsonValue textVal = docObj.value(QLatin1String("message"));
        if (textVal.isNull() || !textVal.isString())
            return;

        emit receivemsg(QString(encrypt.decode(QByteArray::fromHex(textVal.toString().toUtf8()), private_key, QRSAEncryption::RSA_128)));

    }
    else if (typeVal.toString().compare(QLatin1String("image"), Qt::CaseInsensitive) == 0) {
        const QJsonValue imgVal = docObj.value(QLatin1String("image"));
        if (imgVal.isNull() || !imgVal.isString())
            return;

        emit receiveimg(QByteArray::fromHex(imgVal.toString().toUtf8()));

    }
    else if (typeVal.toString().compare(QLatin1String("DisconnectFromChat"), Qt::CaseInsensitive) == 0) {
        const QJsonValue usernameVal = docObj.value(QLatin1String("user"));
        if (usernameVal.isNull() || !usernameVal.isString())
            return;
        emit userLeft(usernameVal.toString());
    }
}

void Msg::WriteToSocket(const QByteArray &data){
    if (time.elapsed() > 60000) emit ConnectionError();
    else{
        QByteArray writebleData;
        writebleData.setNum(data.length());
        writebleData.append(data);
        socket->write(writebleData);
    }
}

void Msg::slotReadyRead()
{
        QByteArray jsonData;
        while (socket->bytesAvailable()) {
            jsonData.append(socket->readAll());
        }

        Data.append(jsonData);

        if(Data.at(Data.length()-1) == '}'){
            QJsonParseError parseError;
            const QJsonDocument jsonDoc = QJsonDocument::fromJson(Data, &parseError);
            if (parseError.error == QJsonParseError::NoError) {
                if (jsonDoc.isObject()){
                           jsonReceived(jsonDoc.object());
                }
            }
            else qDebug() << "Json Parse Error" << parseError.error;
            qDebug() << Data;
            Data.clear();
        }
}

void Msg::InviteUser(const QString &user){
    if (socket->state() == QAbstractSocket::ConnectedState) { // if the client is connected
            QJsonObject message3;
            message3["type"] = QStringLiteral("Invite");
            message3["session_id"] = session_id;
            message3["user"] = user;
            message3["key"] = QString(open_key.toHex());

            WriteToSocket(QJsonDocument(message3).toJson());
        }
}

void Msg::AcceptInvite(){
    if (socket->state() == QAbstractSocket::ConnectedState) { // if the client is connected
            QJsonObject message;
            message["type"] = QStringLiteral("AcceptInvite");
            message["session_id"] = session_id;
            message["user"] = activeUser;
            message["key"] = QString(open_key.toHex());

            WriteToSocket(QJsonDocument(message).toJson());
        }
}

void Msg::DeclineInvite(const QString &User){
    if (socket->state() == QAbstractSocket::ConnectedState) { // if the client is connected
            QJsonObject message;
            message["type"] = QStringLiteral("DeclineInvite");
            message["session_id"] = session_id;
            message["user"] = User;
            message["reason"] = QStringLiteral("The user is busy");

            WriteToSocket(QJsonDocument(message).toJson());
        }
}

void Msg::login(const QString &Login, const QString &Pass){
    if (socket->state() == QAbstractSocket::ConnectedState) { // if the client is connected

            QJsonObject message;
            message["type"] = QStringLiteral("auth");
            message["login"] = Login;
            message["password"] = Pass;

            WriteToSocket(QJsonDocument(message).toJson());
        }
}

void Msg::reg(const QString &Login, const QString &Pass){
    if (socket->state() == QAbstractSocket::ConnectedState) { // if the client is connected
            QJsonObject message;
            message["type"] = QStringLiteral("reg");
            message["login"] = Login;
            message["password"] = Pass;

            WriteToSocket(QJsonDocument(message).toJson());
        }
}

void Msg::SendMessage(const QString &message){
    if (socket->state() == QAbstractSocket::ConnectedState) {
            QJsonObject message3;

            message3["type"] = QStringLiteral("message");
            message3["session_id"] = session_id;
            message3["message"] = QString(encrypt.encode(message.toUtf8(), encrypt_key, QRSAEncryption::RSA_128).toHex());

            WriteToSocket(QJsonDocument(message3).toJson());

        }
}

void Msg::SendImage(const QString &hex){
    if (socket->state() == QAbstractSocket::ConnectedState) { // if the client is connected

            QJsonObject message3;
            message3["type"] = QStringLiteral("image");
            message3["session_id"] = session_id;
            message3["image"] = hex;

            WriteToSocket(QJsonDocument(message3).toJson());

        }
}
