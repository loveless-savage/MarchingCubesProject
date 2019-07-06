/********************************************************************************
** Form generated from reading UI file 'Qtfringehead.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTFRINGEHEAD_H
#define UI_QTFRINGEHEAD_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QOpenGLWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QtfringeheadClass
{
public:
    QWidget *centralWidget;
    QOpenGLWidget *openGLWidget;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *QtfringeheadClass)
    {
        if (QtfringeheadClass->objectName().isEmpty())
            QtfringeheadClass->setObjectName(QStringLiteral("QtfringeheadClass"));
        QtfringeheadClass->resize(600, 400);
        centralWidget = new QWidget(QtfringeheadClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        openGLWidget = new QOpenGLWidget(centralWidget);
        openGLWidget->setObjectName(QStringLiteral("openGLWidget"));
        openGLWidget->setGeometry(QRect(10, 10, 581, 331));
        QtfringeheadClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(QtfringeheadClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 600, 21));
        QtfringeheadClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(QtfringeheadClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        QtfringeheadClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(QtfringeheadClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        QtfringeheadClass->setStatusBar(statusBar);

        retranslateUi(QtfringeheadClass);

        QMetaObject::connectSlotsByName(QtfringeheadClass);
    } // setupUi

    void retranslateUi(QMainWindow *QtfringeheadClass)
    {
        QtfringeheadClass->setWindowTitle(QApplication::translate("QtfringeheadClass", "Qtfringehead", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class QtfringeheadClass: public Ui_QtfringeheadClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTFRINGEHEAD_H
