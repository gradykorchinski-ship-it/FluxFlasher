#ifndef CORE_INTERFACE_H
#define CORE_INTERFACE_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QTimer>
#include <memory>

extern "C" {
    #include "fluxflasher.h"
}

// C++ wrapper for USB device
struct UsbDeviceInfo {
    QString path;
    QString size;
    quint64 sizeBytes;
    bool removable;
    bool mounted;
    bool isSystemDisk;
    QVector<QString> mountPoints;
};

// C++ wrapper for flash operations
class FlashOperation : public QObject {
    Q_OBJECT

public:
    explicit FlashOperation(const QString& imagePath, const QString& devicePath, QObject* parent = nullptr);
    ~FlashOperation();

    float getProgress() const;
    float getVerifyProgress() const;
    QString getStatus() const;
    quint64 getBytesWritten() const;
    bool isRunning() const;
    QString getError() const;
    bool hasError() const;

signals:
    void progressChanged(float progress);
    void statusChanged(const QString& status);
    void completed();
    void error(const QString& message);

private:
    CFlashOperation* m_operation;
    void checkStatus();
    QTimer* m_timer;
    bool m_wasRunning;
};

// Core interface singleton
class CoreInterface : public QObject {
    Q_OBJECT

public:
    static CoreInterface& instance();
    
    bool initialize();
    void cleanup();
    
    QVector<UsbDeviceInfo> listDevices();
    FlashOperation* startFlash(const QString& imagePath, const QString& devicePath);
    
    QString formatSize(quint64 bytes);
    QString formatDuration(quint64 seconds);

private:
    CoreInterface();
    ~CoreInterface();
    CoreInterface(const CoreInterface&) = delete;
    CoreInterface& operator=(const CoreInterface&) = delete;
    
    bool m_initialized;
};

#endif // CORE_INTERFACE_H
