#pragma once
namespace kcd2vr::grip_stow_cb1 {
struct ButtonOwnerH41 {bool previous[2]{};int held[2]{-1,-1};int selected=-1;};
template<class Pad> bool DownH41(const Pad& p,int side,int which) noexcept {
 return which==2?p.thumbstick_down:which==1?(side==0?p.b_down:p.x_down):(side==0?p.a_down:p.y_down);
}
template<class Pad> void MaskH41(Pad& p,int side,int which) noexcept {
 if(which==2){p.thumbstick_down=false;p.thumbstick_pressed=false;}
 else if(which==1){if(side==0){p.b_down=false;p.b_pressed=false;}else{p.x_down=false;p.x_pressed=false;}}
 else{if(side==0){p.a_down=false;p.a_pressed=false;}else{p.y_down=false;p.y_pressed=false;}}
}
template<class Pad,class Request> void RouteH41(ButtonOwnerH41& state,const Pad& rawR,const Pad& rawL,Pad& routeR,Pad& routeL,int button,bool allowed,Request request) noexcept {
 const Pad* raw[2]{&rawR,&rawL};Pad* route[2]{&routeR,&routeL};
 const bool changed=button!=state.selected;state.selected=button;
 for(int side=0;side<2;++side){
  const bool down=DownH41(*raw[side],side,button);
  if(state.held[side]>=0){if(DownH41(*raw[side],side,state.held[side]))MaskH41(*route[side],side,state.held[side]);else state.held[side]=-1;}
  // Startup/config changes while held must not invent a new press.
  if(!changed&&allowed&&down&&!state.previous[side]&&state.held[side]<0&&raw[side]->grip>.5f&&request(side)){
   state.held[side]=button;MaskH41(*route[side],side,button);
  }
  state.previous[side]=down;
 }
}
}
