#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    reg = new RegistrationForm();
    inv = new InviteSystem();
    msg = new Msg();

    ui->LoginLine->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9]{0,12}")));
    ui->PassLine->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9#@)(*&!?$%]{255}")));

    this->setWindowTitle("Login");

    connect(reg, &RegistrationForm::Login, this, &MainWindow::show);
    connect(msg, &Msg::loggedIn, this, &MainWindow::loggedIn);
    connect(msg, SIGNAL(loginError(QString)), this, SLOT(loginError(QString)));
    connect(msg, SIGNAL(ConnectionError()), this, SLOT(ConnectionError()));
    connect(this, SIGNAL(sendDataReg(Msg*)), reg, SLOT(getData(Msg*)));
    connect(this, SIGNAL(sendDataMes(Msg*)), inv, SLOT(getData(Msg*)));


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ConnectionError(){
    if (this->isVisible()){
        QMessageBox::critical(this, "Connection Error", "Connection Close!");
        this->close();
    }
}

void MainWindow::loginError(const QString &reason){
    if (this->isVisible()){
        if (reason == "WrongPassOrLogin") QMessageBox::critical(this, "Login Error", "Incorrect login or password!");
        else if (reason == "UserIsAuthorized") QMessageBox::critical(this, "Login Error", "User is authorized!");
    }
}

void MainWindow::on_AuthButton_clicked(){
    msg->login(ui->LoginLine->text(), QString(QCryptographicHash::hash(ui->PassLine->text().toUtf8(),QCryptographicHash::Md5).toHex()));
}

void MainWindow::loggedIn(){
    emit sendDataMes(msg);
    this->close();
    inv->show();
}

void MainWindow::on_RegButton_clicked(){
    if (reg->DataReady == false) emit sendDataReg(msg);
    this->close();
    reg->show();
}
