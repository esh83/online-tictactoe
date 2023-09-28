#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include "gamedialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    socket = new QTcpSocket(this);
    socket->connectToHost(QHostAddress::LocalHost , 1234);
    connect(socket,&QIODevice::readyRead,this,&MainWindow::read);
    connect(socket,&QAbstractSocket::errorOccurred,this,&MainWindow::error);
    connect(socket,&QAbstractSocket::connected,this,&MainWindow::successConnection);

    //getting current time from online rest api
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager,&QNetworkAccessManager::finished ,this,[=](QNetworkReply *reply)
            {
                if (reply->error() != QNetworkReply::NoError)
                {
                    qDebug()<<"gettin time request error: " << reply->errorString();
                }
                else
                {
                    QByteArray response = reply->readAll();
                    QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
                    QJsonObject jsonObj = jsonDoc.object();
                    QString time = jsonObj.value("datetime").toString();
                    ui->lbl_time->setText(time);

                }
                reply->deleteLater();
            });
    QUrl url("https://worldtimeapi.org/api/timezone/Asia/Tehran");
    QNetworkRequest request(url);
    manager->get(request);
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout,this, [=](){
         manager->get(request);
    });
    //update the time every minute
    timer->start(1000 *60);

}

MainWindow::~MainWindow()
{
    delete ui;
    delete socket;
}

void MainWindow::read()
{
    QByteArray bytes= socket->readAll();
    QDataStream readStream(&bytes,QIODevice::ReadOnly);
    QMap<QString,QString> data;
    readStream >> data;
    QString event = data.value("event");
    //handle different event recived from server
    if(event == "successRegister"){
        QMessageBox::information(this,"success" , "register was successful");

    }else if (event == "failRegister"){
        QMessageBox::critical(this,"fail" , data.value("message"));

    }else if(event == "successLogin"){
        gameDialog *gamePage = new gameDialog(data.value("username"));
        connect(gamePage,&QDialog::finished,this,&QMainWindow::show);
        connect(gamePage,&QDialog::finished,this,[&]{
            socket->write(makeBytes(QMap<QString,QString>{{"event" ,"finished"}}));
        });
        connect(gamePage,&QDialog::finished,gamePage,&QObject::deleteLater);
          connect(gamePage,&gameDialog::turnDone, this,&MainWindow::handleTurnDone);
        connect(this,&MainWindow::turn, gamePage,&gameDialog::turn);
        connect(this,&MainWindow::wait, gamePage,&gameDialog::wait);
         connect(this,&MainWindow::gameFinished, gamePage,&gameDialog::gameFinished);

        this->hide();
        gamePage->show();
    }else if(event == "failLogin"){
           QMessageBox::critical(this,"fail" , data.value("message"));
    }else if(event == "wait"){
           QVector<QString> vec;
           vec.append(data.value("btn1"));vec.append(data.value("btn2"));vec.append(data.value("btn3"));vec.append(data.value("btn4"));
           vec.append(data.value("btn5"));vec.append(data.value("btn6"));vec.append(data.value("btn7"));vec.append(data.value("btn8"));
           vec.append(data.value("btn9"));
           emit wait(vec);
    }else if(event == "turn"){
           QVector<QString> vec;
           vec.append(data.value("btn1"));vec.append(data.value("btn2"));vec.append(data.value("btn3"));vec.append(data.value("btn4"));
           vec.append(data.value("btn5"));vec.append(data.value("btn6"));vec.append(data.value("btn7"));vec.append(data.value("btn8"));
           vec.append(data.value("btn9"));
            emit turn(vec);
    }else if(event == "gameFinished"){
            QVector<QString> vec;
            vec.append(data.value("btn1"));vec.append(data.value("btn2"));vec.append(data.value("btn3"));vec.append(data.value("btn4"));
            vec.append(data.value("btn5"));vec.append(data.value("btn6"));vec.append(data.value("btn7"));vec.append(data.value("btn8"));
            vec.append(data.value("btn9"));
            emit gameFinished(data.value("status"),vec);
    }
    else{
        qDebug() << "unsupported event recieved from server";
    }


}

void MainWindow::error(QAbstractSocket::SocketError err)
{
    QString errorMessage="";
    switch (err) {
    case QAbstractSocket::RemoteHostClosedError:
        errorMessage = "can not connect to server : remote host closed";
        break;
    case QAbstractSocket::HostNotFoundError:
        errorMessage = "can not connect to server : host not found";
        break;
    case QAbstractSocket::ConnectionRefusedError:
        errorMessage = "can not connect to server : connection refused";
        break;
    default:
        errorMessage = "can not connect to server : other errors in tcp";
    }
    qDebug()  << errorMessage;
    QMessageBox::critical(this,"error" , errorMessage);
}

void MainWindow::successConnection()
{
    qDebug()<< "connected success";
}

void MainWindow::handleTurnDone(QString move)
{
socket->write(makeBytes(QMap<QString,QString>{{"event" ,"turnDone"} , {"move" , move}}));
}


void MainWindow::on_register_btn_clicked()
{
    QString username = ui->register_username->text();
    QString password = ui->register_password->text();
    socket->write(makeBytes(QMap<QString,QString>{{"event" ,"register"} , {"username" , username} , {"password" , password}}));
}

QByteArray MainWindow::makeBytes(const QMap<QString, QString> &map)
{
    QByteArray bytes;
    QDataStream writeStream(&bytes,QIODevice::WriteOnly);
    writeStream << map;
    return bytes;
}



void MainWindow::on_login_btn_clicked()
{
    QString username = ui->login_username->text();
    QString password = ui->login_password->text();
    socket->write(makeBytes(QMap<QString,QString>{{"event" ,"login"} , {"username" , username} , {"password" , password}}));
}

