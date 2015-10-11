#include "device.h"
#include "player.h"
#include "../thirdparty/include/bass.h"

namespace core {

    DeviceEnumerator::DeviceEnumerator(Player * player)
        : current_(kUnknownDevice)
        , player_(player)
	{
	}
	DeviceEnumerator::~DeviceEnumerator()
	{
	}
	bool DeviceEnumerator::Enum()
	{
		BASS_DEVICEINFO info;
		devices_.clear();
		for (int i = 1; BASS_GetDeviceInfo(i, &info); ++i)
			if (info.flags & BASS_DEVICE_ENABLED)
				devices_.push_back(info.name);
		return devices_.size() != 0U;
	}
	const char* DeviceEnumerator::GetName(size_t device)
	{
		return devices_[device].c_str();
	}
	size_t DeviceEnumerator::GetCount()
	{
		return devices_.size();
	}
	size_t DeviceEnumerator::GetCurrent()
	{
		return current_;
	}
	bool DeviceEnumerator::Choose(size_t device)
	{
		if (device == current_)
			return true;
		else if (BASS_SetDevice(device) == TRUE)
		{
			current_ = device;
			return true;
		}
		else // an error during device changing
		{
			switch (BASS_ErrorGetCode())
			{
			case BASS_ERROR_DEVICE:
                player_->ShowMessage(message::kError, "Error", "The device is invalid.");
				break;
			case BASS_ERROR_INIT:
				// The device has not been initialized.
                if (BASS_Init(device, 44100, 0, nullptr, nullptr) == TRUE)
				{
					current_ = device;
					return true;
				}
				else
                    player_->ShowMessage(message::kError, "Error", "Can't initialize device");
				break;
			}
			return false;
		}
	}
	
} // namespace core
