#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>

#include "loggerthread.h"
#include "logstorage.h"
#include "settings.h"
#include "loggerthread.h"
#include "texthighlighter.h"
#include "filestream.h"
#include "loggerthread.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_newLogItem(const QString &type, const QString &tag, const QString &text);
    void on_newLogItems(const QList<QString> &types, const QList<QString> &tags, const QList<QString> &texts);
    void on_LoggerThread_finished(LoggerThread *thread);
    void on_pushButtonClear_clicked();

    void on_tagSelectColor();
    void on_tagClearColor();
    void on_typeSelectColor();
    void on_typeClearColor();
    void on_filterAdd();
    void on_filterEdit();
    void on_filterDelete();
    void on_filterSelectColor();
    void on_filterClearColor();
    void onCurrentLog_tagHide();
    void onCurrentLog_tagSelectColor();
    void onCurrentLog_tagClearColor();

    void on_tableWidgetTags_itemChanged(QTableWidgetItem *item);
    void on_tableWidgetTags_customContextMenuRequested(const QPoint &pos);

    void on_tableWidgetTypes_customContextMenuRequested(const QPoint &pos);
    void on_tableWidgetTypes_itemChanged(QTableWidgetItem *item);

    void on_tableWidgetFilters_customContextMenuRequested(const QPoint &pos);

    // menu
    void onActionLoad_highlightingTriggered();
    void onActionSave_highlightingTriggered();
    void onActionGeneral();
    void onActionExit();
    void onActionLoadFile();
    void onActionAbout();

    void onActionNewComPort();
    void onActionComPort(QAction *action);

    void on_pushButtonClearTags_clicked();
    void on_pushButtonClearLog_clicked();

    void on_tableWidgetFilters_itemChanged(QTableWidgetItem *item);

    void on_plainTextEdit_customContextMenuRequested(const QPoint &pos);
    void on_lineEditSearch_textChanged(const QString &arg1);

    void on_pushButtonPauseLogging_clicked();

private:
    Ui::MainWindow *ui;
    int searchTextColorRuleId;
    int searchBackgoundColorRuleId;

    int currentLogNumber; // used for context menu

    LogStorage *logStorage;
    Settings *settings;
    QTableWidgetItem *selectedTableItem;
    TextHighlighter *textHighlighter;
    bool _isLogPaused;

    void addEditComPort(const QString &comPort);
    void generateComPortMenu();
    void connectLoggerThread(LoggerThread *thread);

    void addLogItem(const QString &type, const QString &tag, const QString &text);

    void setTagColor(const QString &tag);
    void clearTagColor(const QString &tag);

    void startComPortThread(const QString &name);
    void stopThread(const QString &name);
    LoggerThread *getRunningThread(const QString &name);
    void startFileThread(const FileSettings &settings);
    void setDarkStyle();
};

#endif // MAINWINDOW_H
