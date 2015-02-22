#include <QFileDialog>
#include <QThread>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "windows.h"
#include "genova.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //more setup
    ui->page1Button->setStyleSheet("background-color: none");
    ui->page2Button->setStyleSheet("background-color: none");
    ui->page3Button->setStyleSheet("background-color: none");
    ui->page4Button->setStyleSheet("background-color: none");
    ui->stackedWidget->setCurrentIndex(0);

    this->gen_ptr = &this->genova;

    QObject::connect(this->gen_ptr, SIGNAL(sendReport(QString)), ui->resultTextEdit, SLOT(appendPlainText(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::releaseButtons(int index)
{
    switch(index){
    case 0:
        {
           ui->page2Button->setDown(false);
           ui->page3Button->setDown(false);
           ui->page4Button->setDown(false);
           break;
        }
    case 1:
        {
           ui->page1Button->setDown(false);
           ui->page3Button->setDown(false);
           ui->page4Button->setDown(false);
           break;
        }
    case 2:
        {
           ui->page1Button->setDown(false);
           ui->page2Button->setDown(false);
           ui->page4Button->setDown(false);
           break;
        }
    case 3:
        {
           ui->page1Button->setDown(false);
           ui->page2Button->setDown(false);
           ui->page3Button->setDown(false);
           break;
        }
    default: ;
    }
}

void MainWindow::on_page1Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->page1Button->setDown(true);
    releaseButtons(0);
}


void MainWindow::on_page2Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->page2Button->setDown(true);
    releaseButtons(1);

}

void MainWindow::on_page3Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    ui->page3Button->setDown(true);
    releaseButtons(2);

}

void MainWindow::on_page4Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
    ui->page4Button->setDown(true);
    releaseButtons(3);

}

void MainWindow::on_startButton_clicked()
{
    genova.initialize(ui->genome_spinBox->value(), ui->chromosome_spinBox->value(), ui->population_spinBox->value(), ui->generations_spinBox->value(),
                ui->cloning_spinBox->value()/100, ui->mutation_spinBox->value()/100, ui->resurrection_spinBox->value()/100, ui->culling_spinBox->value()/100,
                ui->genome_checkBox->isChecked(), ui->chromosome_checkBox->isChecked(), ui->population_checkBox->isChecked(), ui->fileSelectLineEdit->text());
    ui->resultTextEdit->appendPlainText("");
    ui->resultTextEdit->appendPlainText("GENOVA initialized");
    QThread::msleep(500); //make sure the previous msg is displayed
    genova.run();
}

void MainWindow::on_toolButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::on_fileSelectButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Script"),
                                                    "",
                                                    tr("Text Files (*.txt)"));
    ui->fileSelectLineEdit->setText(fileName);
}
