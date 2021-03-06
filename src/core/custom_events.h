#pragma once
#ifndef __CORE_CUSTOM_EVENTS_H__
#define __CORE_CUSTOM_EVENTS_H__

#include <QEvent>
#include <QString>
#include "message.h"

template <class T>
class CustomEvent : public QEvent {
public:
    CustomEvent() : QEvent(type()) {}
    virtual ~CustomEvent() = default;

    static QEvent::Type type()
    {
        static int itype = QEvent::registerEventType();
        return static_cast<QEvent::Type>(itype);
    }
};

class StatusEvent : public CustomEvent<StatusEvent> {
public:
    StatusEvent(const char* status)
        : CustomEvent<StatusEvent>()
        , status_(status)
    {
    }

    QString status() { return status_; }

private:
    QString status_;
};

class TitleEvent : public CustomEvent<TitleEvent> {
public:
    TitleEvent(const char* title)
        : CustomEvent<TitleEvent>()
        , title_(title)
    {
    }

    QString title() { return title_; }

private:
    QString title_;
};

class MessageEvent : public CustomEvent<MessageEvent> {
public:
    MessageEvent(message::Type mtype, const char* caption, const char* text)
        : CustomEvent<MessageEvent>()
        , caption_(caption)
        , text_(text)
        , msg_type_(mtype)
    {
    }

    QString caption() { return caption_; }
    QString text() { return text_; }
    message::Type msg_type() { return msg_type_; }

private:
    QString caption_;
    QString text_;
    message::Type msg_type_;
};

class PlaybackFailureEvent : public CustomEvent<PlaybackFailureEvent> {};

#endif
