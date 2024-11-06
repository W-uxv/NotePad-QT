#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
#include "searchdialog.h"
#include "replacedialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    textChange = false;

    statusLabel.setMaximumWidth(150);
    statusLabel.setText("length:  " + QString::number(0) + "     lines:  " + QString::number(1));
    ui->statusbar->addPermanentWidget(&statusLabel);

    statusCursorLabel.setMaximumWidth(150);
    statusCursorLabel.setText("Ln:  " + QString::number(0) + "     Col:  " + QString::number(1));
    ui->statusbar->addPermanentWidget(&statusCursorLabel);

    QLabel *author = new QLabel(ui->statusbar);
    author->setText(tr("陈维新"));
    ui->statusbar->addPermanentWidget(author);

    ui->actionCopy->setEnabled(false);
    ui->actionCut->setEnabled(false);
    ui->actionUndo->setEnabled(false);
    ui->actionRedo->setEnabled(false);
    ui->actionPaste->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog dlg;
    dlg.exec();

}


void MainWindow::on_actionFind_triggered()
{
    SearchDialog dlg;
    dlg.exec();
}


void MainWindow::on_actionReplace_triggered()
{
    ReplaceDialog dlg;
    dlg.exec();
}


void MainWindow::on_actionNew_triggered()
{
    if(!userEditConfirmed()) return;

    filePath = "";

    ui->textEdit->clear();
    this->setWindowTitle(tr("新建文本文件 - 编辑器"));

    textChange = false;
}


void MainWindow::on_actionOpen_triggered()
{
    if(!userEditConfirmed()) return;

    QString filename = QFileDialog::getOpenFileName(this,"打开文件", ".",tr("Text files (*.txt) ;; All (*.*)"));
    QFile file(filename);

    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox ::warning(this, "..","打开文件失败");
        return;
    }

    filePath = filename;

    QTextStream in(&file);
    QString text = in.readAll();
    ui->textEdit->insertPlainText(text);
    file.close();

    this->setWindowTitle(QFileInfo(filename).absoluteFilePath());

    textChange = false;
}


void MainWindow::on_actionSave_triggered()
{
    // 如果 filePath 为空，则提示用户选择文件路径
    if (filePath == "") {
        QString filename = QFileDialog::getSaveFileName(this, "保存文件", ".", tr("Text files (*.txt)"));
        if (filename == "") {
            return; // 用户取消操作
        }
        filePath = filename;
    }

    QFile file(filePath);

    // 尝试打开文件进行写操作
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "错误", "无法打开文件进行写入");
        return;
    }

    // 将文本编辑器中的内容写入文件
    QTextStream out(&file);
    QString text = ui->textEdit->toPlainText();
    out << text;
    file.flush();
    file.close();

    // 更新窗口标题以显示当前文件路径
    this->setWindowTitle(QFileInfo(filePath).absoluteFilePath());

    // 标记文本未更改
    textChange = false;
}



void MainWindow::on_actionSaveAs_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this,"保存文件", ".",tr("Text files (*.txt) "));

    QFile file(filename);
    if(!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox ::warning(this, "..","打开文件失败");
        return;
    }

    filePath = filename;
    QTextStream out(&file);
    QString text = ui->textEdit->toPlainText();
    out<<text;
    file.flush();
    file.close();

    this->setWindowTitle(QFileInfo(filePath).absoluteFilePath());
}


void MainWindow::on_textEdit_textChanged()
{
    if(!textChange) {
        this->setWindowTitle("*" + this->windowTitle());
        textChange = true;
    }
}

bool MainWindow::userEditConfirmed()
{
    if(textChange) {
        QString path = filePath != ""? filePath: "无标题.txt";

        QMessageBox msg(this);
        msg.setIcon(QMessageBox::Question);
        msg.setWindowTitle("...");
        msg.setWindowFlag(Qt::Drawer);
        msg.setText(QString("是否将更改保存到\n") + "\"" + path + "\" ?");
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        int r = msg.exec();
        switch(r) {
        case QMessageBox::Yes:
            on_actionSave_triggered();
            break;
        case QMessageBox::No:
            textChange = false;
            break;
        case QMessageBox::Cancel:
            return false;
        }
    }

    return true;
}



void MainWindow::on_actionUndo_triggered()
{
    ui->textEdit->undo();
}


void MainWindow::on_actionCut_triggered()
{
    ui->textEdit->cut();
}


void MainWindow::on_actionCopy_triggered()
{
    ui->textEdit->copy();
    ui->actionPaste->setEnabled(true);
}


void MainWindow::on_actionPaste_triggered()
{
    ui->textEdit->paste();
}


void MainWindow::on_actionRedo_triggered()
{
    ui->textEdit->redo();
}


void MainWindow::on_textEdit_copyAvailable(bool b)
{
    ui->actionCopy->setEnabled(b);
    ui->actionCut->setEnabled(b);
}


void MainWindow::on_textEdit_redoAvailable(bool b)
{
    ui->actionRedo->setEnabled(b);
}


void MainWindow::on_textEdit_undoAvailable(bool b)
{
    ui->actionUndo->setEnabled(b);
}
