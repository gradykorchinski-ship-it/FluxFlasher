#include "progressview.h"
#include <QVBoxLayout>

static const QString ACCENT_CYAN = "#00AEEF";
static const QString SUCCESS_GREEN = "#4CAF50";
static const QString TEXT_WHITE = "#FFFFFF";
static const QString TEXT_GREY = "#A0A0AA";

ProgressView::ProgressView(QWidget *parent)
    : QWidget(parent), m_isVerifying(false)
{
    setupUI();
}

void ProgressView::setupUI() {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(20);
    
    // Title (for verification phase)
    m_titleLabel = new QLabel(this);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet(QString("font-size: 32px; font-weight: bold; color: %1;").arg(ACCENT_CYAN));
    m_titleLabel->hide();
    layout->addWidget(m_titleLabel);
    
    // Percentage
    m_percentLabel = new QLabel("0%", this);
    m_percentLabel->setAlignment(Qt::AlignCenter);
    m_percentLabel->setStyleSheet(QString("font-size: 48px; font-weight: bold; color: %1;").arg(TEXT_WHITE));
    layout->addWidget(m_percentLabel);
    
    // Progress bar
    m_progressBar = new QProgressBar(this);
    m_progressBar->setMinimumSize(600, 12);
    m_progressBar->setMaximumSize(600, 12);
    m_progressBar->setTextVisible(false);
    m_progressBar->setStyleSheet(QString(
        "QProgressBar {"
        "    border: none;"
        "    border-radius: 6px;"
        "    background-color: #555;"
        "}"
        "QProgressBar::chunk {"
        "    background-color: %1;"
        "    border-radius: 6px;"
        "}"
    ).arg(ACCENT_CYAN));
    layout->addWidget(m_progressBar, 0, Qt::AlignCenter);
    
    // Speed label
    m_speedLabel = new QLabel(this);
    m_speedLabel->setAlignment(Qt::AlignCenter);
    m_speedLabel->setStyleSheet(QString("font-size: 14px; color: %1;").arg(TEXT_GREY));
    m_speedLabel->hide();
    layout->addWidget(m_speedLabel);
    
    // ETA label
    m_etaLabel = new QLabel(this);
    m_etaLabel->setAlignment(Qt::AlignCenter);
    m_etaLabel->setStyleSheet(QString("font-size: 14px; color: %1;").arg(TEXT_GREY));
    m_etaLabel->hide();
    layout->addWidget(m_etaLabel);
    
    // Status label
    m_statusLabel = new QLabel(this);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setStyleSheet(QString("font-size: 12px; color: %1;").arg(TEXT_GREY));
    layout->addWidget(m_statusLabel);
}

void ProgressView::setProgress(float progress) {
    int percent = static_cast<int>(progress * 100);
    m_percentLabel->setText(QString("%1%").arg(percent));
    m_progressBar->setValue(percent);
}

void ProgressView::setStatus(const QString& status) {
    m_statusLabel->setText(status);
}

void ProgressView::setSpeed(float mbps) {
    if (mbps > 0) {
        m_speedLabel->setText(QString("Speed: %1 MB/s").arg(mbps, 0, 'f', 1));
        m_speedLabel->show();
    } else {
        m_speedLabel->hide();
    }
}

void ProgressView::setETA(const QString& eta) {
    if (!eta.isEmpty()) {
        m_etaLabel->setText(QString("ETA: %1").arg(eta));
        m_etaLabel->show();
    } else {
        m_etaLabel->hide();
    }
}

void ProgressView::setVerifying(bool verifying) {
    m_isVerifying = verifying;
    
    if (verifying) {
        m_titleLabel->setText("Verifying Integrity");
        m_titleLabel->show();
        m_speedLabel->hide();
        m_etaLabel->hide();
        
        m_progressBar->setStyleSheet(QString(
            "QProgressBar {"
            "    border: none;"
            "    border-radius: 6px;"
            "    background-color: #555;"
            "}"
            "QProgressBar::chunk {"
            "    background-color: %1;"
            "    border-radius: 6px;"
            "}"
        ).arg(SUCCESS_GREEN));
    } else {
        m_titleLabel->hide();
        
        m_progressBar->setStyleSheet(QString(
            "QProgressBar {"
            "    border: none;"
            "    border-radius: 6px;"
            "    background-color: #555;"
            "}"
            "QProgressBar::chunk {"
            "    background-color: %1;"
            "    border-radius: 6px;"
            "}"
        ).arg(ACCENT_CYAN));
    }
}
