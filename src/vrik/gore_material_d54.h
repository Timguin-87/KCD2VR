#pragma once
#include <cstddef>
#include <cstdint>
#include <cstring>
namespace kcd2vr::gore_material_d54 {
inline constexpr unsigned kLods=6;
struct Snapshot {std::uintptr_t attachment=0,object=0,skin=0;std::uintptr_t effective[kLods]{},replacement[kLods]{};};
// Read-only data equivalence to exact retail CSKINAttachment::GetBaseMaterial
// F564EC and CSkin::GetMaterial F56528. No SDK layout or guessed field scan.
template<class Read> bool snapshot(Read&& read,bool retail,std::uintptr_t base,std::uintptr_t attachment,Snapshot& out) noexcept {
 out={};if(!retail||!base||!attachment)return false;
 std::uintptr_t vt=0,fn=0,obj=0,skin=0,lods=0,iface=0;std::uint32_t n=0;
 if(!read(attachment,vt))return false;
 const unsigned skinOff=vt==base+0x3A65A08?0x78:vt==base+0x3A8F798?0x98:0;
 const auto ifaceVt=base+(skinOff==0x78?0x3A65958:0x3A8F6E8);
 const auto ifaceGet=base+(skinOff==0x78?0x1A72CA0:0x8CE8D0);
 if(!skinOff||!read(attachment+8,vt)||vt!=ifaceVt||!read(vt+0x10,fn)||fn!=ifaceGet||
    !read(attachment,vt)||!read(vt+0xE0,fn)||fn!=base+0x113CE50||!read(attachment+0x28,obj)||!obj||
    !read(obj+8,iface)||iface!=attachment+8||!read(obj,vt)||vt!=base+0x3A3F158||!read(vt+0x70,fn)||fn!=base+0xF564EC||
    !read(vt+0x80,fn)||fn!=base+0x4A34E0||!read(attachment+skinOff,skin)||!skin||
    !read(skin,vt)||vt!=base+0x406C9F0||!read(vt+0x28,fn)||fn!=base+0xF56528||
    !read(skin+0x10,lods)||!lods||!read(lods-4,n)||n>kLods)return false;
 out.attachment=attachment;out.object=obj;out.skin=skin;
 for(unsigned i=0;i<kLods;++i){
  if(!read(obj+0x10+i*8,out.replacement[i])){out={};return false;}
  out.effective[i]=out.replacement[i];
  if(!out.effective[i]&&i<n&&!read(lods+i*0xB0+8,out.effective[i])){out={};return false;}
  if(out.effective[i]&&(!read(out.effective[i],vt)||vt!=base+0x40283F0)){out={};return false;}
 }
 return true;
}
template<class Read> bool name(Read&& read,bool retail,std::uintptr_t base,std::uintptr_t material,char* out,std::size_t cap) noexcept {
 if(!out||cap<2)return false;out[0]=0;
 std::uintptr_t vt=0,fn=0,str=0;
 if(!retail||!base||!material||!read(material,vt)||vt!=base+0x40283F0||
    !read(vt+0x40,fn)||fn!=base+0x113CE50||!read(material+0x28,str)||!str)return false;
 for(std::size_t i=0;i<cap;++i){char ch=0;if(!read(str+i,ch)){out[0]=0;return false;}
  if(!ch){out[i]=0;return i>0;}
  // The string is only a wrapper-selection hint, never code or a native pointer.
  if(ch=='\\')ch='/';if(ch>='A'&&ch<='Z')ch=char(ch-'A'+'a');
  if(!((ch>='a'&&ch<='z')||(ch>='0'&&ch<='9')||ch=='_'||ch=='/'||ch=='.'||ch=='-')||i+1==cap){out[0]=0;return false;}
  out[i]=ch;
 }out[0]=0;return false;
}
enum class Result {Applied,Empty,OwnerChanged,RetainFailed,Restored,RestoreFailed};
// Strong references preserve both source and fallback material objects through
// all setters. A refused/readback-failed setter restores the exact old six-slot
// replacement set, including a setter which applied before reporting failure.
template<class IO> Result transfer(IO& io,const Snapshot& src,const Snapshot& dst,unsigned& changed) noexcept {
 changed=0;if(!src.object||!dst.object||src.object==dst.object||!io.current())return Result::OwnerChanged;
 bool heldSrc[kLods]{},heldOld[kLods]{},active[kLods]{};bool any=false,retainOk=true;
 for(unsigned i=0;i<kLods;++i){
  active[i]=src.effective[i]&&src.effective[i]!=dst.replacement[i];if(!active[i])continue;any=true;
  if(!(heldSrc[i]=io.retain(src.effective[i]))){retainOk=false;break;}
  if(dst.replacement[i]&&!(heldOld[i]=io.retain(dst.replacement[i]))){retainOk=false;break;}
 }
 const auto release=[&](){for(unsigned i=0;i<kLods;++i){if(heldOld[i])io.release(dst.replacement[i]);if(heldSrc[i])io.release(src.effective[i]);}};
 if(!retainOk){release();return Result::RetainFailed;}
 if(!any){release();return Result::Empty;}
 if(!io.current()){release();return Result::OwnerChanged;}
 bool ok=true;unsigned attempted=0;
 for(unsigned i=0;i<kLods;++i){if(!active[i])continue;
  if(!io.current()){ok=false;break;}attempted=i+1;
  if(!io.set(i,src.effective[i])||!io.equals(i,src.effective[i])){ok=false;break;}++changed;}
 if(ok&&io.current()){release();return Result::Applied;}
 bool restored=io.current();
 if(restored)for(unsigned i=0;i<attempted;++i)if(active[i]){
  const bool set=io.set(i,dst.replacement[i]);const bool equal=io.equals(i,dst.replacement[i]);restored=set&&equal&&restored;
 }
 changed=0;release();return restored?Result::Restored:Result::RestoreFailed;
}
inline const char* resultName(Result r) noexcept {switch(r){case Result::Applied:return "APPLIED_READBACK";case Result::Empty:return "NO_DIFFERENT_MATERIAL";case Result::OwnerChanged:return "OWNER_CHANGED";case Result::RetainFailed:return "RETAIN_FAILED";case Result::Restored:return "FAILED_EXACT_FALLBACK_RESTORED";default:return "FAILED_ROLLBACK_UNCERTAIN";}}
}
