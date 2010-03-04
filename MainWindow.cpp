#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "NewPasswordDialog.h"
#include "AboutDialog.h"
#include "ChangePasswordDialog.h"
#include "ExeParser.h"
#include "PasswordInputDialog.h"

#include <QApplication>
#include <QMessageBox>
#include <QFileInfo>
#include <QFileDialog>
#include <QDateTime>

MainWindow::MainWindow(QString targetExe, QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_tainted(false),
    m_targetExe(QString())
{
    m_ui->setupUi(this);

    m_ui->actionExit->setShortcut(QKeySequence(Qt::AltModifier | Qt::Key_F4));
    setCentralWidget(m_ui->txtDocument);

    if (targetExe.isNull())
        guiNew();
    else
        guiOpen(targetExe);
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

bool MainWindow::guiOpen(QString targetExe)
{
    QByteArray doc = ExeParser::read(targetExe);
    if (doc.size() == 0) {
        // new document
        m_ui->txtDocument->setPlainText("");
    } else {
        // prompt for password and decrypt
        while(true) {
            PasswordInputDialog dialog(this);
            if (dialog.exec() == QDialog::Rejected)
                return false; // user cancel
            QString password = dialog.password();
            // TODO: something like
            // DecryptObject obj = decrypted(doc, password);
            // if (obj.success()) {
            //    m_password = password;
            //    m_ui.txtDocument.setText(obj.text());
            //    break;
            // }
        }
    }

    m_targetExe = targetExe;
    m_tainted = false;
    updateGui();
    return true;
}

void MainWindow::updateGui()
{
    m_ui->txtDocument->setLineWrapMode(m_ui->actionWordWrap->isChecked() ?
                                       QPlainTextEdit::WidgetWidth :
                                       QPlainTextEdit::NoWrap);
    m_ui->actionChangePassword->setEnabled(! m_password.isNull());
    updateCaption();
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::on_txtDocument_undoAvailable(bool b)
{
    m_ui->actionUndo->setEnabled(b);
}

void MainWindow::on_txtDocument_redoAvailable(bool b)
{
    m_ui->actionRedo->setEnabled(b);
}

void MainWindow::on_actionUndo_triggered()
{
    m_ui->txtDocument->undo();
}

void MainWindow::on_actionRedo_triggered()
{
    m_ui->txtDocument->redo();
}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    if (ensureSaved())
        e->accept();
    else
        e->ignore();
}

bool MainWindow::ensureSaved()
{
    if (m_tainted) {
        QMessageBox::StandardButton response = QMessageBox::question(this,
            QApplication::applicationName(),
            tr("You have unsaved changes. Would you like to save?"),
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
            QMessageBox::Yes);
        if (response == QMessageBox::Yes)
            guiSave();
        return response != QMessageBox::Cancel;
    } else {
        return true;
    }
}

bool MainWindow::guiSave()
{
    if (! ensurePassword())
        return false; // user cancelled

    if (! m_tainted)
        return true; // no need to save

    if (m_targetExe.isNull())
        return guiSaveAs(); // need to pick a file

    save();
    return true;
}

void MainWindow::save()
{
    // TODO: something like
    // ExeParser::write(m_targetExe, encrypted(m_ui.txtDocument.text(), m_password));

    m_tainted = false;
    updateGui();
}

bool MainWindow::ensurePassword()
{
    if (m_password.isNull()) {
        // prompt for a new password
        NewPasswordDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted) {
            m_password = dialog.password();
            m_tainted = true;
        } else {
            return false; // cancelled save
        }
    }

    return true;
}

bool MainWindow::guiSaveAs()
{
    // prompt for a location to save the new file
    QString file = QFileDialog::getSaveFileName(this, tr("Save As"),
        QFileInfo(m_targetExe).absoluteDir().path(), tr("Application (*.exe)"));
    if (file.isNull())
        return false; // pressed cancel
    m_targetExe = file;

    if (! ensurePassword())
        return false; // user cancelled

    save();
    return true;
}

void MainWindow::on_txtDocument_textChanged()
{
    m_tainted = true;
    updateCaption();
}

void MainWindow::updateCaption()
{
    QString tainted = m_tainted ? "*" : "";

    QString title;
    if (m_targetExe.isNull())
        title = tr("Unsaved");
    else
        title = QFileInfo(m_targetExe).baseName();

    this->setWindowTitle(title + tainted + QString(" - ") + QApplication::applicationName());
}

void MainWindow::on_actionSave_triggered()
{
    guiSave();
}

void MainWindow::on_actionSaveAs_triggered()
{
    guiSaveAs();
}

bool MainWindow::guiNew()
{
    if (! ensureSaved())
        return false;

    m_targetExe = QString();
    m_ui->txtDocument->setPlainText("");
    m_tainted = false;
    updateGui();
    return true;
}

void MainWindow::on_actionNew_triggered()
{
    guiNew();
}

void MainWindow::on_actionWordWrap_toggled(bool )
{
    updateGui();
}

void MainWindow::on_actionSelectAll_triggered()
{
    m_ui->txtDocument->selectAll();
}

void MainWindow::on_actionDelete_triggered()
{
    m_ui->txtDocument->textCursor().deleteChar();
}

void MainWindow::on_actionCut_triggered()
{
    m_ui->txtDocument->cut();
}

void MainWindow::on_actionCopy_triggered()
{
    m_ui->txtDocument->copy();
}

void MainWindow::on_actionPaste_triggered()
{
    m_ui->txtDocument->paste();
}

void MainWindow::on_actionInsertDateTime_triggered()
{
    m_ui->txtDocument->textCursor().insertText(QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss"));
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog(this).exec();
}

void MainWindow::on_actionChangePassword_triggered()
{
    ChangePasswordDialog dialog(m_password, this);
    if (dialog.exec() == QDialog::Accepted) {
        m_password = dialog.password();
        guiSave();
    }
}
