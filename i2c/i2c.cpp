#include "i2c.h"
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <fcntl.h>    // For O_RDWR
#include <unistd.h>   // For open()
#include <sys/ioctl.h>
#include <QDebug>
#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream

Q_GLOBAL_STATIC(I2C, self);
using namespace std;
const char EOT = 0x04;

I2C::I2C(QObject *parent) :
    QObject(parent),
    fd(0)
{
    initBus();
}

I2C* I2C::instance()
{
    return self;
}

I2C::~I2C()
{
    //if file handle is open close it.
    if(fd)
    {
        close(fd);
    }
    std::clog << "I2C Deleted\n";
}

void I2C::initBus()
{
    //Open the i2c file descriptor in read/write mode.
    fd = open("/dev/i2c-1", O_RDWR);
    if (fd < 0)
    {
        qDebug("Can't open I2C BUS: %s\n", strerror(errno));
        return;
    }

    qDebug() << "I2C bus initialized with handle" << fd;
}

bool I2C::connectSlave(int slave)
{
    //Using ioctl set the i2c device to talk to address in the "addr" variable.
    if (ioctl(fd, I2C_SLAVE, slave) < 0)
    {
        qDebug("Can't set the I2C address for the slave device: %s\n", strerror(errno));
        return false;
    }

    return true;
}

int I2C::readData(char *data, int length)
{
    //Read "length" number of bytes into the "data" buffer from the I2C bus.
    int er = read(fd,data,static_cast<size_t>(length));
    qDebug("reading from bus: %s\n", strerror(errno));
    return er;
}

int I2C::readData(QByteArray &out, int len, int slave)
{
    if(!connectSlave(slave))
    {
        return -1;
    }

    if(len == 0)
        return 0;

    int l = qMin(32,len);
    char arr[32] = {'#'};
    int r = readData(arr,l);
    if(r == -1)
        return r;

    for(int i = 0; i < r; i++)
    {
        out.append(arr[i]);
        if(arr[i] == EOT)
            return i+1;
    }

    return r;
}

int I2C::writeData(char *data,int length)
{
    //Write "length" number of bytes from the "data" buffer to the I2C bus.
    int er = write(fd,data,static_cast<size_t>(length));
    qDebug("writting to bus: %s\n", strerror(errno));
    return er;
}

int I2C::writeData(const QByteArray& data, int slave)
{
    if(!connectSlave(slave))
    {
        return -1;
    }

    int l = data.length();
    if(l == 0)
        return 0;

    l = qMin(l,32);
    char* d = const_cast<char*>(data.data());
    return writeData(d,l);
}
