#include "gameworker.h"
#include <QCryptographicHash>
#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
GameWorker::GameWorker(qintptr socketDesc,QString id, QObject *parent) :
    QObject(parent)
{
    this->m_socketDescriptor = socketDesc;
    this->m_id = id;
}

void GameWorker::run()
{
    QString dbId = QString("thread_%1").arg(m_id);
    QString path = QCoreApplication::applicationDirPath();
    QString dbPath = path + "/data.db";
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE",  dbId);
    db.setDatabaseName(dbPath);
    if (!db.open()) {
        qDebug() << "error in opening database : " << db.lastError().text();
        return;
    } else {
        qDebug() << "Database opened successfully";
        QSqlQuery qry(QSqlDatabase::database(dbId));
        qry.prepare("CREATE TABLE IF NOT EXISTS tblusers (id INTEGER UNIQUE PRIMARY KEY, username TEXT UNIQUE ,password TEXT)");
        if( !qry.exec() ){
            qDebug() << "error in creating tblusers : " << db.lastError().text();
        }
        else{
            //qDebug() << "Table users created!";
        }
    }

    m_socket = new QTcpSocket();

    // set the descriptor
    if(!m_socket->setSocketDescriptor(this->m_socketDescriptor))
    {
        emit error(m_socket->error());
        return;
    }

    connect(m_socket, &QAbstractSocket::readyRead, this, &GameWorker::readyRead, Qt::DirectConnection);
    connect(m_socket, &QAbstractSocket::disconnected, this, &GameWorker::disconnected);

    qDebug() << m_socketDescriptor << " Client connected";
}

void GameWorker::readyRead()
{
    //reading data and store it in a map
    QByteArray bytes = m_socket->readAll();
    QDataStream readStream(&bytes, QIODevice::ReadOnly);
    QMap<QString,QString> data;
    readStream >> data;
    QString event = data.value("event");
    QString dbId = QString("thread_%1").arg(m_id);

    //handling different events
    if(event == "register"){
        QString username = data.value("username");
        QString password = data.value("password");
        QString hashedPassword = QString(QCryptographicHash::hash(password.toLocal8Bit(),QCryptographicHash::Md5).toHex());

        QSqlQuery qry(QSqlDatabase::database(dbId));
        qry.prepare( "INSERT INTO tblusers (username,password) VALUES (:uname,:pass)" );
        qry.bindValue(":uname" , username);
        qry.bindValue(":pass" , hashedPassword);

        if( !qry.exec() ){
            qDebug() << "register fail because " << qry.lastError().text();
            QString errMessage = qry.lastError().nativeErrorCode() == "2067" ? "username has already taken please use another username" : "error in register";
            m_socket->write(makeBytes(QMap<QString,QString>{{"event" , "failRegister"},{"message" ,errMessage}}));
        }
        else{
            qDebug() << "register succesfull";
            m_socket->write(makeBytes(QMap<QString,QString>{{"event" , "successRegister"}}));
        }

    }else if(event == "login"){
        QString username = data.value("username");
        QString password = data.value("password");
        QString hashedPassword = QString(QCryptographicHash::hash(password.toLocal8Bit(),QCryptographicHash::Md5).toHex());
        QSqlQuery qry((QSqlDatabase::database(dbId)));
        qry.prepare( "SELECT * FROM tblusers WHERE username=:uname and password=:pass" );
        qry.bindValue(":uname" , username);
        qry.bindValue(":pass" , hashedPassword);
        if( !qry.exec() ){
            qDebug() << "login fail because " << qry.lastError().text();
            m_socket->write(makeBytes(QMap<QString,QString>{{"event" , "failLogin"},{"message" ,"login failed"}}));
        }
        else
        {
            if(qry.next()){
                emit playerLoggedIn(m_id);
                qDebug() << "login succesfull";
                m_username = username;
                m_socket->write(makeBytes(QMap<QString,QString>{{"event" , "successLogin"},{"username" , username}}));

            }else{
                qDebug() << "user not found";
                m_socket->write(makeBytes(QMap<QString,QString>{{"event" , "failLogin"},{"message" ,"user not found"}}));
            }

        }

    }else if(event=="turnDone"){
        emit turnDone(data.value("move").toInt());
    }else if(event=="finished"){
        emit finishedGameAbnormal();
    }else{
        qDebug()<< "unsupported event from client";
    }


}

void GameWorker::disconnected()
{
    qDebug() << m_socketDescriptor << "Disconnected";
    m_socket->deleteLater();
    emit finished();
}



void GameWorker::turnSlot(QString playeId,QVector<QString> pieces)
{
    if(playeId != m_id) return;
    m_socket->write(makeBytes(QMap<QString,QString>{{"event" , "turn"},{"btn1" , pieces[0]},{"btn2" , pieces[1]},{"btn3" , pieces[2]},{"btn4" , pieces[3]},{"btn5" , pieces[4]},{"btn6" , pieces[5]},{"btn7" , pieces[6]},{"btn8" , pieces[7]},{"btn9" , pieces[8]}}));
}

void GameWorker::waitSlot(QString playeId,QVector<QString> pieces)
{
if(playeId != m_id) return;
m_socket->write(makeBytes(QMap<QString,QString>{{"event" , "wait"},{"btn1" , pieces[0]},{"btn2" , pieces[1]},{"btn3" , pieces[2]},{"btn4" , pieces[3]},{"btn5" , pieces[4]},{"btn6" , pieces[5]},{"btn7" , pieces[6]},{"btn8" , pieces[7]},{"btn9" , pieces[8]}}));
}

void GameWorker::gameFinishedSlot(int status, QVector<QString> pieces)
{
QString strStatus;
enum ResultType
{
    DRAW,
    WIN_CROSS,
    WIN_NAUGHT,
    CONTINUE
};
switch (status) {
case DRAW:
        strStatus = "game finished as DRAW";
        break;
case WIN_CROSS:
        strStatus = "X win the game";
        break;
case WIN_NAUGHT:
        strStatus = "O win the game";
        break;

}
m_socket->write(makeBytes(QMap<QString,QString>{{"event" , "gameFinished"},{"status" , strStatus},{"btn1" , pieces[0]},{"btn2" , pieces[1]},{"btn3" , pieces[2]},{"btn4" , pieces[3]},{"btn5" , pieces[4]},{"btn6" , pieces[5]},{"btn7" , pieces[6]},{"btn8" , pieces[7]},{"btn9" , pieces[8]}}));
}

QByteArray GameWorker::makeBytes(const QMap<QString, QString> &map)
{
    QByteArray bytes;
    QDataStream writeStream(&bytes,QIODevice::WriteOnly);
    writeStream << map;
    return bytes;
}
