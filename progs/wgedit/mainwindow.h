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
QSettings *s;
QDir dir;

void openProj(QString directory);
void closeProj();
void saveProjAs(QString directory);
void saveProj();
void newProj();
bool checkProj();
void browse (QLineEdit *edit); //Browses for file, pastes its name into *edit

//функции-помощники
void saveOther;
void saveDVD;
private slots:
    void on_cmdPostInstall_clicked();
    void on_cmdPreInstall_clicked();
    void on_actSave_triggered();
    void on_actOpen_triggered();
    void on_action_About_triggered();
};

#endif // MAINWINDOW_H
