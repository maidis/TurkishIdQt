#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QStatusBar>
#include <QDebug>
#include <QRandomGenerator>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->radioButtonRandom->setChecked(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonCheck_clicked()
{
    if(TurkishIdCheck(ui->lineEditTC->text()))
    {
        ui->statusBar->setStyleSheet("color:green");
        ui->statusBar->showMessage("Turkish identification number is valid.", 5000);
    }
    else
    {
        ui->statusBar->setStyleSheet("color:red");
        ui->statusBar->showMessage("Turkish identification number is invalid.", 5000);
    }


}

bool MainWindow::TurkishIdCheck(QString TCstring)
{
    // TC ID numbers only consist of numbers
    bool ok;

    long TCnumber = TCstring.toLong(&ok, 10);

    if (!ok)
    {
        return false;
    }

    // TC ID numbers can't be empty or null
    if (TCstring.isEmpty() or TCstring.isNull())
    {
        return false;
    }
    // TC ID numbers consist of 11 digits
    // The first digit cannot be 0
    else if (TCstring.size() != 11 or TCstring.at(0) == '0')
    {
        return false;
    }

    // TC ID numbers' tenth digit d10 = ((d1 + d3 + d5 + d7 + d9) * 7 - (d2 + d4 + d6 + d8)) mod 10
    // TC ID numbers' eleventh digit d11 = sum(d1..d10) mod 10
    int oddSum = 0;
    int evenSum = 0;
    int i = 0;

    do{
        oddSum = oddSum + TCstring.mid(i, 1).toInt();
        evenSum = evenSum + TCstring.mid(i + 1, 1).toInt();
        //qInfo() << TCstring.mid(i, 1).toInt();
        //qInfo() << TCstring.mid(i + 1, 1).toInt();
        i = i + 2;
    } while(i < 8);
    oddSum = oddSum + TCstring.mid(i, 1).toInt();
    //qInfo() << TCstring.mid(i, 1).toInt();

    int firstChecksum = TCstring.mid(9, 1).toInt();
    int finalChecksum = TCstring.mid(10, 1).toInt();

    // int final = (oddSum + evenSum + firstChecksum) % 10;

    // alternative way to control finalChecksum:
    int final = (oddSum * 8) % 10;

    if (finalChecksum != final)
    {
        return false;
    }

    // https://stackoverflow.com/questions/38251229/modulo-with-a-negative-integer-in-c
    // https://stackoverflow.com/questions/4003232/how-to-code-a-modulo-operator-in-c-c-obj-c-that-handles-negative-numbers
    /*int first = ((oddSum * 7) - evenSum) % 10;

    if (first < 0)
    {
        first = first + 10;
    }*/

    // alternative way to control firstChecksum:
    //int first = (((oddSum * 7) - evenSum) % 10 + 10) % 10;

    // alternative and maybe a more optimized way to control firstChecksum:
    int first = (oddSum * 7 + evenSum * 9) % 10;

    return (firstChecksum == first);
}

void MainWindow::on_pushButtonGenerate_clicked()
{
    ui->plainTextEditGenerate->clear();

    if (ui->radioButtonRandom->isChecked())
    {
        for (int i = 0; i < ui->spinBoxGenerate->value(); ++i)
        {
            QRandomGenerator gen = QRandomGenerator::securelySeeded();
            int random = gen.bounded(0, 899999998);
            ui->plainTextEditGenerate->insertPlainText(TurkishIdGenerate(random) + " ");
            //ui->plainTextEditGenerate->appendPlainText(TurkishIdGenerate(i));
            if (i != 0 and i % 99 == 0)
            {
                ui->plainTextEditGenerate->insertPlainText("\n");
            }
        }
    }
    else
    {
        for (int i = 0; i < ui->spinBoxGenerate->value(); ++i)
        {
            int z = 0;
            if (TurkishIdCheck(ui->lineEditTC->text()))
            {
                z = ui->lineEditTC->text().mid(0, 9).toInt() - 100000000;
            }
            ui->plainTextEditGenerate->insertPlainText(TurkishIdGenerate(i + z) + " ");
            //ui->plainTextEditGenerate->appendPlainText(TurkishIdGenerate(i));
            if (i != 0 and i % 99 == 0)
            {
                ui->plainTextEditGenerate->insertPlainText("\n");
            }
        }
    }
}

QString MainWindow::TurkishIdGenerate(unsigned int y)
{
    int x = 100000001 + y;
    int d1 = x / 100000000;
    int d2 = (x / 10000000) % 10;
    int d3 = (x / 1000000) % 10;
    int d4 = (x / 100000) % 10;
    int d5 = (x / 10000) % 10;
    int d6 = (x / 1000) % 10;
    int d7 = (x / 100) % 10;
    int d8 = (x / 10) % 10;
    int d9 = x % 10;

    int oddSum = d1 + d3 + d5 + d7 + d9;
    int evenSum = d2 + d4 + d6 + d8;

    int firstChecksum = ((oddSum * 7) - evenSum) % 10;
    if (firstChecksum < 0)
    {
        firstChecksum += 10;
    }

    int secondChecksum = (oddSum + evenSum + firstChecksum) % 10;

    return QString::number(x) + QString::number(firstChecksum) + QString::number(secondChecksum);
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::information(this, "About", "This application is written for <b>educational purposes</b> only.<br>Do not use it for any illegal purpose.");
}
