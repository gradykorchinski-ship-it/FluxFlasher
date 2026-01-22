#include "messagedialog.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

static const QString ACCENT_CYAN = "#00AEEF";
static const QString SUCCESS_GREEN = "#4CAF50";
static const QString TEXT_WHITE = "#FFFFFF";
static const QString TEXT_GREY = "#A0A0AA";

MessageDialog::MessageDialog(MessageType type, const QString& title, const QString& message, QWidget *parent)
    : QDialog(parent)
{
    setupUI(type, title, message);
    setWindowTitle(title);
    setModal(true);
    setFixedSize(400, 300);
}

void MessageDialog::setupUI(MessageType type, const QString& title, const QString& message) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(15);
    
    // Icon
    QString icon;
    QString iconColor;
    
    switch (type) {
        case MessageType::Success:
            icon = "✓";
            iconColor = SUCCESS_GREEN;
            break;
        case MessageType::Error:
            icon = "❌";
            iconColor = "#FF4500";
            break;
        case MessageType::Info:
            icon = "ℹ";
            iconColor = ACCENT_CYAN;
            break;
    }
    
    QLabel* iconLabel = new QLabel(icon, this);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet(QString("font-size: 64px; color: %1;").arg(iconColor));
    layout->addWidget(iconLabel);
    
    layout->addSpacing(15);
    
    QLabel* titleLabel = new QLabel(title, this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(QString("font-size: 20px; font-weight: bold; color: %1;").arg(TEXT_WHITE));
    titleLabel->setWordWrap(true);
    layout->addWidget(titleLabel);
    
    layout->addSpacing(10);
    
    if (!message.isEmpty()) {
        QLabel* messageLabel = new QLabel(message, this);
        messageLabel->setAlignment(Qt::AlignCenter);
        messageLabel->setStyleSheet(QString("font-size: 12px; color: %1;").arg(TEXT_GREY));
        messageLabel->setWordWrap(true);
        layout->addWidget(messageLabel);
    }
    
    layout->addSpacing(30);
    
    QPushButton* okButton = new QPushButton(type == MessageType::Error ? "Dismiss" : "OK", this);
    okButton->setMinimumSize(150, 45);
    okButton->setStyleSheet(QString(
        "QPushButton {"
        "    background-color: %1;"
        "    color: %2;"
        "    border: none;"
        "    border-radius: 20px;"
        "    font-size: 16px;"
        "}"
        "QPushButton:hover {"
        "    background-color: %3;"
        "}"
    ).arg(ACCENT_CYAN, TEXT_WHITE, "#00C0FF"));
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addStretch();
    
    layout->addLayout(buttonLayout);
    
    layout->addSpacing(10);
}
