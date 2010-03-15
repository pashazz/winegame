#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "engine.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //показываем в статусбаре пть к диску
    if (diskpath != qApp->applicationFilePath())
    {
    QLabel * cdlab = new QLabel (diskpath);
   statusBar()->addWidget(cdlab);


}
buildList();
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

void MainWindow::on_buttonBox_rejected()
{
    qApp->exit(2);
}
void MainWindow::buildList()
{
    QDir wdir (gamepath);
    foreach (QString entry, wdir.entryList(QDir::Dirs  | QDir::NoDotAndDotDot))
    {
        QListWidgetItem *it = new QListWidgetItem (ui->lstGames);
        it->setData(Qt::UserRole, gamepath + QDir::separator() + entry);
        it->setText(entry);
    }
}
void MainWindow::lauchEngine(QString pkgpath)
{

}

void MainWindow::on_buttonBox_accepted()
{
    lauchEngine(ui->lstGames->currentItem()->data(Qt::UserRole).toString());
}
