#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
