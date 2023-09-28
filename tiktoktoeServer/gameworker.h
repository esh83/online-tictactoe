#ifndef GAMEWORKER_H
#define GAMEWORKER_H

#include <QThread>
#include <QObject>
#include <QTcpSocket>

class GameWorker : public QObject
{
    Q_OBJECT
public:
    explicit GameWorker(qintptr socketDesc,QString id, QObject *parent = nullptr);
signals:
    void error(QTcpSocket::SocketError socketerror);
    void finished();
    void finishedGameAbnormal();
    void playerLoggedIn(QString playerId);
    void turnDone(int move);

public slots:
    void run();
    void readyRead();
    void disconnected();
    void turnSlot(QString playeId,QVector<QString> pieces);
    void waitSlot(QString playeId,QVector<QString> pieces);
    void gameFinishedSlot(int status ,QVector<QString> pieces);


private:
    QTcpSocket *m_socket;
    qintptr m_socketDescriptor;
    QString m_id;
    int m_piece;
    QString m_username;
    static QByteArray makeBytes(const QMap<QString,QString> &map);

};

#endif // GAMEWORKER_H
