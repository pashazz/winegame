#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //set dir
    dir = QDir("");
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

void MainWindow::openProj(QString directory)
{
      //openProject
    //check if dir is writable
    QFileInfo info (directory);
    if (!info.isWritable())
    {
        QMessageBox::critical(this, tr ("Critical error"), tr("Directory not writable, failed to open project."));
        return;
    }
    closeProj();
    dir = QDir (directory);
    if (!checkProj())
    {
        QMessageBox::critical(this, tr("Critical error"), tr("Unable to open project. Some files are not found!"));
        return;
    }
    s = new QSettings (directory + "/control", QSettings::IniFormat, this);
    //Application group
    s->beginGroup("application");
    ui->txtExepath->setText(s->value("exepath").toString());
    ui->txtSetup->setText(s->value("setup").toString());
    ui->txtPrefix->setText(s->value("prefix").toString());
    //TODO: другие группы
    QFile f;
    QTextStream stream (&f);
    f.setFileName(directory + "/.name");
    f.open(QIODevice::Text | QIODevice::ReadOnly);
    ui->txtName->setText(stream.readAll());
    f.close();
    f.setFileName(directory + "/.note");
    f.open(QIODevice::Text | QIODevice::ReadOnly);
    ui->txtNote->setText(stream.readAll());
    f.close();
    //Фсе вроде.
    ui->tabWidget->setEnabled(true);
    statusBar()->showMessage(tr("Project %1 opened").arg(dir.path()));
    setWindowTitle(tr("%1 - wgedit").arg(ui->txtName->text()));
}

bool MainWindow::checkProj ()
{
    QStringList list = dir.entryList(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable | QDir::Hidden);
    QStringList filesNeed;
    filesNeed << "control";
    filesNeed << ".name";
    filesNeed << ".note";
    foreach (QString str, filesNeed)
    {
        if (!list.contains(str))
            return false;
    }
    return true;
}

void MainWindow::closeProj()
{
    //Перед закрытием проекта, его, конечно же, надо и сохранить saveProj
    ui->tabWidget->setEnabled(false);
    ui->txtName->setText("");
    ui->txtNote->setText("");
    ui->txtExepath->setText("");
    ui->txtPrefix->setText("");
    ui->txtSetup->setText("");

    dir.setPath("");
setWindowTitle("wgedit");
}

void MainWindow::on_actOpen_triggered()
{
    openProj(QFileDialog::getExistingDirectory(this, tr("Open a project directory")));
}

void MainWindow::on_actSave_triggered()
{
    if (dir.path().isEmpty())
        saveProjAs();
    else
        saveProj();
}

void MainWindow::saveProj()
{
    if (ui->txtName->text().trimmed().isEmpty() || ui->txtNote->toPlainText().trimmed().isEmpty())
    {
        QMessageBox::warning(this, tr("Not saving"), tr("Not saving because name and/or note is not set"));
        return;
    }
    s = new QSettings (dir + "/control", QSettings::IniFormat, this);
    s->setValue("application/prefix", ui->txtPrefix->text());
    s->setValue("application/setup", ui->txtSetup->text());
    s->setValue("application/container", ui->txtContainer->text());
    s->setValue("wine/distr", ui->txtDistr->text());
    s->setValue("wine/memory", ui->cbMemory->isChecked());
    // components will be here
    s->sync();

//now, .name
QFile f;
QTextStream stream (&f);
f.setFileName(dir + "/.name");
f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
stream << ui->txtName->text();
f.close();

f.setFileName(dir + "/.note");
f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
stream << ui->txtNote->text();
f.close();

//CD/DVD info

}

void browse(QLineEdit *edit)
{
    selectfile:
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select script"), QDir::homePath());
    if (fileName.isEmpty()) {return;}
    QFileInfo info (fileName);
    if (!info.isExecutable())
    {
        QMessageBox::warning(this, tr("Warning"), tr("This file is not executable. Select other file"));
        goto selectfile;
    }
edit->setText(fileName);
}

void MainWindow::on_cmdPreInstall_clicked()
{
    browse(ui->txtPreinst);
}

void MainWindow::on_cmdPostInstall_clicked()
{
    browse (ui->txtPostinst);
}

void MainWindow::saveDVD()
{

}
