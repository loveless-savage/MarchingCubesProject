/********************************************************************************
** Form generated from reading UI file 'QtOGL.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTOGL_H
#define UI_QTOGL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QtOGLClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *QtOGLClass)
    {
        if (QtOGLClass->objectName().isEmpty())
            QtOGLClass->setObjectName(QStringLiteral("QtOGLClass"));
        QtOGLClass->resize(600, 400);
        menuBar = new QMenuBar(QtOGLClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        QtOGLClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(QtOGLClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        QtOGLClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(QtOGLClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        QtOGLClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(QtOGLClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        QtOGLClass->setStatusBar(statusBar);

        retranslateUi(QtOGLClass);

        QMetaObject::connectSlotsByName(QtOGLClass);
    } // setupUi

    void retranslateUi(QMainWindow *QtOGLClass)
    {
        QtOGLClass->setWindowTitle(QApplication::translate("QtOGLClass", "QtOGL", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class QtOGLClass: public Ui_QtOGLClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTOGL_H
