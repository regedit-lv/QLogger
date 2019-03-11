#include "comportstream.h"

#include <QMessageBox>

ComPortStream::ComPortStream(const ComPortSettings &comPortSettings)
    : LogStream(comPortSettings.streamSettings)
{
    _comPortSettings = comPortSettings;
}

ComPortStream::~ComPortStream()
{

}

void ComPortStream::start()
{
    _port = new QSerialPort();
    _port->setPortName(_comPortSettings.port);
    _port->setBaudRate(_comPortSettings.baudRate);
    _port->setDataBits(_comPortSettings.dataBits);
    _port->setStopBits(_comPortSettings.stopBits);
    _port->setFlowControl(_comPortSettings.flowControl);
    _port->setParity(_comPortSettings.parity);
    //
    //if(!_port->open(QIODevice::ReadWrite) )
    //{
    //    QMessageBox *msg = new QMessageBox();
    //    msg->setText(QString("failed to open %1 %2").arg(_settings.name).arg(_settings.port));
    //    msg->show();
    //}

    //QSerialPort *_port = new QSerialPort();
    //_port->setPortName("COM3");
    //_port->setBaudRate(QSerialPort::Baud9600);
    //_port->setDataBits(QSerialPort::Data7);
    //_port->setStopBits(QSerialPort::OneStop);
    //_port->setFlowControl(QSerialPort::NoFlowControl);
    //_port->setParity(QSerialPort::EvenParity);

    if(!_port->open(QIODevice::ReadWrite) )
    {
//        QMessageBox *msg = new QMessageBox();
//        msg->setText(QString("failed to open com3"));
//        msg->show();
    }

    //while (true)
    //{
    //    _port->waitForReadyRead();
    //    QByteArray data = _port->readAll();
    //
    //    printf("%s", data.data());
    //}
}

void ComPortStream::end()
{
    if(_port->isOpen())
    {
        _port->close();
    }
    delete _port;
    _port = nullptr;
}

QByteArray ComPortStream::getBytes()
{
    if (_port->isOpen())
    {
        _port->waitForReadyRead();
        return _port->readAll();
    }
    else
    {
        return QByteArray();
    }
}

