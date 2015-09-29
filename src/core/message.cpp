#include "message.h"
#include <QMessageBox>

namespace message {

    void Error(const char* caption, const char* text)
    {
        QMessageBox::critical(nullptr, QString(caption), QString(text));
    }
    void Info(const char* caption, const char* text)
    {
        QMessageBox::information(nullptr, QString(caption), QString(text));
    }
    void Warning(const char* caption, const char* text)
    {
        QMessageBox::warning(nullptr, QString(caption), QString(text));
    }

} // namespace message
