#include "qt_std1.h"
#include "../include/json_utils/json_utils.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include <QDir>
#include <QMessageBox>
#include <QProcess>

void logDebug(const QString& message) {
    qDebug() << message;
    QFile logFile("debug_log.txt");
    if (logFile.open(QIODevice::Append)) {
        QTextStream stream(&logFile);
        stream << QDateTime::currentDateTime().toString() << ": " << message << "\n";
        logFile.close();
    }
}

qt_std1::qt_std1(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui_qt_std1)
{
    ui->setupUi(this);
    logDebug("UI setup completed");
    loadFromJson();
    logDebug("Current values after load: " + ui->lineEdit->text() + " " + ui->lineEdit_2->text());
    connect(ui->pushButton, &QPushButton::clicked, this, &qt_std1::saveToJson);
}

qt_std1::~qt_std1()
{
    delete ui; 
}

void qt_std1::closeEvent(QCloseEvent* event)
{
    hide(); // 隐藏窗口而不是关闭
    event->ignore(); // 忽略关闭事件
}

void qt_std1::loadFromJson()
{
    logDebug("Loading from json...");
    logDebug("Current working directory: " + QDir::currentPath());
    QString configPath = QDir::currentPath() + "/global_config.json";
    logDebug("Config file path: " + configPath);
    QFile file(configPath);
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        logDebug("File not found or cannot open, using defaults");
        ui->lineEdit->setText("60"); // 默认锁定鼠标时间
        ui->lineEdit_2->setText("120"); // 默认蓝屏时间
        logDebug("Set default values: " + ui->lineEdit->text() + " " + ui->lineEdit_2->text());
        return;
    }
    logDebug("File opened successfully");

    QByteArray jsonData = file.readAll();
    file.close();

    logDebug("Raw JSON data: " + jsonData);
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull()) {
        logDebug("Failed to parse JSON document");
        return;
    }

    if (!doc.isObject()) {
        logDebug("JSON document is not an object");
        return;
    }

    QJsonObject obj = doc.object();
    logDebug("JSON object keys: " + obj.keys().join(", "));

    if (obj.contains("mouse_lock_time")) {
        QJsonValue val = obj["mouse_lock_time"];
        if (!val.isNull() && !val.isUndefined()) {
            QString strValue = val.isString() ? val.toString() : QString::number(val.toInt());
            bool ok;
            int value = strValue.toInt(&ok);
            if (ok && !strValue.isEmpty()) {
                logDebug("mouse_lock_time value: " + QString::number(value));
                ui->lineEdit->setText(QString::number(value));
            } else {
                logDebug("Invalid mouse_lock_time value: " + strValue);
                ui->lineEdit->setText("60"); // 设置默认值
            }
        } else {
            logDebug("mouse_lock_time value is null or undefined");
            ui->lineEdit->setText("60"); // 设置默认值
        }
    } else {
        logDebug("mouse_lock_time key not found");
        ui->lineEdit->setText("60"); // 设置默认值
    }

    if (obj.contains("blue_screen_time")) {
        QJsonValue val = obj["blue_screen_time"];
        if (!val.isNull() && !val.isUndefined()) {
            QString strValue = val.isString() ? val.toString() : QString::number(val.toInt());
            bool ok;
            int value = strValue.toInt(&ok);
            if (ok && !strValue.isEmpty()) {
                logDebug("blue_screen_time value: " + QString::number(value));
                ui->lineEdit_2->setText(QString::number(value));
            } else {
                logDebug("Invalid blue_screen_time value: " + strValue);
                ui->lineEdit_2->setText("120"); // 设置默认值
            }
        } else {
            logDebug("blue_screen_time value is null or undefined");
            ui->lineEdit_2->setText("120"); // 设置默认值
        }
    } else {
        logDebug("blue_screen_time key not found");
        ui->lineEdit_2->setText("120"); // 设置默认值
    }
}

void qt_std1::saveToJson()
{
    QJsonObject obj;
    obj["mouse_lock_time"] = ui->lineEdit->text().toInt();
    obj["blue_screen_time"] = ui->lineEdit_2->text().toInt();

    QJsonDocument doc(obj);
    QString configPath = QDir::currentPath() + "/global_config.json";
    logDebug("Saving config to: " + configPath);
    QFile file(configPath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
        
        QMessageBox msgBox;
        msgBox.setText("设置已保存");
        msgBox.setInformativeText("需要重启应用使更改生效，是否现在重启？");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();
        
        if (ret == QMessageBox::Yes) {
            qApp->quit();
            QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
        }
    }
}