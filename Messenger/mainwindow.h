#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#pragma once
#include <QMainWindow>
#include "registrationform.h"
#include "invitesystem.h"
#include "msg.h"
#include <QMessageBox>
#include <QCryptographicHash>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void loggedIn();
    void ConnectionError();
    void loginError(const QString &reason);

    void on_AuthButton_clicked();
    void on_RegButton_clicked();

private:
    InviteSystem *inv;
    RegistrationForm *reg;
    Ui::MainWindow *ui;
    Msg *msg;

signals:
    void sendDataReg(Msg*);
    void sendDataMes(Msg*);
};
#endif // MAINWINDOW_H
