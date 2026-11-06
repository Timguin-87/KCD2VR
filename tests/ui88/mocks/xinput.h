#pragma once
#include "windows.h"
struct XINPUT_GAMEPAD{WORD wButtons{};BYTE bLeftTrigger{},bRightTrigger{};SHORT sThumbLX{},sThumbLY{},sThumbRX{},sThumbRY{};};
struct XINPUT_STATE{DWORD dwPacketNumber{};XINPUT_GAMEPAD Gamepad{};};
struct XINPUT_VIBRATION{WORD wLeftMotorSpeed{},wRightMotorSpeed{};};
struct XINPUT_CAPABILITIES{BYTE Type{},SubType{};WORD Flags{};XINPUT_GAMEPAD Gamepad{};XINPUT_VIBRATION Vibration{};};
constexpr BYTE XINPUT_DEVTYPE_GAMEPAD=1,XINPUT_DEVSUBTYPE_GAMEPAD=1;
