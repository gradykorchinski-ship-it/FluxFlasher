#include "settingsdialog.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    setWindowTitle("Settings");
    setModal(true);
    setFixedSize(400, 250);
}

void SettingsDialog::setupUI() {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(15);
    
    m_reportErrorsCheck = new QCheckBox("Anonymously report errors", this);
    layout->addWidget(m_reportErrorsCheck);
    
    m_trimSpaceCheck = new QCheckBox("Trim unallocated space", this);
    layout->addWidget(m_trimSpaceCheck);
    
    layout->addSpacing(20);
    
    QLabel* versionLabel = new QLabel("⚡ v0.1.0", this);
    versionLabel->setOpenExternalLinks(true);
    versionLabel->setTextFormat(Qt::RichText);
    versionLabel->setText("<a href='https://github.com/gradykorchinski-ship-it/fluxflasher'>⚡ v0.1.0</a>");
    layout->addWidget(versionLabel);
    
    layout->addStretch();
    
    QPushButton* okButton = new QPushButton("OK", this);
    okButton->setMinimumSize(150, 40);
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addStretch();
    
    layout->addLayout(buttonLayout);
}

bool SettingsDialog::reportErrors() const {
    return m_reportErrorsCheck->isChecked();
}

bool SettingsDialog::trimSpace() const {
    return m_trimSpaceCheck->isChecked();
}

void SettingsDialog::setReportErrors(bool enabled) {
    m_reportErrorsCheck->setChecked(enabled);
}

void SettingsDialog::setTrimSpace(bool enabled) {
    m_trimSpaceCheck->setChecked(enabled);
}
