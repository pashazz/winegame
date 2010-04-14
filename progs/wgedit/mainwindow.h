#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>

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
QSettings  *s;
QDir myDir;
private slots:


private slots:
    void on_actNew_triggered();
    void on_action_About_triggered();
};

#endif // MAINWINDOW_H
