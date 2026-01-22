#ifndef DEVICEDIALOG_H
#define DEVICEDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QVector>
#include "../core_interface.h"

class DeviceDialog : public QDialog {
    Q_OBJECT

public:
    explicit DeviceDialog(QWidget *parent = nullptr);
    
    void setDevices(const QVector<UsbDeviceInfo>& devices);
    void setSelectedIndex(int index);
    int selectedIndex() const { return m_selectedIndex; }

signals:
    void deviceSelected(int index);

private slots:
    void onItemClicked(QListWidgetItem* item);
    void onSelectClicked();
    void onRefreshClicked();

private:
    void setupUI();
    
    QListWidget* m_listWidget;
    QPushButton* m_selectButton;
    QPushButton* m_refreshButton;
    QVector<UsbDeviceInfo> m_devices;
    int m_selectedIndex;
};

#endif // DEVICEDIALOG_H
