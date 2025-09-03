#include "../include/json_utils/json_utils.h"
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QDebug>

JsonManager::JsonManager(DebugLogger* logger) : logger_(logger) {}

std::map<std::string, std::string> JsonManager::readJson(const std::string& filePath) {
    QFile file(QString::fromStdString(filePath));
    if (!file.open(QIODevice::ReadOnly)) {
        throw std::runtime_error("Failed to open file: " + filePath);
    }

    QByteArray jsonData = file.readAll();
    file.close();

    return parseJson(jsonData.toStdString());
}

void JsonManager::writeJson(const std::string& filePath, const std::map<std::string, std::string>& data) {
    QFile file(QString::fromStdString(filePath));
    if (!file.open(QIODevice::WriteOnly)) {
        throw std::runtime_error("Failed to open file: " + filePath);
    }

    QJsonObject jsonObj;
    for (const auto& [key, value] : data) {
        jsonObj[QString::fromStdString(key)] = QString::fromStdString(value);
    }

    QJsonDocument doc(jsonObj);
    file.write(doc.toJson());
    file.close();
}

std::map<std::string, std::string> JsonManager::parseConfig(const std::string& jsonStr) {
    return parseJson(jsonStr);
}

std::map<std::string, std::string> JsonManager::parseJson(const std::string& jsonStr) {
    QJsonDocument doc = QJsonDocument::fromJson(QString::fromStdString(jsonStr).toUtf8());
    if (doc.isNull()) {
        throw std::runtime_error("Invalid JSON format");
    }

    QJsonObject obj = doc.object();
    std::map<std::string, std::string> config;

    for (auto it = obj.begin(); it != obj.end(); ++it) {
        QString key = it.key();
        QJsonValue value = it.value();
        config[key.toStdString()] = value.toString().toStdString();
    }

    if (logger_ && logger_->isDebugMode()) {
        qDebug() << "Parsed JSON config:";
        for (const auto& [key, value] : config) {
            qDebug() << QString::fromStdString(key) << ":" << QString::fromStdString(value);
        }
    }

    return config;
}
