#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "genova.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_page1Button_clicked();

    void on_page2Button_clicked();

    void on_page3Button_clicked();

    void on_page4Button_clicked();

    void on_startButton_clicked();

    void on_toolButton_clicked();

    void on_fileSelectButton_clicked();

private:
    Genova genova;
    const Genova *gen_ptr;
    Ui::MainWindow *ui;

    void releaseButtons(int);

};

#endif // MAINWINDOW_H
