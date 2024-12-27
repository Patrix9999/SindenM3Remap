#pragma once

#include <vector>
#include <windows.h>

#include "DeviceMeta.h"

std::vector<DeviceMeta> GetRawInputDeviceMetas(size_t deviceType);
size_t GetRawInputDeviceMetaID(const std::vector<DeviceMeta>& deviceMetas, const DeviceMeta& searchedDeviceMeta);