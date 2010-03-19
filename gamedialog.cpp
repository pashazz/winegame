#include "gamedialog.h"
#include "ui_gamedialog.h"

GameDialog::GameDialog(QWidget *parent, QString path) :
    QDialog(parent),
    ui(new Ui::GameDialog)
{
    ui->setupUi(this);
    //setting the UI
    if (qApp->arguments().length() > 1)
        ui->lblIcon->setPixmap(getIcoFromDisc());
    else
        ui->lblIcon->setPixmap(QPixmap(path + "/icon"));
    ui->lblIcon->setText("");
    ui->lblName->setText(tr("A Microsoft Windows(r) application is found on this disc. <br><br><b>%1</b><br><br> Would you like to install it? ").arg(engine::getName(path)));
    ui->lblDesc->setText(engine::getNote(path));

}

GameDialog::~GameDialog()
{
    delete ui;
}
QPixmap GameDialog::getIcoFromDisc()
{
if (qApp->arguments().length() > 1)
    {
    QSettings stg (qApp->arguments().at(1) + "/autorun.inf", QSettings::IniFormat, this);
    stg.beginGroup("autorun");
    QString icon = qApp->arguments().at(1) + QDir::separator() + stg.value("Icon").toString();
    qDebug() << "engine: ico file detected" << icon;
    return QPixmap(icon);
}
return QPixmap();
}
