#include "devicedialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

static const QString ACCENT_CYAN = "#00AEEF";
static const QString TEXT_WHITE = "#FFFFFF";
static const QString TEXT_GREY = "#A0A0AA";

DeviceDialog::DeviceDialog(QWidget *parent)
    : QDialog(parent), m_selectedIndex(-1)
{
    setupUI();
    setWindowTitle("Select Target USB");
    setModal(true);
    resize(600, 400);
}

void DeviceDialog::setupUI() {
    QVBoxLayout* layout = new QVBoxLayout(this);
    
    // List widget
    m_listWidget = new QListWidget(this);
    m_listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(m_listWidget, &QListWidget::itemClicked, this, &DeviceDialog::onItemClicked);
    layout->addWidget(m_listWidget);
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    m_refreshButton = new QPushButton("Refresh", this);
    connect(m_refreshButton, &QPushButton::clicked, this, &DeviceDialog::onRefreshClicked);
    buttonLayout->addWidget(m_refreshButton);
    
    QPushButton* cancelButton = new QPushButton("Cancel", this);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(cancelButton);
    
    m_selectButton = new QPushButton("Select", this);
    m_selectButton->setEnabled(false);
    connect(m_selectButton, &QPushButton::clicked, this, &DeviceDialog::onSelectClicked);
    buttonLayout->addWidget(m_selectButton);
    
    layout->addLayout(buttonLayout);
}

void DeviceDialog::setDevices(const QVector<UsbDeviceInfo>& devices) {
    m_devices = devices;
    m_listWidget->clear();
    
    if (devices.isEmpty()) {
        QListWidgetItem* item = new QListWidgetItem("No removable USB devices found.");
        item->setFlags(Qt::NoItemFlags);
        m_listWidget->addItem(item);
        return;
    }
    
    for (int i = 0; i < devices.size(); ++i) {
        const UsbDeviceInfo& dev = devices[i];
        
        QString text = QString("%1 - %2").arg(dev.path, dev.size);
        if (dev.isSystemDisk) {
            text += " (System Disk - Protected)";
        } else if (dev.mounted) {
            text += " (Mounted)";
        }
        
        QListWidgetItem* item = new QListWidgetItem(text);
        item->setData(Qt::UserRole, i);
        
        if (dev.isSystemDisk) {
            item->setFlags(Qt::NoItemFlags);
            item->setForeground(QColor("#FF4500"));
        }
        
        m_listWidget->addItem(item);
    }
}

void DeviceDialog::setSelectedIndex(int index) {
    m_selectedIndex = index;
    
    for (int i = 0; i < m_listWidget->count(); ++i) {
        QListWidgetItem* item = m_listWidget->item(i);
        if (item->data(Qt::UserRole).toInt() == index) {
            m_listWidget->setCurrentItem(item);
            break;
        }
    }
}

void DeviceDialog::onItemClicked(QListWidgetItem* item) {
    if (item->flags() & Qt::ItemIsEnabled) {
        m_selectedIndex = item->data(Qt::UserRole).toInt();
        m_selectButton->setEnabled(true);
    }
}

void DeviceDialog::onSelectClicked() {
    if (m_selectedIndex >= 0) {
        emit deviceSelected(m_selectedIndex);
        accept();
    }
}

void DeviceDialog::onRefreshClicked() {
    emit deviceSelected(-2); // Signal to refresh
}
