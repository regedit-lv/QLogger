#ifndef DEBUG_H
#define DEBUG_H

#include <QDebug>

#define DBG_OUT if (0) qDebug()
//#define DBG_OUT qDebug()

#define ERR_OUT qDebug() << __PRETTY_FUNCTION__ << __LINE__

#endif // DEBUG_H
