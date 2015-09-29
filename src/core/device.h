#pragma once
#ifndef __CORE_DEVICE_H__
#define __CORE_DEVICE_H__

#include <vector>
#include <string>

namespace core {

    const size_t kUnknownDevice = size_t(-1);

	class DeviceEnumerator {
	public:
		DeviceEnumerator();
		virtual ~DeviceEnumerator();

		bool Enum();
		const char* GetName(size_t device);
		size_t GetCount();
		size_t GetCurrent();
		
		bool Choose(size_t device);
		
	private:
		size_t current_;
		std::vector<std::string> devices_;
	};
	
} // namespace core

#endif
