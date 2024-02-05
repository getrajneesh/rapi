# rapi
This repo contains GPIO and I2C library for RaspberryPI written in Qt/C++.

### Usage
Add .cpp and .h files to your Qt project, build and use it.

GPIO:

`code`
```
// initialize gpio
gpio->setMode(Pin::GPIO4, Mode::Output);
gpio->setEdgeTrigger(Pin::GPIO4, Edge::Rising);
// set value
gpio->setValue(Pin::GPIO4, Value::High);
```

I2C:

below methods can be used to read and write serialised data (32 bytes max protocol standard)

    /**
     * @brief readData reads data from bus and fills in out.
     * @param out buffer out parameter.
     * @param len length of data to be read.
     * @return no of bytes read.
     */
    int readData(QByteArray& out, int len, int slave);

    /**
     * @brief writeData: writes data to i2c bus.
     * @param data: data to be sent.
     * @return no of bytes written.
     */
    int writeData(const QByteArray& data, int slave);

### sample code to send data.

    int slave = 8; //Slave device identifier
    char* buff = nullptr;
    int len = request.serialize(buff); //serialize data to be sent
    QByteArray ba(buff, len);

    int err = i2c->writeData(ba, slave);
    qDebug()<< "sending:" << len << "bytes sent " << err ;
    if(err == -1)
    {
        qDebug()<< "sending failed:" << len << err ;
    }


### sample code to recieve data.

    int slave = 8; //Slave device identifier
    QByteArray ba;
    int ret = i2c->readData(ba, 32, slave);
    if(ret == -1)
    {
        qDebug() << "slave not available: read failed";
    }


