#ifndef GAMEDIALOG_H
#define GAMEDIALOG_H

#include <QDialog>

namespace Ui {
class gameDialog;
}

class gameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit gameDialog(QString username,QWidget *parent = nullptr);
    ~gameDialog();

signals:
    void turnDone(QString move);
public slots:
    void turn(QVector<QString> pieces);
    void wait(QVector<QString> pieces);
    void gameFinished(QString result,QVector<QString> pieces);

private slots:
    void on_btn1_clicked();

    void on_btn2_clicked();

    void on_btn3_clicked();

    void on_btn4_clicked();

    void on_btn5_clicked();

    void on_btn6_clicked();

    void on_btn7_clicked();

    void on_btn8_clicked();

    void on_btn9_clicked();

private:
    Ui::gameDialog *ui;
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
    void enableAllButtons();
    void disableAllButtons();
    void updateBoardUi(QVector<QString> pieces);
    QString m_playerUsername;
};

#endif // GAMEDIALOG_H
