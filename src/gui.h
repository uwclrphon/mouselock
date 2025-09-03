#pragma once
#include "ui_gui.h"
#include "../include/json_utils/json_utils.h"
#include <QMainWindow>
#include <QCloseEvent>
#include <QDebug>

class GuiDebugLogger : public DebugLogger {
public:
    bool isDebugMode() const override { return true; }
};

class qt_std1 : public QMainWindow {
    Q_OBJECT
    
public:
    qt_std1(QWidget* parent = nullptr);
    ~qt_std1();

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void saveToJson();
    
private:
    void loadFromJson();
    Ui_qt_std1* ui;
    JsonManager* jsonManager;
};