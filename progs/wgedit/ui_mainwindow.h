/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Wed Apr 14 21:52:45 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actNew;
    QAction *actOpen;
    QAction *actSave;
    QAction *action_About;
    QAction *action_About_Qt;
    QAction *action_Quit;
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QMenu *menu_Help;
    QMenu *menu_File;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(600, 400);
        actNew = new QAction(MainWindow);
        actNew->setObjectName(QString::fromUtf8("actNew"));
        actOpen = new QAction(MainWindow);
        actOpen->setObjectName(QString::fromUtf8("actOpen"));
        actSave = new QAction(MainWindow);
        actSave->setObjectName(QString::fromUtf8("actSave"));
        action_About = new QAction(MainWindow);
        action_About->setObjectName(QString::fromUtf8("action_About"));
        action_About_Qt = new QAction(MainWindow);
        action_About_Qt->setObjectName(QString::fromUtf8("action_About_Qt"));
        action_Quit = new QAction(MainWindow);
        action_Quit->setObjectName(QString::fromUtf8("action_Quit"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 600, 21));
        menu_Help = new QMenu(menuBar);
        menu_Help->setObjectName(QString::fromUtf8("menu_Help"));
        menu_File = new QMenu(menuBar);
        menu_File->setObjectName(QString::fromUtf8("menu_File"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menu_File->menuAction());
        menuBar->addAction(menu_Help->menuAction());
        menu_Help->addAction(action_About);
        menu_Help->addAction(action_About_Qt);
        menu_File->addAction(actNew);
        menu_File->addAction(actOpen);
        menu_File->addAction(actSave);
        menu_File->addSeparator();
        menu_File->addAction(action_Quit);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "wgedit", 0, QApplication::UnicodeUTF8));
        actNew->setText(QApplication::translate("MainWindow", "&New", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actNew->setStatusTip(QApplication::translate("MainWindow", "Creates a new WineGame package", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actNew->setShortcut(QApplication::translate("MainWindow", "Ctrl+N", 0, QApplication::UnicodeUTF8));
        actOpen->setText(QApplication::translate("MainWindow", "&Open", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actOpen->setStatusTip(QApplication::translate("MainWindow", "Open a existing WineGame package", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actOpen->setShortcut(QApplication::translate("MainWindow", "Ctrl+O", 0, QApplication::UnicodeUTF8));
        actSave->setText(QApplication::translate("MainWindow", "&Save", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actSave->setStatusTip(QApplication::translate("MainWindow", "Save the package", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actSave->setShortcut(QApplication::translate("MainWindow", "Ctrl+S", 0, QApplication::UnicodeUTF8));
        action_About->setText(QApplication::translate("MainWindow", "&About", 0, QApplication::UnicodeUTF8));
        action_About_Qt->setText(QApplication::translate("MainWindow", "&About Qt", 0, QApplication::UnicodeUTF8));
        action_Quit->setText(QApplication::translate("MainWindow", "&Quit", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        action_Quit->setStatusTip(QApplication::translate("MainWindow", "Quit the program", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        action_Quit->setShortcut(QApplication::translate("MainWindow", "Ctrl+Q, Ctrl+W", 0, QApplication::UnicodeUTF8));
        menu_Help->setTitle(QApplication::translate("MainWindow", "&Help", 0, QApplication::UnicodeUTF8));
        menu_File->setTitle(QApplication::translate("MainWindow", "&File", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
