#ifndef GAMESERVER_H
#define GAMESERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QObject>

class GameServer : public QTcpServer
{
    Q_OBJECT
public:
    GameServer();
    void startServer();

signals:
    void wait(QString playerId,QVector<QString> pieces);
    void turn(QString playerId,QVector<QString> pieces);
    void gameFinished(int status,QVector<QString> pieces);
public slots:
    void showError(QTcpSocket::SocketError err);
    void addPlayer(QString id);
    void handleDoneTurn(int move);

protected:
    void incomingConnection(qintptr socketDescriptor) override;
    QVector<QString> m_players;
    int turnIndex;

    //game logics
    const int convertTo25[10];
    const int directions[4];
    const QString symbols[4];
    QVector<QString> makePiecesVector();
    int board[25];
    enum  PiecesType{
        CROSS,
        NAUGHT,
        EMPTY,
        BORDER,
    };
    enum ResultType
    {
        DRAW,
        WIN_CROSS,
        WIN_NAUGHT,
        CONTINUE
    };

    int foundInDir(int square, int player, int dir);
    bool isThreeInRow(int square, int player);
    bool hasEmpty();
    int gameStatus(int player, int square);
    void resetBoard();

};

#endif // GAMESERVER_H
