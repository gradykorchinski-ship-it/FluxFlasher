#ifndef CONFIRMDIALOG_H
#define CONFIRMDIALOG_H

#include <QDialog>
#include <QString>

class ConfirmDialog : public QDialog {
    Q_OBJECT

public:
    explicit ConfirmDialog(const QString& deviceName, QWidget *parent = nullptr);

private:
    void setupUI(const QString& deviceName);
};

#endif // CONFIRMDIALOG_H
