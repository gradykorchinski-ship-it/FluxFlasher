#ifndef PROGRESSVIEW_H
#define PROGRESSVIEW_H

#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QString>

class ProgressView : public QWidget {
    Q_OBJECT

public:
    explicit ProgressView(QWidget *parent = nullptr);
    
    void setProgress(float progress);
    void setStatus(const QString& status);
    void setSpeed(float mbps);
    void setETA(const QString& eta);
    void setVerifying(bool verifying);

private:
    void setupUI();
    
    QLabel* m_titleLabel;
    QLabel* m_percentLabel;
    QProgressBar* m_progressBar;
    QLabel* m_speedLabel;
    QLabel* m_etaLabel;
    QLabel* m_statusLabel;
    bool m_isVerifying;
};

#endif // PROGRESSVIEW_H
