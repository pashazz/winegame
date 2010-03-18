#ifndef PREFIXDIALOG_H
#define PREFIXDIALOG_H

#include <QDialog>
#include "prefix.h"
namespace Ui {
    class PrefixDialog;
}

class PrefixDialog : public QDialog {
    Q_OBJECT
public:
    PrefixDialog(QWidget *parent, QString path);
    ~PrefixDialog();

private:
    Ui::PrefixDialog *ui;
    Prefix *pr;

private slots:
    void on_cmdControl_clicked();
    void on_cmdRemove_clicked();
    void on_cmdBoot_clicked();
    void on_cmdUninst_clicked();
    void on_cmdNotepad_clicked();
    void on_cmdTerm_clicked();
    void on_cmdWP_clicked();
    void on_cmdEXE_clicked();
    void on_cmdTask_clicked();
    void on_cmdFM_clicked();
    void on_cmdReg_clicked();
    void on_cmdWinecfg_clicked();
};

#endif // PREFIXDIALOG_H
