#include "bass_player.h"
#include "device.h"
#include "message.h"
#include <atomic>
#include <chrono>
#include <cstdlib>

namespace core {

    struct StatusInfo {
        DWORD request;
        BassPlayer * player;
    };

    const DWORD kMaxVolume = 10000U;

    static std::atomic<bool> g_quit(false);
    static std::atomic<DWORD> g_request(0);

    // update stream title from metadata
    void BassPlayer::DoMeta(BassPlayer * player)
    {
        const char *meta = BASS_ChannelGetTags(player->channel_, BASS_TAG_META);
        if (meta) { // got Shoutcast metadata
            const char *p = strstr(meta,"StreamTitle='"); // locate the title
            if (p) {
                const char *p2 = strstr(p,"';"); // locate the end of it
                if (p2) {
                    char *t = strdup(p+13);
                    t[p2-(p+13)] = 0;
                    player->SetTitle(t);
                    free(t);
                }
            }
        } else {
            meta = BASS_ChannelGetTags(player->channel_, BASS_TAG_OGG);
            if (meta) { // got Icecast/OGG tags
                const char *artist=NULL,*title=NULL,*p=meta;
                for (;*p;p+=strlen(p)+1) {
                    if (!strncmp(p,"artist=",7)) // found the artist
                        artist=p+7;
                    if (!strncmp(p,"title=",6)) // found the title
                        title=p+6;
                }
                if (title) {
                    if (artist) {
                        char text[100];
                        snprintf(text,sizeof(text),"%s - %s",artist,title);
                        player->SetTitle(text);
                    } else
                        player->SetTitle(title);
                }
            }
        }
    }

    void CALLBACK BassPlayer::MetaSync(HSYNC handle, DWORD channel, DWORD data, void *user)
    {
        BassPlayer * player = reinterpret_cast<BassPlayer*>(user);
        DoMeta(player);
    }

    void CALLBACK BassPlayer::EndSync(HSYNC handle, DWORD channel, DWORD data, void *user)
    {
        BassPlayer * player = reinterpret_cast<BassPlayer*>(user);
        player->SetStatus("not playing");
    }

    void CALLBACK BassPlayer::StatusProc(const void *buffer, DWORD length, void *user)
    {
        StatusInfo * info = (StatusInfo*) user;
        if (buffer && !length && info->request==g_request) // got HTTP/ICY tags, and this is still the current request
        {
            info->player->SetStatus((const char*)buffer); // display status
        }
    }
    void BassPlayer::OpenUrlProc(BassPlayer * player)
    {
        DWORD c,r;
        r = ++g_request; // increment the request counter for this request
        player->StopTimer(); // stop prebuffer monitoring
        BASS_StreamFree(player->channel_); // close old stream
        player->SetStatus("connecting...");
        size_t device = player->device_enumerator_->GetCurrent();
        if (device == kUnknownDevice ||
            device != kUnknownDevice && BASS_SetDevice(device)) // device exists
        {
            StatusInfo info;
            info.request = r;
            info.player = player;
            c = BASS_StreamCreateURL(player->GetUrl(), 0,
                                     BASS_STREAM_BLOCK | BASS_STREAM_STATUS | BASS_STREAM_AUTOFREE,
                                     StatusProc, (void*)&info); // open URL
        }
        else // device doesnt exist
        {
            return;
        }
        if (r != g_request) // there is a newer request, discard this stream
        {
            if (c) BASS_StreamFree(c);
            return;
        }
        player->channel_ = c; // this is now the current stream
        if (0 == player->channel_) // failed to open
        {
            player->SetStatus("not playing");
            //message::Error("Error", "Can't play the stream");
            player->Stop();
        }
        else
            player->StartTimer(); // start prebuffer monitoring
    }
    void BassPlayer::TimerProc(BassPlayer * player)
    {
        while (!g_quit)
        {
            // Monitor prebuffering progress
            bool end_of_download = BASS_StreamGetFilePosition(player->channel_, BASS_FILEPOS_CONNECTED);
            QWORD pos_buffer = BASS_StreamGetFilePosition(player->channel_, BASS_FILEPOS_BUFFER);
            QWORD pos_end = BASS_StreamGetFilePosition(player->channel_, BASS_FILEPOS_END);
            DWORD progress = static_cast<DWORD>(pos_buffer * 100 / pos_end); // percentage of buffer filled
            if (progress > 75 || end_of_download)
            {
                g_quit = true; // finished prebuffering, stop monitoring
                {
                    // Get the broadcast name and URL
                    const char *icy = BASS_ChannelGetTags(player->channel_, BASS_TAG_ICY);
                    if (!icy) icy = BASS_ChannelGetTags(player->channel_, BASS_TAG_HTTP); // no ICY tags, try HTTP
                    if (icy)
                    {
                        for ( ; *icy; icy += strlen(icy)+1 )
                        {
                            if (! strncmp(icy, "icy-name:", 9))
                                player->SetStatus(icy+9);
                            if (! strncmp(icy, "icy-url:", 8))
                                player->SetStatus(icy+8);
                        }
                    }
                    else
                        player->SetStatus("");
                }
                // Get the stream title and set sync for subsequent titles
                DoMeta(player);
                void * void_player = reinterpret_cast<void*>(player);
                BASS_ChannelSetSync(player->channel_, BASS_SYNC_META, 0, &MetaSync, void_player); // Shoutcast
                BASS_ChannelSetSync(player->channel_, BASS_SYNC_OGG_CHANGE, 0, &MetaSync, void_player); // Icecast/OGG
                // Set sync for end of stream
                BASS_ChannelSetSync(player->channel_, BASS_SYNC_END, 0, &EndSync, void_player);
                // Play it!
                BASS_ChannelPlay(player->channel_, FALSE);
            }
            else
            {
                char text[20];
                sprintf(text,"buffering... %d%%", progress);
                player->SetStatus(text);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }

    BassPlayer::BassPlayer()
    : url_thread_(nullptr)
    , timer_thread_(nullptr)
    , channel_(0U)
    , device_enumerator_(nullptr)
    {
        BASS_Init(-1, 44100, 0, nullptr, nullptr);

        // Enable playlist processing
        BASS_SetConfig(BASS_CONFIG_NET_PLAYLIST, 1);
        // Minimize automatic pre-buffering, so we can do it (and display it) instead
        BASS_SetConfig(BASS_CONFIG_NET_PREBUF, 0);
        // Setup proxy server location
        BASS_SetConfigPtr(BASS_CONFIG_NET_PROXY, nullptr);

        device_enumerator_ = new DeviceEnumerator();
    }
    BassPlayer::~BassPlayer()
    {
        delete device_enumerator_;
        StopThreads();
        BASS_Free();
    }
    void BassPlayer::StopThreads()
    {
        StopUrl();
        StopTimer();
    }
    void BassPlayer::StartUrl()
    {
        StopUrl();
        url_thread_ = new std::thread(OpenUrlProc, this);
    }
    void BassPlayer::StopUrl()
    {
        if (url_thread_)
        {
            if (url_thread_->joinable())
                url_thread_->join();
            delete url_thread_;
            url_thread_ = nullptr;
        }
    }
    void BassPlayer::StartTimer()
    {
        StopTimer();
        g_quit = false;
        timer_thread_ = new std::thread(TimerProc, this);
    }
    void BassPlayer::StopTimer()
    {
        LockTimer();
        g_quit = true;
        if (timer_thread_)
        {
            if (timer_thread_->joinable())
                timer_thread_->join();
            delete timer_thread_;
            timer_thread_ = nullptr;
        }
        UnlockTimer();
    }
    void BassPlayer::LockTimer()
    {
        timer_mutex_.lock();
    }
    void BassPlayer::UnlockTimer()
    {
        timer_mutex_.unlock();
    }
    void BassPlayer::Lock()
    {
        //mutex_.lock();
    }
    void BassPlayer::Unlock()
    {
        //mutex_.unlock();
    }
    void BassPlayer::Play()
    {
        state_ = State::kPlaying;
        StartTimer();
        StartUrl();
    }
    void BassPlayer::Stop()
    {
        state_ = State::kStopped;
        StopThreads();
        BASS_StreamFree(channel_);
    }
    void BassPlayer::VolumeUp()
    {
        IncreaseVolume();
        DWORD volume = static_cast<DWORD>(volume_ * static_cast<double>(kMaxVolume));
        BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, volume);
    }
    void BassPlayer::VolumeDown()
    {
        DecreaseVolume();
        DWORD volume = static_cast<DWORD>(volume_ * static_cast<double>(kMaxVolume));
        BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, volume);
    }
    void BassPlayer::SetVolume(double val)
    {
        if (val > 1.0)
            volume_ = 1.0;
        else if (val < 0.0)
            volume_ = 0.0;
        else
            volume_ = val;
        DWORD volume = static_cast<DWORD>(volume_ * static_cast<double>(kMaxVolume));
        BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, volume);
    }

} // namespace core
