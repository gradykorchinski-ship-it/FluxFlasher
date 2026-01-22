#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QString>
#include <QVector>
#include <QDateTime>
#include "core_interface.h"

class StepCard;
class ProgressView;
class DeviceDialog;
class SettingsDialog;
class ConfirmDialog;
class MessageDialog;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSelectImage();
    void onSelectDevice();
    void onFlash();
    void onSettings();
    void onDeviceSelected(int index);
    void onFlashConfirmed();
    void onFlashProgress(float progress);
    void onFlashStatus(const QString& status);
    void onFlashCompleted();
    void onFlashError(const QString& error);

private:
    void setupUI();
    void setupConnections();
    void updateStepCards();
    void showError(const QString& title, const QString& message);
    
    // UI Components
    QWidget* m_centralWidget;
    QWidget* m_topBar;
    QWidget* m_stepContainer;
    
    StepCard* m_step1Card;
    StepCard* m_step2Card;
    StepCard* m_step3Card;
    
    ProgressView* m_progressView;
    
    // Dialogs
    DeviceDialog* m_deviceDialog;
    SettingsDialog* m_settingsDialog;
    ConfirmDialog* m_confirmDialog;
    MessageDialog* m_completionDialog;
    MessageDialog* m_errorDialog;
    
    // State
    QString m_imagePath;
    quint64 m_imageSize;
    QVector<UsbDeviceInfo> m_devices;
    int m_selectedDeviceIndex;
    FlashOperation* m_flashOperation;
    bool m_isFlashing;
    bool m_isVerifying;
    QDateTime m_flashStartTime;
};

#endif // MAINWINDOW_H
