#include "RawInput.h"
#include <regex>

std::vector<DeviceMeta> GetRawInputDeviceMetas(size_t deviceType)
{
	UINT nDevices;
	if (GetRawInputDeviceList(NULL, &nDevices, sizeof(RAWINPUTDEVICELIST)) != 0 || nDevices == 0)
		return {}; // RawInput not initialized or no raw input devices were found

	std::vector<RAWINPUTDEVICELIST> devices(nDevices);
	if (GetRawInputDeviceList(devices.data(), &nDevices, sizeof(RAWINPUTDEVICELIST)) == -1)
		return {}; // Failed to enumerate RawInput devices

	std::vector<DeviceMeta> deviceMetas;
	static std::regex regex(R"(\\?\HID#VID_([0-9A-Z]+)&PID_([0-9A-Z]+))");

	for (size_t i = nDevices - 1; i != -1; --i)
	{
		if (devices[i].dwType != deviceType)
			continue;

		char name[256] = {};
		UINT nLength = sizeof(name);
		if (GetRawInputDeviceInfoA(devices[i].hDevice, RIDI_DEVICENAME, name, &nLength) == -1)
			continue; // Cannot retrieve RawInput device name

		if (strstr(name, "Root#RDP_") != NULL)
			continue; // Ignore any RDP devices

		std::cmatch match;
		if (!std::regex_search(name, match, regex))
			continue; // regex doesn't match, skip the record

		// All good, add the entry
		deviceMetas.push_back({ match[1].str(), match[2].str() });
	}

	return deviceMetas;
}

size_t GetRawInputDeviceMetaID(const std::vector<DeviceMeta>& deviceMetas, const DeviceMeta& searchedDeviceMeta)
{
	auto it = std::find_if(deviceMetas.begin(), deviceMetas.end(), [&](const DeviceMeta& deviceMeta)
	{
		return deviceMeta.vid == searchedDeviceMeta.vid && deviceMeta.pid == searchedDeviceMeta.pid;
	});

	if (it == deviceMetas.end())
		return 0;

	return std::distance(deviceMetas.begin(), it) + 1;
}