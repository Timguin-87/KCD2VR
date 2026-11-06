#pragma once
#include <cstddef>
#include <cstdint>
namespace kcd2vr::semi_smith46::pins {
struct Pin {std::uintptr_t rva;const char* bytes;std::size_t size;};
inline constexpr Pin all[]{
    {0x399AE0, "\x48\x89\x5C\x24\x08\x48\x89\x7C\x24\x10\x55\x48\x8B\xEC\x48\x83\xEC\x30", 18}, // Exact native callback builder entry
    {0x399539, "\xE8\xA2\x05\x00\x00", 5}, // Sole smith child builder call
    {0x399522, "\xE8\x81\x98\x2E\x00", 5}, // Allocate generic C_CallbackAction
    {0x399BA8, "\x48\x8D\x05\xE1\xCE\x4B\x00", 7}, // Native smith event callback remains unchanged
    {0x399BCF, "\x48\x8B\x4B\x78\xE8\x2C\x5C\x33\x00", 9}, // Callback queue obtains player action controller
    {0x399BE6, "\x4C\x8B\x81\x98\x00\x00\x00\x48\x8B\xC8\x41\xFF", 12}, // Native Queue virtual 0x98 before child slot storage
    {0x399BF3, "\x48\x8D\x8B\x88\x00\x00\x00\x48\x8B\xD7\xE8\x22\x00\x00\x00", 15}, // Parent child pointer stored AFTER Queue
    {0x857126, "\x4C\x8D\x46\x70", 4}, // Parent listener is active minigame +70
    {0x857135, "\x48\x8D\x05\xFC\x1D\x20\x03", 7}, // Exact derived smith parent class
    {0x857142, "\x48\x8D\x05\xAF\x1F\x20\x03\x48\x89\x47\x10", 11}, // Exact secondary framework action class at parent+10
    {0x9CD58A, "\x66\xC7\x41\x18\x01\x00", 6}, // Native parent is started before child builder
    {0x9CD5A6, "\x48\x8B\x53\xF0\x48\x8D\x4B\xF0\x89\x43\x38\x4C\x8B\x82\x30\x01\x00\x00\x49\x8B\xD2\x41\xFF\xD0", 24}, // Native secondary Start dispatches primary +130 child builder
    {0x856BC5, "\x48\x89\x73\x78", 4}, // Parent native player
    {0x856BD8, "\x48\x89\xAB\x80\x00\x00\x00", 7}, // Parent native listener backlink
    {0x856BE6, "\x48\xC7\x83\x88\x00\x00\x00\x00\x00\x00\x00", 11}, // Parent child initially empty
    {0x85729E, "\x40\x88\x68\x50", 4}, // Native action-code byte at parent+50
    {0x8577FD, "\xB2\x0B", 2}, // StrokeFinish selects native code11
    {0x857801, "\xB2\x0A", 2}, // StrokeStart selects native code10
    {0x856FE6, "\xFF\xD3\x48\x8D\x8E\xC8\x00\x00\x00\x48\x8D\x54\x24\x40\xE8\x83\x00\x00\x00", 19}, // Start parent before storing current action at instance+C8
    {0x682DD3, "\xB8\x00\x00\x80\x3F\x89\x43\x5C\x89\x43\x60", 11}, // Native child speed starts at one; refs are not speed
    {0x682DE5, "\x48\x89\x4B\x08\x89\x4B\x10\x48", 8}, // Child context and native work time
    {0x682E3F, "\x48\x8D\x05\x6A\x22\x3C\x03\x48\x89\x03", 10}, // Final generic C_CallbackAction class
    {0x684DC4, "\x48\x8B\x01\x83\x61\x2C\xDF\x48\x89\x51\x08\xC7\x41\x28\x01\x00\x00\x00\x48\xC7\x41\x30\x00\x00\x00\x00\xC7\x41\x10\x00\x00\x00\x00", 33}, // Queue establishes native context status1 root0 time0
    {0x3B6E80, "\xC2\x00\x00", 3}, // Exact callback child OnQueued is a no-op
    {0x4B3F12, "\x0F\x28\xCF\xF3\x0F\x59\x4B\x5C\x48\x8B\xCB\xFF\x50\x30", 14}, // Native action Update consumes child speed only
    {0x4B3F91, "\xF3\x44\x0F\x59\x43\x5C", 6}, // Native oneshot checks consume child speed
    {0x4B3A76, "\xF3\x0F\x10\x7D\x5C\xF3\x0F\x10\x43\x58\x0F\x2E\xC7", 13}, // Native scope detects child speed changes
    {0x1A7E0E0, "\xF3\x0F\x58\x49\x10\x8B\x41\x28\xF3\x0F\x11\x49\x10\xC3", 14}, // Child Update adds scaled dt to native work counter
    {0x1A73130, "\xF3\x0F\x10\x81\x9C\x00\x00\x00\xC3", 9}, // Controller scale getter reads unchanged scale+9C
    {0xA98518, "\x80\x7F\x0E\x00", 4}, // Full charge native flag
    {0xA985CA, "\x4C\x8B\x81\xE0\x00\x00\x00\x48\x8B\xC8\x41\xFF\xD0", 13}, // Finish derives intensity from normalized CAnimation time
    {0xA98BDA, "\xF3\x0F\x10\x43\x44", 5}, // Normalized native animation segment time
    {0x858BEF, "\xC6\x40\x0E\x01", 4}, // FullStrokeCharge event sets existing flag
    {0x859B6E, "\xFF\x90\x40\x02\x00\x00", 6}, // Native RPG impact evaluator remains owner
    {0x857E0A, "\x48\x8B\x8F\xB0\x00\x00\x00\x0F\x28\xCE\xE8\x3F\x07\x00\x00", 15}, // Preserved smith process update dt
    {0x2E5BD3F, "\xF3\x0F\x10\x1D\xC9\xE4\x23\x01", 8}, // Native forge repeat interval address
    {0x1538F34, "\xE8\xFB\xDA\x49\xFF", 5}, // Native forge repeat timer reset
};
struct Slot {std::uintptr_t rva,target;};
inline constexpr Slot slots[]{
    {0x3A59068,0x399464},
    {0x3A6A2D0,0x8589E8},
    {0x3A450E0,0x1A7E0E0},
    {0x3A45130,0x3B6E80},
    {0x3A712C0,0xA98B64},
    {0x4051AC8,0x1A73130},
};
}
