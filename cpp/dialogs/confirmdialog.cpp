#include "confirmdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

static const QString ACCENT_CYAN = "#00AEEF";
static const QString TEXT_WHITE = "#FFFFFF";
static const QString TEXT_GREY = "#A0A0AA";

ConfirmDialog::ConfirmDialog(const QString& deviceName, QWidget *parent)
    : QDialog(parent)
{
    setupUI(deviceName);
    setWindowTitle("Confirm Flash");
    setModal(true);
    setFixedSize(450, 300);
}

void ConfirmDialog::setupUI(const QString& deviceName) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(15);
    
    QLabel* warningLabel = new QLabel("⚠ WARNING: DATA DESTRUCTION", this);
    warningLabel->setAlignment(Qt::AlignCenter);
    warningLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #FF4500;");
    layout->addWidget(warningLabel);
    
    layout->addSpacing(15);
    
    QLabel* messageLabel = new QLabel("This will permanently erase all data on:", this);
    messageLabel->setAlignment(Qt::AlignCenter);
    messageLabel->setStyleSheet(QString("font-size: 14px; color: %1;").arg(TEXT_WHITE));
    layout->addWidget(messageLabel);
    
    QLabel* deviceLabel = new QLabel(deviceName, this);
    deviceLabel->setAlignment(Qt::AlignCenter);
    deviceLabel->setStyleSheet(QString("font-size: 18px; font-weight: bold; color: %1;").arg(ACCENT_CYAN));
    layout->addWidget(deviceLabel);
    
    layout->addSpacing(20);
    
    QLabel* confirmLabel = new QLabel("Are you absolutely sure you want to proceed?", this);
    confirmLabel->setAlignment(Qt::AlignCenter);
    confirmLabel->setStyleSheet(QString("font-size: 14px; color: %1;").arg(TEXT_GREY));
    layout->addWidget(confirmLabel);
    
    layout->addSpacing(25);
    
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    QPushButton* cancelButton = new QPushButton("Cancel", this);
    cancelButton->setMinimumSize(120, 40);
    cancelButton->setStyleSheet(QString(
        "QPushButton {"
        "    background-color: transparent;"
        "    color: %1;"
        "    border: 1px solid %1;"
        "    border-radius: 20px;"
        "    font-size: 14px;"
        "}"
    ).arg(TEXT_GREY));
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(cancelButton);
    
    buttonLayout->addSpacing(20);
    
    QPushButton* flashButton = new QPushButton("Flash Now!", this);
    flashButton->setMinimumSize(120, 40);
    flashButton->setStyleSheet(QString(
        "QPushButton {"
        "    background-color: #FF4500;"
        "    color: %1;"
        "    border: none;"
        "    border-radius: 20px;"
        "    font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #FF6347;"
        "}"
    ).arg(TEXT_WHITE));
    connect(flashButton, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(flashButton);
    
    buttonLayout->addStretch();
    
    layout->addLayout(buttonLayout);
}
