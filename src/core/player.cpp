#include "player.h"

namespace core {

    const double kVolumeStep = 0.1;

    Player::Player()
    : volume_(1.0) // TODO: should be loaded from config file
    , state_(State::kInitial)
    {

    }
    Player::~Player()
    {

    }
    const char * Player::GetUrl() const
    {
        // TODO: should be loaded from config file too
        return "http://mp3.nashe.ru:80/ultra-128.mp3";
    }
    void Player::AddTitleObserver(TitleEventFunction func)
    {
        title_observers_.push_back(func);
    }
    void Player::AddStatusObserver(StatusEventFunction func)
    {
        status_observers_.push_back(func);
    }
    void Player::AddMessageObserver(MessageEventFunction func)
    {
        message_observers_.push_back(func);
    }
    void Player::AddPlaybackFailureObserver(PlaybackFailureEventFunction func)
    {
        playback_failure_observers_.push_back(func);
    }
    void Player::SetStatus(const char* status)
    {
        Lock();
        for (auto &f : status_observers_)
        {
            f(status);
        }
        Unlock();
    }
    void Player::SetTitle(const char* title)
    {
        Lock();
        for (auto &f : title_observers_)
        {
            f(title);
        }
        Unlock();
    }
    void Player::ShowMessage(const char* caption, const char* text)
    {
        Lock();
        for (auto &f : message_observers_)
        {
            f(caption, text);
        }
        Unlock();
    }
    void Player::PlaybackFailed()
    {
        Lock();
        for (auto &f : playback_failure_observers_)
        {
            f();
        }
        Unlock();
    }
    void Player::IncreaseVolume()
    {
        volume_ += kVolumeStep;
        if (volume_ > 1.0)
            volume_ = 1.0;
    }
    void Player::DecreaseVolume()
    {
        volume_ -= kVolumeStep;
        if (volume_ < 0.0)
            volume_ = 0.0;
    }
    Player::State Player::state() const
    {
        return state_;
    }

}
