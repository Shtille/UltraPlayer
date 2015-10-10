#pragma once
#ifndef __CORE_PLAYER_H__
#define __CORE_PLAYER_H__

#include <vector>
#include <string>

namespace core {

    typedef void (*StatusEventFunction)(const char*);
    typedef void (*TitleEventFunction)(const char*);
    typedef void (*MessageEventFunction)(const char*, const char*);
    typedef void (*PlaybackFailureEventFunction)();

    class Player {
    public:
        enum class State {
            kInitial,
            kPlaying,
            kStopped
        };

        Player();
        virtual ~Player();

        State state() const;
        std::string status() const;
        std::string title() const;

        void AddTitleObserver(TitleEventFunction func);
        void AddStatusObserver(StatusEventFunction func);
        void AddMessageObserver(MessageEventFunction func);
        void AddPlaybackFailureObserver(PlaybackFailureEventFunction func);

        const char * GetUrl() const;

        virtual void Lock() = 0;
        virtual void Unlock() = 0;
        virtual void Play() = 0;
        virtual void Stop() = 0;
        virtual void VolumeUp() = 0;
        virtual void VolumeDown() = 0;
        virtual void SetVolume(double val) = 0;

    protected:
        void SetStatus(const char* status);
        void SetTitle(const char* title);
        void ShowMessage(const char* caption, const char* text);
        void PlaybackFailed();

        void IncreaseVolume();
        void DecreaseVolume();

        double volume_;
        State state_;

        std::vector<TitleEventFunction> title_observers_;
        std::vector<StatusEventFunction> status_observers_;
        std::vector<MessageEventFunction> message_observers_;
        std::vector<PlaybackFailureEventFunction> playback_failure_observers_;
    };

} // namespace core

#endif
