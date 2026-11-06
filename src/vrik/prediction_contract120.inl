// Generated from exact SHA by tools/research_prediction120.py. Do not hand-edit.
static bool PredictionContract120(std::uintptr_t b) noexcept {
    if(!b||!SupportedWhBuild(b)||!retail_build112::Ready(b))return false;
    const unsigned char expected0[]{0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x70,0x10,0x48,0x89,0x78,0x18,0x55};
    unsigned char actual0[16]{};
    if(!Read(reinterpret_cast<void*>(b+0xA7C624),actual0)||std::memcmp(actual0,expected0,16))return false;
    const unsigned char expected1[]{0xF2,0x0F,0x10,0x87,0x54,0x01,0x00,0x00,0x4C,0x8D,0x6F,0x74};
    unsigned char actual1[12]{};
    if(!Read(reinterpret_cast<void*>(b+0xA7C65F),actual1)||std::memcmp(actual1,expected1,12))return false;
    const unsigned char expected2[]{0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x70,0x10,0x48,0x89,0x78,0x18,0x4C};
    unsigned char actual2[16]{};
    if(!Read(reinterpret_cast<void*>(b+0xA7CA90),actual2)||std::memcmp(actual2,expected2,16))return false;
    const unsigned char expected3[]{0xE8,0x36,0xF8,0xFF,0xFF};
    unsigned char actual3[5]{};
    if(!Read(reinterpret_cast<void*>(b+0xA7CDE9),actual3)||std::memcmp(actual3,expected3,5))return false;
    const unsigned char expected4[]{0xE8,0x23,0x01,0x00,0x00};
    unsigned char actual4[5]{};
    if(!Read(reinterpret_cast<void*>(b+0xA7C968),actual4)||std::memcmp(actual4,expected4,5))return false;
    const unsigned char expected5[]{0xE8,0x37,0x03,0x00,0x00};
    unsigned char actual5[5]{};
    if(!Read(reinterpret_cast<void*>(b+0xA7C990),actual5)||std::memcmp(actual5,expected5,5))return false;
    const unsigned char expected6[]{0xE8,0x63,0xD7,0xFF,0xFF};
    unsigned char actual6[5]{};
    if(!Read(reinterpret_cast<void*>(b+0xA7C9BC),actual6)||std::memcmp(actual6,expected6,5))return false;
    return true;
}
