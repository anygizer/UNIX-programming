#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void selectSrcDir();
    void selectTgtDir();
    void copyDirStruct();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    void cpdr(const char *src, const char *dst);
    void cleardir(const char* dirpath);
    void rm(const char *path);
};

#endif // MAINWINDOW_H
