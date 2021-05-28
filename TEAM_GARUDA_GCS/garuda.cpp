#include "garuda.h"
#include "ui_garuda.h"

#include <QtCharts/QChartView>
#include <gmap.h>
#include <QMainWindow>
#include <QtCharts/QtCharts>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QQuickWidget>
#include <QtSerialPort/QSerialPortInfo>
#include <QtQuick/QQuickView>
#include <QtSerialPort/QSerialPort>
#include <QQmlContext>
#include <ledc.h>
#include <bar.h>
#include <QDebug>
#include <backend.h>
#include <qcustomplot.h>
#include <QtMqtt/QMqttClient>
#include <QFile>
#include <QtMath>


gmap locator;
QFile file("./Serial_data.csv");


GARUDA::GARUDA(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GARUDA)
{
    ui->setupUi(this);
    ui->map->setSource(QUrl::fromLocalFile( path + "/TEAM_GARUDA_GCS/maps/maps/main.qml"));
    ui->map->rootContext()->setContextProperty("gmap",&locator);
    ui->map->show();

    configureGraph();

    arduino = new QSerialPort();
    m_client = new QMqttClient();

    // configure serial
    configureSerial();
    connectSerial();

    //mqtt initialization
    configureMqtt();
    connectMqtt();
    connect(m_client, &QMqttClient::connected, this, &GARUDA::subscribeMqtt);
}

GARUDA::~GARUDA()
{
    delete ui;
}

void GARUDA::checkTelemetry()
{
    int altitude = sensorData[7].toInt();
    int state = sensorData[15].toInt();

    if(altitude <= 500 && state == 4 && !isP1rel) {
        ui->SP1X_Rel->clicked();
        isP1rel = true;
    }
    else if(altitude <= 400 && state == 5 && !isP2rel) {
        ui->SP2X_Rel->clicked();
        isP2rel = true;
    }

}

void GARUDA::configureGraph()
{
    ui->sp1Pressure->addGraph();
    ui->sp1Altitude->addGraph();
    ui->sp1Rotation->addGraph();
    ui->sp1Temp->addGraph();

    graphSetting(ui->sp1Pressure);
    graphSetting(ui->sp1Altitude);
    graphSetting(ui->sp1Rotation);
    graphSetting(ui->sp1Temp);

    ui->sp2Pressure->addGraph();
    ui->sp2Altitude->addGraph();
    ui->sp2Rotation->addGraph();
    ui->sp2Temp->addGraph();

    graphSetting(ui->sp2Pressure);
    graphSetting(ui->sp2Altitude);
    graphSetting(ui->sp2Rotation);
    graphSetting(ui->sp2Temp);

    ui->containerAltitude->addGraph();
    ui->containerVoltage->addGraph();

    graphSetting(ui->containerAltitude);
    graphSetting(ui->containerVoltage);


    ui->gpsaltitude->addGraph();
    graphSetting(ui->gpsaltitude);

}

void GARUDA::graphSetting(QCustomPlot *p)
{
    QColor col = QColor(233, 250, 255);
    QColor text = QColor(46, 27, 58);
    p->setBackground(col);
    p->graph(0)->setScatterStyle(QCPScatterStyle::ssDisc);
    p->graph(0)->setLineStyle(QCPGraph::lsLine);
    QPen pen;
    pen.setColor(text);
    p->xAxis->setBasePen(pen);
    p->xAxis->setTickLabelColor(text);
    p->yAxis->setBasePen(pen);
    p->yAxis->setTickLabelColor(text);
    p->xAxis->grid()->setPen(Qt::NoPen);
    p->yAxis->grid()->setPen(Qt::NoPen);
    p->xAxis->grid()->setSubGridVisible(false);
    p->yAxis->grid()->setSubGridVisible(false);

}

void GARUDA::updateGraphArray(QStringList sensorData)
{
    // 7,9,23,24,25,30,31,32,p(23),p(30);
    graphTime.append(count++);

    CAltitude.append(sensorData[7].toDouble());
    CVoltage.append(sensorData[9].toDouble());

    SP1Altitude.append(sensorData[23].toDouble());
    SP1TEMP.append(sensorData[24].toDouble());
    SP1Rotation.append(sensorData[25].toDouble());
    SP1Pressure.append(altitudeToPressure(sensorData[23]));

    SP2Altitude.append(sensorData[30].toDouble());
    SP2TEMP.append(sensorData[31].toDouble());
    SP2Rotation.append(sensorData[32].toDouble());
    SP2Pressure.append(altitudeToPressure(sensorData[30]));

    GPSALTITUDE.append(sensorData[13].toDouble());
}

void GARUDA::plot()
{
    ui->sp1Altitude->graph(0)->setData(graphTime, SP1Altitude);
    ui->sp1Altitude->replot();
    ui->sp1Altitude->xAxis->rescale(true);
    ui->sp1Altitude->yAxis->rescale(true);

    ui->sp2Altitude->graph()->setData(graphTime, SP2Altitude);
    ui->sp2Altitude->replot();
    ui->sp2Altitude->xAxis->rescale(true);
    ui->sp2Altitude->yAxis->rescale(true);

    ui->sp1Pressure->graph()->setData(graphTime, SP1Pressure);
    ui->sp1Pressure->replot();
    ui->sp1Pressure->xAxis->rescale(true);
    ui->sp1Pressure->yAxis->rescale(true);

    ui->sp2Pressure->graph()->setData(graphTime, SP2Pressure);
    ui->sp2Pressure->replot();
    ui->sp2Pressure->xAxis->rescale(true);
    ui->sp2Pressure->yAxis->rescale(true);

    ui->sp1Temp->graph()->setData(graphTime,  SP1TEMP);
    ui->sp1Temp->replot();
    ui->sp1Temp->xAxis->rescale(true);
    ui->sp1Temp->yAxis->rescale(true);

    ui->sp2Temp->graph()->setData(graphTime, SP2TEMP);
    ui->sp2Temp->replot();
    ui->sp2Temp->xAxis->rescale(true);
    ui->sp2Temp->yAxis->rescale(true);

    ui->sp1Rotation->graph()->setData(graphTime, SP1Rotation);
    ui->sp1Rotation->replot();
    ui->sp1Rotation->xAxis->rescale(true);
    ui->sp1Rotation->yAxis->rescale(true);

    ui->sp2Rotation->graph()->setData(graphTime, SP2Rotation);
    ui->sp2Rotation->replot();
    ui->sp2Rotation->xAxis->rescale(true);
    ui->sp2Rotation->yAxis->rescale(true);

    ui->containerAltitude->graph()->setData(graphTime, CAltitude);
    ui->containerAltitude->replot();
    ui->containerAltitude->xAxis->rescale(true);
    ui->containerAltitude->yAxis->rescale(true);

    ui->containerVoltage->graph()->setData(graphTime, CVoltage);
    ui->containerVoltage->replot();
    ui->containerVoltage->xAxis->rescale(true);
    ui->containerVoltage->yAxis->setRange(8.0, 9.0);
//    ui->containerVoltage->xAxis->rescale(true);


    ui->gpsaltitude->graph()->setData(graphTime, GPSALTITUDE);
    ui->gpsaltitude->replot();
    ui->gpsaltitude->xAxis->rescale(true);
    ui->gpsaltitude->yAxis->rescale(true);

}

void GARUDA::updateContainerLabel(QStringList sensorData)
{
    ui->teamIDval->setText(sensorData[0]);
    ui->missionTimeval->setText(sensorData[1]);
    ui->packetCountval->setText(sensorData[2]);
    ui->packetTypeval->setText(sensorData[3]);
    ui->modeval->setText(sensorData[4]);
    ui->sp1Releasedval->setText(sensorData[5]);
    ui->sp2Releasedval->setText(sensorData[6]);
    ui->gpsTimeval->setText(sensorData[10]);
    ui->gpsLatitudeval->setText(sensorData[11]);
    ui->gpsLongitudeval->setText(sensorData[12]);
    ui->gpsAltitudeval->setText(sensorData[13]);
    ui->gpsStateval->setText(sensorData[14]);
    ui->softwareStateval->setText(sensorData[15]);

    ui->cmdEchoval->setText(sensorData[18]);
}

void GARUDA::updatePayload1Label(QStringList sensorData)
{
    // 21,23,24,25,p(23)
    ui->SP1PACKETCOUNT->setText(sensorData[21]);
    ui->SP1ALTITUDE->setText(sensorData[23]);
    ui->SP1TEMPERATURE->setText(sensorData[24]);
    ui->SP1ROTATION->setText(sensorData[25]);
    ui->SP1PRESSURE->setText(QString::number(altitudeToPressure(sensorData[23])));
}

void GARUDA::updatePayload2Label(QStringList sensorData)
{
    //28,30,31,32, p(30)
    ui->SP2PACKETCOUNT->setText(sensorData[28]);
    ui->SP2ALTITUDE->setText(sensorData[30]);
    ui->SP2TEMPERATURE->setText(sensorData[31]);
    ui->SP2ROTATION->setText(sensorData[32]);
    ui->SP2PRESSURE->setText(QString::number(altitudeToPressure(sensorData[30])));

}

void GARUDA::updateLed(QString state)
{
    ui->state1->toggle(0);
    ui->state2->toggle(0);
    ui->state3->toggle(0);
    ui->state4->toggle(0);
    ui->state5a->toggle(0);
    ui->state5b->toggle(0);
    ui->state6->toggle(0);
    int st = state.toInt();
    switch(st) {
    case 1: ui->state1->toggle(1);
        break;
    case 2: ui->state2->toggle(1);
        break;
    case 3: ui->state3->toggle(1);
        break;
    case 4: ui->state4->toggle(1);
        break;
    case 5: ui->state5a->toggle(1);
        break;
    case 6: ui->state5b->toggle(1);
        break;
    case 7: ui->state6->toggle(1);
        break;
    }
}

void GARUDA::updateBar(bar *b, QString val)
{
    int value = val.toInt();
    b->getpos(value);
}

void GARUDA::convertTelemetry()
{
    long time;
    if(sensorData.length() >= 2) {
        time = sensorData[1].toLong();
        time = time % (24*60*60);
        sensorData[1] = QString::number(time / 3600) + ":" + QString::number((time % 3600) / 60) + ":" + QString::number((time % 3600) % 60);
    }

    if(sensorData.length() >= 11) {
        time = sensorData[10].toLong();
        time = time % (24*60*60);
        sensorData[10] = QString::number(time / 3600) + ":" + QString::number((time % 3600) / 60) + ":" + QString::number((time % 3600) % 60);
    }

    if(sensorData.length() >= 21) {
        time = sensorData[20].toLong();
        time = time % (24*60*60);
        sensorData[20] = QString::number(time / 3600) + ":" + QString::number((time % 3600) / 60) + ":" + QString::number((time % 3600) % 60);
    }

    if(sensorData.length() >= 28) {
        time = sensorData[27].toLong();
        time = time % (24*60*60);
        sensorData[27] = QString::number(time / 3600) + ":" + QString::number((time % 3600) / 60) + ":" + QString::number((time % 3600) % 60);
    }
}

void GARUDA::writeCSV(QString telemetry)
{
    if(file.open(QFile::WriteOnly | QFile::Append)) {
        QTextStream stream(&file);
        stream << telemetry << "\n";
    }
    file.close();
}

bool GARUDA::checkArduinoConnection()
{
    bool arduino_is_available = false;
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        //  check if the serialport has both a product identifier and a vendor identifier
        if(serialPortInfo.hasProductIdentifier() && serialPortInfo.hasVendorIdentifier()){
            //  check if the product ID and the vendor ID match those of the arduino uno
            if((serialPortInfo.productIdentifier() ==  arduino_uno_product_id)
                    && (serialPortInfo.vendorIdentifier() == arduino_uno_vendor_id)){
                arduino_is_available = true; //    arduino uno is available on this port
                arduino_uno_port_name = serialPortInfo.portName();
            }

        }
    }
    return arduino_is_available;
}

void GARUDA::configureSerial()
{
    if(checkArduinoConnection()) {
        qDebug() << "Found the arduino port...\n";
        arduino->setPortName(arduino_uno_port_name);
        arduino->open(QSerialPort::ReadWrite);
        arduino->setBaudRate(QSerialPort::Baud9600);
        arduino->setDataBits(QSerialPort::Data8);
        arduino->setFlowControl(QSerialPort::NoFlowControl);
        arduino->setParity(QSerialPort::NoParity);
        arduino->setStopBits(QSerialPort::OneStop);
        arduino->setReadBufferSize(5000);

    }
    else {
        QMessageBox::information(this, "Serial Port Error", "Couldn't open serial port to arduino.");
    }
}

void GARUDA::connectSerial()
{
    QObject::connect(arduino, SIGNAL(readyRead()), this, SLOT(readSerial()));
}

void GARUDA::disconnectSerial()
{
    QObject::disconnect(arduino, SIGNAL(readyRead()), this, SLOT(readSerial()));
}

void GARUDA::configureMqtt()
{
    m_client->setProtocolVersion(QMqttClient::MQTT_3_1_1);
    m_client->setPort(1883);
    m_client->setHostname("cansat.info");
    m_client->setClientId("Team G.A.R.U.D.A");
    m_client->setUsername("3394");
    m_client->setPassword("Jeorgifa191!");
}

void GARUDA::connectMqtt()
{
    m_client->connectToHost();
}

void GARUDA::publishMqtt(QStringList sensorData)
{
    QString container = "";
    for(int i=0; i<18; i++)
        container += sensorData[i] + ",";
    container += sensorData[18];

    QString payload1 = "";
    for(int i = 19; i<26; i++)
        payload1 += sensorData[i] + ",";
    payload1 += ",,,,,,,,,,,";

    QString payload2 = "";

    for(int i=26; i<33; i++)
        payload2 += sensorData[i] + ",";
    payload2 += ",,,,,,,,,,,";

    m_client->publish(topic, container.toUtf8());

    if(sensorData[19] != "0")
        m_client->publish(topic, payload1.toUtf8());
    if(sensorData[26] != "0")
        m_client->publish(topic, payload2.toUtf8());

}

void GARUDA::subscribeMqtt()
{
    if(m_client->state() == 2) {
        QMqttTopicFilter sub = QLatin1String(topic.toLatin1());
        auto subcheck = m_client->subscribe(sub);
        connect(m_client, &QMqttClient::messageReceived, this, &GARUDA::getSimCSV);
    }
}

void GARUDA::writeSerial(QString cmd)
{
    if(CX) {
        arduino->write(cmd.toLocal8Bit());
    }
}

double GARUDA::altitudeToPressure(QString altitude)
{
    pressure = 101325*qPow((1 - (altitude.toDouble()/44330)), 5.33);
    return pressure;
}

//void GARUDA::readSampleCSV()
//{
//    QFile d("C:/Users/vivas/Downloads/TEAM_GARUDA_GCS-20210328T065343Z-001/TEAM_GARUDA_GCS/sampleDataFile.csv");
//    qDebug() << "hello";


//    if(d.open(QFile::ReadOnly)) {
//        while(!d.atEnd()) {
//            QString s = d.readLine();
//            plotTest.append(s);
//        }
//    }
//    timer = new QTimer(this);
//    connect(timer, SIGNAL(timeout()), this, SLOT(plotSec()));
//    timer->start(1000);
//}

QString c = "1";
void GARUDA::readSerial()
{
    QStringList buffer = serialBuffer.split("\r\n");

    // check if complete telemetry is recieved
    if(buffer.length() > 1) {
        // clearing buffer;
        serialBuffer = "";
        telemetry = buffer[0];
        telemetry = telemetry.replace("><", ",").replace("<","").replace(">","");
        sensorData = telemetry.split(",");

        //convert telemetry to desired format
        convertTelemetry();
        telemetry = sensorData.join(',');
        ui->telemetryDisplay->setPlainText(telemetry);

        // telemetry recieved from 1 container + 2 payloads
        if(sensorData.length() == 33 && sensorData[0].length() > 0) {
            // telemetry is complete and without loss

            // check for paylaod release commands
            checkTelemetry();

            // publish telemetry to mqtt
            if(startMqtt)
                publishMqtt(sensorData);


            updateGraphArray(sensorData);

            //plot telemetry on graphs
            plot();

            //show data on labels;
            updateContainerLabel(sensorData);

            // update map
            locator.setData(sensorData[11], sensorData[12]);

            updateLed(sensorData[15]);

            updatePayload1Label(sensorData);
            updatePayload2Label(sensorData);

            ui->sp1Pressurebar->getpos(altitudeToPressure(sensorData[23]));
            ui->sp2Pressurebar->getpos(altitudeToPressure(sensorData[30]));

            // write data to csv file
            writeCSV(telemetry);

        }
    }

    serial = arduino->readAll();
    serialBuffer += serial;

}

void GARUDA::getSimCSV(const QByteArray &message, const QMqttTopicName &topic)
{
    QFile f("./Simp.csv");

    if(f.open(QFile::WriteOnly) && getCsv) {
        QTextStream stream(&f);
        qDebug() << message;
        stream << message;
        getCsv = false;
        ui->telemetryDisplay->setText("SIMPLATED PRESSURE FILE RECIEVED!");
    }
    f.close();
}




void GARUDA::on_SETTIME_clicked()
{
    QDateTime date_time = QDateTime::currentDateTimeUtc();
    QString time = "<CMD,3394,ST,";
    time += date_time.time().toString();
    time += ">";
    writeSerial(time);
}


void GARUDA::on_SIMENABLE_clicked()
{
    QString string = "<CMD,3394,SIM,ENABLE>";
    writeSerial(string);
    simEnable = true;
}

void GARUDA::on_SIMDISABLE_clicked()
{
    QString string = "<CMD,3394,SIM,DISABLE>";
    writeSerial(string);
    simEnable = false;
    simActivate = false;
}

void GARUDA::on_SIMACTIVATE_clicked()
{
    QString string = "<CMD,3394,SIM,ACTIVATE>";
    writeSerial(string);
    if(simEnable && !simActivate) {
        simActivate = true;
        qDebug() << "sub";
        getCsv = true;
    }
}

void GARUDA::on_SIMP_clicked()
{
    QFile simpFile("./simp.csv");

    if(!simpFile.open(QIODevice::ReadOnly))
        qDebug() << "cound not open file";
    else {
        while(!simpFile.atEnd()) {
            QString data = simpFile.readLine();
//            if(data[0] == 'C' && data[1] == 'M' && data[2] == 'D' && data[3] == ',')
                simpList.append(data);
        }
        simpFile.close();
//        qDebug() << simpList;

        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(sendSimp()));
        timer->start(1000);
    }
}


void GARUDA::sendSimp()
{
    if(simpIndex < simpList.size()) {
        QString simpstr = simpList[simpIndex++];
        simpstr = "<" + simpstr + ">";
        qDebug() << simpstr;
        writeSerial(simpstr);
    }
}
int loop1 = 0;
//void GARUDA::plotSec()
//{
//    QStringList a = plotTest[loop1++].split("\r\n");
//    qDebug() << a[0];
//    QStringList test = a[0].split(",");
//    ui->telemetryDisplay->setPlainText(a[0]);
//    publishMqtt(a[0]);
//    updateGraphArray(test);

//    //plot telemetry on graphs
//    plot();

//    //show data on labels;
//    updateContainerLabel(test);

//    // update map
//    locator.setData(test[11], test[12]);

//    updateLed(test[15]);

//    updatePayload1Label(test);
//    updatePayload2Label(test);

//    ui->sp1Pressurebar->getpos(altitudeToPressure(test[26]));
//    ui->sp2Pressurebar->getpos(altitudeToPressure(test[33]));

//}


void GARUDA::on_CX_clicked()
{
    if(!CX) {
        ui->CX->setText("CX OFF");
        CX = true;

        QString string = "<CMD,3394,CX,ON>";
        writeSerial(string);


        if (file.open(QFile::WriteOnly|QFile::Truncate)) {
            QTextStream stream(&file);
            stream << "<TEAM_ID>,<MISSION_TIME>,<PACKET_COUNT>,<PACKET_TYPE>,<MODE>,<SP1_RELEASED>,<SP2_RELEASED>,<ALTITUDE>,<TEMP>,<VOLTAGE>,<GPS_TIME>,<GPS_LATITUDE>,<GPS_LONGITUDE>,<GPS_ALTITUDE>,<GPS_SATS>,<SOFTWARE_STATE>,<SP1_PACKET_COUNT>,<SP2_PACKET_COUNT>,<CMD_ECHO>,<TEAM_ID>,<MISSION_TIME>,<PACKET_COUNT>,<PACKET_TYPE>,<SP_ALTITUDE>,<SP_TEMP>,<SP_ROTATION_RATE>,<TEAM_ID>,<MISSION_TIME>,<PACKET_COUNT>,<PACKET_TYPE>,<SP_ALTITUDE>,<SP_TEMP>,<SP_ROTATION_RATE>" << "\n";
        }
        file.close();
    }
    else {
        ui->CX->setText("CX ON");
        CX = false;
        QString string = "<CMD,3394,CX,OFF>";
        writeSerial(string);
        disconnectSerial();
    }
}

void GARUDA::on_MQTT_clicked()
{
    if(!startMqtt) {
        ui->MQTT->setText("DISABLE MQTT");
        startMqtt = true;
    }
    else {
        ui->MQTT->setText("ENABLE MQTT");
        startMqtt = false;
    }
}

void GARUDA::on_SP1X_clicked()
{
    if(!SP1X) {
        ui->SP1X->setText("SP1X OFF");
        QString string = "<CMD,3394,SP1X,ON>";
        writeSerial(string);
        SP1X = true;
    }
    else {
        ui->SP1X->setText("SP1X ON");
        QString string = "<CMD,3394,SP1X,OFF>";
        writeSerial(string);
        SP1X = false;
    }
}

void GARUDA::on_SP2X_clicked()
{
    if(!SP2X) {
        ui->SP2X->setText("SP2X OFF");
        QString string = "<CMD,3394,SP2X,ON>";
        writeSerial(string);
        SP2X = true;
    }
    else {
        ui->SP2X->setText("SP2X ON");
        QString string = "<CMD,3394,SP2X,OFF>";
        writeSerial(string);
        SP2X = false;
    }
}

void GARUDA::on_CALIBRATION_clicked()
{
    QString string = "<CMD,3394,CAL,ON>";
    writeSerial(string);
}

void GARUDA::on_SP2X_Rel_clicked()
{
    qDebug()<<"sp2 released";
    QString string = "<CMD,3394,SP2X,REL>";
    writeSerial(string);
}

void GARUDA::on_SP1X_Rel_clicked()
{
    qDebug()<<"sp1 released";
    QString string = "<CMD,3394,SP1X,REL>";
    writeSerial(string);
}
