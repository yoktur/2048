#include <stdio.h>
#include <graphics/gfx.h>
#include <intuition/intuition.h>

struct GfxBase *GfxBase;
struct IntuitionBase *IntuitionBase;

struct Screen *scr;
struct RastPort *rp;
struct RastPort *rp1;
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

  Move(rp,120,50);
  Text(rp,"2048",4);

rp1=&(scr->RastPort);
  Move(rp1,100,100);
  Text(rp1,"0",1);
  Move(rp1,100,100);
  Text(rp1,"2",1);
  char quit=getchar();
  if(quit="q"){
  // Delay(500);
  CloseScreen(scr);
  CloseLibrary(GfxBase);
  CloseLibrary(IntuitionBase);
  return(0);
}}
