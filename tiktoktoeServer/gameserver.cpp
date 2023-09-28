#include "gameserver.h"
#include "gameworker.h"
#include <QUuid>
#include <QVector>

GameServer::GameServer() : QTcpServer(nullptr),convertTo25{0, 6, 7, 8, 11, 12, 13, 16, 17, 18},directions{1, 5, 4, 6},symbols{"X", "O", "_", "B"}
{
    resetBoard();
    turnIndex = 0;
}

void GameServer::startServer()
{
    int port = 1234;
    if(!this->listen(QHostAddress::LocalHost, port))
    {
        qDebug() << "Could not start server";
    }
    else
    {
        qDebug() << "Listening to port " << port << " on localhost" << "...";
    }

}

void GameServer::showError(QAbstractSocket::SocketError err)
{
    QString errorMessage="";
    switch (err) {
    case QAbstractSocket::RemoteHostClosedError:
        errorMessage = "remote host closed";
        break;
    case QAbstractSocket::HostNotFoundError:
        errorMessage = "host not found";
        break;
    case QAbstractSocket::ConnectionRefusedError:
        errorMessage = "connection refued";
        break;
    default:
        errorMessage = "other erroers in tcp";
    }
    qDebug()  << errorMessage;

}

void GameServer::addPlayer(QString id)
{
    m_players.append(id);
    //start the game
    if(m_players.length() == 2){
        emit turn(m_players[0],makePiecesVector());
        emit wait(m_players[1],makePiecesVector());
    }
}

void GameServer::handleDoneTurn(int move)
{
    int status = CONTINUE;

        //  FIRST PLAYER IS NAIGHT AND SECOND PLAYER IS CROSS

        // FIRST PLAYER MOVE
    if(turnIndex==0){
        turnIndex =1;
        board[convertTo25[move]] = NAUGHT;
        status = gameStatus(NAUGHT, convertTo25[move]);
        switch (status)
        {
        case DRAW:
            emit gameFinished(DRAW,makePiecesVector());
            resetBoard();
            turnIndex=0;
            m_players.clear();
            break;
        case WIN_NAUGHT:
            emit gameFinished(WIN_NAUGHT,makePiecesVector());
            resetBoard();
            turnIndex=0;
            m_players.clear();
            break;
        case WIN_CROSS:
            emit gameFinished(WIN_CROSS,makePiecesVector());
            resetBoard();
            turnIndex=0;
            m_players.clear();
            break;
        case CONTINUE:
            emit turn(m_players[1],makePiecesVector());
            emit wait(m_players[0],makePiecesVector());
            break;
        }
        // SECOND PLAYER MOVE
    }else if(turnIndex==1){
        turnIndex=0;
        board[convertTo25[move]] = CROSS;
        status = gameStatus(CROSS, convertTo25[move]);
        switch (status)
        {
        case DRAW:
        emit gameFinished(DRAW,makePiecesVector());
            resetBoard();
             turnIndex=0;
            m_players.clear();
            break;
        case WIN_NAUGHT:
        emit gameFinished(WIN_NAUGHT,makePiecesVector());
            resetBoard();
            turnIndex=0;
            m_players.clear();
            break;
        case WIN_CROSS:
        emit gameFinished(WIN_CROSS,makePiecesVector());
            resetBoard();
            turnIndex=0;
            m_players.clear();
            break;
        case CONTINUE:
            emit turn(m_players[0],makePiecesVector());
            emit wait(m_players[1],makePiecesVector());
            break;
        }

    }



}

void GameServer::incomingConnection(qintptr socketDescriptor)
{
    if(m_players.length() == 2){
          qDebug() << "server currently can not handle more than 2 client";
        return;
    }
        // We have a new connection
        qDebug() << socketDescriptor << " Connecting...";
         QString uid = QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces);

        // Every new connection will be run in a newly created thread


         QThread* thread = new QThread();
         GameWorker *worker = new GameWorker(socketDescriptor,uid, nullptr);
         worker->moveToThread(thread);
         connect( worker, &GameWorker::error, this, &GameServer::showError);
         connect( thread, &QThread::started, worker, &GameWorker::run);
         connect( worker, &GameWorker::finishedGameAbnormal, this, [&]{
             resetBoard();
             turnIndex=0;
             m_players.clear();
         });
         connect( worker, &GameWorker::finished, thread, &QThread::quit);
         connect( worker, &GameWorker::finished, worker, &GameWorker::deleteLater);
         connect( thread, &QThread::finished, thread, &QThread::deleteLater);
         thread->start();

        connect(worker,&GameWorker::playerLoggedIn,this,&GameServer::addPlayer);
         connect(worker,&GameWorker::turnDone,this,&GameServer::handleDoneTurn);

        connect(this,&GameServer::turn,worker,&GameWorker::turnSlot);
        connect(this,&GameServer::wait,worker,&GameWorker::waitSlot);
        connect(this,&GameServer::gameFinished,worker,&GameWorker::gameFinishedSlot);
        thread->start();

}

QVector<QString> GameServer::makePiecesVector()
{

        QVector<QString> vec;
        for (int i = 1; i <= 9; i++)
        {
        vec.append(symbols[board[convertTo25[i]]]);
        }
        return vec;


}


int GameServer::foundInDir(int square, int player, int dir)
{
        int found = 0;
        while (board[square] == player)
        {
        found++;
        square += dir;
        }
        return found;
}

bool GameServer::isThreeInRow(int square, int player)
{
        int counts = 1;
        for (int i = 0; i < 4; i++)
        {
        int dir = directions[i];
        counts += foundInDir( square + dir, player, dir);
        counts += foundInDir(square - dir, player, -1 * dir);
        if (counts == 3)
        {
            return true;
        }
        counts = 1;
        }
        return false;
}

bool GameServer::hasEmpty()
{
        for (int i = 1; i <= 9; i++)
        {
        if (board[convertTo25[i]] == EMPTY)
            return true;
        }
        return false;
}

int GameServer::gameStatus(int player, int square)
{
        if (isThreeInRow(square, player))
        {
        if (player == NAUGHT)
            return WIN_NAUGHT;
        else
            return WIN_CROSS;
        }
        if (!hasEmpty())
        return DRAW;
        return CONTINUE;
}

void GameServer::resetBoard()
{
        for (int i = 0; i < 25; i++)
        {
        board[i] = BORDER;
        }
        for (int i = 1; i <= 9; i++)
        {
        board[convertTo25[i]] = EMPTY;
        }
}
