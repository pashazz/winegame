#include "prefixdialog.h"
#include "ui_prefixdialog.h"

PrefixDialog::PrefixDialog(QWidget *parent, QString path) :
    QDialog(parent),
    ui(new Ui::PrefixDialog),
    pr (new Prefix (this, path))
{
    ui->setupUi(this);
    ui->toolBox->setCurrentIndex(0);//это если вдруг я забыл переместить страницу в designer
    setWindowTitle(pr->getName());
    connect (ui->cmdC, SIGNAL(clicked()), pr, SLOT (lauch_c()));
}

PrefixDialog::~PrefixDialog()
{
    delete ui;
}

void PrefixDialog::on_cmdWinecfg_clicked()
{
    pr->runProgram("winecfg");
}


void PrefixDialog::on_cmdReg_clicked()
{
    pr->runProgram("regedit");
}


void PrefixDialog::on_cmdFM_clicked()
{
    pr->runProgram("winefile");
}





void PrefixDialog::on_cmdTask_clicked()
{
    pr->runProgram("taskmgr");
}

void PrefixDialog::on_cmdEXE_clicked()
{
    QFileDialog *dlg = new QFileDialog (this, tr("Select Windows program to run"), QDir::homePath(), tr("Windows executables (*.exe)"));
    dlg->setFileMode(QFileDialog::ExistingFile);
    if (dlg->exec() == QDialog::Accepted)
        pr->runProgram(dlg->selectedFiles().at(0));

}

void PrefixDialog::on_cmdWP_clicked()
{
    pr->runProgram("wordpad");
}

void PrefixDialog::on_cmdTerm_clicked()
{
    pr->runProgram("wineconsole cmd");
}

void PrefixDialog::on_cmdNotepad_clicked()
{
    pr->runProgram("notepad");
}

void PrefixDialog::on_cmdUninst_clicked()
{
    pr->runProgram("uninstaller");
}


void PrefixDialog::on_cmdBoot_clicked()
{
    pr->runProgram("wineboot");
}

void PrefixDialog::on_cmdRemove_clicked()
{
pr->removePrefix();
close();
}

void PrefixDialog::on_cmdControl_clicked()
{
     pr->runProgram("control");
}
