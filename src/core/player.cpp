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
    void Player::SetStatus(const char* status)
    {

    }
    void Player::SetTitle(const char* title)
    {

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
    void Player::OnSongChange()
    {

    }
    void Player::OnStatusChange()
    {

    }

}
