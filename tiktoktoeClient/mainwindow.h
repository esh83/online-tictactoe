#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals :
    void wait(QVector<QString> pieces);
    void turn(QVector<QString> pieces);
    void gameFinished(QString result,QVector<QString> pieces);

public slots:
    void read();
    void error(QAbstractSocket::SocketError err);
    void successConnection();
    void handleTurnDone(QString move);

private slots:
    void on_register_btn_clicked();

    void on_login_btn_clicked();

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;
    QByteArray makeBytes(const QMap<QString,QString> &map);
};
#endif // MAINWINDOW_H
