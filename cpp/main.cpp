#include "mainwindow.h"
#include "core_interface.h"
#include <QApplication>
#include <QPalette>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Set dark theme
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(71, 75, 79));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(60, 63, 67));
    darkPalette.setColor(QPalette::AlternateBase, QColor(71, 75, 79));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(60, 63, 67));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(0, 174, 239));
    darkPalette.setColor(QPalette::Highlight, QColor(0, 174, 239));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    
    app.setPalette(darkPalette);
    
    MainWindow window;
    window.show();
    
    return app.exec();
}
