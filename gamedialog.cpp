#include "gamedialog.h"
#include "ui_gamedialog.h"

GameDialog::GameDialog(QWidget *parent, QString path) :
    QDialog(parent),
    ui(new Ui::GameDialog)
{
    ui->setupUi(this);
    //setting the UI
    ui->lblIcon->setPixmap(QPixmap(path + "/icon"));
    ui->lblIcon->setText("");
    ui->lblName->setText(tr("A Microsoft Windows(r) application is found on this disc. <br><br><b>%1</b><br><br> Would you like to install it? ").arg(engine::getName(path)));
    ui->lblDesc->setText(engine::getNote(path));

}

GameDialog::~GameDialog()
{
    delete ui;
}
