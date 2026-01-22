#ifndef STEPCARD_H
#define STEPCARD_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QString>

class StepCard : public QWidget {
    Q_OBJECT

public:
    explicit StepCard(int stepNumber, const QString& title, QWidget *parent = nullptr);
    
    void setComplete(bool complete);
    void setInfo(const QString& info);
    void setSubInfo(const QString& subInfo);
    void setButtonText(const QString& text);
    void setButtonEnabled(bool enabled);
    QPushButton* button() const { return m_button; }

signals:
    void buttonClicked();

private:
    void setupUI();
    void updateStyle();
    
    int m_stepNumber;
    QString m_title;
    bool m_isComplete;
    
    QLabel* m_numberLabel;
    QLabel* m_titleLabel;
    QLabel* m_infoLabel;
    QLabel* m_subInfoLabel;
    QLabel* m_statusLabel;
    QPushButton* m_button;
};

#endif // STEPCARD_H
