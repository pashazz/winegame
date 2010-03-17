#include "mainwindow.h"
#include "ui_mainwindow.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    if (qApp->arguments().empty())
        cdMode = false; //Not to check CD
    else
    {
        if (qApp->arguments().length() > 1) {
        QString cdpath = qApp->arguments().at(1);
        QDir td (QDir::rootPath());
        cdMode = td.exists(cdpath);
        diskpath = cdpath;
    }
        else
            cdMode = false;
    }
    //показываем в статусбаре пть к диску
    QLabel * cdlab = new QLabel (diskpath);
   statusBar()->addWidget(cdlab);
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
        it->setText(engine::getName( gamepath + QDir::separator() + entry));
        //загружаем icon как значок игры (если есть)
        it->setIcon(engine::getIcon(gamepath + QDir::separator() + entry));
        }
    }


void MainWindow::lauchEngine(QString pkgpath)
{

    engine *eng = new engine (this);
   eng->setDiskpath(diskpath);
   eng->setCdMode(cdMode);
   eng->lauch(pkgpath);

}

void MainWindow::on_buttonBox_accepted()
{
    if (!ui->lstGames->selectedItems().isEmpty()) {
        lauchEngine(ui->lstGames->selectedItems().first()->data(Qt::UserRole).toString());
    }
    else
    {
        QMessageBox::warning(this, tr("Warning"), tr("Select item to run"));
    }
}



void MainWindow::on_lstGames_itemClicked(QListWidgetItem* item)
{
    ui->lblNote->setText(engine::getNote(item->data(Qt::UserRole).toString()));
}
