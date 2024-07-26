#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QToolBar>
#include <QPlainTextEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupWindowProperties();
    setupWindowMenuAndWidgets();
    setupEditor();
    fileName = "";
    wasModified = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupWindowProperties()
{
    setMinimumSize(640, 480);
    setWindowTitle(WIN_TITLE);
}

void MainWindow::setupWindowMenuAndWidgets()
{
    //-------------------------------- actions
    QAction *newFile = new QAction("New", menuBar());
    QAction *openFile = new QAction("Open...", menuBar());
    QAction *saveFile = new QAction("Save", menuBar());
    QAction *saveAsFile = new QAction("Save as...", menuBar());
    QAction *exitApp = new QAction("Exit", menuBar());
    QAction *about = new QAction("About", menuBar());

    //-------------------------------- menu
    ui->menuFile->addAction(newFile);
    ui->menuFile->addSeparator();
    ui->menuFile->addAction(openFile);
    ui->menuFile->addAction(saveFile);
    ui->menuFile->addAction(saveAsFile);
    ui->menuFile->addSeparator();
    ui->menuFile->addAction(exitApp);
    exitApp->setShortcut(tr("CTRL+Q"));
    ui->menuHelp->addAction(about);

    //-------------------------------- tool bar
    QToolBar *toolBar = new QToolBar();
    QAction *toolNewFile = toolBar -> addAction("New");

//    toolBar->addAction(newFile);
    toolBar->addSeparator();
    toolBar->addAction(openFile);
    toolBar->addAction(saveFile);
    addToolBar(Qt::TopToolBarArea, toolBar);

    //-------------------------------- signal & slots
    connect(exitApp, &QAction::triggered, this, &QMainWindow::close);
    connect(toolNewFile, &QAction::triggered, newFile, &QAction::trigger);
    connect(newFile, &QAction::triggered, this, &MainWindow::onNewFile);
    connect(saveFile, &QAction::triggered, this, &MainWindow::onSaveFile);
    connect(saveAsFile, &QAction::triggered, this, &MainWindow::onSaveAsFile);
    connect(openFile, &QAction::triggered, this, &MainWindow::onOpenFile);
}

void MainWindow::setupEditor()
{
    editor = new QPlainTextEdit(this);
    setCentralWidget(editor);
    connect(editor, &QPlainTextEdit::textChanged,
            this, &MainWindow::changeSaveState);
}

void MainWindow::showCriticalMessage(QString title,
                                     QString message, QString status)
{
    statusBar()->showMessage(status, STATUS_TIMEOUT);
    QMessageBox::critical(this, title, message);

}

bool MainWindow::showSaveConfirmationDialog()
{
    QMessageBox::StandardButton result =
            QMessageBox::question(this, "Text not saved",
                                  "Text was modified, discard changes?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::Yes) return true;
    return false;
}

void MainWindow::onNewFile()
{
    if (wasModified && !showSaveConfirmationDialog()) return;

    editor->clear();
    statusBar()->showMessage("New file created", STATUS_TIMEOUT);
    wasModified = false;
    setWindowTitle(WIN_TITLE);
    fileName = "";
}

void MainWindow::onSaveFile()
{
    if(!wasModified) {
        statusBar()->showMessage("Text was not modified, no need to save",
                                 STATUS_TIMEOUT);
        return;
    }

    if (fileName == "")
        fileName = QFileDialog::getSaveFileName(this, "Save File",
                  "", "Text file (*.txt)");
    if (fileName == "")
    {
        showCriticalMessage("Save error",
                            "Failed to choose name for file!",
                            "File was not saved");
        return;
    }

    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
    {
        showCriticalMessage("Save error",
                            "Failed to opend file!",
                            "File was not saved");
        return;
    }

    QTextStream outStream(&file);
    outStream << editor -> toPlainText();
    outStream.flush();
    file.close();

    statusBar()->showMessage("File saved!", STATUS_TIMEOUT);
    QFileInfo fileInfo(file);
    setWindowTitle(WIN_TITLE "-" + fileInfo.fileName());

    wasModified = false;
}

void MainWindow::onSaveAsFile()
{
    QString tmpName = QFileDialog::getSaveFileName(this, "Save File",
                                                   "", "Text file (*.txt)");
    if(tmpName == "")
    {
        showCriticalMessage("Save error",
                            "Failed to choose new name for file!",
                            "File was not saved");
        return;
    }

    QFile file(tmpName);
    if(!file.open(QIODevice::WriteOnly))
    {
        showCriticalMessage("Save error",
                            "Failed to opend file!",
                            "File was not saved");
        return;
    }

    QTextStream outStream(&file);
    outStream << editor -> toPlainText();
    outStream.flush();
    file.close();

    fileName = tmpName;
    statusBar()->showMessage("File saved", STATUS_TIMEOUT);
    QFileInfo fileInfo(file);
    setWindowTitle(WIN_TITLE " - " + fileInfo.fileName());

    wasModified = false;
}

void MainWindow::onOpenFile()
{
    if(wasModified && !showSaveConfirmationDialog()) return;

    QString tmpName = QFileDialog::getOpenFileName(this,
                      "Open File", "", "Any file (*.*)");
    if(tmpName == "")
    {
        showCriticalMessage("Open error", "Failed to choose file to open!",
                            "File was not opened");
        return;
    }

    QFile file(tmpName);
    if(!file.open(QIODevice::ReadOnly))
    {
        showCriticalMessage("Open error", "Failed to choose file to open!",
                            "File was not opened");
        return;
    }

    QTextStream inStream(&file);
    editor->setPlainText(inStream.readAll());
    file.close();

    fileName = tmpName;
    statusBar()->showMessage("File opened!", STATUS_TIMEOUT);
    QFileInfo fileInfo(file);
    setWindowTitle(WIN_TITLE " - " + fileInfo.fileName());
    wasModified = false;
}

void MainWindow::changeSaveState()
{
    //autosave
    if(!wasModified)
    {
        setWindowTitle("*" + windowTitle());
        wasModified = true;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(wasModified)
    {
        if(showSaveConfirmationDialog()) event->accept();
        else event->ignore();
    }

}

