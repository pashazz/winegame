#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "linux.h"
#include "engine.h"
namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
QString diskpath;
bool cdMode;

    void buildList();
    void lauchEngine(QString pkgpath);
private slots:
    void on_lstGames_itemClicked(QListWidgetItem* item);
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
};

#endif // MAINWINDOW_H
