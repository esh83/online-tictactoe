#include "gamedialog.h"
#include "ui_gamedialog.h"
#include <QMessageBox>

gameDialog::gameDialog(QString username,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::gameDialog),m_playerUsername{username}
{
    ui->setupUi(this);
    disableAllButtons();
    ui->lbl_username->setText("hi " + username);
}

gameDialog::~gameDialog()
{
    delete ui;
}

void gameDialog::turn(QVector<QString> pieces)
{
    updateBoardUi(pieces);
    ui->lbl_wait->setText("it's your turn");

}
void gameDialog::wait(QVector<QString> pieces)
{
    updateBoardUi(pieces);
    disableAllButtons();
    ui->lbl_wait->setText("it's opponent's turn");
}

void gameDialog::gameFinished(QString result, QVector<QString> pieces)
{
    updateBoardUi(pieces);
    disableAllButtons();
    ui->lbl_wait->setText(result);
}


void gameDialog::enableAllButtons()
{
    ui->btn1->setDisabled(false);
    ui->btn2->setDisabled(false);
    ui->btn3->setDisabled(false);
    ui->btn4->setDisabled(false);
    ui->btn5->setDisabled(false);
    ui->btn6->setDisabled(false);
    ui->btn7->setDisabled(false);
    ui->btn8->setDisabled(false);
    ui->btn9->setDisabled(false);
}

void gameDialog::disableAllButtons()
{
    ui->btn1->setDisabled(true);
    ui->btn2->setDisabled(true);
    ui->btn3->setDisabled(true);
    ui->btn4->setDisabled(true);
    ui->btn5->setDisabled(true);
    ui->btn6->setDisabled(true);
    ui->btn7->setDisabled(true);
    ui->btn8->setDisabled(true);
    ui->btn9->setDisabled(true);
}

void gameDialog::updateBoardUi(QVector<QString> pieces)
{

    if(pieces[0]=="X"){
        ui->btn1->setText("X");
        ui->btn1->setDisabled(true);
    }else if(pieces[0]=="O"){
        ui->btn1->setText("O");
        ui->btn1->setDisabled(true);

    }else if(pieces[0]=="_"){
        ui->btn1->setText("_");
        ui->btn1->setDisabled(false);
    }

    if(pieces[1]=="X"){
        ui->btn2->setText("X");
        ui->btn2->setDisabled(true);
    }else if(pieces[1]=="O"){
        ui->btn2->setText("O");
        ui->btn2->setDisabled(true);

    }else if(pieces[1]=="_"){
        ui->btn2->setText("_");
        ui->btn2->setDisabled(false);
    }


    if(pieces[2]=="X"){
        ui->btn3->setText("X");
        ui->btn3->setDisabled(true);
    }else if(pieces[2]=="O"){
        ui->btn3->setText("O");
        ui->btn3->setDisabled(true);

    }else if(pieces[2]=="_"){
        ui->btn3->setText("_");
        ui->btn3->setDisabled(false);
    }

    if(pieces[3]=="X"){
        ui->btn4->setText("X");
        ui->btn4->setDisabled(true);
    }else if(pieces[3]=="O"){
        ui->btn4->setText("O");
        ui->btn4->setDisabled(true);

    }else if(pieces[3]=="_"){
        ui->btn4->setText("_");
        ui->btn4->setDisabled(false);
    }

    if(pieces[4]=="X"){
        ui->btn5->setText("X");
        ui->btn5->setDisabled(true);
    }else if(pieces[4]=="O"){
        ui->btn5->setText("O");
        ui->btn5->setDisabled(true);

    }else if(pieces[4]=="_"){
        ui->btn5->setText("_");
        ui->btn5->setDisabled(false);
    }

    if(pieces[5]=="X"){
        ui->btn6->setText("X");
        ui->btn6->setDisabled(true);
    }else if(pieces[5]=="O"){
        ui->btn6->setText("O");
        ui->btn6->setDisabled(true);

    }else if(pieces[5]=="_"){
        ui->btn6->setText("_");
        ui->btn6->setDisabled(false);
    }

    if(pieces[6]=="X"){
        ui->btn7->setText("X");
        ui->btn7->setDisabled(true);
    }else if(pieces[6]=="O"){
        ui->btn7->setText("O");
        ui->btn7->setDisabled(true);

    }else if(pieces[6]=="_"){
        ui->btn7->setText("_");
        ui->btn7->setDisabled(false);
    }

    if(pieces[7]=="X"){
        ui->btn8->setText("X");
        ui->btn8->setDisabled(true);
    }else if(pieces[7]=="O"){
        ui->btn8->setText("O");
        ui->btn8->setDisabled(true);

    }else if(pieces[7]=="_"){
        ui->btn8->setText("_");
        ui->btn8->setDisabled(false);
    }

    if(pieces[8]=="X"){
        ui->btn9->setText("X");
        ui->btn9->setDisabled(true);
    }else if(pieces[8]=="O"){
        ui->btn9->setText("O");
        ui->btn9->setDisabled(true);

    }else if(pieces[8]=="_"){
        ui->btn9->setText("_");
        ui->btn9->setDisabled(false);
    }

}

void gameDialog::on_btn1_clicked()
{
    emit turnDone("1");
}


void gameDialog::on_btn2_clicked()
{
     emit turnDone("2");
}


void gameDialog::on_btn3_clicked()
{
     emit turnDone("3");
}


void gameDialog::on_btn4_clicked()
{
     emit turnDone("4");
}


void gameDialog::on_btn5_clicked()
{
     emit turnDone("5");
}


void gameDialog::on_btn6_clicked()
{
     emit turnDone("6");
}


void gameDialog::on_btn7_clicked()
{
     emit turnDone("7");
}


void gameDialog::on_btn8_clicked()
{
     emit turnDone("8");
}


void gameDialog::on_btn9_clicked()
{
     emit turnDone("9");
}

