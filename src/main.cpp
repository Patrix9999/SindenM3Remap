#include <argparse/argparse.hpp>
#include <regex>
#include <optional>
#include <iostream>
#include "RawInput.h"

static std::optional<DeviceMeta> GetLightgunDeviceMeta(const std::string& param)
{
    static std::regex regex(R"(VID:([0-9A-Z]+)\s+PID:([0-9A-Z]+))");

    std::smatch match;
    if (!std::regex_search(param, match, regex))
        return std::nullopt;

    return DeviceMeta{ match[1].str(), match[2].str() };
}

static void WriteIniString(const std::string& section, const std::string& key, const std::string& value, const std::string& file)
{
    std::string value_escaped = " \"" + value + "\"";
    WritePrivateProfileStringA(section.c_str(), key.c_str(), value_escaped.c_str(), file.c_str());
}

static void UpdateGunIni(int gun_id, int lighgun_mouse_id, int lightgun_keyboard_id, const std::string& iniFile)
{
    // global
    WriteIniString("GLOBAL", gun_id == 1 ? "InputStart1" : "InputStart2", "KEY" + std::to_string(lightgun_keyboard_id) + "_UP", iniFile);
    WriteIniString("GLOBAL", gun_id == 1 ? "InputCoin1" : "InputCoin2", "KEY" + std::to_string(lightgun_keyboard_id) + "_LEFT", iniFile);

    // Star Wars Trilogy
    if (gun_id == 1)
    {
        WriteIniString("GLOBAL", "InputAnalogJoyX", "MOUSE" + std::to_string(lighgun_mouse_id) + "_XAXIS", iniFile);
        WriteIniString("GLOBAL", "InputAnalogJoyY", "MOUSE" + std::to_string(lighgun_mouse_id) + "_YAXIS", iniFile);
        WriteIniString("GLOBAL", "InputAnalogJoyTrigger", "MOUSE" + std::to_string(lighgun_mouse_id) + "_LEFT_BUTTON", iniFile);
        WriteIniString("GLOBAL", "InputAnalogJoyEvent", "MOUSE" + std::to_string(lighgun_mouse_id) + "_RIGHT_BUTTON", iniFile);
    }

    // Lost World
    WriteIniString("GLOBAL", gun_id == 1 ? "InputGunX" : "InputGunX2", "MOUSE" + std::to_string(lighgun_mouse_id) + "_XAXIS", iniFile);
    WriteIniString("GLOBAL", gun_id == 1 ? "InputGunY" : "InputGunY2", "MOUSE" + std::to_string(lighgun_mouse_id) + "_YAXIS", iniFile);
    WriteIniString("GLOBAL", gun_id == 1 ? "InputTrigger" : "InputTrigger2", "MOUSE" + std::to_string(lighgun_mouse_id) + "_LEFT_BUTTON", iniFile);
    WriteIniString("GLOBAL", gun_id == 1 ? "InputOffscreen" : "InputOffscreen2", "MOUSE" + std::to_string(lighgun_mouse_id) + "_RIGHT_BUTTON", iniFile);

    // Ocean Hunter
    WriteIniString("GLOBAL", gun_id == 1 ? "InputAnalogGunX" : "InputAnalogGunX2", "MOUSE" + std::to_string(lighgun_mouse_id) + "_XAXIS", iniFile);
    WriteIniString("GLOBAL", gun_id == 1 ? "InputAnalogGunY" : "InputAnalogGunY2", "MOUSE" + std::to_string(lighgun_mouse_id) + "_YAXIS", iniFile);
    WriteIniString("GLOBAL", gun_id == 1 ? "InputAnalogTriggerLeft" : "InputAnalogTriggerLeft2", "MOUSE" + std::to_string(lighgun_mouse_id) + "_LEFT_BUTTON", iniFile);
    WriteIniString("GLOBAL", gun_id == 1 ? "InputAnalogTriggerRight" : "InputAnalogTriggerRight2", "MOUSE" + std::to_string(lighgun_mouse_id) + "_RIGHT_BUTTON", iniFile);
}

int main(int argc, char* argv[])
{
    // Usage:
	// LightgunM3Remap.exe --gun1 "VID:16C0 PID:0F38" --gun2 "VID:16C0 PID:0F39"

    std::string iniFile = argv[0];
    iniFile = iniFile.substr(0, iniFile.find_last_of("\\"));
    iniFile += "\\Config\\Supermodel.ini";

    argparse::ArgumentParser program("LightgunM3Remap");

    program.add_argument("--gun1")
        .help("Pass first gun VendorId and ProductId, e.g. \"VID:16C0 PID:0F38\"")
        .required();

    program.add_argument("--gun2")
        .help("Optionally pass second gun VendorID and ProductId, e.g. \"VID:16C0 PID:0F39\"")
        .default_value(std::string("")); // Default empty if not provided

    std::optional<DeviceMeta> gun1_device_meta, gun2_device_meta;

    try
    {
        program.parse_args(argc, argv);

        std::string gun1_param = program.get<std::string>("--gun1");
        gun1_device_meta = GetLightgunDeviceMeta(gun1_param);

        std::string gun2_param = program.get<std::string>("--gun2");
        if (!gun2_param.empty())
            gun2_device_meta = GetLightgunDeviceMeta(gun2_param);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << program;

        return 1;
    }

    if (!gun1_device_meta.has_value())
    {
        std::cerr << "invalid \"gun1\" argument provided" << std::endl;
        return 2;
    }

	std::vector<DeviceMeta> keyboardDeviceMetas = GetRawInputDeviceMetas(RIM_TYPEKEYBOARD);
	std::vector<DeviceMeta> mouseDeviceMetas = GetRawInputDeviceMetas(RIM_TYPEMOUSE);

	size_t lightun_1_mouse_id = GetRawInputDeviceMetaID(mouseDeviceMetas, *gun1_device_meta);
	size_t lightun_1_keyboard_id = GetRawInputDeviceMetaID(keyboardDeviceMetas, *gun1_device_meta);

    if (lightun_1_mouse_id == 0 || lightun_1_keyboard_id == 0)
    {
        std::cerr << "lightgun 1 not found via RawInput API, make sure you've passed the correct PID and VID" << std::endl;
        return 3;
    }

    std::cout << "[lightgun 1 found]" << std::endl;
    std::cout << "- keyboard_id: " << lightun_1_keyboard_id << std::endl;
    std::cout << "- mouse_id: " << lightun_1_mouse_id << std::endl;
    std::cout << std::endl;

    std::cout << "Updating lightgun 1 Supermodel.ini entries..." << std::endl;
    UpdateGunIni(1, lightun_1_mouse_id, lightun_1_keyboard_id, iniFile);
    std::cout << "Done!" << std::endl;

    if (gun2_device_meta.has_value())
    {
        size_t lightun_2_mouse_id = GetRawInputDeviceMetaID(mouseDeviceMetas, *gun2_device_meta);
        size_t lightun_2_keyboard_id = GetRawInputDeviceMetaID(keyboardDeviceMetas, *gun2_device_meta);

        if (lightun_2_mouse_id == 0 || lightun_2_keyboard_id == 0)
        {
            std::cerr << "lightgun 2 not found via RawInput API, make sure you've passed the correct PID and VID" << std::endl;
            return 3;
        }

        std::cout << "[lightgun 2 found]" << std::endl;
        std::cout << "- keyboard_id: " << lightun_2_keyboard_id << std::endl;
        std::cout << "- mouse_id: " << lightun_2_mouse_id << std::endl;
        std::cout << std::endl;

        std::cout << "Updating lightgun 2 Supermodel.ini entries..." << std::endl;
        UpdateGunIni(2, lightun_2_mouse_id, lightun_2_keyboard_id, iniFile);
        std::cout << "Done!" << std::endl;
    }

#ifdef _DEBUG
    std::cin.get();
#endif

    return 0;
}