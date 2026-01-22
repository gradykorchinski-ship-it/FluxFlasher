#include "stepcard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

// Color constants matching Rust version
static const QString BG_DARK = "#2F3235";
static const QString BG_MEDIUM = "#474B4F";
static const QString CARD_BG = "#3C3F43";
static const QString ACCENT_CYAN = "#00AEEF";
static const QString SUCCESS_GREEN = "#4CAF50";
static const QString DISABLED_GREY = "#64646E";
static const QString TEXT_WHITE = "#FFFFFF";
static const QString TEXT_GREY = "#A0A0AA";

StepCard::StepCard(int stepNumber, const QString& title, QWidget *parent)
    : QWidget(parent), m_stepNumber(stepNumber), m_title(title), m_isComplete(false)
{
    setupUI();
    setFixedSize(280, 200);
}

void StepCard::setupUI() {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(10);
    layout->setContentsMargins(20, 20, 20, 20);
    
    // Step number
    m_numberLabel = new QLabel(QString::number(m_stepNumber), this);
    m_numberLabel->setAlignment(Qt::AlignCenter);
    m_numberLabel->setStyleSheet(QString("font-size: 32px; font-weight: bold; color: %1;").arg(TEXT_GREY));
    layout->addWidget(m_numberLabel);
    
    // Title
    m_titleLabel = new QLabel(m_title, this);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet(QString("font-size: 16px; color: %1;").arg(TEXT_WHITE));
    layout->addWidget(m_titleLabel);
    
    layout->addSpacing(15);
    
    // Info label
    m_infoLabel = new QLabel(this);
    m_infoLabel->setAlignment(Qt::AlignCenter);
    m_infoLabel->setStyleSheet(QString("font-size: 14px; color: %1;").arg(TEXT_WHITE));
    m_infoLabel->setWordWrap(true);
    m_infoLabel->hide();
    layout->addWidget(m_infoLabel);
    
    // Sub info label
    m_subInfoLabel = new QLabel(this);
    m_subInfoLabel->setAlignment(Qt::AlignCenter);
    m_subInfoLabel->setStyleSheet(QString("font-size: 12px; color: %1;").arg(TEXT_GREY));
    m_subInfoLabel->hide();
    layout->addWidget(m_subInfoLabel);
    
    // Button
    m_button = new QPushButton(this);
    m_button->setMinimumSize(180, 40);
    m_button->setCursor(Qt::PointingHandCursor);
    connect(m_button, &QPushButton::clicked, this, &StepCard::buttonClicked);
    layout->addWidget(m_button, 0, Qt::AlignCenter);
    
    // Status label
    m_statusLabel = new QLabel(this);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setStyleSheet(QString("font-size: 12px; color: %1;").arg(SUCCESS_GREEN));
    m_statusLabel->hide();
    layout->addWidget(m_statusLabel);
    
    updateStyle();
}

void StepCard::setComplete(bool complete) {
    m_isComplete = complete;
    updateStyle();
    
    if (complete) {
        m_statusLabel->setText("✓ Ready");
        m_statusLabel->show();
    } else {
        m_statusLabel->hide();
    }
}

void StepCard::setInfo(const QString& info) {
    m_infoLabel->setText(info);
    m_infoLabel->setVisible(!info.isEmpty());
}

void StepCard::setSubInfo(const QString& subInfo) {
    m_subInfoLabel->setText(subInfo);
    m_subInfoLabel->setVisible(!subInfo.isEmpty());
}

void StepCard::setButtonText(const QString& text) {
    m_button->setText(text);
}

void StepCard::setButtonEnabled(bool enabled) {
    m_button->setEnabled(enabled);
    updateStyle();
}

void StepCard::updateStyle() {
    QString borderColor = m_isComplete ? SUCCESS_GREEN : DISABLED_GREY;
    
    setStyleSheet(QString(
        "StepCard {"
        "    background-color: %1;"
        "    border: 2px solid %2;"
        "    border-radius: 8px;"
        "}"
    ).arg(CARD_BG, borderColor));
    
    QString buttonBg = m_button->isEnabled() ? ACCENT_CYAN : DISABLED_GREY;
    m_button->setStyleSheet(QString(
        "QPushButton {"
        "    background-color: %1;"
        "    color: %2;"
        "    border: none;"
        "    border-radius: 20px;"
        "    font-size: 14px;"
        "    padding: 10px 20px;"
        "}"
        "QPushButton:hover {"
        "    background-color: %3;"
        "}"
        "QPushButton:disabled {"
        "    background-color: %4;"
        "}"
    ).arg(buttonBg, TEXT_WHITE, "#00C0FF", DISABLED_GREY));
}
