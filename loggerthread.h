#ifndef LOGGERTHREAD_H
#define LOGGERTHREAD_H

#include <QThread>
#include <QString>
#include <QList>
#include <QByteArray>

class LoggerThread : public QThread
{
    Q_OBJECT
public:
    typedef enum {
        RUNNING,
        PAUSED,
        STOPPING,
        STOPPED
    } Status;

    LoggerThread();

    void resume();
    void pause();
    void stop();
    Status getStatus();
signals:
    void newLogItems(const QList<QString> &types, const QList<QString> &tags, const QList<QString> &texts);

protected:
    void run();

    virtual void startLogging() = 0;
    virtual QByteArray getLogBytes() = 0;
    virtual void endLogging() = 0;
    virtual void parseLog(const QString &line, QString &type, QString &tag, QString &text) = 0;

    Status status;
};

#endif // LOGGERTHREAD_H
