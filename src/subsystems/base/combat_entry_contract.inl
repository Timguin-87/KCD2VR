static bool EntryIs596(std::uintptr_t rva,const unsigned char* want,std::size_t n) noexcept {
    unsigned char a[32]{};return n<=sizeof(a)&&SehCopy146(reinterpret_cast<const void*>(g_base+rva),a,n)&&std::memcmp(a,want,n)==0;}
