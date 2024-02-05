#ifndef I2C_H
#define I2C_H

#include <QObject>

class I2C : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief I2C constructor.
     * @param parent
     */
    explicit I2C(QObject *parent = nullptr);

    /**
     *@brief ~I2C desctructor.
     */
    ~I2C();

    /**
     * @brief instance creates singleton instance.
     * @return object pointer.
     */
    static I2C* instance();


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

private:
    /**
     * @brief initBus: initialises the bus.
     */
    void initBus();

    /**
     * @brief read: reads data from bus.
     * @param data: pointer to buffer array which will hold data.
     * @param length: length of the expected data.
     * @return: return size of data read, < 0 means there is and error.
     */
    int readData(char *data, int length);

    /**
     * @brief write: writes data to bus.
     * @param data: pointer to buffer array.
     * @param length: lenth of data
     * @return size of data written or erro if < 0
     */
    int writeData(char *data,int length);

    /**
     * @brief connectSlave: do handshake with slave.
     * @param slave: slave id.
     */
    bool connectSlave(int slave);

private:
    /**
     * @brief fd: i2c bus handle.
     */
    int fd;
};

#define i2c I2C::instance()

#endif // I2C_H
