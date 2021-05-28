#ifndef REGISTRATIONFORM_H
#define REGISTRATIONFORM_H

#include <QWidget>
#include "msg.h"
#include <QCryptographicHash>
#include <QRegExpValidator>


namespace Ui {
class RegistrationForm;
}

class RegistrationForm : public QWidget
{
    Q_OBJECT

public:
    explicit RegistrationForm(QWidget *parent = nullptr);
    ~RegistrationForm();
    bool DataReady = false;

public slots:
    void getData(Msg* data);

signals:
    void Login();

private slots:
    void regOK();
    void regError();

    void on_RegistationButton_clicked();

    void on_BackToLoginButton_clicked();

    void ConnectionError();

private:
    Ui::RegistrationForm *ui;
    Msg *msg;
};

#endif // REGISTRATIONFORM_H
