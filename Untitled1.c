#include <stdio.h>
#include <graphics/gfx.h>
#include <intuition/intuition.h>

#define PORT 2

short *joy=0xdff008+2*PORT;
char*cia=0xbfe001;
struct GfxBase *GfxBase;
struct IntuitionBase *IntuitionBase;

struct Screen *scr;
struct RastPort *rp;
struct RastPort *rp1;
struct RastPort *rp2;
struct RastPort *rp3;
struct RastPort *rp4;
struct RastPort *rp5;
struct RastPort *rp6;
struct RastPort *rp7;
struct RastPort *rp8;
struct RastPort *rp9;
struct RastPort *rp10;
struct RastPort *rp11;
struct RastPort *rp12;
struct RastPort *rp13;
struct RastPort *rp14;
struct RastPort *rp15;
struct RastPort *rp16;

struct NewScreen nw={
0,0,320,200,4,0,1,NULL,CUSTOMSCREEN,NULL,NULL,NULL,NULL };

main()
{
IntuitionBase=OpenLibrary ("intuition.library",0);
if(IntuitionBase==NULL) {
  printf("intuitionbase error!\n");
  return(1);
}

GfxBase=OpenLibrary ("graphics.library", 0);
if(GfxBase==NULL) {
  printf("gfxbase error!\n");
  CloseLibrary(IntuitionBase);
  return(1);
}

scr=OpenScreen(&nw);
if(scr==NULL) {
  printf("screen error!\n");
  CloseLibrary(GfxBase);
  CloseLibrary(IntuitionBase);
  return(1);
}

rp=&(scr->RastPort);
 SetAPen(rp, 1);
  Move(rp,130,50);
  Text(rp,"2048",4);

rp1=&(scr->RastPort);
 SetAPen(rp, 2);
 SetBPen(rp, 5);
  Move(rp1,100,63);
  Text(rp1,"   ",3);
  Move(rp1,100,70);
  Text(rp1,"1024",4);
  Move(rp1,100,77);
  Text(rp1,"   ",3);
rp2=&(scr->RastPort);
 SetAPen(rp, 3);
  Move(rp2,130,70);
  Text(rp2,"0",1);
rp3=&(scr->RastPort);
 SetAPen(rp, 4);
  Move(rp3,160,70);
  Text(rp3,"0",1);
rp4=&(scr->RastPort);
 SetAPen(rp, 5);
  Move(rp4,190,70);
  Text(rp4,"0",1);
rp5=&(scr->RastPort);
  SetAPen(rp, 6);
  Move(rp5,100,100);
  Text(rp5,"0",1);
rp6=&(scr->RastPort);
  SetAPen(rp, 7);
  Move(rp6,130,100);
  Text(rp6,"0",1);
rp7=&(scr->RastPort);
  SetAPen(rp, 8);
  Move(rp7,160,100);
  Text(rp7,"0",1);
rp8=&(scr->RastPort);
  SetAPen(rp, 9);
  Move(rp8,190,100);
  Text(rp8,"0",1);
rp9=&(scr->RastPort);
  SetAPen(rp, 10);
  Move(rp9,100,130);
  Text(rp9,"0",1);
rp10=&(scr->RastPort);
  SetAPen(rp, 11);
  Move(rp10,130,130);
  Text(rp10,"0",1);
rp11=&(scr->RastPort);
  SetAPen(rp, 12);
  Move(rp11,160,130);
  Text(rp11,"0",1);
rp12=&(scr->RastPort);
  SetAPen(rp, 13);
  Move(rp12,190,130);
  Text(rp12,"0",1);
rp13=&(scr->RastPort);
  SetAPen(rp, 14);
  Move(rp13,100,160);
  Text(rp13,"0",1);
rp14=&(scr->RastPort);
  SetAPen(rp, 15);
  Move(rp14,130,160);
  Text(rp14,"0",1);
rp15=&(scr->RastPort);
  SetAPen(rp, 16);
  Move(rp15,160,160);
  Text(rp15,"0",1);
rp16=&(scr->RastPort);
  SetAPen(rp, 17);
  Move(rp16,190,160);
  Text(rp16,"0",1);
    while(1) {
 /*   if(*joy&2){
        Move(rp1,100,100);
        Text(rp1,"SAG   ",6);}
    if(*joy&512){
         Move(rp1,100,100);
         Text(rp1,"SOL   ",6);}
    if((*joy>>1^*joy)&1){
         Move(rp1,100,100);
         Text(rp1,"ASAGI ",6);}
    if((*joy>>1^*joy)&256){
         Move(rp1,100,100);
         Text(rp1,"YUKARI",6);}
    printf("\n");

  //Move(rp1,100,100);
  //Text(rp1,"2",1);
*/
  if(!(*cia&64*PORT)){
  CloseScreen(scr);
  CloseLibrary(GfxBase);
  CloseLibrary(IntuitionBase);
  return(0);
  break;
}}  }

