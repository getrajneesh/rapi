#include "gpio.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <QDebug>
#include <poll.h>
#include <QFile>
#include <QSocketNotifier>
#include <QFileSystemWatcher>

Q_GLOBAL_STATIC(GPIO,self);
GPIO::GPIO(QObject *parent) : QObject(parent)
{
}

GPIO* GPIO::instance()
{
    return self;
}

GPIO::~GPIO()
{
    // Unexport the pin by writing to /sys/class/gpio/unexport

    int fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (fd == -1)
    {
        qDebug("Unable to open /sys/class/gpio/unexport");
        return;
    }

    for(int i = 0 ; i < pinList.size(); i++)
    {
        QString pinStr = QString::number(pinList.at(i));
        if (write(fd, pinStr.toStdString().data(), static_cast<size_t>(pinStr.length())) != pinStr.length())
        {
            qDebug("Error writing to /sys/class/gpio/unexport");
        }
        else
        {
         qDebug() << QString("gpio pin %1 closed.").arg(pinStr);
        }
    }

    close(fd);
}

void GPIO::setMode(Pin pin, Mode mode)
{
    int p = static_cast<int>(pin);
    //store pin for unexport.
    pinList.push_back(p);

    // Export the desired pin by writing to /sys/class/gpio/export

    int fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd == -1)
    {
        qDebug("Unable to open /sys/class/gpio/export");
        return;
    }

    QString pinStr = QString::number(p);
    if (write(fd, pinStr.toStdString().data(), static_cast<size_t>(pinStr.length())) != pinStr.length())
    {
        qDebug("Error writing to /sys/class/gpio/export");
    }

    close(fd);

    //wait 50ms
    usleep(50000);

    // Set the pin mode.
    QString direction = QStringLiteral("/sys/class/gpio/gpio%1/direction").arg(p);
    fd = open(direction.toStdString().data(), O_WRONLY);
    if (fd == -1)
    {
        qDebug() << "Unable to open " << direction;
        return;
    }

    if(mode == Mode::Output)
    {
        if (write(fd, "out", 3) != 3)
        {
            qDebug() << "Error writing to " << direction;
            return;
        }
    }
    else
    {
        if (write(fd, "in", 2) != 2)
        {
            qDebug() << "Error writing to " << direction;
            return;
        }
    }

    close(fd);
}

void GPIO::setValue(Pin pin, Value value)
{
    int p = static_cast<int>(pin);
    QString valueStr = QStringLiteral("/sys/class/gpio/gpio%1/value").arg(p);
    int fd = open(valueStr.toStdString().data(), O_WRONLY);
    if (fd == -1)
    {
        qDebug() << "Unable to open " << valueStr;
        return;
    }

    if(value == Value::Low)
    {
        if (write(fd, "0", 1) != 1)
        {
            qDebug() << "Error writing to " << valueStr;
        }
    }
    else
    {
        if (write(fd, "1", 1) != 1)
        {
            qDebug() << "Error writing to " << valueStr;
        }
    }

    close(fd);
}

Value GPIO::getValue(Pin pin)
{
//    poll(2);
    int p = static_cast<int>(pin);
    QString valueStr = QStringLiteral("/sys/class/gpio/gpio%1/value").arg(p);
    int fd = open(valueStr.toStdString().data(), O_RDONLY);
    if (fd == -1)
    {
        qDebug() << "Unable to open " << valueStr;
        return Value::Low;
    }

    char ch;
    if(read(fd, &ch, 1) != 1)
    {
      qDebug() << "Unable to read value";
      return Value::Low;
    }

    close(fd);
    Value val  = Value(ch - '0');
    return val;
}

void GPIO::setEdgeTrigger(Pin pin, Edge edge)
{
    int p = static_cast<int>(pin);
    QString edgeStr = QStringLiteral("/sys/class/gpio/gpio%1/edge").arg(p);
    int fd = open(edgeStr.toStdString().data(), O_WRONLY);
    if(fd == -1)
    {
       qDebug() << "Unable to open " << edgeStr;
       return;
    }

    QString edgeValue;
    switch(edge)
    {
    case Edge::Rising:
        edgeValue = "rising";
        break;
    case Edge::Falling:
        edgeValue = "falling";
        break;
    case Edge::Both:
        edgeValue = "both";
        break;
    case Edge::None:
        edgeValue = "none";
        break;
    }

   if( write(fd, edgeValue.toStdString().data(), static_cast<size_t>(edgeValue.length())) != edgeValue.length())
   {
        qDebug() << "Error writing to " << edgeStr;
   }
   close(fd);

   //wait 50ms
   usleep(50000);

   QString valueStr = QString("/sys/class/gpio/gpio%1/value").arg(p);
   QFileSystemWatcher* watcher = new QFileSystemWatcher(this);
   watcher->setProperty("pin", p);
   watcher->addPath(valueStr);
   connect(watcher, &QFileSystemWatcher::fileChanged, this, &GPIO::onInterrupt);
}

void GPIO::onInterrupt()
{
    QFileSystemWatcher* watcher = qobject_cast<QFileSystemWatcher*>(sender());
    disconnect(watcher, &QFileSystemWatcher::fileChanged, this, &GPIO::onInterrupt);
    int p = watcher->property("pin").toInt();
    auto val = this->getValue(Pin(p));
    if(val == Value::High)
    {
        qDebug() << "interrupt rising edge";
        emit risingEdgeTriggered();
    }
    else if(val == Value::Low)
    {
        qDebug() << "interrupt falling edge";
        emit fallingEdgeTriggered();
    }
    connect(watcher, &QFileSystemWatcher::fileChanged, this, &GPIO::onInterrupt);
}
