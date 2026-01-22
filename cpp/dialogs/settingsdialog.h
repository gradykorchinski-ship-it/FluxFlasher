#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QCheckBox>

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    
    bool reportErrors() const;
    bool trimSpace() const;
    void setReportErrors(bool enabled);
    void setTrimSpace(bool enabled);

private:
    void setupUI();
    
    QCheckBox* m_reportErrorsCheck;
    QCheckBox* m_trimSpaceCheck;
};

#endif // SETTINGSDIALOG_H
