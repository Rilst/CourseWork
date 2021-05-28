#include "registrationform.h"
#include "ui_registrationform.h"

RegistrationForm::RegistrationForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegistrationForm)
{
    ui->setupUi(this);
    this->setWindowTitle("Registration");

    ui->lineEdit->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9]{0,12}")));
    ui->lineEdit_2->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9#@)(*&!?$%]{255}")));
}

RegistrationForm::~RegistrationForm()
{
    delete ui;
}
void RegistrationForm::getData(Msg* data){
    msg = data;
    connect(msg, &Msg::regOK, this, &RegistrationForm::regOK);
    connect(msg, &Msg::regError, this, &RegistrationForm::regError);
    connect(msg, SIGNAL(ConnectionError()), this, SLOT(ConnectionError()));

    DataReady = true;
}

void RegistrationForm::on_RegistationButton_clicked()
{
   msg->reg(ui->lineEdit->text(), QString(QCryptographicHash::hash(ui->lineEdit_2->text().toUtf8(),QCryptographicHash::Md5).toHex()));
}

void RegistrationForm::ConnectionError(){
    if (this->isVisible()){
        QMessageBox::critical(this, "Connection Error", "Connection Close!");
        this->close();
    }
}

void RegistrationForm::regError(){
    QMessageBox::critical(this,"Registration Error","Login already registered!");
}

void RegistrationForm::regOK(){
    QMessageBox::information(this,"Registration","You are successfully registered!");
    this->close();
    emit Login();
}

void RegistrationForm::on_BackToLoginButton_clicked()
{
    this->close();
    emit Login();
}
