#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <iostream>
#include <cstring>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

	// test classes with diagnostic string
	void setMsg(std::string mymsg);
	void showMsg();

private:
    Ui::MainWindow *ui;
	// diagnostic string
	std::string msg;
};

#endif // MAINWINDOW_HPP
