// Generated from src/frik_pose_probe.cpp; do not edit.
template<class T> bool Read(const void* p,T& out) noexcept {
    static_assert(std::is_trivially_copyable_v<T>);
    if(!Safe(p,sizeof(T))) return false;
    __try { std::memcpy(&out,p,sizeof(T)); return true; }
    __except(EXCEPTION_EXECUTE_HANDLER) { NoteReadFault178r7(p,0); return false; }
}
template<class T,std::size_t N> bool Read(const void* p,T (&out)[N]) noexcept {
    if(!Safe(p,sizeof(out)))return false;
    __try {std::memcpy(out,p,sizeof(out));return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
template<class T> bool SafeWrite(void* p,const T& in) noexcept {
    static_assert(std::is_trivially_copyable_v<T>);
    if(!Safe(p,sizeof(T))) return false;
    __try { std::memcpy(p,&in,sizeof(T)); return true; }
    __except(EXCEPTION_EXECUTE_HANDLER) { NoteReadFault178r7(p,0); return false; }
}
