#pragma once
namespace kcd2vr::prompt_family_d49 {
// D68: saved style4 chooses PS art only. Native action lookup stays Xbox2.
constexpr int semanticFamilyD68(int) noexcept { return 2; }
enum class Result { Unchanged, Changed, Unavailable, Rejected };
// Native callbacks may re-enter the manager setter. The caller owns its
// thread-local recursion guard; this helper verifies the actual post-write value.
template<class Read,class Write>
Result reassert(int forced,Read&& read,Write&& write) noexcept {
    int before=0,after=0;
    if((forced!=2&&forced!=4)||!read(before))return Result::Unavailable;
    if(before==forced)return Result::Unchanged;
    if(!write(forced)||!read(after)||after!=forced)return Result::Rejected;
    return Result::Changed;
}
}
