#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#define WIN_TITLE "Final example - Simple text editor"
#define STATUS_TIMEOUT 3000

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QPlainTextEdit;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow();

private:
    Ui::MainWindow *ui;
    QPlainTextEdit *editor;
    QString fileName;
    bool wasModified;

    void setupWindowProperties(void);
    void setupWindowMenuAndWidgets(void);
    void setupEditor(void);
    void showCriticalMessage(QString, QString, QString);
    bool showSaveConfirmationDialog(void);

private slots:
    void onNewFile();
    void onSaveFile();
    void onSaveAsFile();
    void onOpenFile();
    void changeSaveState();

    // QWidget interface
protected:
    virtual void closeEvent(QCloseEvent *event) override;
};

#endif // MAINWINDOW_H
