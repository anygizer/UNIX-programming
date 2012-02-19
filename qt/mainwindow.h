#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QErrorMessage>

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
    QErrorMessage *errMsg;
    void cpdr(const char *src, const char *dst);
    void cleardir(const char* dirpath);
    void rm(const char *path);
    void fireMessage(const QString & message);
};

#endif // MAINWINDOW_H
