#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFileSystemModel>
#include <QErrorMessage>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(selectSrcDir()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(selectTgtDir()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(copyDirStruct()));
}

MainWindow::~MainWindow()
{
    delete ui;
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
    //TODO: move it to the lineedit validators and expand it
    if(!(new QFileInfo(ui->lineEdit_2->text()))->isWritable())
    {
        QErrorMessage mess;
        mess.showMessage(tr("Target directory is not writable."));
        if(mess.exec())
        {
            return;
        }
    }

    if(ui->checkBox->isChecked())
    {
        cleardir(ui->lineEdit_2->text().toLocal8Bit().constData());
    }

    //    QDir srcDir(ui->lineEdit->text());
    //    srcDir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
    cpdr(ui->lineEdit->text().toLocal8Bit().constData(), ui->lineEdit_2->text().toLocal8Bit().constData());
}

void MainWindow::cpdr(const char *src, const char *dst)
{
    /* Get the current directory to know where to return in the end. */
    char cwd[FILENAME_MAX];

    DIR *dp;
    struct dirent *ep;
    char *subDst;
    int dstLen = strlen(dst);
    char absoluteSrc[FILENAME_MAX];

    struct stat st_buf;

    getcwd(cwd, sizeof(cwd));
    qDebug("startwd: %s\n", cwd);

    dp = opendir(src);
    if(dp != NULL)
    {
        if(chdir(src) < 0)
        {
            qWarning("chdir(in): %s\n", src);
            return;
        }
        while((ep = readdir(dp)) != NULL)
        {
            if((strcmp(ep->d_name, ".") == 0) || (strcmp(ep->d_name, "..") == 0))
                continue;

            /* Get the status of the file system object. */
            if (stat(ep->d_name, &st_buf) != 0)
            {
                qWarning("Error: %s, %s cwd: %s\n",
                         strerror(errno), ep->d_name, getcwd(cwd, sizeof(cwd)));
                return;
            }

            if(S_ISDIR(st_buf.st_mode))
            {
                getcwd(absoluteSrc, sizeof(absoluteSrc));

                qDebug("dirs: %s/%s\n", absoluteSrc, ep->d_name);

                if(chdir(dst) < 0)
                {
                    qWarning("chdir(indst): %s\n", dst);
                    return;
                }

                /* Checking for existance */
                if((mkdir(ep->d_name, st_buf.st_mode) != 0) && (errno != EEXIST))
                {
                    qWarning("Error: %s, %s cwd: %s\n",
                             strerror(errno), ep->d_name, getcwd(cwd, sizeof(cwd)));
                    return;
                }
                if(chdir(absoluteSrc) < 0)
                {
                    qWarning("chdir(outdst): %s\n", absoluteSrc);
                    return;
                }

                if(dst[dstLen-1] == '/')
                {
                    subDst = (char *) malloc(sizeof(char)*(dstLen + strlen(ep->d_name)));
                    subDst = strcat(strcpy(subDst, dst), ep->d_name);
                }
                else
                {
                    subDst = (char *) malloc(sizeof(char)*(dstLen + 1 + strlen(ep->d_name)));
                    subDst = strcat(strcat(strcpy(subDst, dst), "/"), ep->d_name);
                }

                cpdr(ep->d_name, subDst);
                free(subDst);
            }
        }
        (void) closedir(dp);
        if(chdir(cwd) < 0)
        {
            qWarning("chdir(out): %s\n", cwd);
        }
    }
    else
    {
        qWarning("Couldn't open the directory: %s\n", src);
    }
}

void MainWindow::rm(const char *path)
{
    if(QFileInfo(tr(path)).isDir())
    {
        char cwd[FILENAME_MAX];
        DIR *dp = opendir(path);
        struct dirent *ep;

        getcwd(cwd, sizeof(cwd));
        if(dp != NULL)
        {
            if(chdir(path) < 0)
            {
                fprintf(stderr, "chdir(in): %s\n", path);
                exit(1);
            }
            while((ep = readdir(dp)) != NULL)
            {
                if((strcmp(ep->d_name, ".") != 0) && (strcmp(ep->d_name, "..") != 0))
                {
                    rm(ep->d_name);
                }
            }
            if(chdir(cwd) < 0)
            {
                qWarning("chdir(out): %s\n", cwd);
                return;
            }
            (void) closedir(dp);
            rmdir(path);
        }
    }
    else
    {
        unlink(path);
    }
}


void MainWindow::cleardir(const char* dirpath)
{
    struct stat st_buf;
    char buf[FILENAME_MAX];

    /* Get the status of the file system object. */
    if (stat(dirpath, &st_buf) != 0)
    {
        qWarning("Error: %s, %s cwd: %s\n",
                 strerror(errno), dirpath, getcwd(buf, sizeof(buf)));
        return;
    }

    if(S_ISDIR(st_buf.st_mode))
    {
        rm(dirpath);
        if(mkdir(dirpath, st_buf.st_mode) != 0)
        {
            qWarning("Error: %s, %s cwd: %s\n",
                     strerror(errno), dirpath, getcwd(buf, sizeof(buf)));
        }
    }
}
