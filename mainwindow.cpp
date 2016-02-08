#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QList>
#include <stdio.h>
#include <QScrollBar>
#include <QMenu>
#include <QColorDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QStyleFactory>

#include "adblogcatthread.h"
#include "customdialog.h"
#include "debug.h"

#include "filethread.h"
#include "tzlogthread.h"
#include "adblogcatthread.h"
#include "kernellogthread.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    for (int i = 0; i < Settings::LogThreadSize; i++) {
        loggerThreads[i] = NULL;
    }

    searchTextColorRuleId = searchBackgoundColorRuleId = 0;
    ui->setupUi(this);

    setDarkStyle();

    logStorage = LogStorage::getInstance();
    logStorage->setUI(ui->plainTextEdit, ui->tableWidgetTags, ui->tableWidgetTypes, ui->tableWidgetFilters);

    // setting document for text highlighter should be before settings init call
    TextHighlighter::setDocument(ui->plainTextEdit->document());
    textHighlighter = TextHighlighter::instance();

    settings = Settings::getInstance();

    QList<int> sizes;
    sizes << 250 << 500;
    ui->splitter->setSizes(sizes);


    connect(ui->actionGeneral, SIGNAL(triggered()), this, SLOT(onActionGeneral()));
    connect(ui->actionLoad_highlighting, SIGNAL(triggered()), this, SLOT(onActionLoad_highlightingTriggered()));
    connect(ui->actionSave_highlighting, SIGNAL(triggered()), this, SLOT(onActionSave_highlightingTriggered()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(onActionExit()));
    connect(ui->actionLoad_file, SIGNAL(triggered()), this, SLOT(onActionLoadFile()));
    connect(ui->actionAbout_QLogger, SIGNAL(triggered()), this, SLOT(onActionAbout()));

    connect(ui->actionAdb_logcat, SIGNAL(toggled(bool)), this, SLOT(onActionAdbLogcatToggled(bool)));
    connect(ui->actionTrustzone, SIGNAL(toggled(bool)), this, SLOT(onActionTrustzoneToggled(bool)));
    connect(ui->actionKernel_log, SIGNAL(toggled(bool)), this, SLOT(onActionKernelLogToggled(bool)));

    ui->actionAdb_logcat->setChecked(Settings::getInstance()->logThreadStatus[Settings::LogThreadAdbLogcat]);
    ui->actionTrustzone->setChecked(Settings::getInstance()->logThreadStatus[Settings::LogThreadTrustzone]);
    ui->actionKernel_log->setChecked(Settings::getInstance()->logThreadStatus[Settings::LogThreadKernel]);

    startLoggerThreads();

    threadsStatus = LoggerThread::RUNNING;
    ui->pushButtonStartLogging->setText("Pause");
}

MainWindow::~MainWindow()
{
    stopLoggerThreads();
    delete ui;
}

void MainWindow::connectLoggerThread(LoggerThread *thread) {
    connect(thread, SIGNAL(newLogItem(const QString &, const QString &, const QString &)),
            this, SLOT(on_newLogItem(const QString &, const QString &, const QString &)), Qt::BlockingQueuedConnection);
    connect(thread, SIGNAL(newLogItems(QList<QString>,QList<QString>,QList<QString>)),
            this, SLOT(on_newLogItems(QList<QString>,QList<QString>,QList<QString>)), Qt::BlockingQueuedConnection);
}

void MainWindow::createLoggerThread(Settings::LogThread thread, bool start) {
    LoggerThread *loggerThread = NULL;
    deleteLoggerThread(thread);

    switch (thread) {
        case Settings::LogThreadAdbLogcat:
            loggerThread = new AdbLogcatThread();
            break;

        case Settings::LogThreadTrustzone:
            loggerThread = new TZLogThread();
            break;

        case Settings::LogThreadKernel:
            loggerThread = new KernelLogThread();
            break;

        default:
            break;
    }

    if (NULL != loggerThread) {
        loggerThreads[thread] = loggerThread;
        connectLoggerThread(loggerThread);

        if (start) {
            loggerThread->start();
        }
    }
}

void MainWindow::deleteLoggerThread(Settings::LogThread thread) {
    if (NULL != loggerThreads[thread]) {
        loggerThreads[thread]->stop();
        loggerThreads[thread] = NULL;
    }
}

void MainWindow::restartLoggerThread(Settings::LogThread thread) {
    if (Settings::getInstance()->logThreadStatus[thread]) {
        deleteLoggerThread(thread);
        createLoggerThread(thread, threadsStatus == LoggerThread::RUNNING);
    }
}

void MainWindow::startLoggerThreads() {
    for (int i = 0; i < Settings::LogThreadSize; i++) {
        if (NULL != loggerThreads[i]) {
            loggerThreads[i]->start();
        }
    }
}

void MainWindow::stopLoggerThreads() {
    for (int i = 0; i < Settings::LogThreadSize; i++) {
        if (NULL != loggerThreads[i]) {
            loggerThreads[i]->stop();
        }
    }
}

void MainWindow::resumeLoggerThreads() {
    for (int i = 0; i < Settings::LogThreadSize; i++) {
        if (NULL != loggerThreads[i]) {
            loggerThreads[i]->resume();
        }
    }
}

void MainWindow::pauseLoggerThreads() {
    for (int i = 0; i < Settings::LogThreadSize; i++) {
        if (NULL != loggerThreads[i]) {
            loggerThreads[i]->pause();
        }
    }
}

void MainWindow::addLogItem(const QString &type, const QString &tag, const QString &text) {
    LogItem li;
    li.tag = tag;
    li.type = type;
    li.text = text;
    logStorage->add(li);
}

void MainWindow::on_newLogItem(const QString &type, const QString &tag, const QString &text)
{
    QScrollBar *vb = ui->plainTextEdit->verticalScrollBar();
    QScrollBar *hb = ui->plainTextEdit->verticalScrollBar();
    bool scrollDown = vb->maximum() == vb->value();

    addLogItem(type, tag, text);

    if (scrollDown) {
        vb->scroll(hb->value(), vb->maximum());
    }
}

void MainWindow::on_newLogItems(const QList<QString> &types, const QList<QString> &tags, const QList<QString> &texts) {
    QScrollBar *vb = ui->plainTextEdit->verticalScrollBar();
    QScrollBar *hb = ui->plainTextEdit->verticalScrollBar();
    bool scrollDown = vb->maximum() == vb->value();

    for (int i = 0; i < types.size(); i++) {
        const QString &type = types[i];
        const QString &tag = tags[i];
        const QString &text = texts[i];
        addLogItem(type, tag, text);
    }

    if (scrollDown) {
        vb->scroll(hb->value(), vb->maximum());
    }
}

void MainWindow::on_pushButtonStartLogging_clicked()
{
    if (ui->pushButtonStartLogging->text() == "Pause") {
        threadsStatus = LoggerThread::PAUSED;
        pauseLoggerThreads();
        ui->pushButtonStartLogging->setText("Resume");
    } else if (ui->pushButtonStartLogging->text() == "Resume") {
        threadsStatus = LoggerThread::RUNNING;
        resumeLoggerThreads();
        ui->pushButtonStartLogging->setText("Pause");
    }
}

void MainWindow::on_pushButtonClear_clicked()
{
    logStorage->clear();
}

void MainWindow::on_tableWidgetTags_itemChanged(QTableWidgetItem *item)
{
    QString tag = item->text();
    logStorage->setTagVisibility(tag, item->checkState() == Qt::Checked);
}

void MainWindow::on_tableWidgetTags_customContextMenuRequested(const QPoint &pos)
{
    selectedTableItem = ui->tableWidgetTags->itemAt(pos);
    if (NULL != selectedTableItem) {
        QMenu *menu = new QMenu();

        menu->addAction(tr("Select color"), this, SLOT(on_tagSelectColor()));
        menu->addAction(tr("Clear color"), this, SLOT(on_tagClearColor()));
        menu->exec(ui->tableWidgetTags->mapToGlobal(pos));
    }
}

void MainWindow::on_tagSelectColor() {
    if (selectedTableItem == NULL) {
        return;
    }

    QString tag = selectedTableItem->text();
    setTagColor(tag);
}

void MainWindow::on_tagClearColor() {
    if (selectedTableItem == NULL) {
        return;
    }

    QString tag = selectedTableItem->text();
    clearTagColor(tag);
}

void MainWindow::on_tableWidgetTypes_customContextMenuRequested(const QPoint &pos)
{
    selectedTableItem = ui->tableWidgetTypes->itemAt(pos);
    if (NULL != selectedTableItem) {
        QMenu *menu = new QMenu();
        menu->addAction(tr("Select color"), this, SLOT(on_typeSelectColor()));
        menu->addAction(tr("Clear color"), this, SLOT(on_typeClearColor()));
        menu->exec(ui->tableWidgetTypes->mapToGlobal(pos));
    }
}

void MainWindow::on_typeSelectColor() {
    if (selectedTableItem == NULL) {
        return;
    }

    QColor color = QColorDialog::getColor(Qt::black, this, "Text Color",  QColorDialog::DontUseNativeDialog);
    if(color.isValid()) {
        selectedTableItem->setBackgroundColor(color);

        TypeItem *ti = logStorage->getTypes()->get(selectedTableItem);
        int ruleId = NULL == ti ? 0 : ti->ruleId;

        TextHighlighterRule rule(ruleId);

        ti->ruleId = rule.id;
        rule.color = color;
        rule.colorType = TextHighlighterRule::COLOR_BACKGROUND;
        rule.pattern = selectedTableItem->text();
        rule.patternType = TextHighlighterRule::TYPE;
        rule.highlightType = TextHighlighterRule::HIGHLIGHT_LINE;

        textHighlighter->addRule(rule);
        logStorage->refresh();
    }
}

void MainWindow::on_typeClearColor()
{
    if (selectedTableItem == NULL) {
        return;
    }

    selectedTableItem->setBackground(QBrush(Qt::NoBrush));
    TypeItem *ti = logStorage->getTypes()->get(selectedTableItem);
    textHighlighter->removeRule(ti->ruleId);
    logStorage->refresh();
}

void MainWindow::on_tableWidgetTypes_itemChanged(QTableWidgetItem *item)
{
    QString type = item->text();
    logStorage->setTypeVisibility(type, item->checkState() == Qt::Checked);
}

void MainWindow::onActionLoad_highlightingTriggered()
{
    /*
    QString fileName = QFileDialog::getOpenFileName(this, "Open Highlight scheme", ".", "Highlight scheme (*.ini)");

    if (!fileName.endsWith(".ini")) {
        fileName.append(".ini");
    }
    loadHighlightingSettings(fileName);
    */
    Settings::getInstance()->loadSettings();
}

void MainWindow::onActionSave_highlightingTriggered()
{
    /*
    QString fileName = QFileDialog::getSaveFileName(this, "Save Highlight scheme", ".", "Highlight scheme (*.ini)");

    if (!fileName.endsWith(".ini")) {
        fileName.append(".ini");
    }
    saveHighlightingSettings(fileName);
    */
    Settings::getInstance()->saveSettings();
}

void MainWindow::onActionGeneral() {
    CustomDialog d("General settings");

    string maxLogCount = QString::number(Settings::getInstance()->maxLinesOfLogs).toStdString();
    Settings *settings = Settings::getInstance();

    d.addLabel("Logs", true, "");
    d.addLineEdit("Max count: ", &maxLogCount, maxLogCount.c_str());

    d.addLabel("adb logcat flags", true);
    d.addCheckBox("-v time", &settings->flagVTime);
    d.addCheckBox("-b main", &settings->flagBMain);
    d.addCheckBox("-b system", &settings->flagBSystem);
    d.addCheckBox("-b radio", &settings->flagBRadio);
    d.addCheckBox("-b events", &settings->flagBEvents);
    d.addCheckBox("-b crash", &settings->flagBCrash);

    d.exec();

    if(!d.wasCancelled()) {
        bool ok = false;
        int c = QString(maxLogCount.c_str()).toInt(&ok);
        if (ok) {
            Settings::getInstance()->maxLinesOfLogs = c;
            Settings::getInstance()->saveSettings();
        }

        restartLoggerThread(Settings::LogThreadAdbLogcat);
    }
}

void MainWindow::onActionExit() {
    QApplication::quit();
}

void MainWindow::onActionLoadFile() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("All file (*)"));
    ERR_OUT << fileName;

    if (QFile::exists(fileName)) {
        FileThread *fileThread = new FileThread(fileName);
        connect(fileThread, SIGNAL(newLogItems(QList<QString>,QList<QString>,QList<QString>)),
                this, SLOT(on_newLogItems(QList<QString>,QList<QString>,QList<QString>)), Qt::BlockingQueuedConnection);
        fileThread->start();
    }
}

void MainWindow::onActionAdbLogcatToggled(bool checked) {
    if (checked) {
        createLoggerThread(Settings::LogThreadAdbLogcat, checked);
    } else {
        deleteLoggerThread(Settings::LogThreadAdbLogcat);
    }

    Settings::getInstance()->logThreadStatus[Settings::LogThreadAdbLogcat] = checked;
    Settings::getInstance()->saveSettings();
}

void MainWindow::onActionTrustzoneToggled(bool checked) {
    if (checked) {
        createLoggerThread(Settings::LogThreadTrustzone);
        if (threadsStatus == LoggerThread::RUNNING) {
            startLoggerThreads();
        }
    } else {
        deleteLoggerThread(Settings::LogThreadTrustzone);
    }

    Settings::getInstance()->logThreadStatus[Settings::LogThreadTrustzone] = checked;
    Settings::getInstance()->saveSettings();
}

void MainWindow::onActionKernelLogToggled(bool checked) {
    if (checked) {
        createLoggerThread(Settings::LogThreadKernel);
        if (threadsStatus == LoggerThread::RUNNING) {
            startLoggerThreads();
        }
    } else {
        deleteLoggerThread(Settings::LogThreadKernel);
    }

    Settings::getInstance()->logThreadStatus[Settings::LogThreadKernel] = checked;
    Settings::getInstance()->saveSettings();
}

void MainWindow::onActionAbout() {
    QMessageBox::about(this, "QLogger", "Reading android logs.<br>" \
                       "Version 0.9 <br>" \
                       "Powered by Qt Creator 3.1.2 <br>" \
                       "Based on Qt 5.3.1 (GCC 4.6.1)");
}

void MainWindow::on_pushButtonClearTags_clicked()
{
    logStorage->clearTags();
}

void MainWindow::on_pushButtonClearLog_clicked()
{
    logStorage->clearLogs();
}

void MainWindow::on_tableWidgetFilters_customContextMenuRequested(const QPoint &pos)
{
    selectedTableItem = ui->tableWidgetFilters->itemAt(pos);
    if (NULL != selectedTableItem) {
        QMenu *menu = new QMenu();

        menu->addAction(tr("Add filter"), this, SLOT(on_filterAdd()));
        menu->addAction(tr("Edit filter"), this, SLOT(on_filterEdit()));
        menu->addAction(tr("Delete filter"), this, SLOT(on_filterDelete()));
        menu->addSeparator();
        menu->addAction(tr("Select color"), this, SLOT(on_filterSelectColor()));
        menu->addAction(tr("Clear color"), this, SLOT(on_filterClearColor()));
        menu->exec(ui->tableWidgetTags->mapToGlobal(pos));
    } else {
        QMenu *menu = new QMenu();

        menu->addAction(tr("Add filter"), this, SLOT(on_filterAdd()));
        menu->exec(ui->tableWidgetTags->mapToGlobal(pos));
    }
}

void MainWindow::on_filterAdd() {
    string name = "";
    string expression = "";
    int actionIndex = 0;
    CustomDialog d("Add filter");

    d.addLineEdit("name: ", &name, "filter name");
    d.addComboBox("Action: ", "show|hide", &actionIndex);
    d.addLineEdit("Expression: ", &expression, "filter expression");

    d.exec();

    if(!d.wasCancelled()) {
        FilterItem filter(name.c_str(), (FilterItem::FilterAction)actionIndex, expression.c_str());
        logStorage->add(filter);
    }
}

void MainWindow::on_filterEdit() {
    if (selectedTableItem == NULL) {
        return;
    }

    FilterItem *filter = logStorage->getFilters()->get(selectedTableItem);

    if (filter == NULL) {
        ERR_OUT << "Filter not found";
        return;
    }

    string name = filter->name.toStdString();
    string expression = filter->expression().toStdString();
    int actionIndex = filter->action;

    CustomDialog d("Edit filter");

    d.addLineEdit("name: ", &name, "filter name");
    d.addComboBox("Action: ", "show|hide", &actionIndex);
    d.addLineEdit("Expression: ", &expression, "filter expression");

    d.exec();

    if(!d.wasCancelled()) {
        filter->name = name.c_str();
        filter->setExpression(expression.c_str());
        filter->action = (FilterItem::FilterAction)actionIndex;
        filter->widget->setText(filter->name);
        int ruleIndex = TextHighlighter::instance()->indexOfRule(filter->ruleId);
        if (-1 != ruleIndex) {
            TextHighlighter::instance()->rules[ruleIndex].pattern = expression.c_str();
            logStorage->refresh();
        }
    }
}

void MainWindow::on_filterDelete() {
    if (selectedTableItem == NULL) {
        return;
    }

    FilterItem *filter = logStorage->getFilters()->get(selectedTableItem);

    string name = filter->name.toStdString();
    string expression = filter->expression().toStdString();
    int actionIndex = filter->action;

    CustomDialog d("Delete filter");

    d.addLineEdit("name: ", &name, "filter name");
    d.addComboBox("Action: ", "show|hide", &actionIndex);
    d.addLineEdit("Expression: ", &expression, "filter expression");

    d.exec();

    if(!d.wasCancelled()) {
        logStorage->remove(*filter);
    }
}

void MainWindow::on_filterSelectColor() {
    if (selectedTableItem == NULL) {
        return;
    }

    QColor color = QColorDialog::getColor(Qt::black, this, "Text Color",  QColorDialog::DontUseNativeDialog);
    if(color.isValid()) {
        selectedTableItem->setTextColor(color);

        FilterItem *fi = logStorage->getFilters()->get(selectedTableItem);
        int ruleId = NULL == fi ? 0 : fi->ruleId;

        TextHighlighterRule rule(ruleId);

        fi->ruleId = rule.id;
        rule.color = color;
        rule.colorType = TextHighlighterRule::COLOR_TEXT;
        rule.pattern = fi->expression();
        rule.patternType = TextHighlighterRule::LOG_REGEXP;
        rule.highlightType = TextHighlighterRule::HIGHLIGHT_TEXT;
        rule.isActive = fi->isVisible;

        textHighlighter->addRule(rule);
        logStorage->refresh();
    }
}

void MainWindow::on_filterClearColor() {
    if (selectedTableItem == NULL) {
        return;
    }

    selectedTableItem->setBackground(QBrush(Qt::NoBrush));
    FilterItem *fi = logStorage->getFilters()->get(selectedTableItem);
    textHighlighter->removeRule(fi->ruleId);
    fi->ruleId = 0;
    logStorage->refresh();
}

void MainWindow::onCurrentLog_tagHide() {
    LogItem *li = logStorage->getLogByNumber(currentLogNumber);
    if (NULL == li) {
        return;
    }

    TagItem *ti = logStorage->getTagItem(*li, true);

    if (NULL != ti) {
        ti->widget->setCheckState(Qt::Unchecked);
    }
}

void MainWindow::onCurrentLog_tagSelectColor() {
    LogItem *li = logStorage->getLogByNumber(currentLogNumber);
    if (NULL == li) {
        return;
    }
    setTagColor(li->tag);
}

void MainWindow::onCurrentLog_tagClearColor() {
    LogItem *li = logStorage->getLogByNumber(currentLogNumber);
    if (NULL == li) {
        return;
    }
    clearTagColor(li->tag);
}

void MainWindow::on_tableWidgetFilters_itemChanged(QTableWidgetItem *item)
{
    QString filter = item->text();
    logStorage->setFilterVisibility(filter, item->checkState() == Qt::Checked);
}

void MainWindow::on_plainTextEdit_customContextMenuRequested(const QPoint &pos)
{
    QTextCursor cursor = ui->plainTextEdit->cursorForPosition(pos);
    const QString text = cursor.block().text();

    if (text.size() < Settings::getInstance()->lineNumberWidth) {
        return;
    }

    bool ok;
    int number = text.mid(0, Settings::getInstance()->lineNumberWidth).toInt(&ok);

    if (!ok) {
        return;
    }

    currentLogNumber = number;

    LogItem *li = logStorage->getLogByNumber(number);

    if (li == NULL) {
        return;
    }

    QMenu *menu = new QMenu();

    menu->addAction(QString("Hide %1 tag").arg(li->tag), this, SLOT(onCurrentLog_tagHide()));
    menu->addAction(QString("Set color %1 tag").arg(li->tag), this, SLOT(onCurrentLog_tagSelectColor()));
    menu->addAction(QString("Clear color %1 tag").arg(li->tag), this, SLOT(onCurrentLog_tagClearColor()));

    menu->exec(ui->plainTextEdit->mapToGlobal(pos));
}

void MainWindow::setTagColor(const QString &tag) {
    TagItem *ti = logStorage->getTags()->get(tag);

    if (NULL == ti) {
        logStorage->addTag(tag);
        ti = logStorage->getTags()->get(tag);
    }

    if (NULL == ti) {
        return;
    }

    QColor color = QColorDialog::getColor(Qt::black, this, "Text Color",  QColorDialog::DontUseNativeDialog);
    if(color.isValid()) {
        ti->widget->setTextColor(color);
        int ruleId = NULL == ti ? 0 : ti->ruleId;

        TextHighlighterRule rule(ruleId);

        ti->ruleId = rule.id;
        rule.color = color;
        rule.colorType = TextHighlighterRule::COLOR_TEXT;
        rule.pattern = tag;
        rule.patternType = TextHighlighterRule::TAG;
        rule.highlightType = TextHighlighterRule::HIGHLIGHT_LINE;

        textHighlighter->addRule(rule);
        logStorage->refresh();
    }

}

void MainWindow::clearTagColor(const QString &tag) {
    TagItem *ti = logStorage->getTags()->get(tag);
    ti->widget->setTextColor(ui->tableWidgetTypes->palette().text().color());
    textHighlighter->removeRule(ti->ruleId);
    logStorage->refresh();
}

void MainWindow::setDarkStyle()
{
    qApp->setStyle(QStyleFactory::create("Fusion"));

    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53,53,53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25,25,25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53,53,53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, QColor(230,230,230));
    darkPalette.setColor(QPalette::Button, QColor(53,53,53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));

    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    qApp->setPalette(darkPalette);

    qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");
}


void MainWindow::on_lineEditSearch_textChanged(const QString &text)
{
    int index = -1;

    /*
    index = textHighlighter->indexOfRule(searchTextColorRuleId);
    if (-1 == index) {
        // add rule
        TextHighlighterRule rule;
        searchTextColorRuleId = rule.id;
        rule.color = QColor(Qt::gray);
        rule.colorType = TextHighlighterRule::COLOR_TEXT;
        rule.pattern = text;
        rule.patternType = TextHighlighterRule::LOG_CONTAIN;
        rule.highlightType = TextHighlighterRule::HIGHLIGHT_TEXT;
        textHighlighter->addRule(rule);
        index = textHighlighter->indexOfRule(searchTextColorRuleId);
    }
    if (-1 != index) {
        textHighlighter->rules[index].pattern = text;
    }
    */

    index = textHighlighter->indexOfRule(searchBackgoundColorRuleId);
    if (-1 == index) {
        // add rule
        TextHighlighterRule rule;
        searchBackgoundColorRuleId = rule.id;
        rule.color = QColor(Qt::darkGreen);
        rule.colorType = TextHighlighterRule::COLOR_BACKGROUND;
        rule.pattern = text;
        rule.patternType = TextHighlighterRule::LOG_CONTAIN;
        rule.highlightType = TextHighlighterRule::HIGHLIGHT_TEXT;
        textHighlighter->addRule(rule);
        index = textHighlighter->indexOfRule(searchTextColorRuleId);
    }
    if (-1 != index) {
        textHighlighter->rules[index].pattern = text;
    }

    logStorage->refresh();
    //int indexBackgroundColor = textHighlighter->indexOfRule(searchBackgoundColorRuleId);
    //TextHighlighterRule ruleBackgroundColor(indexBackgroundColor);

}
