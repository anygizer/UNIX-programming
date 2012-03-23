#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>




MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(selectSrcDir()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(selectTgtDir()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(copyDirStruct()));
    errMsg = new QErrorMessage(this);
    fireMessage(tr("Program started"), false, 500);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete errMsg;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::fireMessage(const QString &message, bool popup, int timeout)
{
    statusBar()->showMessage(message, timeout);
    if(popup)
    {
        errMsg->showMessage(message);
        errMsg->exec();
    }
}

void MainWindow::selectSrcDir()
{
    QString fileName = QFileDialog::getExistingDirectory(this,  tr("Select Source Directory"), "",
                                                         QFileDialog::ShowDirsOnly | QFileDialog::ReadOnly);
    ui->lineEdit->setText(fileName);
}

void MainWindow::selectTgtDir()
{
    QString fileName = QFileDialog::getExistingDirectory(this,  tr("Select Target Directory"), "",
                                                         QFileDialog::ShowDirsOnly | QFileDialog::ReadOnly);
    ui->lineEdit_2->setText(fileName);
}

void MainWindow::copyDirStruct()
{
    if(ui->lineEdit->text().isEmpty())
    {
        fireMessage(tr("Specify the source directory please."));
        return;
    }
    QFileInfo fi = QFileInfo(ui->lineEdit->text());
    if(!fi.exists())
    {
        fireMessage(tr("Specified source directory does not exist."));
        return;
    }
    if(!fi.isDir())
    {
        fireMessage(tr("Specified source directory is not a directory."));
        return;
    }

    if(ui->lineEdit_2->text().isEmpty())
    {
        fireMessage(tr("Specify the target directory please."));
        return;
    }
    fi = QFileInfo(ui->lineEdit_2->text());
    if(!fi.exists())
    {
        QDir d = QDir(fi.path());
        d.mkdir(fi.baseName());
        fi = QFileInfo(d.absolutePath());
    }
    if(!fi.isDir())
    {
        fireMessage(tr("Specified target directory is not a directory."));
        return;
    }
    if(!fi.isWritable())
    {
        fireMessage(tr("Target directory is not writable."));
        return;
    }

    //Clear target directory if user asked to
    if(ui->checkBox->isChecked())
    {
        cleardir(ui->lineEdit_2->text());
    }

    cpdr(ui->lineEdit->text(), ui->lineEdit_2->text());
}

/**
 *  Qt way of recursive directory tree copy.
 */
void MainWindow::cpdr(const QString &src, const QString &dst)
{
    QDir srcDir(src);
    if(!srcDir.exists())
    {
        qWarning("The source directory does not exist: %s", src.toLocal8Bit().constData());
        return;
    }
    QDir dstDir(dst);
    if(!dstDir.exists())
    {
        dstDir.mkpath(dst);
    }
    foreach(QString s, srcDir.entryList(QDir::NoDotAndDotDot | QDir::Dirs))
    {
        s = QDir::separator() + s;
        cpdr(srcDir.absolutePath() + s, dstDir.absolutePath() + s);
    }
    fireMessage(tr("Directory structure successfully copied"), false);
}

void MainWindow::rm(const QString &path)
{
    QFileInfo pathFI(path);
    if(pathFI.isDir())
    {
        QDir d(path);
        foreach(QFileInfo fi, d.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries))
        {
            if(fi.isDir())
            {
                rm(fi.absolutePath()+QDir::separator()+fi.baseName());
            }
            else
            {
                d.remove(fi.baseName());
            }
        }
        pathFI.dir().rmdir(pathFI.baseName());
    }
    else
    {
        pathFI.dir().remove(pathFI.baseName());
    }
}


void MainWindow::cleardir(const QString &dirpath)
{
    QFileInfo fi(dirpath);
    if(fi.isDir())
    {
        foreach(QString s, QDir(dirpath).entryList(QDir::NoDotAndDotDot | QDir::AllEntries))
        {
            rm(fi.absolutePath() + QDir::separator() + fi.baseName() + QDir::separator() + s);
        }
        fireMessage(tr("Directory ") + dirpath + tr(" successfully cleared"), false, 2000);
    }
}
