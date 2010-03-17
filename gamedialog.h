#ifndef GAMEDIALOG_H
#define GAMEDIALOG_H

#include <QDialog>
#include "engine.h"
namespace Ui {
    class GameDialog;
}

class GameDialog : public QDialog {
    Q_OBJECT
public:
    GameDialog(QWidget *parent, QString path);
    ~GameDialog();

private:
    Ui::GameDialog *ui;
};

#endif // GAMEDIALOG_H
