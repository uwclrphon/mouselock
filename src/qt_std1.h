#pragma once
#include "ui_qt_std1.h"
#include <QMainWindow>
#include <QCloseEvent>
#include <QDebug>

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
};