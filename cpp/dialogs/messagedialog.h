#ifndef MESSAGEDIALOG_H
#define MESSAGEDIALOG_H

#include <QDialog>
#include <QString>

enum class MessageType {
    Success,
    Error,
    Info
};

class MessageDialog : public QDialog {
    Q_OBJECT

public:
    explicit MessageDialog(MessageType type, const QString& title, const QString& message, QWidget *parent = nullptr);

private:
    void setupUI(MessageType type, const QString& title, const QString& message);
};

#endif // MESSAGEDIALOG_H
