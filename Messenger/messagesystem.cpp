#include "messagesystem.h"
#include "ui_messagesystem.h"

messagesystem::messagesystem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::messagesystem)
{
    ui->setupUi(this);
    this->setWindowTitle("Chatting");


}

messagesystem::~messagesystem()
{
    delete ui;
}

void messagesystem::getData(Msg* data){
    msg = data;
    connect(msg, SIGNAL(ConnectionError()), this, SLOT(ConnectionError()));
    connect(msg, SIGNAL(userLeft(QString)), this, SLOT(userLeft(QString)));
    connect(this, SIGNAL(sendmsg(QString)), msg, SLOT(SendMessage(QString)));
    connect(this, SIGNAL(sendimg(QString)), msg, SLOT(SendImage(QString)));

    connect(msg, SIGNAL(receivemsg(QString)), this, SLOT(ReceiveMessage(QString)));
    connect(msg, SIGNAL(receiveimg(QByteArray)), this, SLOT(ReceiveImage(QByteArray)));

    DataReady = true;
}

void messagesystem::ConnectionError(){
    if (this->isVisible()){
        QMessageBox::critical(this, "Connection Error", "Connection Close!");
    }
}
void messagesystem::userLeft(const QString &username){
    emit backToInv();
    this->close();

    ui->MessageEdit->clear();
    ui->MessageBrowser->clear();

    QMessageBox msgBox;
    msgBox.setWindowTitle("Disconnect!");
    msgBox.setText(username + " left from chat!");
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setStandardButtons(QMessageBox::Ok);

    msgBox.exec();

}

void messagesystem::ReceiveMessage(const QString &text){
    ui->MessageBrowser->append(QString("<br><font color=\"red\">" + msg->activeUser + ": " + "</font>") + text + "<\br>");
}

void messagesystem::ReceiveImage(const QByteArray &img){

    QPixmap PixImg;
    PixImg.loadFromData(img);
    QByteArray image;
    QBuffer buffer(&image);
    buffer.open(QIODevice::WriteOnly);

    auto newPixmap = PixImg.scaled(messagesystem::width()/100*80,messagesystem::height()/100*80,Qt::KeepAspectRatio);

    newPixmap.save(&buffer, "JPG");

    ui->MessageBrowser->append(QString("<br><font color=\"red\">" + msg->activeUser + ": " + "</font>") + "<img src=\"data:image/jpeg;base64," + image.toBase64() + "\"><\br>");
}

void messagesystem::on_InputFileButtn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"), "", tr("Image Files (*.png *.jpg)"));

    QPixmap img(fileName);
    if (img.isNull()) return;
    QByteArray image;
    QBuffer buffer(&image);
    buffer.open(QIODevice::WriteOnly);
    img = img.scaled(1280,720,Qt::KeepAspectRatio);
    auto newPixmap = img.scaled(messagesystem::width()/100*80,messagesystem::height()/100*80,Qt::KeepAspectRatio);
    newPixmap.save(&buffer, "JPG");

    emit sendimg(QString(image.toHex()));

    ui->MessageBrowser->append(QString("<br><font color=\"blue\">You: </font>") + "<img src=\"data:image/jpeg;base64," + image.toBase64() + "\"><\br>");
}

void messagesystem::on_SendButtn_clicked()
{
   ui->MessageBrowser->append(QString("<br><font color=\"blue\">You: </font>") + ui->MessageEdit->toPlainText() + "<\br>");
   emit sendmsg(ui->MessageEdit->toPlainText());
   ui->MessageEdit->clear();
}
