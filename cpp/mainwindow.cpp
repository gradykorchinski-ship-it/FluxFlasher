#include "mainwindow.h"
#include "widgets/stepcard.h"
#include "widgets/progressview.h"
#include "dialogs/devicedialog.h"
#include "dialogs/settingsdialog.h"
#include "dialogs/confirmdialog.h"
#include "dialogs/messagedialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QFileInfo>
#include <QDateTime>

static const QString BG_DARK = "#2F3235";
static const QString BG_MEDIUM = "#474B4F";
static const QString TEXT_WHITE = "#FFFFFF";
static const QString TEXT_GREY = "#A0A0AA";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_selectedDeviceIndex(-1),
      m_flashOperation(nullptr),
      m_isFlashing(false),
      m_isVerifying(false),
      m_imageSize(0)
{
    setupUI();
    setupConnections();
    
    // Initialize core
    CoreInterface::instance().initialize();
    
    // Load devices
    m_devices = CoreInterface::instance().listDevices();
    
    setWindowTitle("FluxFlasher");
    resize(1000, 600);
    setMinimumSize(900, 550);
}

MainWindow::~MainWindow() {
    if (m_flashOperation) {
        delete m_flashOperation;
    }
    CoreInterface::instance().cleanup();
}

void MainWindow::setupUI() {
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(m_centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Top bar
    m_topBar = new QWidget(this);
    m_topBar->setStyleSheet(QString("background-color: %1;").arg(BG_DARK));
    m_topBar->setFixedHeight(50);
    
    QHBoxLayout* topLayout = new QHBoxLayout(m_topBar);
    topLayout->setContentsMargins(12, 12, 12, 12);
    
    QLabel* titleLabel = new QLabel("⚡ FluxFlasher", m_topBar);
    titleLabel->setStyleSheet(QString("font-size: 18px; font-weight: bold; color: %1;").arg(TEXT_WHITE));
    topLayout->addWidget(titleLabel);
    
    topLayout->addStretch();
    
    QPushButton* settingsButton = new QPushButton("⚙", m_topBar);
    settingsButton->setFixedSize(30, 30);
    settingsButton->setStyleSheet("font-size: 16px; border: none; background: transparent;");
    connect(settingsButton, &QPushButton::clicked, this, &MainWindow::onSettings);
    topLayout->addWidget(settingsButton);
    
    mainLayout->addWidget(m_topBar);
    
    // Content area
    QWidget* contentWidget = new QWidget(this);
    contentWidget->setStyleSheet(QString("background-color: %1;").arg(BG_MEDIUM));
    
    QVBoxLayout* contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(40, 40, 40, 40);
    
    // Step container
    m_stepContainer = new QWidget(contentWidget);
    QHBoxLayout* stepLayout = new QHBoxLayout(m_stepContainer);
    stepLayout->setAlignment(Qt::AlignCenter);
    stepLayout->setSpacing(20);
    
    m_step1Card = new StepCard(1, "Select Image", m_stepContainer);
    m_step1Card->setButtonText("Select Image");
    connect(m_step1Card->button(), &QPushButton::clicked, this, &MainWindow::onSelectImage);
    stepLayout->addWidget(m_step1Card);
    
    QLabel* arrow1 = new QLabel(">", m_stepContainer);
    arrow1->setStyleSheet(QString("font-size: 24px; color: %1;").arg(TEXT_GREY));
    stepLayout->addWidget(arrow1);
    
    m_step2Card = new StepCard(2, "Select Target", m_stepContainer);
    m_step2Card->setButtonText("Select USB");
    connect(m_step2Card->button(), &QPushButton::clicked, this, &MainWindow::onSelectDevice);
    stepLayout->addWidget(m_step2Card);
    
    QLabel* arrow2 = new QLabel(">", m_stepContainer);
    arrow2->setStyleSheet(QString("font-size: 24px; color: %1;").arg(TEXT_GREY));
    stepLayout->addWidget(arrow2);
    
    m_step3Card = new StepCard(3, "Flash!", m_stepContainer);
    m_step3Card->setButtonText("Flash!");
    m_step3Card->setButtonEnabled(false);
    connect(m_step3Card->button(), &QPushButton::clicked, this, &MainWindow::onFlash);
    stepLayout->addWidget(m_step3Card);
    
    contentLayout->addWidget(m_stepContainer, 0, Qt::AlignCenter);
    
    // Progress view (initially hidden)
    m_progressView = new ProgressView(contentWidget);
    m_progressView->hide();
    contentLayout->addWidget(m_progressView, 0, Qt::AlignCenter);
    
    mainLayout->addWidget(contentWidget);
    
    // Create dialogs
    m_deviceDialog = new DeviceDialog(this);
    m_settingsDialog = new SettingsDialog(this);
    m_confirmDialog = nullptr;
    m_completionDialog = nullptr;
    m_errorDialog = nullptr;
}

void MainWindow::setupConnections() {
    connect(m_deviceDialog, &DeviceDialog::deviceSelected, this, &MainWindow::onDeviceSelected);
}

void MainWindow::updateStepCards() {
    // Update step 1
    if (!m_imagePath.isEmpty()) {
        QFileInfo fileInfo(m_imagePath);
        m_step1Card->setInfo(fileInfo.fileName());
        m_step1Card->setSubInfo(CoreInterface::instance().formatSize(m_imageSize));
        m_step1Card->setButtonText("Change");
        m_step1Card->setComplete(true);
    } else {
        m_step1Card->setInfo("");
        m_step1Card->setSubInfo("");
        m_step1Card->setButtonText("Select Image");
        m_step1Card->setComplete(false);
    }
    
    // Update step 2
    if (m_selectedDeviceIndex >= 0 && m_selectedDeviceIndex < m_devices.size()) {
        const UsbDeviceInfo& dev = m_devices[m_selectedDeviceIndex];
        m_step2Card->setInfo(dev.path);
        m_step2Card->setSubInfo(dev.size);
        m_step2Card->setButtonText("Change");
        m_step2Card->setComplete(true);
    } else {
        m_step2Card->setInfo("");
        m_step2Card->setSubInfo("");
        m_step2Card->setButtonText("Select USB");
        m_step2Card->setComplete(false);
    }
    
    // Update step 3
    bool canFlash = !m_imagePath.isEmpty() && m_selectedDeviceIndex >= 0;
    m_step3Card->setButtonEnabled(canFlash && !m_isFlashing);
}

void MainWindow::onSelectImage() {
    QString fileName = QFileDialog::getOpenFileName(this, "Select Disk Image", "", "Disk Images (*.iso *.img);;All Files (*)");
    
    if (!fileName.isEmpty()) {
        m_imagePath = fileName;
        QFileInfo fileInfo(fileName);
        m_imageSize = fileInfo.size();
        updateStepCards();
    }
}

void MainWindow::onSelectDevice() {
    m_devices = CoreInterface::instance().listDevices();
    m_deviceDialog->setDevices(m_devices);
    m_deviceDialog->setSelectedIndex(m_selectedDeviceIndex);
    m_deviceDialog->exec();
}

void MainWindow::onFlash() {
    if (m_imagePath.isEmpty() || m_selectedDeviceIndex < 0) {
        return;
    }
    
    // Check size
    if (m_imageSize > m_devices[m_selectedDeviceIndex].sizeBytes) {
        showError("Target Device Too Small",
                  QString("Image size (%1) is larger than device capacity (%2).")
                  .arg(CoreInterface::instance().formatSize(m_imageSize))
                  .arg(m_devices[m_selectedDeviceIndex].size));
        return;
    }
    
    // Show confirmation
    if (m_confirmDialog) delete m_confirmDialog;
    m_confirmDialog = new ConfirmDialog(m_devices[m_selectedDeviceIndex].path, this);
    connect(m_confirmDialog, &QDialog::accepted, this, &MainWindow::onFlashConfirmed);
    m_confirmDialog->exec();
}

void MainWindow::onSettings() {
    m_settingsDialog->exec();
}

void MainWindow::onDeviceSelected(int index) {
    if (index == -2) {
        // Refresh requested
        m_devices = CoreInterface::instance().listDevices();
        m_deviceDialog->setDevices(m_devices);
        return;
    }
    
    m_selectedDeviceIndex = index;
    updateStepCards();
}

void MainWindow::onFlashConfirmed() {
    m_isFlashing = true;
    m_isVerifying = false;
    m_flashStartTime = QDateTime::currentDateTime();
    
    // Hide step cards, show progress
    m_stepContainer->hide();
    m_progressView->show();
    m_progressView->setProgress(0.0f);
    m_progressView->setStatus("Initializing...");
    m_progressView->setVerifying(false);
    
    // Start flash operation
    m_flashOperation = CoreInterface::instance().startFlash(m_imagePath, m_devices[m_selectedDeviceIndex].path);
    
    connect(m_flashOperation, &FlashOperation::progressChanged, this, &MainWindow::onFlashProgress);
    connect(m_flashOperation, &FlashOperation::statusChanged, this, &MainWindow::onFlashStatus);
    connect(m_flashOperation, &FlashOperation::completed, this, &MainWindow::onFlashCompleted);
    connect(m_flashOperation, &FlashOperation::error, this, &MainWindow::onFlashError);
}

void MainWindow::onFlashProgress(float progress) {
    if (m_isVerifying) {
        m_progressView->setProgress(m_flashOperation->getVerifyProgress());
    } else {
        m_progressView->setProgress(progress);
        
        // Calculate speed and ETA
        qint64 elapsed = m_flashStartTime.secsTo(QDateTime::currentDateTime());
        quint64 bytesWritten = m_flashOperation->getBytesWritten();
        
        if (elapsed > 0 && bytesWritten > 0) {
            float speedMBps = (bytesWritten / (float)elapsed) / (1024.0f * 1024.0f);
            m_progressView->setSpeed(speedMBps);
            
            if (m_imageSize > 0 && speedMBps > 0) {
                quint64 remaining = m_imageSize - bytesWritten;
                quint64 etaSecs = (quint64)(remaining / (speedMBps * 1024.0f * 1024.0f));
                m_progressView->setETA(CoreInterface::instance().formatDuration(etaSecs));
            }
        }
    }
}

void MainWindow::onFlashStatus(const QString& status) {
    m_progressView->setStatus(status);
    
    // Check if we entered verification phase
    if (status.contains("Verifying") && !m_isVerifying) {
        m_isVerifying = true;
        m_progressView->setVerifying(true);
        m_progressView->setSpeed(0);
        m_progressView->setETA("");
    }
}

void MainWindow::onFlashCompleted() {
    m_isFlashing = false;
    m_isVerifying = false;
    
    // Show completion dialog
    if (m_completionDialog) delete m_completionDialog;
    m_completionDialog = new MessageDialog(MessageType::Success, "Flash Completed Successfully!", "", this);
    connect(m_completionDialog, &QDialog::accepted, [this]() {
        // Reset state
        m_imagePath.clear();
        m_imageSize = 0;
        m_selectedDeviceIndex = -1;
        updateStepCards();
        
        // Show step cards again
        m_progressView->hide();
        m_stepContainer->show();
    });
    m_completionDialog->exec();
    
    if (m_flashOperation) {
        delete m_flashOperation;
        m_flashOperation = nullptr;
    }
}

void MainWindow::onFlashError(const QString& error) {
    m_isFlashing = false;
    m_isVerifying = false;
    
    showError("Operation Failed", error);
    
    // Show step cards again
    m_progressView->hide();
    m_stepContainer->show();
    
    if (m_flashOperation) {
        delete m_flashOperation;
        m_flashOperation = nullptr;
    }
}

void MainWindow::showError(const QString& title, const QString& message) {
    if (m_errorDialog) delete m_errorDialog;
    m_errorDialog = new MessageDialog(MessageType::Error, title, message, this);
    m_errorDialog->exec();
}
