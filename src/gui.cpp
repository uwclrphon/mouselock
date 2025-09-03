#include "gui.h"
#include "../include/json_utils/json_utils.h"
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
    , jsonManager(new JsonManager(new GuiDebugLogger()))
{
    ui->setupUi(this);
    logDebug("UI setup completed");
    
    this->setWindowTitle("设置");

    // 连接checkbox信号
    connect(ui->checkBox, &QCheckBox::checkStateChanged, this, [this](Qt::CheckState state) {
        bool enabled = state == Qt::Checked;
        ui->lineEdit->setEnabled(enabled);
    });
    
    connect(ui->checkBox_2, &QCheckBox::checkStateChanged, this, [this](Qt::CheckState state) {
        bool enabled = state == Qt::Checked;
        ui->lineEdit_2->setEnabled(enabled);
    });
    
    loadFromJson();
    logDebug("Current values after load: " + ui->lineEdit->text() + " " + ui->lineEdit_2->text());
    connect(ui->pushButton, &QPushButton::clicked, this, &qt_std1::saveToJson);
}

qt_std1::~qt_std1()
{
    delete ui;
    delete jsonManager;
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
    std::string configPath = (QDir::currentPath() + "/global_config.json").toStdString();
    logDebug("Config file path: " + QString::fromStdString(configPath));

    try {
        auto config = jsonManager->readJson(configPath);
        
        // 设置鼠标锁定时间
        if (config.count("mouse_lock_time")) {
            std::string value = config["mouse_lock_time"];
            logDebug("mouse_lock_time value: " + QString::fromStdString(value));
            ui->lineEdit->setText(QString::fromStdString(value));
        } else {
            logDebug("mouse_lock_time key not found, using default");
            ui->lineEdit->setText("60");
        }

        // 设置鼠标锁定启用状态
        if (config.count("mouse_lock_enabled")) {
            bool enabled = config["mouse_lock_enabled"] == "true";
            ui->checkBox->setChecked(enabled);
            ui->lineEdit->setEnabled(enabled);
        }

        // 设置蓝屏时间
        if (config.count("blue_screen_time")) {
            std::string value = config["blue_screen_time"];
            logDebug("blue_screen_time value: " + QString::fromStdString(value));
            ui->lineEdit_2->setText(QString::fromStdString(value));
        } else {
            logDebug("blue_screen_time key not found, using default");
            ui->lineEdit_2->setText("120");
        }

        // 设置蓝屏启用状态
        if (config.count("blue_screen_enabled")) {
            bool enabled = config["blue_screen_enabled"] == "true";
            ui->checkBox_2->setChecked(enabled);
            ui->lineEdit_2->setEnabled(enabled);
        }
    } catch (const std::exception& e) {
        logDebug("Error loading config: " + QString(e.what()));
        logDebug("Using default values");
        ui->lineEdit->setText("60");
        ui->lineEdit_2->setText("120");
        ui->checkBox->setChecked(true);
        ui->checkBox_2->setChecked(true);
    }
}

void qt_std1::saveToJson()
{
    std::map<std::string, std::string> config;
    config["mouse_lock_time"] = ui->lineEdit->text().toStdString();
    config["blue_screen_time"] = ui->lineEdit_2->text().toStdString();
    config["mouse_lock_enabled"] = ui->checkBox->isChecked() ? "true" : "false";
    config["blue_screen_enabled"] = ui->checkBox_2->isChecked() ? "true" : "false";

    std::string configPath = (QDir::currentPath() + "/global_config.json").toStdString();
    logDebug("Saving config to: " + QString::fromStdString(configPath));

    try {
        jsonManager->writeJson(configPath, config);
        
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
    } catch (const std::exception& e) {
        logDebug("Error saving config: " + QString(e.what()));
        QMessageBox::critical(this, "错误", "保存配置失败: " + QString(e.what()));
    }
}