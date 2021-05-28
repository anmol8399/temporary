#ifndef GARUDA_H
#define GARUDA_H
#include <gmap.h>
#include <bar.h>
#include <QMainWindow>
#include <QtCharts/QtCharts>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QQuickWidget>
#include <QtSerialPort/QSerialPortInfo>
#include <QtQuick/QQuickView>
#include <QtSerialPort/QSerialPort>
#include <QSerialPort>
#include <QByteArray>
#include <QtMqtt/QMqttMessage>
#include <QtMqtt/QMqttSubscription>
#include <QFile>
#include <qcustomplot.h>

#include <QMainWindow>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class GARUDA; }
QT_END_NAMESPACE

class GARUDA : public QMainWindow
{
    Q_OBJECT

public:
    GARUDA(QWidget *parent = nullptr);
    ~GARUDA();


    void checkTelemetry();
    void convertTelemetry();

    // graph function
    void configureGraph();
    void graphSetting(QCustomPlot *p);

    // data plotting on gui
    void updateGraphArray(QStringList sensorData);
    void plot();
    void updateContainerLabel(QStringList sensorData);
    void updatePayload1Label(QStringList sensorData);
    void updatePayload2Label(QStringList sensorData);
    void updateLed(QString state);
    void updateBar(bar *b, QString val);

    // CSV
    void writeCSV(QString telemetry);
    void readCSV();

    // serialport function
    bool checkArduinoConnection();
    void configureSerial();
    void connectSerial();
    void disconnectSerial();

    // mqtt
    void configureMqtt();
    void connectMqtt();

    // publisher
    void publishMqtt(QStringList sensorData);

    // subscriber
    void subscribeMqtt();
    void writeSerial(QString cmd);

    //helper
    double altitudeToPressure(QString altitude);

    // test plotting
//    void readSampleCSV();

private slots:
    void readSerial();

    void getSimCSV(const QByteArray &message, const QMqttTopicName &topic);

    void on_SETTIME_clicked();

    void on_SIMENABLE_clicked();

    void on_SIMDISABLE_clicked();

    void on_SIMACTIVATE_clicked();

    void on_SIMP_clicked();

    void on_CX_clicked();

    void on_MQTT_clicked();

    void on_SP1X_clicked();

    void on_SP2X_clicked();

    void sendSimp();

//    void plotSec();


    void on_CALIBRATION_clicked();

    void on_SP2X_Rel_clicked();

    void on_SP1X_Rel_clicked();

private:
    Ui::GARUDA *ui;

    static const quint16 arduino_uno_vendor_id = 9025;
    static const quint16 arduino_uno_product_id = 67;

//    static const quint16 arduino_uno_vendor_id = 6790;
//    static const quint16 arduino_uno_product_id = 29987;

    QString arduino_uno_port_name;
    QSerialPort *arduino;
    QMqttClient *m_client;
    QTimer *timer;

    // for mqtt connection;
    QString topic = "teams/3394";
    bool startMqtt = false;

    // for serial data collection
    bool CX = false;
    QString serialBuffer;
    QByteArray serial;
    QString telemetry;
    QStringList sensorData;
    QStringList containerTel, payload1Tel, payload2Tel;

    // simulation mode
    bool simEnable = false;
    bool simActivate = false;
    bool getCsv = false;
    QList<QString> simpList;
    int simpIndex = 0;

    // payload variables
    bool SP1X = false;
    bool SP2X = false;
    bool isP1rel = false;
    bool isP2rel = false;

    QVector<double> CAltitude, CVoltage, SP1Altitude, SP2Altitude, SP1Pressure, SP2Pressure, SP1Rotation, SP2Rotation, SP1TEMP, SP2TEMP;
    double pressure;
    QVector<double> graphTime;
    QVector<double> GPSALTITUDE;
    double count = 1;
    QString path = "C:/Users/vivas/Downloads/TEAM_GARUDA_GCS-20210328T065343Z-001";
//    QStringList plotTest;
};
#endif // GARUDA_H
