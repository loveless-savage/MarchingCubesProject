#include "mainwindow.hpp"
#include "ui_mainwindow.h"

using namespace std;


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

// enter diagnostic string
void MainWindow::setMsg(string mymsg) {
	msg = mymsg;
}

// print diagnostic string
void MainWindow::showMsg() {
	cout<<msg<<endl;
}
