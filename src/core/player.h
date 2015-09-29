#pragma once
#ifndef __CORE_PLAYER_H__
#define __CORE_PLAYER_H__

#include <vector>
#include <functional>

namespace core {

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

        const char * GetUrl() const;

        virtual void Lock() = 0;
        virtual void Unlock() = 0;
        virtual void Play() = 0;
        virtual void Stop() = 0;
        virtual void VolumeUp() = 0;
        virtual void VolumeDown() = 0;
        virtual void SetVolume(double val) = 0;

        void OnSongChange();
        void OnStatusChange();

    protected:
        void SetStatus(const char* status);
        void SetTitle(const char* title);

        void IncreaseVolume();
        void DecreaseVolume();

        double volume_;
        State state_;

        std::vector<std::function<void(void)>> listeners_;
    };

} // namespace core

#endif
