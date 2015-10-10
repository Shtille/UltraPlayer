#pragma once
#ifndef __CORE_BASS_PLAYER_H__
#define __CORE_BASS_PLAYER_H__

#include "player.h"
#include "../thirdparty/include/bass.h"

#include <mutex>
#include <thread>

namespace core {

    class DeviceEnumerator;

    class BassPlayer : public Player {
    public:
        BassPlayer();
        virtual ~BassPlayer();

        void Lock() final;
        void Unlock() final;
        void Play() final;
        void Stop() final;
        void VolumeUp() final;
        void VolumeDown() final;
        void SetVolume(double val) final;

    private:
        static void DoMeta(BassPlayer * player);
        static void CALLBACK MetaSync(HSYNC handle, DWORD channel, DWORD data, void *user);
        static void CALLBACK EndSync(HSYNC handle, DWORD channel, DWORD data, void *user);
        static void CALLBACK StatusProc(const void *buffer, DWORD length, void *user);

        static void OpenUrlProc(BassPlayer * player);
        static void TimerProc(BassPlayer * player);

        void StopPlayback();
        void StopThreads();
        void StartUrl();
        void StopUrl();
        void StartTimer();
        void StopTimer();
        void LockTimer();
        void UnlockTimer();

        std::mutex mutex_;
        std::mutex timer_mutex_;
        std::thread * url_thread_;
        std::thread * timer_thread_;

        HSTREAM channel_;
        DeviceEnumerator * device_enumerator_;
    };

} // namespace core

#endif
