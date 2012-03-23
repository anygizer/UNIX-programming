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
    void cpdr(const QString &src, const QString &dst);
    void cleardir(const QString &dirpath);
    void rm(const QString &path);
    void fireMessage(const QString & message, bool popup = true, int timeout = 0);
};

#endif // MAINWINDOW_H
