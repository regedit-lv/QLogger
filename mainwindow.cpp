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

#include "customdialog.h"
#include "debug.h"

#include "threadmanager.h"
#include "comportstream.h"
#include "filestream.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
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

//    connect(ui->actionAdb_logcat, SIGNAL(toggled(bool)), this, SLOT(onActionAdbLogcatToggled(bool)));
//    connect(ui->actionTrustzone, SIGNAL(toggled(bool)), this, SLOT(onActionTrustzoneToggled(bool)));
//    connect(ui->actionKernel_log, SIGNAL(toggled(bool)), this, SLOT(onActionKernelLogToggled(bool)));

//    ui->actionAdb_logcat->setChecked(Settings::getInstance()->logThreadStatus[Settings::LogThreadAdbLogcat]);
//    ui->actionTrustzone->setChecked(Settings::getInstance()->logThreadStatus[Settings::LogThreadTrustzone]);
//    ui->actionKernel_log->setChecked(Settings::getInstance()->logThreadStatus[Settings::LogThreadKernel]);

    connect(ui->actionComPortNew, SIGNAL(triggered()), this, SLOT(onActionNewComPort()));
    generateComPortMenu();

    _isLogPaused = false;
    ui->pushButtonPauseLogging->setText("Pause");
}

MainWindow::~MainWindow()
{
    // TODO: stop all runnings logger threads
    delete ui;
}

void MainWindow::addEditComPort(const QString &comPort)
{
    CustomDialog d("Com Port Settings");

    ComPortSettings *existingComPort = Settings::getInstance()->getComPortSettings(comPort);

    std::string sName;
    std::string sPort;
    std::string sBaudRate;
    std::string sDataBits;
    std::string sParity;
    std::string sStopBits;
    std::string sFlowControl;
    std::string sTagScript = "(function (line) {\n // -- your code here \n    return \"\";\n})";

    if (existingComPort)
    {
        sName = std::string(existingComPort->streamSettings.name.toLatin1().constData());
        sPort = std::string(existingComPort->port.toLatin1().constData());
        sBaudRate = std::to_string(existingComPort->baudRate);
        sDataBits = std::to_string(existingComPort->dataBits);
        switch(existingComPort->parity)
        {
        case QSerialPort::NoParity:
            sParity = "no";
            break;
        case QSerialPort::EvenParity:
            sParity = "even";
            break;
        case QSerialPort::OddParity:
            sParity = "odd";
            break;
        case QSerialPort::SpaceParity:
            sParity = "space";
            break;
        case QSerialPort::MarkParity:
            sParity = "mark";
            break;
        default:
            break;
        }

        switch (existingComPort->stopBits)
        {
        case QSerialPort::OneStop:
            sStopBits = "1";
            break;
        case QSerialPort::OneAndHalfStop:
            sStopBits = "1.5";
            break;
        case QSerialPort::TwoStop:
            sStopBits = "2";
            break;
        default:
            break;
        }

        switch (existingComPort->flowControl)
        {
        case QSerialPort::NoFlowControl:
            sFlowControl = "no";
            break;
        case QSerialPort::SoftwareControl:
            sFlowControl = "software";
            break;
        case QSerialPort::HardwareControl:
            sFlowControl = "software";
            break;
        default:
            break;
        }

        sTagScript = std::string(existingComPort->streamSettings.tagScript.toLatin1().constData());
    }

    d.addLineEdit("Name: ", &sName, sName.c_str());
    d.addLineEdit("Port: ", &sPort, sPort.c_str());
    d.addLineEdit("Baud rate: ", &sBaudRate, sBaudRate.c_str());
    d.addLineEdit("Data bits: ", &sDataBits, sDataBits.c_str());
    d.addLineEdit("Parity: ", &sParity, sParity.c_str());
    d.addLineEdit("Stop bits: ", &sStopBits, sStopBits.c_str());
    d.addLineEdit("Flow control: ", &sFlowControl, sFlowControl.c_str());
    d.addLabel("Tag script");
    d.addTextEdit(&sTagScript, false, false);

    d.exec();

    if(!d.wasCancelled()) {
        bool ok = false;
        int n = QString(sBaudRate.c_str()).toInt(&ok);
        if (!ok)
        {
            QMessageBox::critical(this, "Com Port Settings", "Wrong baud rate. Should be int value");
            return;
        }

        QSerialPort::BaudRate baudRate = QSerialPort::UnknownBaud;

        switch (n)
        {
        case QSerialPort::Baud1200:
            baudRate = QSerialPort::Baud1200;
            break;
        case QSerialPort::Baud2400:
            baudRate = QSerialPort::Baud2400;
            break;
        case QSerialPort::Baud4800:
            baudRate = QSerialPort::Baud4800;
            break;
        case QSerialPort::Baud9600:
            baudRate = QSerialPort::Baud9600;
            break;
        case QSerialPort::Baud19200:
            baudRate = QSerialPort::Baud19200;
            break;
        case QSerialPort::Baud38400:
            baudRate = QSerialPort::Baud38400;
            break;
        case QSerialPort::Baud57600:
            baudRate = QSerialPort::Baud57600;
            break;
        case QSerialPort::Baud115200:
            baudRate = QSerialPort::Baud115200;
            break;
        default:
            QMessageBox::critical(this, "Com Port Settings", "Unsupported baud rate");
            return;
        }

        QSerialPort::DataBits dataBits = QSerialPort::UnknownDataBits;

        n = QString(sDataBits.c_str()).toInt(&ok);
        if (!ok)
        {
            QMessageBox::critical(this, "Com Port Settings", "Wrong baud rate. Should be int value");
            return;
        }

        switch (n)
        {
        case QSerialPort::Data5:
            dataBits = QSerialPort::Data5;
            break;
        case QSerialPort::Data6:
            dataBits = QSerialPort::Data6;
            break;
        case QSerialPort::Data7:
            dataBits = QSerialPort::Data7;
            break;
        case QSerialPort::Data8:
            dataBits = QSerialPort::Data8;
            break;
        default:
            QMessageBox::critical(this, "Com Port Settings", "Unsupported data bits");
            return;
        }

        QSerialPort::Parity parity = QSerialPort::UnknownParity;

        if (strcmp(sParity.c_str(), "no") == 0)
        {
            parity = QSerialPort::NoParity;
        }
        else if (strcmp(sParity.c_str(), "even") == 0)
        {
            parity = QSerialPort::EvenParity;
        }
        else if (strcmp(sParity.c_str(), "odd") == 0)
        {
            parity = QSerialPort::OddParity;
        }
        else if (strcmp(sParity.c_str(), "space") == 0)
        {
            parity = QSerialPort::SpaceParity;
        }
        else if (strcmp(sParity.c_str(), "mark") == 0)
        {
            parity = QSerialPort::MarkParity;
        }
        else
        {
            QMessageBox::critical(this, "Com Port Settings", "Unsupported parity. Supported: no, even, odd, space or mark");
            return;
        }

        QSerialPort::StopBits stopBits = QSerialPort::UnknownStopBits;

        if (strcmp(sStopBits.c_str(), "1") == 0)
        {
            stopBits = QSerialPort::OneStop;
        }
        else if (strcmp(sStopBits.c_str(), "1.5") == 0)
        {
            stopBits = QSerialPort::OneAndHalfStop;
        }
        else if (strcmp(sStopBits.c_str(), "2") == 0)
        {
            stopBits = QSerialPort::TwoStop;
        }
        else
        {
            QMessageBox::critical(this, "Com Port Settings", "Unsupported stop bits. Supported: 1, 1.5, 2");
            return;
        }

        QSerialPort::FlowControl flowControl = QSerialPort::UnknownFlowControl;
        if (strcmp(sFlowControl.c_str(), "no") == 0)
        {
            flowControl = QSerialPort::NoFlowControl;
        }
        else if (strcmp(sFlowControl.c_str(), "software") == 0)
        {
            flowControl = QSerialPort::SoftwareControl;
        }
        else if (strcmp(sFlowControl.c_str(), "hardware") == 0)
        {
            flowControl = QSerialPort::HardwareControl;
        }
        else
        {
            QMessageBox::critical(this, "Com Port Settings", "Unsupported flow control. Supported: no, software, hardware");
            return;
        }

        if (existingComPort == nullptr)
        {
            ComPortSettings comPort;

            comPort.streamSettings.name = QString(sName.c_str());
            comPort.port = QString(sPort.c_str());
            comPort.baudRate = baudRate;
            comPort.dataBits = dataBits;
            comPort.parity = parity;
            comPort.stopBits = stopBits;
            comPort.flowControl = flowControl;
            comPort.streamSettings.tagScript = QString(sTagScript.c_str());
            Settings::getInstance()->comPorts.append(comPort);
        }
        else
        {
            existingComPort->streamSettings.name = QString(sName.c_str());
            existingComPort->port = QString(sPort.c_str());
            existingComPort->baudRate = baudRate;
            existingComPort->dataBits = dataBits;
            existingComPort->parity = parity;
            existingComPort->stopBits = stopBits;
            existingComPort->flowControl = flowControl;
            existingComPort->streamSettings.tagScript = QString(sTagScript.c_str());
        }
    }
}

void MainWindow::generateComPortMenu()
{
    while (ui->menuCom_Port->actions().size() > 1)
    {
        QAction *action = ui->menuCom_Port->actions().at(1);
        ui->menuCom_Port->removeAction(action);
        delete action;
    }

    for (ComPortSettings &comPort : settings->getInstance()->comPorts)
    {
        bool running = ThreadManager::getInstance()->get(comPort.streamSettings.name) != nullptr;

        QMenu *menu = ui->menuCom_Port->addMenu(QString("%1 %2").arg(comPort.streamSettings.name).arg(running ? "(running)" : ""));

        connect(menu, SIGNAL(triggered(QAction *)), this, SLOT(onActionComPort(QAction *)), Qt::QueuedConnection);

        if (running)
        {
            menu->addAction("stop");
        }
        else
        {
            menu->addAction("start");
        }
        menu->addAction("edit");
        menu->addAction("delete");
    }

}
void MainWindow::connectLoggerThread(LoggerThread *thread)
{
    connect(thread, SIGNAL(newLogItems(QList<QString>,QList<QString>,QList<QString>)),
            this, SLOT(on_newLogItems(QList<QString>,QList<QString>,QList<QString>)), Qt::BlockingQueuedConnection);
    connect(thread,  SIGNAL(finished(LoggerThread*)),
            this, SLOT(on_LoggerThread_finished(LoggerThread*)), Qt::BlockingQueuedConnection);
}

void MainWindow::addLogItem(const QString &type, const QString &tag, const QString &text)
{
    LogItem li;
    li.tag = tag;
    li.type = type;
    li.text = text;
    logStorage->add(li);
}

void MainWindow::on_newLogItem(const QString &type, const QString &tag, const QString &text)
{
    if (_isLogPaused)
    {
        return;
    }

    QScrollBar *vb = ui->plainTextEdit->verticalScrollBar();
    QScrollBar *hb = ui->plainTextEdit->verticalScrollBar();
    bool scrollDown = vb->maximum() == vb->value();

    addLogItem(type, tag, text);

    if (scrollDown) {
        vb->scroll(hb->value(), vb->maximum());
    }
}

void MainWindow::on_newLogItems(const QList<QString> &types, const QList<QString> &tags, const QList<QString> &texts)
{
    if (_isLogPaused)
    {
        return;
    }

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

void MainWindow::on_LoggerThread_finished(LoggerThread *thread)
{
    ThreadManager::getInstance()->remove(thread);
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
    if (nullptr != selectedTableItem) {
        QMenu *menu = new QMenu();

        menu->addAction(tr("Select color"), this, SLOT(on_tagSelectColor()));
        menu->addAction(tr("Clear color"), this, SLOT(on_tagClearColor()));
        menu->exec(ui->tableWidgetTags->mapToGlobal(pos));
    }
}

void MainWindow::on_tagSelectColor() {
    if (selectedTableItem == nullptr) {
        return;
    }

    QString tag = selectedTableItem->text();
    setTagColor(tag);
}

void MainWindow::on_tagClearColor() {
    if (selectedTableItem == nullptr) {
        return;
    }

    QString tag = selectedTableItem->text();
    clearTagColor(tag);
}

void MainWindow::on_tableWidgetTypes_customContextMenuRequested(const QPoint &pos)
{
    selectedTableItem = ui->tableWidgetTypes->itemAt(pos);
    if (nullptr != selectedTableItem) {
        QMenu *menu = new QMenu();
        menu->addAction(tr("Select color"), this, SLOT(on_typeSelectColor()));
        menu->addAction(tr("Clear color"), this, SLOT(on_typeClearColor()));
        menu->exec(ui->tableWidgetTypes->mapToGlobal(pos));
    }
}

void MainWindow::on_typeSelectColor() {
    if (selectedTableItem == nullptr) {
        return;
    }

    QColor color = QColorDialog::getColor(Qt::black, this, "Text Color",  QColorDialog::DontUseNativeDialog);
    if(color.isValid()) {
        selectedTableItem->setBackgroundColor(color);

        TypeItem *ti = logStorage->getTypes()->get(selectedTableItem);
        int ruleId = nullptr == ti ? 0 : ti->ruleId;

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
    if (selectedTableItem == nullptr) {
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

    d.exec();

    if(!d.wasCancelled())
    {
        bool ok = false;
        int c = QString(maxLogCount.c_str()).toInt(&ok);
        if (ok)
        {
            settings->maxLinesOfLogs = c;
            settings->saveSettings();
        }
    }
}

void MainWindow::onActionExit() {
    QApplication::quit();
}

void MainWindow::onActionLoadFile() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("All file (*)"));
    FileSettings fileStreamSettings;
    ERR_OUT << fileName;

    if (QFile::exists(fileName)) {
        QString profile = "";
        int profileIndex = -1;

        CustomDialog d("File settings");
        d.addLabel("Use settings from");

        Settings *settings = Settings::getInstance();

        for (const ComPortSettings &comPort : settings->comPorts)
        {
            if (comPort.streamSettings.tagScript != "")
            {
                if (profile != "")
                {
                    profile += "|";
                }
                profile += comPort.streamSettings.name;
            }
        }
        d.addComboBox("Profile: ", profile, &profileIndex);

        d.exec();

        if(!d.wasCancelled()) {
            ComPortSettings &comPort = settings->comPorts[profileIndex];
            fileStreamSettings.streamSettings = comPort.streamSettings;
            fileStreamSettings.name = fileName;
            startFileThread(fileStreamSettings);
        }


    }
}

void MainWindow::onActionAbout() {
    QMessageBox::about(this, "QLogger", "Reading logs.<br>" \
                       "Version 0.9 <br>" \
                       "Powered by Qt Creator 3.1.2 <br>" \
                       "Based on Qt 5.3.1 (GCC 4.6.1)");
}

void MainWindow::onActionNewComPort()
{
    addEditComPort("");
    Settings::getInstance()->saveSettings();
    generateComPortMenu();
}

void MainWindow::onActionComPort(QAction *action)
{
    QMenu *menu = static_cast<QMenu*>(action->parentWidget());
    QString actionStr = action->text();
    // title text is <name> (<status>). For example: MyComPort (running)
    QString comPortName = menu->title().split(" ").at(0);

    if (actionStr == "start")
    {
        startComPortThread(comPortName);
    }
    else if (actionStr == "stop")
    {
        startComPortThread(comPortName);
    }

    else if (actionStr == "edit")
    {
        addEditComPort(comPortName);
        Settings::getInstance()->saveSettings();
    }
    else if (actionStr == "delete")
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this,
                                      "Delete confirmation", QString("Are you sure that you want to delete %1?").arg(comPortName),
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            Settings::getInstance()->deleteComPortSettings(comPortName);
            Settings::getInstance()->saveSettings();
        }
    }

    generateComPortMenu();
}

void MainWindow::startComPortThread(const QString &name)
{
    const ComPortSettings *settings = Settings::getInstance()->getComPortSettings(name);
    ComPortStream *stream = new ComPortStream(*settings);
    LoggerThread *thread = ThreadManager::getInstance()->add(stream);

    connectLoggerThread(thread);
    thread->start();
}

void MainWindow::stopThread(const QString &name)
{
    LoggerThread *thread = ThreadManager::getInstance()->get(name);
    if (thread != nullptr)
    {
        thread->stop();
    }
}

void MainWindow::startFileThread(const FileSettings &settings)
{
    FileStream *stream = new FileStream(settings);
    LoggerThread *thread = ThreadManager::getInstance()->add(stream);

    connectLoggerThread(thread);
    thread->start();
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
    if (nullptr != selectedTableItem) {
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
        FilterItem filter(name.c_str(), static_cast<FilterItem::FilterAction>(actionIndex), expression.c_str());
        logStorage->add(filter);
    }
}

void MainWindow::on_filterEdit() {
    if (selectedTableItem == nullptr) {
        return;
    }

    FilterItem *filter = logStorage->getFilters()->get(selectedTableItem);

    if (filter == nullptr) {
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
        filter->action = static_cast<FilterItem::FilterAction>(actionIndex);
        filter->widget->setText(filter->name);
        int ruleIndex = TextHighlighter::instance()->indexOfRule(filter->ruleId);
        if (-1 != ruleIndex) {
            TextHighlighter::instance()->rules[ruleIndex].pattern = expression.c_str();
            logStorage->refresh();
        }
    }
}

void MainWindow::on_filterDelete() {
    if (selectedTableItem == nullptr) {
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
    if (selectedTableItem == nullptr) {
        return;
    }

    QColor color = QColorDialog::getColor(Qt::black, this, "Text Color",  QColorDialog::DontUseNativeDialog);
    if(color.isValid()) {
        selectedTableItem->setTextColor(color);

        FilterItem *fi = logStorage->getFilters()->get(selectedTableItem);
        int ruleId = nullptr == fi ? 0 : fi->ruleId;

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
    if (selectedTableItem == nullptr) {
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
    if (nullptr == li) {
        return;
    }

    TagItem *ti = logStorage->getTagItem(*li, true);

    if (nullptr != ti) {
        ti->widget->setCheckState(Qt::Unchecked);
    }
}

void MainWindow::onCurrentLog_tagSelectColor() {
    LogItem *li = logStorage->getLogByNumber(currentLogNumber);
    if (nullptr == li) {
        return;
    }
    setTagColor(li->tag);
}

void MainWindow::onCurrentLog_tagClearColor() {
    LogItem *li = logStorage->getLogByNumber(currentLogNumber);
    if (nullptr == li) {
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

    if (li == nullptr) {
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

    if (nullptr == ti) {
        logStorage->addTag(tag);
        ti = logStorage->getTags()->get(tag);
    }

    if (nullptr == ti) {
        return;
    }

    QColor color = QColorDialog::getColor(Qt::black, this, "Text Color",  QColorDialog::DontUseNativeDialog);
    if(color.isValid()) {
        ti->widget->setTextColor(color);
        int ruleId = nullptr == ti ? 0 : ti->ruleId;

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

void MainWindow::on_pushButtonPauseLogging_clicked()
{
    _isLogPaused = !_isLogPaused;
    if (_isLogPaused)
    {
        ui->pushButtonPauseLogging->setText("Resume");
    }
    else
    {
        ui->pushButtonPauseLogging->setText("Pause");
    }
}
