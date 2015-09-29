#pragma once
#ifndef __CORE_MESSAGE_H__
#define __CORE_MESSAGE_H__

// Platform independent wrapper for messages
namespace message {

    void Error(const char* caption, const char* text);
    void Info(const char* caption, const char* text);
    void Warning(const char* caption, const char* text);

} // namespace message

#endif
