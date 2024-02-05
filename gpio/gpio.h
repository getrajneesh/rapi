#ifndef GPIO_H
#define GPIO_H

#include <QObject>

enum class Pin
{
    GPIO0   = 0,
    GPIO1   = 1,
    GPIO2   = 2,
    GPIO3   = 3,
    GPIO4   = 4,
    GPIO5   = 5,
    GPIO6   = 6,
    GPIO7   = 7,
    GPIO8   = 8,
    GPIO9   = 9,
    GPIO10  = 10,
    GPIO11  = 11,
    GPIO12  = 12,
    GPIO13  = 13,
    GPIO14  = 14,
    GPIO15  = 15,
    GPIO16  = 16,
    GPIO17  = 17,
    GPIO18  = 18,
    GPIO19  = 19,
    GPIO20  = 20,
    GPIO21  = 21,
    GPIO22  = 22,
    GPIO23  = 23,
    GPIO24  = 24,
    GPIO25  = 25,
    GPIO26  = 26,
    GPIO27  = 27,
};
Q_DECLARE_METATYPE(Pin);

enum class Mode
{
    Input = 0,
    Output = 1,
};
Q_DECLARE_METATYPE(Mode);

enum class Value
{
    Low  = 0,
    High = 1,
};
Q_DECLARE_METATYPE(Value);

enum class Edge
{
    None,
    Rising,
    Falling,
    Both,
};
Q_DECLARE_METATYPE(Edge);

class GPIO : public QObject
{
    Q_OBJECT
public:
    explicit GPIO(QObject *parent = nullptr);
    ~GPIO();
    static GPIO* instance();
    void setMode(Pin pin , Mode mode);
    void setValue(Pin pin, Value value);
    Value getValue(Pin pin);
    void setEdgeTrigger(Pin pin, Edge edge);

private:
    QList<int> pinList;

signals:
    void fallingEdgeTriggered();
    void risingEdgeTriggered();

private slots:
    void onInterrupt();
};

#define gpio GPIO::instance()
#endif // GPIO_H
