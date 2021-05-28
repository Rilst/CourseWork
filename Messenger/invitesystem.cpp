#include "invitesystem.h"
#include "ui_invitesystem.h"

InviteSystem::InviteSystem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InviteSystem)
{
    ui->setupUi(this);
    ui->UserInvLine->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9]{0,12}")));

    messsys = new messagesystem;
    connect(this, SIGNAL(SendDataMess(Msg*)), messsys, SLOT(getData(Msg*)));
    connect(messsys, &messagesystem::backToInv, this, &InviteSystem::show);

    this->setWindowTitle("Invite System");
}

InviteSystem::~InviteSystem()
{
    delete ui;
}

void InviteSystem::getData(Msg* data){
    msg = data;
    connect(msg, SIGNAL(ConnectionError()), this, SLOT(ConnectionError()));
    connect(msg, SIGNAL(InvFromUser(QString,QString)), this, SLOT(getInvite(QString,QString)));
    connect(msg, SIGNAL(DeclinedInv(QString,QString)), this, SLOT(getDecline(QString,QString)));
    connect(msg, SIGNAL(AcceptedInv()), this, SLOT(getAccept()));
    connect(this, &InviteSystem::AcceptInvite, msg, &Msg::AcceptInvite);
    connect(this, SIGNAL(DeclineInvite(QString)), msg, SLOT(DeclineInvite(QString)));
}

void InviteSystem::getInvite(const QString &user, const QString &encrypt_key){
    if (this->isVisible()){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Invite!");
        msgBox.setText("Invitation from " + user);
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        msgBox.setEscapeButton(QMessageBox::No);
        msgBox.setButtonText(QMessageBox::Yes, tr("Accept"));
        msgBox.setButtonText(QMessageBox::No, tr("Cancel"));

        int ret = msgBox.exec();

        switch (ret) {
          case QMessageBox::Yes:
            msg->encrypt_key = QByteArray::fromHex(encrypt_key.toUtf8());
            msg->activeUser = user;
            emit AcceptInvite();
            if (messsys->DataReady == false) emit SendDataMess(msg);
            this->close();
            messsys->show();


            break;
          case QMessageBox::No:
            emit DeclineInvite(user);
            break;
        }
    }
    else{
        emit DeclineInvite(user);
    }
}

void InviteSystem::getDecline(const QString &user, const QString &reason){
    if (this->isVisible()){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Invite Decline!");
        msgBox.setText(user + " canceled the invitation. " + reason);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setStandardButtons(QMessageBox::Ok);

        msgBox.exec();
       }
}

void InviteSystem::getAccept(){
    if (this->isVisible()){
        emit SendDataMess(msg);
        this->close();
        messsys->show();
    }

}
void InviteSystem::ConnectionError(){
    if (this->isVisible()){
        QMessageBox::critical(this, "Connection Error", "Connection Close!");
        this->close();
    }
}

void InviteSystem::on_InvButtn_clicked()
{
    msg->InviteUser(ui->UserInvLine->text());
}
