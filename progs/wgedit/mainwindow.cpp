#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //connect some things
    connect (ui->action_About_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(ui->action_Quit, SIGNAL(triggered()), qApp, SLOT (quit()));
    //build toolbar
    ui->actOpen->setIcon(QIcon::fromTheme("document-open"));
    ui->actNew->setIcon(QIcon::fromTheme("document-new"));
    ui->actSave->setIcon(QIcon::fromTheme("document-save"));
    ui->mainToolBar->addAction(ui->actNew);
    ui->mainToolBar->addAction(ui->actOpen);
    ui->mainToolBar->addAction(ui->actSave);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}



void MainWindow::on_action_About_triggered()
{
    AboutDialog *dlg = new AboutDialog(this);
    dlg->exec();
}
