#include "core_interface.h"
#include <QTimer>
#include <QDebug>

// FlashOperation implementation
FlashOperation::FlashOperation(const QString& imagePath, const QString& devicePath, QObject* parent)
    : QObject(parent), m_operation(nullptr), m_wasRunning(false)
{
    QByteArray imagePathBytes = imagePath.toUtf8();
    QByteArray devicePathBytes = devicePath.toUtf8();
    
    m_operation = flux_start_flash(imagePathBytes.constData(), devicePathBytes.constData());
    
    if (m_operation) {
        m_wasRunning = true;
        m_timer = new QTimer(this);
        connect(m_timer, &QTimer::timeout, this, &FlashOperation::checkStatus);
        m_timer->start(100); // Check every 100ms
    }
}

FlashOperation::~FlashOperation() {
    if (m_operation) {
        flux_free_operation(m_operation);
    }
}

float FlashOperation::getProgress() const {
    if (!m_operation) return 0.0f;
    return flux_get_progress(m_operation);
}

float FlashOperation::getVerifyProgress() const {
    if (!m_operation) return 0.0f;
    return flux_get_verify_progress(m_operation);
}

QString FlashOperation::getStatus() const {
    if (!m_operation) return QString();
    char* status = flux_get_status(m_operation);
    if (!status) return QString();
    QString result = QString::fromUtf8(status);
    flux_free_string(status);
    return result;
}

quint64 FlashOperation::getBytesWritten() const {
    if (!m_operation) return 0;
    return flux_get_bytes_written(m_operation);
}

bool FlashOperation::isRunning() const {
    if (!m_operation) return false;
    return flux_is_running(m_operation);
}

QString FlashOperation::getError() const {
    if (!m_operation) return QString();
    char* error = flux_get_error(m_operation);
    if (!error) return QString();
    QString result = QString::fromUtf8(error);
    flux_free_string(error);
    return result;
}

bool FlashOperation::hasError() const {
    return !getError().isEmpty();
}

void FlashOperation::checkStatus() {
    if (!m_operation) return;
    
    emit progressChanged(getProgress());
    emit statusChanged(getStatus());
    
    bool running = isRunning();
    
    if (m_wasRunning && !running) {
        m_timer->stop();
        
        if (hasError()) {
            emit error(getError());
        } else {
            emit completed();
        }
    }
    
    m_wasRunning = running;
}

// CoreInterface implementation
CoreInterface::CoreInterface() : m_initialized(false) {}

CoreInterface::~CoreInterface() {
    cleanup();
}

CoreInterface& CoreInterface::instance() {
    static CoreInterface instance;
    return instance;
}

bool CoreInterface::initialize() {
    if (m_initialized) return true;
    
    int result = flux_init();
    m_initialized = (result == 0);
    return m_initialized;
}

void CoreInterface::cleanup() {
    if (m_initialized) {
        flux_cleanup();
        m_initialized = false;
    }
}

QVector<UsbDeviceInfo> CoreInterface::listDevices() {
    QVector<UsbDeviceInfo> devices;
    
    CDeviceList* list = flux_list_devices();
    if (!list) return devices;
    
    for (size_t i = 0; i < list->count; ++i) {
        CUsbDevice& cdev = list->devices[i];
        
        UsbDeviceInfo dev;
        dev.path = QString::fromUtf8(cdev.path);
        dev.size = QString::fromUtf8(cdev.size);
        dev.sizeBytes = cdev.size_bytes;
        dev.removable = cdev.removable;
        dev.mounted = cdev.mounted;
        dev.isSystemDisk = cdev.is_system_disk;
        
        for (size_t j = 0; j < cdev.mount_point_count; ++j) {
            dev.mountPoints.append(QString::fromUtf8(cdev.mount_points[j]));
        }
        
        devices.append(dev);
    }
    
    flux_free_device_list(list);
    return devices;
}

FlashOperation* CoreInterface::startFlash(const QString& imagePath, const QString& devicePath) {
    return new FlashOperation(imagePath, devicePath);
}

QString CoreInterface::formatSize(quint64 bytes) {
    char* formatted = flux_format_size(bytes);
    if (!formatted) return QString();
    QString result = QString::fromUtf8(formatted);
    flux_free_string(formatted);
    return result;
}

QString CoreInterface::formatDuration(quint64 seconds) {
    char* formatted = flux_format_duration(seconds);
    if (!formatted) return QString();
    QString result = QString::fromUtf8(formatted);
    flux_free_string(formatted);
    return result;
}
