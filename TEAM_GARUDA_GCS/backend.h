#ifndef BACKEND_H
#define BACKEND_H

#include <QMainWindow>
#include <QSerialPort>
#include <QtMqtt/QMqttMessage>
#include <QtMqtt/QMqttSubscription>
#include <QFile>

class backend
{
public:
    backend();
    // plotting functions
    void configureGraph();

    // serialport functions
    bool checkArduinoConnection();
    void configureSerial();
    void connectSerial();

    // mqtt functions
    void configureMqtt();
    void connectMqtt();
    void subscribeMqtt();
    void seriaWrite(QString string);

    // command functions
    void setTime();
    void simEnable();
    void simActivate();
    void simDisable();
    void simp();

    // csv writing functions
    void writeCSV(QString telemetry);


private slots:
    void readSerial();
    void writeSerial();
    void plot();

private:
    QSerialPort *arduino;
};

#endif // BACKEND_H
