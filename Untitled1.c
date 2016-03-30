#include <stdio.h>
#include <graphics/gfx.h>
#include <intuition/intuition.h>
#include <stdbool.h>
#include <time.h>

#define PORT 2

short *joy=0xdff008+2*PORT;
char*cia=0xbfe001;
struct GfxBase *GfxBase;
struct IntuitionBase *IntuitionBase;

struct Screen *scr;
struct RastPort *rp;
struct RastPort *rp1;

struct NewScreen nw={
        0,0,320,200,4,0,1,NULL,CUSTOMSCREEN,NULL,NULL,NULL,NULL };

int board[4][4];
int backupBoardArr[4][4];
int response=0;
int apocalypse;
int score=0;
int keyPressed=0;
char charScore[];
char charKey[];

void castIntToStringA() {
    if(keyPressed<10)
        snprintf(charKey, 6, "%d     ", keyPressed);
    else if(keyPressed<100)
        snprintf(charKey, 6, "%d    ", keyPressed);
    else if(keyPressed<1000)
        snprintf(charKey, 6, "%d   ", keyPressed);
    else
        snprintf(charKey, 6, "%d  ", keyPressed);
}
void updateMoveNumber() {
    keyPressed++;
    castIntToStringA();
    SetAPen(rp, 1);
    SetBPen(rp, 0);
    Move(rp,275,40);
    Text(rp,(&charKey[0]),5);
}
void castIntToString() {
    if(score<10)
        snprintf(charScore, 7, "%d     ", score);
    else if(score<100)
        snprintf(charScore, 7, "%d    ", score);
    else if(score<1000)
        snprintf(charScore, 7, "%d   ", score);
    else if(score<10000)
        snprintf(charScore, 7, "%d  ", score);
    else
        snprintf(charScore, 7, "%d ", score);
}
void updateScore() {
    castIntToString();
    SetAPen(rp, 1);
    SetBPen(rp, 0);
    Move(rp,70,40);
    Text(rp,(&charScore[0]),6);
}
int random_index(int x)
{
    int index;
    index=rand()%x+0;

    return index;
}
void startBoard() {
    int i=0,j=0,k=0,l=0;
    srand(time(NULL));
    i=rand() % 4;
    j=rand() % 4;
    board[i][j]=2;
    k=rand() % 4;
    l=rand() % 4;
    while(i==k && j==l){
        k=rand() % 4;
        l=rand() % 4;
    }
    board[k][l]=2;
}
void spawn() {
    int i,j,k;
    srand(time(NULL));
    do
    {
        i=rand() % 4;
        j=rand() % 4;
        k=rand() % 10;

    }while(board[i][j]);
    if(k<2)
        board[i][j]=4;
    else
        board[i][j]=2;
}
int full(){
    int k=1;
    int i=0;
    for(i=0;i<4;i++){
        int j=0;
        for(j=0;j<4;j++)
        {
            if(!board[i][j])
                k=0;
        }
    }
    return k;
}
void backupBoard() {
    int i = 0;
    for (i = 0; i < 4; i++) {
        int j = 0;
        for (j = 0; j < 4; j++) {
            backupBoardArr[i][j] = board[i][j];
        }
    }
}
bool blockMoves() {
    bool k=false;
    int i;
    for(i=0;i<4;i++) {
        int j = 0;
        for (j = 0; j < 4; j++) {
            if (backupBoardArr[i][j] != board[i][j]) {
                k = true;
                break;
            }
        }
    }
    return k;
}

void fillSpaceUp() {
    int i;
    for (i = 0; i < 4; i++) {
        int j;
        for (j = 0; j < 4; j++) {
            if (!board[j][i]) {
                int k;
                for (k = j + 1; k < 4; k++) {
                    if (board[k][i]) {
                        board[j][i] = board[k][i];
                        board[k][i] = 0;
                        break;
                    }
                }
            }
        }
    }
}
void fillSpaceDown() {
    int i;
    for (i = 0; i < 4; i++) {
        int j;
        for (j = 3; j >= 0; j--) {
            if (!board[j][i]) {
                int k;
                for (k = j - 1; k >= 0; k--) {
                    if (board[k][i]) {
                        board[j][i] = board[k][i];
                        board[k][i] = 0;
                        break;
                    }
                }
            }
        }
    }
}
void fillSpaceLeft() {
    int i;
    for (i = 0; i < 4; i++) {
        int j;
        for (j = 0; j < 4; j++) {
            if (!board[i][j]) {
                int k;
                for (k = j + 1; k < 4; k++) {
                    if (board[i][k]) {
                        board[i][j] = board[i][k];
                        board[i][k] = 0;
                        break;
                    }
                }
            }
        }
    }
}
void fillSpaceRight() {
    int i;
    for (i = 0; i < 4; i++) {
        int j;
        for (j = 3; j >= 0; j--) {
            if (!board[i][j]) {
                int k;
                for (k = j - 1; k >= 0; k--) {
                    if (board[i][k]) {
                        board[i][j] = board[i][k];
                        board[i][k] = 0;
                        break;
                    }
                }
            }
        }
    }
}

void applyUp() {
    apocalypse = 1;
    int i;
    for (i = 0; i < 4; i++) {
        int j;
        for (j = 0; j < 3; j++) {
            if (board[j][i] && board[j][i] == board[j+1][i]) {
                apocalypse = 0;
                board[j][i] += board[j + 1][i];
                board[j + 1][i] = 0;
                score += ((((log10(board[j][i])) / log10(2)) - 1) * board[j][i]);
            }
        }
    }
}
void applyDown() {
    apocalypse = 1;
    int i;
    for (i = 0; i < 4; i++) {
        int j;
        for (j = 3; j > 0; j--) {
            if (board[j][i] && board[j][i] == board[j-1][i]) {
                apocalypse = 0;
                board[j][i] += board[j-1][i];
                board[j-1][i] = 0;
                score += ((((log10(board[j][i])) / log10(2)) - 1) * board[j][i]);
            }
        }
    }
}
void applyLeft() {
    apocalypse = 1;
    int i;
    for (i = 0; i < 4; i++) {
        int j;
        for (j = 0; j < 3; j++) {
            if(board[i][j]&&board[i][j]==board[i][j+1]){
                apocalypse = 0;
                board[i][j]+=board[i][j+1];
                board[i][j+1]=0;
                score += ((((log10(board[i][j])) / log10(2)) - 1) * board[i][j]);
            }
        }
    }
}
void applyRight() {
    apocalypse = 1;
    int i;
    for (i = 0; i < 4; i++) {
        int j;
        for (j = 3; j > 0; j--) {
            if(board[i][j]&&board[i][j]==board[i][j-1]){
                apocalypse = 0;
                board[i][j]+=board[i][j-1];
                board[i][j-1]=0;
                score += ((((log10(board[i][j])) / log10(2)) - 1) * board[i][j]);
            }
        }
    }
}

void updateBoard() {

    rp1=&(scr->RastPort);

//Board1
    if(board[0][0]==0){
        SetAPen(rp, 1);
        SetBPen(rp, 15);
        Move(rp1,70,53);
        Text(rp1,"    ",4);
        Move(rp1,70,61);
        Text(rp1,"    ",4);
        Move(rp1,70,69);
        Text(rp1,"    ",4);
    }

    if(board[0][0]==2){
        SetAPen(rp, 14);
        SetBPen(rp, 12);
        Move(rp1,70,53);
        Text(rp1,"    ",4);
        Move(rp1,70,61);
        Text(rp1," 2  ",4);
        Move(rp1,70,69);
        Text(rp1,"    ",4);
    }

    if(board[0][0]==4){
        SetAPen(rp, 4);
        SetBPen(rp, 11);
        Move(rp1,70,53);
        Text(rp1,"    ",4);
        Move(rp1,70,61);
        Text(rp1," 4  ",4);
        Move(rp1,70,69);
        Text(rp1,"    ",4);
    }

    if(board[0][0]==8){
        SetAPen(rp, 2);
        SetBPen(rp, 9);
        Move(rp1,70,53);
        Text(rp1,"    ",4);
        Move(rp1,70,61);
        Text(rp1," 8  ",4);
        Move(rp1,70,69);
        Text(rp1,"    ",4);
    }

    if(board[0][0]==16){
        SetAPen(rp, 9);
        SetBPen(rp, 10);
        Move(rp1,70,53);
        Text(rp1,"    ",4);
        Move(rp1,70,61);
        Text(rp1," 16 ",4);
        Move(rp1,70,69);
        Text(rp1,"    ",4);
    }

    if(board[0][0]==32){
        SetAPen(rp, 2);
        SetBPen(rp, 5);
        Move(rp1,70,53);
        Text(rp1,"    ",4);
        Move(rp1,70,61);
        Text(rp1," 32 ",4);
        Move(rp1,70,69);
        Text(rp1,"    ",4);
    }

    if(board[0][0]==64){
        SetAPen(rp, 9);
        SetBPen(rp, 6);
        Move(rp1,70,53);
        Text(rp1,"    ",4);
        Move(rp1,70,61);
        Text(rp1," 64  ",4);
        Move(rp1,70,69);
        Text(rp1,"    ",4);
    }

    if(board[0][0]==128){
        SetAPen(rp, 12);
        SetBPen(rp, 14);
        Move(rp1,70,53);
        Text(rp1,"    ",4);
        Move(rp1,70,61);
        Text(rp1,"128 ",4);
        Move(rp1,70,69);
        Text(rp1,"    ",4);
    }

    if(board[0][0]==256){
        SetAPen(rp, 11);
        SetBPen(rp, 4);
        Move(rp1,70,53);
        Text(rp1,"    ",4);
        Move(rp1,70,61);
        Text(rp1,"256 ",4);
        Move(rp1,70,69);
        Text(rp1,"    ",4);
    }

    if(board[0][0]==512){
        SetAPen(rp, 6);
        SetBPen(rp, 13);
        Move(rp1,70,53);
        Text(rp1,"    ",4);
        Move(rp1,70,61);
        Text(rp1,"512 ",4);
        Move(rp1,70,69);
        Text(rp1,"    ",4);
    }

    if(board[0][0]==1024){
        SetAPen(rp, 14);
        SetBPen(rp, 1);
        Move(rp1,70,53);
        Text(rp1,"    ",4);
        Move(rp1,70,61);
        Text(rp1,"1024");
        Move(rp1,70,69);
        Text(rp1,"    ",4);
    }

    if(board[0][0]==2048){
        SetAPen(rp, 1);
        SetBPen(rp, 2);
        Move(rp1,70,53);
        Text(rp1,"    ",4);
        Move(rp1,70,61);
        Text(rp1,"2048");
        Move(rp1,70,69);
        Text(rp1,"    ",4);
    }
//Board2
    if(board[0][1]==0){
        SetAPen(rp, 1);
        SetBPen(rp, 15);
        Move(rp1,103,53);
        Text(rp1,"    ",4);
        Move(rp1,103,61);
        Text(rp1,"    ",4);
        Move(rp1,103,69);
        Text(rp1,"    ",4);
    }

    if(board[0][1]==2){
        SetAPen(rp, 14);
        SetBPen(rp, 12);
        Move(rp1,103,53);
        Text(rp1,"    ",4);
        Move(rp1,103,61);
        Text(rp1," 2  ",4);
        Move(rp1,103,69);
        Text(rp1,"    ",4);
    }

    if(board[0][1]==4){
        SetAPen(rp, 4);
        SetBPen(rp, 11);
        Move(rp1,103,53);
        Text(rp1,"    ",4);
        Move(rp1,103,61);
        Text(rp1," 4  ",4);
        Move(rp1,103,69);
        Text(rp1,"    ",4);
    }

    if(board[0][1]==8){
        SetAPen(rp, 2);
        SetBPen(rp, 9);
        Move(rp1,103,53);
        Text(rp1,"    ",4);
        Move(rp1,103,61);
        Text(rp1," 8  ",4);
        Move(rp1,103,69);
        Text(rp1,"    ",4);
    }

    if(board[0][1]==16){
        SetAPen(rp, 9);
        SetBPen(rp, 10);
        Move(rp1,103,53);
        Text(rp1,"    ",4);
        Move(rp1,103,61);
        Text(rp1," 16 ",4);
        Move(rp1,103,69);
        Text(rp1,"    ",4);
    }

    if(board[0][1]==32){
        SetAPen(rp, 2);
        SetBPen(rp, 5);
        Move(rp1,103,53);
        Text(rp1,"    ",4);
        Move(rp1,103,61);
        Text(rp1," 32 ",4);
        Move(rp1,103,69);
        Text(rp1,"    ",4);
    }

    if(board[0][1]==64){
        SetAPen(rp, 9);
        SetBPen(rp, 6);
        Move(rp1,103,53);
        Text(rp1,"    ",4);
        Move(rp1,103,61);
        Text(rp1," 64  ",4);
        Move(rp1,103,69);
        Text(rp1,"    ",4);
    }

    if(board[0][1]==128){
        SetAPen(rp, 12);
        SetBPen(rp, 14);
        Move(rp1,103,53);
        Text(rp1,"    ",4);
        Move(rp1,103,61);
        Text(rp1,"128 ",4);
        Move(rp1,103,69);
        Text(rp1,"    ",4);
    }

    if(board[0][1]==256){
        SetAPen(rp, 11);
        SetBPen(rp, 4);
        Move(rp1,103,53);
        Text(rp1,"    ",4);
        Move(rp1,103,61);
        Text(rp1,"256 ",4);
        Move(rp1,103,69);
        Text(rp1,"    ",4);
    }

    if(board[0][1]==512){
        SetAPen(rp, 6);
        SetBPen(rp, 13);
        Move(rp1,103,53);
        Text(rp1,"    ",4);
        Move(rp1,103,61);
        Text(rp1,"512 ",4);
        Move(rp1,103,69);
        Text(rp1,"    ",4);
    }

    if(board[0][1]==1024){
        SetAPen(rp, 14);
        SetBPen(rp, 1);
        Move(rp1,103,53);
        Text(rp1,"    ",4);
        Move(rp1,103,61);
        Text(rp1,"1024");
        Move(rp1,103,69);
        Text(rp1,"    ",4);
    }

    if(board[0][1]==2048){
        SetAPen(rp, 1);
        SetBPen(rp, 2);
        Move(rp1,103,53);
        Text(rp1,"    ",4);
        Move(rp1,103,61);
        Text(rp1,"2048");
        Move(rp1,103,69);
        Text(rp1,"    ",4);
    }

//Board3
    if(board[0][2]==0){
        SetAPen(rp, 1);
        SetBPen(rp, 15);
        Move(rp1,136,53);
        Text(rp1,"    ",4);
        Move(rp1,136,61);
        Text(rp1,"    ",4);
        Move(rp1,136,69);
        Text(rp1,"    ",4);
    }

    if(board[0][2]==2){
        SetAPen(rp, 14);
        SetBPen(rp, 12);
        Move(rp1,136,53);
        Text(rp1,"    ",4);
        Move(rp1,136,61);
        Text(rp1," 2  ",4);
        Move(rp1,136,69);
        Text(rp1,"    ",4);
    }

    if(board[0][2]==4){
        SetAPen(rp, 4);
        SetBPen(rp, 11);
        Move(rp1,136,53);
        Text(rp1,"    ",4);
        Move(rp1,136,61);
        Text(rp1," 4  ",4);
        Move(rp1,136,69);
        Text(rp1,"    ",4);
    }

    if(board[0][2]==8){
        SetAPen(rp, 2);
        SetBPen(rp, 9);
        Move(rp1,136,53);
        Text(rp1,"    ",4);
        Move(rp1,136,61);
        Text(rp1," 8  ",4);
        Move(rp1,136,69);
        Text(rp1,"    ",4);
    }

    if(board[0][2]==16){
        SetAPen(rp, 9);
        SetBPen(rp, 10);
        Move(rp1,136,53);
        Text(rp1,"    ",4);
        Move(rp1,136,61);
        Text(rp1," 16 ",4);
        Move(rp1,136,69);
        Text(rp1,"    ",4);
    }

    if(board[0][2]==32){
        SetAPen(rp, 2);
        SetBPen(rp, 5);
        Move(rp1,136,53);
        Text(rp1,"    ",4);
        Move(rp1,136,61);
        Text(rp1," 32 ",4);
        Move(rp1,136,69);
        Text(rp1,"    ",4);
    }

    if(board[0][2]==64){
        SetAPen(rp, 9);
        SetBPen(rp, 6);
        Move(rp1,136,53);
        Text(rp1,"    ",4);
        Move(rp1,136,61);
        Text(rp1," 64  ",4);
        Move(rp1,136,69);
        Text(rp1,"    ",4);
    }

    if(board[0][2]==128){
        SetAPen(rp, 12);
        SetBPen(rp, 14);
        Move(rp1,136,53);
        Text(rp1,"    ",4);
        Move(rp1,136,61);
        Text(rp1,"128 ",4);
        Move(rp1,136,69);
        Text(rp1,"    ",4);
    }

    if(board[0][2]==256){
        SetAPen(rp, 11);
        SetBPen(rp, 4);
        Move(rp1,136,53);
        Text(rp1,"    ",4);
        Move(rp1,136,61);
        Text(rp1,"256 ",4);
        Move(rp1,136,69);
        Text(rp1,"    ",4);
    }

    if(board[0][2]==512){
        SetAPen(rp, 6);
        SetBPen(rp, 13);
        Move(rp1,136,53);
        Text(rp1,"    ",4);
        Move(rp1,136,61);
        Text(rp1,"512 ",4);
        Move(rp1,136,69);
        Text(rp1,"    ",4);
    }

    if(board[0][2]==1024){
        SetAPen(rp, 14);
        SetBPen(rp, 1);
        Move(rp1,136,53);
        Text(rp1,"    ",4);
        Move(rp1,136,61);
        Text(rp1,"1024");
        Move(rp1,136,69);
        Text(rp1,"    ",4);
    }

    if(board[0][2]==2048){
        SetAPen(rp, 1);
        SetBPen(rp, 2);
        Move(rp1,136,53);
        Text(rp1,"    ",4);
        Move(rp1,136,61);
        Text(rp1,"2048");
        Move(rp1,136,69);
        Text(rp1,"    ",4);
    }
//Board4
    if(board[0][3]==0){
        SetAPen(rp, 0);
        SetBPen(rp, 15);
        Move(rp1,169,53);
        Text(rp1,"    ",4);
        Move(rp1,169,61);
        Text(rp1,"    ",4);
        Move(rp1,169,69);
        Text(rp1,"    ",4);
    }

    if(board[0][3]==2){
        SetAPen(rp, 14);
        SetBPen(rp, 12);
        Move(rp1,169,53);
        Text(rp1,"    ",4);
        Move(rp1,169,61);
        Text(rp1," 2  ",4);
        Move(rp1,169,69);
        Text(rp1,"    ",4);
    }

    if(board[0][3]==4){
        SetAPen(rp, 4);
        SetBPen(rp, 11);
        Move(rp1,169,53);
        Text(rp1,"    ",4);
        Move(rp1,169,61);
        Text(rp1," 4  ",4);
        Move(rp1,169,69);
        Text(rp1,"    ",4);
    }

    if(board[0][3]==8){
        SetAPen(rp, 2);
        SetBPen(rp, 9);
        Move(rp1,169,53);
        Text(rp1,"    ",4);
        Move(rp1,169,61);
        Text(rp1," 8  ",4);
        Move(rp1,169,69);
        Text(rp1,"    ",4);
    }

    if(board[0][3]==16){
        SetAPen(rp, 9);
        SetBPen(rp, 10);
        Move(rp1,169,53);
        Text(rp1,"    ",4);
        Move(rp1,169,61);
        Text(rp1," 16 ",4);
        Move(rp1,169,69);
        Text(rp1,"    ",4);
    }

    if(board[0][3]==32){
        SetAPen(rp, 2);
        SetBPen(rp, 5);
        Move(rp1,169,53);
        Text(rp1,"    ",4);
        Move(rp1,169,61);
        Text(rp1," 32 ",4);
        Move(rp1,169,69);
        Text(rp1,"    ",4);
    }

    if(board[0][3]==64){
        SetAPen(rp, 9);
        SetBPen(rp, 6);
        Move(rp1,169,53);
        Text(rp1,"    ",4);
        Move(rp1,169,61);
        Text(rp1," 64  ",4);
        Move(rp1,169,69);
        Text(rp1,"    ",4);
    }

    if(board[0][3]==128){
        SetAPen(rp, 12);
        SetBPen(rp, 14);
        Move(rp1,169,53);
        Text(rp1,"    ",4);
        Move(rp1,169,61);
        Text(rp1,"128 ",4);
        Move(rp1,169,69);
        Text(rp1,"    ",4);
    }

    if(board[0][3]==256){
        SetAPen(rp, 11);
        SetBPen(rp, 4);
        Move(rp1,169,53);
        Text(rp1,"    ",4);
        Move(rp1,169,61);
        Text(rp1,"256 ",4);
        Move(rp1,169,69);
        Text(rp1,"    ",4);
    }

    if(board[0][3]==512){
        SetAPen(rp, 6);
        SetBPen(rp, 13);
        Move(rp1,169,53);
        Text(rp1,"    ",4);
        Move(rp1,169,61);
        Text(rp1,"512 ",4);
        Move(rp1,169,69);
        Text(rp1,"    ",4);
    }

    if(board[0][3]==1024){
        SetAPen(rp, 14);
        SetBPen(rp, 1);
        Move(rp1,169,53);
        Text(rp1,"    ",4);
        Move(rp1,169,61);
        Text(rp1,"1024");
        Move(rp1,169,69);
        Text(rp1,"    ",4);
    }

    if(board[0][3]==2048){
        SetAPen(rp, 1);
        SetBPen(rp, 2);
        Move(rp1,169,53);
        Text(rp1,"    ",4);
        Move(rp1,169,61);
        Text(rp1,"2048");
        Move(rp1,169,69);
        Text(rp1,"    ",4);
    }
//Board Finisher
    SetAPen(rp, 0);
    SetBPen(rp, 0);
    Move(rp1,202,52);
    Text(rp1,"    ",4);
    Move(rp1,202,60);
    Text(rp1,"    ",4);
    Move(rp1,202,68);
    Text(rp1,"    ",4);
    Move(rp1,202,70);
    Text(rp1,"    ",4);

//Board5
    if(board[1][0]==0){
        SetAPen(rp, 0);
        SetBPen(rp, 15);
        Move(rp1,70,78);
        Text(rp1,"    ",4);
        Move(rp1,70,86);
        Text(rp1,"    ",4);
        Move(rp1,70,94);
        Text(rp1,"    ",4);
    }

    if(board[1][0]==2){
        SetAPen(rp, 14);
        SetBPen(rp, 12);
        Move(rp1,70,78);
        Text(rp1,"    ",4);
        Move(rp1,70,86);
        Text(rp1," 2  ",4);
        Move(rp1,70,94);
        Text(rp1,"    ",4);
    }

    if(board[1][0]==4){
        SetAPen(rp, 4);
        SetBPen(rp, 11);
        Move(rp1,70,78);
        Text(rp1,"    ",4);
        Move(rp1,70,86);
        Text(rp1," 4  ",4);
        Move(rp1,70,94);
        Text(rp1,"    ",4);
    }

    if(board[1][0]==8){
        SetAPen(rp, 2);
        SetBPen(rp, 9);
        Move(rp1,70,78);
        Text(rp1,"    ",4);
        Move(rp1,70,86);
        Text(rp1," 8  ",4);
        Move(rp1,70,94);
        Text(rp1,"    ",4);
    }

    if(board[1][0]==16){
        SetAPen(rp, 9);
        SetBPen(rp, 10);
        Move(rp1,70,78);
        Text(rp1,"    ",4);
        Move(rp1,70,86);
        Text(rp1," 16 ",4);
        Move(rp1,70,94);
        Text(rp1,"    ",4);
    }

    if(board[1][0]==32){
        SetAPen(rp, 2);
        SetBPen(rp, 5);
        Move(rp1,70,78);
        Text(rp1,"    ",4);
        Move(rp1,70,86);
        Text(rp1," 32 ",4);
        Move(rp1,70,94);
        Text(rp1,"    ",4);
    }

    if(board[1][0]==64){
        SetAPen(rp, 9);
        SetBPen(rp, 6);
        Move(rp1,70,78);
        Text(rp1,"    ",4);
        Move(rp1,70,86);
        Text(rp1," 64  ",4);
        Move(rp1,70,94);
        Text(rp1,"    ",4);
    }

    if(board[1][0]==128){
        SetAPen(rp, 12);
        SetBPen(rp, 14);
        Move(rp1,70,78);
        Text(rp1,"    ",4);
        Move(rp1,70,86);
        Text(rp1,"128 ",4);
        Move(rp1,70,94);
        Text(rp1,"    ",4);
    }

    if(board[1][0]==256){
        SetAPen(rp, 11);
        SetBPen(rp, 4);
        Move(rp1,70,78);
        Text(rp1,"    ",4);
        Move(rp1,70,86);
        Text(rp1,"256 ",4);
        Move(rp1,70,94);
        Text(rp1,"    ",4);
    }

    if(board[1][0]==512){
        SetAPen(rp, 6);
        SetBPen(rp, 13);
        Move(rp1,70,78);
        Text(rp1,"    ",4);
        Move(rp1,70,86);
        Text(rp1,"512 ",4);
        Move(rp1,70,94);
        Text(rp1,"    ",4);
    }

    if(board[1][0]==1024){
        SetAPen(rp, 14);
        SetBPen(rp, 1);
        Move(rp1,70,78);
        Text(rp1,"    ",4);
        Move(rp1,70,86);
        Text(rp1,"1024");
        Move(rp1,70,94);
        Text(rp1,"    ",4);
    }

    if(board[1][0]==2048){
        SetAPen(rp, 1);
        SetBPen(rp, 2);
        Move(rp1,70,78);
        Text(rp1,"    ",4);
        Move(rp1,70,86);
        Text(rp1,"2048");
        Move(rp1,70,94);
        Text(rp1,"    ",4);
    }

    //Board6
    if(board[1][1]==0){
        SetAPen(rp, 0);
        SetBPen(rp, 15);
        Move(rp1,103,78);
        Text(rp1,"    ",4);
        Move(rp1,103,86);
        Text(rp1,"    ",4);
        Move(rp1,103,94);
        Text(rp1,"    ",4);
    }

    if(board[1][1]==2){
        SetAPen(rp, 14);
        SetBPen(rp, 12);
        Move(rp1,103,78);
        Text(rp1,"    ",4);
        Move(rp1,103,86);
        Text(rp1," 2  ",4);
        Move(rp1,103,94);
        Text(rp1,"    ",4);
    }

    if(board[1][1]==4){
        SetAPen(rp, 4);
        SetBPen(rp, 11);
        Move(rp1,103,78);
        Text(rp1,"    ",4);
        Move(rp1,103,86);
        Text(rp1," 4  ",4);
        Move(rp1,103,94);
        Text(rp1,"    ",4);
    }

    if(board[1][1]==8){
        SetAPen(rp, 2);
        SetBPen(rp, 9);
        Move(rp1,103,78);
        Text(rp1,"    ",4);
        Move(rp1,103,86);
        Text(rp1," 8  ",4);
        Move(rp1,103,94);
        Text(rp1,"    ",4);
    }

    if(board[1][1]==16){
        SetAPen(rp, 9);
        SetBPen(rp, 10);
        Move(rp1,103,78);
        Text(rp1,"    ",4);
        Move(rp1,103,86);
        Text(rp1," 16 ",4);
        Move(rp1,103,94);
        Text(rp1,"    ",4);
    }

    if(board[1][1]==32){
        SetAPen(rp, 2);
        SetBPen(rp, 5);
        Move(rp1,103,78);
        Text(rp1,"    ",4);
        Move(rp1,103,86);
        Text(rp1," 32 ",4);
        Move(rp1,103,94);
        Text(rp1,"    ",4);
    }

    if(board[1][1]==64){
        SetAPen(rp, 9);
        SetBPen(rp, 6);
        Move(rp1,103,78);
        Text(rp1,"    ",4);
        Move(rp1,103,86);
        Text(rp1," 64  ",4);
        Move(rp1,103,94);
        Text(rp1,"    ",4);
    }

    if(board[1][1]==128){
        SetAPen(rp, 12);
        SetBPen(rp, 14);
        Move(rp1,103,78);
        Text(rp1,"    ",4);
        Move(rp1,103,86);
        Text(rp1,"128 ",4);
        Move(rp1,103,94);
        Text(rp1,"    ",4);
    }

    if(board[1][1]==256){
        SetAPen(rp, 11);
        SetBPen(rp, 4);
        Move(rp1,103,78);
        Text(rp1,"    ",4);
        Move(rp1,103,86);
        Text(rp1,"256 ",4);
        Move(rp1,103,94);
        Text(rp1,"    ",4);
    }

    if(board[1][1]==512){
        SetAPen(rp, 6);
        SetBPen(rp, 13);
        Move(rp1,103,78);
        Text(rp1,"    ",4);
        Move(rp1,103,86);
        Text(rp1,"512 ",4);
        Move(rp1,103,94);
        Text(rp1,"    ",4);
    }

    if(board[1][1]==1024){
        SetAPen(rp, 14);
        SetBPen(rp, 1);
        Move(rp1,103,78);
        Text(rp1,"    ",4);
        Move(rp1,103,86);
        Text(rp1,"1024");
        Move(rp1,103,94);
        Text(rp1,"    ",4);
    }

    if(board[1][1]==2048){
        SetAPen(rp, 1);
        SetBPen(rp, 2);
        Move(rp1,103,78);
        Text(rp1,"    ",4);
        Move(rp1,103,86);
        Text(rp1,"2048");
        Move(rp1,103,94);
        Text(rp1,"    ",4);
    }

    //Board7
    if(board[1][2]==0){
        SetAPen(rp, 0);
        SetBPen(rp, 15);
        Move(rp1,136,78);
        Text(rp1,"    ",4);
        Move(rp1,136,86);
        Text(rp1,"    ",4);
        Move(rp1,136,94);
        Text(rp1,"    ",4);
    }

    if(board[1][2]==2){
        SetAPen(rp, 14);
        SetBPen(rp, 12);
        Move(rp1,136,78);
        Text(rp1,"    ",4);
        Move(rp1,136,86);
        Text(rp1," 2  ",4);
        Move(rp1,136,94);
        Text(rp1,"    ",4);
    }

    if(board[1][2]==4){
        SetAPen(rp, 4);
        SetBPen(rp, 11);
        Move(rp1,136,78);
        Text(rp1,"    ",4);
        Move(rp1,136,86);
        Text(rp1," 4  ",4);
        Move(rp1,136,94);
        Text(rp1,"    ",4);
    }

    if(board[1][2]==8){
        SetAPen(rp, 2);
        SetBPen(rp, 9);
        Move(rp1,136,78);
        Text(rp1,"    ",4);
        Move(rp1,136,86);
        Text(rp1," 8  ",4);
        Move(rp1,136,94);
        Text(rp1,"    ",4);
    }

    if(board[1][2]==16){
        SetAPen(rp, 9);
        SetBPen(rp, 10);
        Move(rp1,136,78);
        Text(rp1,"    ",4);
        Move(rp1,136,86);
        Text(rp1," 16 ",4);
        Move(rp1,136,94);
        Text(rp1,"    ",4);
    }

    if(board[1][2]==32){
        SetAPen(rp, 2);
        SetBPen(rp, 5);
        Move(rp1,136,78);
        Text(rp1,"    ",4);
        Move(rp1,136,86);
        Text(rp1," 32 ",4);
        Move(rp1,136,94);
        Text(rp1,"    ",4);
    }

    if(board[1][2]==64){
        SetAPen(rp, 9);
        SetBPen(rp, 6);
        Move(rp1,136,78);
        Text(rp1,"    ",4);
        Move(rp1,136,86);
        Text(rp1," 64  ",4);
        Move(rp1,136,94);
        Text(rp1,"    ",4);
    }

    if(board[1][2]==128){
        SetAPen(rp, 12);
        SetBPen(rp, 14);
        Move(rp1,136,78);
        Text(rp1,"    ",4);
        Move(rp1,136,86);
        Text(rp1,"128 ",4);
        Move(rp1,136,94);
        Text(rp1,"    ",4);
    }

    if(board[1][2]==256){
        SetAPen(rp, 11);
        SetBPen(rp, 4);
        Move(rp1,136,78);
        Text(rp1,"    ",4);
        Move(rp1,136,86);
        Text(rp1,"256 ",4);
        Move(rp1,136,94);
        Text(rp1,"    ",4);
    }

    if(board[1][2]==512){
        SetAPen(rp, 6);
        SetBPen(rp, 13);
        Move(rp1,136,78);
        Text(rp1,"    ",4);
        Move(rp1,136,86);
        Text(rp1,"512 ",4);
        Move(rp1,136,94);
        Text(rp1,"    ",4);
    }

    if(board[1][2]==1024){
        SetAPen(rp, 14);
        SetBPen(rp, 1);
        Move(rp1,136,78);
        Text(rp1,"    ",4);
        Move(rp1,136,86);
        Text(rp1,"1024");
        Move(rp1,136,94);
        Text(rp1,"    ",4);
    }

    if(board[1][2]==2048){
        SetAPen(rp, 1);
        SetBPen(rp, 2);
        Move(rp1,136,78);
        Text(rp1,"    ",4);
        Move(rp1,136,86);
        Text(rp1,"2048");
        Move(rp1,136,94);
        Text(rp1,"    ",4);
    }

    //Board8
    if(board[1][3]==0){
        SetAPen(rp, 0);
        SetBPen(rp, 15);
        Move(rp1,169,78);
        Text(rp1,"    ",4);
        Move(rp1,169,86);
        Text(rp1,"    ",4);
        Move(rp1,169,94);
        Text(rp1,"    ",4);
    }

    if(board[1][3]==2){
        SetAPen(rp, 14);
        SetBPen(rp, 12);
        Move(rp1,169,78);
        Text(rp1,"    ",4);
        Move(rp1,169,86);
        Text(rp1," 2  ",4);
        Move(rp1,169,94);
        Text(rp1,"    ",4);
    }

    if(board[1][3]==4){
        SetAPen(rp, 4);
        SetBPen(rp, 11);
        Move(rp1,169,78);
        Text(rp1,"    ",4);
        Move(rp1,169,86);
        Text(rp1," 4  ",4);
        Move(rp1,169,94);
        Text(rp1,"    ",4);
    }

    if(board[1][3]==8){
        SetAPen(rp, 2);
        SetBPen(rp, 9);
        Move(rp1,169,78);
        Text(rp1,"    ",4);
        Move(rp1,169,86);
        Text(rp1," 8  ",4);
        Move(rp1,169,94);
        Text(rp1,"    ",4);
    }

    if(board[1][3]==16){
        SetAPen(rp, 9);
        SetBPen(rp, 10);
        Move(rp1,169,78);
        Text(rp1,"    ",4);
        Move(rp1,169,86);
        Text(rp1," 16 ",4);
        Move(rp1,169,94);
        Text(rp1,"    ",4);
    }

    if(board[1][3]==32){
        SetAPen(rp, 2);
        SetBPen(rp, 5);
        Move(rp1,169,78);
        Text(rp1,"    ",4);
        Move(rp1,169,86);
        Text(rp1," 32 ",4);
        Move(rp1,169,94);
        Text(rp1,"    ",4);
    }

    if(board[1][3]==64){
        SetAPen(rp, 9);
        SetBPen(rp, 6);
        Move(rp1,169,78);
        Text(rp1,"    ",4);
        Move(rp1,169,86);
        Text(rp1," 64  ",4);
        Move(rp1,169,94);
        Text(rp1,"    ",4);
    }

    if(board[1][3]==128){
        SetAPen(rp, 12);
        SetBPen(rp, 14);
        Move(rp1,169,78);
        Text(rp1,"    ",4);
        Move(rp1,169,86);
        Text(rp1,"128 ",4);
        Move(rp1,169,94);
        Text(rp1,"    ",4);
    }

    if(board[1][3]==256){
        SetAPen(rp, 11);
        SetBPen(rp, 4);
        Move(rp1,169,78);
        Text(rp1,"    ",4);
        Move(rp1,169,86);
        Text(rp1,"256 ",4);
        Move(rp1,169,94);
        Text(rp1,"    ",4);
    }

    if(board[1][3]==512){
        SetAPen(rp, 6);
        SetBPen(rp, 13);
        Move(rp1,169,78);
        Text(rp1,"    ",4);
        Move(rp1,169,86);
        Text(rp1,"512 ",4);
        Move(rp1,169,94);
        Text(rp1,"    ",4);
    }

    if(board[1][3]==1024){
        SetAPen(rp, 14);
        SetBPen(rp, 1);
        Move(rp1,169,78);
        Text(rp1,"    ",4);
        Move(rp1,169,86);
        Text(rp1,"1024");
        Move(rp1,169,94);
        Text(rp1,"    ",4);
    }

    if(board[1][3]==2048){
        SetAPen(rp, 1);
        SetBPen(rp, 2);
        Move(rp1,169,78);
        Text(rp1,"    ",4);
        Move(rp1,169,86);
        Text(rp1,"2048");
        Move(rp1,169,94);
        Text(rp1,"    ",4);
    }
//Board Finisher
    SetAPen(rp, 0);
    SetBPen(rp, 0);
    Move(rp1,202,78);
    Text(rp1,"    ",4);
    Move(rp1,202,86);
    Text(rp1,"    ",4);
    Move(rp1,202,94);
    Text(rp1,"    ",4);
    Move(rp1,202,95);
    Text(rp1,"    ",4);
    //Board9
    if(board[2][0]==0){
        SetAPen(rp, 0);
        SetBPen(rp, 15);
        Move(rp1,70,103);
        Text(rp1,"    ",4);
        Move(rp1,70,111);
        Text(rp1,"    ",4);
        Move(rp1,70,119);
        Text(rp1,"    ",4);
    }

    if(board[2][0]==2){
        SetAPen(rp, 14);
        SetBPen(rp, 12);
        Move(rp1,70,103);
        Text(rp1,"    ",4);
        Move(rp1,70,111);
        Text(rp1," 2  ",4);
        Move(rp1,70,119);
        Text(rp1,"    ",4);
    }

    if(board[2][0]==4){
        SetAPen(rp, 4);
        SetBPen(rp, 11);
        Move(rp1,70,103);
        Text(rp1,"    ",4);
        Move(rp1,70,111);
        Text(rp1," 4  ",4);
        Move(rp1,70,119);
        Text(rp1,"    ",4);
    }

    if(board[2][0]==8){
        SetAPen(rp, 2);
        SetBPen(rp, 9);
        Move(rp1,70,103);
        Text(rp1,"    ",4);
        Move(rp1,70,111);
        Text(rp1," 8  ",4);
        Move(rp1,70,119);
        Text(rp1,"    ",4);
    }

    if(board[2][0]==16){
        SetAPen(rp, 9);
        SetBPen(rp, 10);
        Move(rp1,70,103);
        Text(rp1,"    ",4);
        Move(rp1,70,111);
        Text(rp1," 16 ",4);
        Move(rp1,70,119);
        Text(rp1,"    ",4);
    }

    if(board[2][0]==32){
        SetAPen(rp, 2);
        SetBPen(rp, 5);
        Move(rp1,70,103);
        Text(rp1,"    ",4);
        Move(rp1,70,111);
        Text(rp1," 32 ",4);
        Move(rp1,70,119);
        Text(rp1,"    ",4);
    }

    if(board[2][0]==64){
        SetAPen(rp, 9);
        SetBPen(rp, 6);
        Move(rp1,70,103);
        Text(rp1,"    ",4);
        Move(rp1,70,111);
        Text(rp1," 64  ",4);
        Move(rp1,70,119);
        Text(rp1,"    ",4);
    }

    if(board[2][0]==128){
        SetAPen(rp, 12);
        SetBPen(rp, 14);
        Move(rp1,70,103);
        Text(rp1,"    ",4);
        Move(rp1,70,111);
        Text(rp1,"128 ",4);
        Move(rp1,70,119);
        Text(rp1,"    ",4);
    }

    if(board[2][0]==256){
        SetAPen(rp, 11);
        SetBPen(rp, 4);
        Move(rp1,70,103);
        Text(rp1,"    ",4);
        Move(rp1,70,111);
        Text(rp1,"256 ",4);
        Move(rp1,70,119);
        Text(rp1,"    ",4);
    }

    if(board[2][0]==512){
        SetAPen(rp, 6);
        SetBPen(rp, 13);
        Move(rp1,70,103);
        Text(rp1,"    ",4);
        Move(rp1,70,111);
        Text(rp1,"512 ",4);
        Move(rp1,70,119);
        Text(rp1,"    ",4);
    }

    if(board[2][0]==1024){
        SetAPen(rp, 14);
        SetBPen(rp, 1);
        Move(rp1,70,103);
        Text(rp1,"    ",4);
        Move(rp1,70,111);
        Text(rp1,"1024");
        Move(rp1,70,119);
        Text(rp1,"    ",4);
    }

    if(board[2][0]==2048){
        SetAPen(rp, 1);
        SetBPen(rp, 2);
        Move(rp1,70,103);
        Text(rp1,"    ",4);
        Move(rp1,70,111);
        Text(rp1,"2048");
        Move(rp1,70,119);
        Text(rp1,"    ",4);
    }

    //Board10
    if(board[2][1]==0){
        SetAPen(rp, 0);
        SetBPen(rp, 15);
        Move(rp1,103,103);
        Text(rp1,"    ",4);
        Move(rp1,103,111);
        Text(rp1,"    ",4);
        Move(rp1,103,119);
        Text(rp1,"    ",4);
    }

    if(board[2][1]==2){
        SetAPen(rp, 14);
        SetBPen(rp, 12);
        Move(rp1,103,103);
        Text(rp1,"    ",4);
        Move(rp1,103,111);
        Text(rp1," 2  ",4);
        Move(rp1,103,119);
        Text(rp1,"    ",4);
    }

    if(board[2][1]==4){
        SetAPen(rp, 4);
        SetBPen(rp, 11);
        Move(rp1,103,103);
        Text(rp1,"    ",4);
        Move(rp1,103,111);
        Text(rp1," 4  ",4);
        Move(rp1,103,119);
        Text(rp1,"    ",4);
    }

    if(board[2][1]==8){
        SetAPen(rp, 2);
        SetBPen(rp, 9);
        Move(rp1,103,103);
        Text(rp1,"    ",4);
        Move(rp1,103,111);
        Text(rp1," 8  ",4);
        Move(rp1,103,119);
        Text(rp1,"    ",4);
    }

    if(board[2][1]==16){
        SetAPen(rp, 9);
        SetBPen(rp, 10);
        Move(rp1,103,103);
        Text(rp1,"    ",4);
        Move(rp1,103,111);
        Text(rp1," 16 ",4);
        Move(rp1,103,119);
        Text(rp1,"    ",4);
    }

    if(board[2][1]==32){
        SetAPen(rp, 2);
        SetBPen(rp, 5);
        Move(rp1,103,103);
        Text(rp1,"    ",4);
        Move(rp1,103,111);
        Text(rp1," 32 ",4);
        Move(rp1,103,119);
        Text(rp1,"    ",4);
    }

    if(board[2][1]==64){
        SetAPen(rp, 9);
        SetBPen(rp, 6);
        Move(rp1,103,103);
        Text(rp1,"    ",4);
        Move(rp1,103,111);
        Text(rp1," 64  ",4);
        Move(rp1,103,119);
        Text(rp1,"    ",4);
    }

    if(board[2][1]==128){
        SetAPen(rp, 12);
        SetBPen(rp, 14);
        Move(rp1,103,103);
        Text(rp1,"    ",4);
        Move(rp1,103,111);
        Text(rp1,"128 ",4);
        Move(rp1,103,119);
        Text(rp1,"    ",4);
    }

    if(board[2][1]==256){
        SetAPen(rp, 11);
        SetBPen(rp, 4);
        Move(rp1,103,103);
        Text(rp1,"    ",4);
        Move(rp1,103,111);
        Text(rp1,"256 ",4);
        Move(rp1,103,119);
        Text(rp1,"    ",4);
    }

    if(board[2][1]==512){
        SetAPen(rp, 6);
        SetBPen(rp, 13);
        Move(rp1,103,103);
        Text(rp1,"    ",4);
        Move(rp1,103,111);
        Text(rp1,"512 ",4);
        Move(rp1,103,119);
        Text(rp1,"    ",4);
    }

    if(board[2][1]==1024){
        SetAPen(rp, 14);
        SetBPen(rp, 1);
        Move(rp1,103,103);
        Text(rp1,"    ",4);
        Move(rp1,103,111);
        Text(rp1,"1024");
        Move(rp1,103,119);
        Text(rp1,"    ",4);
    }

    if(board[2][1]==2048){
        SetAPen(rp, 1);
        SetBPen(rp, 2);
        Move(rp1,103,103);
        Text(rp1,"    ",4);
        Move(rp1,103,111);
        Text(rp1,"2048");
        Move(rp1,103,119);
        Text(rp1,"    ",4);
    }

    //Board11
    if(board[2][2]==0){
        SetAPen(rp, 0);
        SetBPen(rp, 15);
        Move(rp1,136,103);
        Text(rp1,"    ",4);
        Move(rp1,136,111);
        Text(rp1,"    ",4);
        Move(rp1,136,119);
        Text(rp1,"    ",4);
    }

    if(board[2][2]==2){
        SetAPen(rp, 14);
        SetBPen(rp, 12);
        Move(rp1,136,103);
        Text(rp1,"    ",4);
        Move(rp1,136,111);
        Text(rp1," 2  ",4);
        Move(rp1,136,119);
        Text(rp1,"    ",4);
    }

    if(board[2][2]==4){
        SetAPen(rp, 4);
        SetBPen(rp, 11);
        Move(rp1,136,103);
        Text(rp1,"    ",4);
        Move(rp1,136,111);
        Text(rp1," 4  ",4);
        Move(rp1,136,119);
        Text(rp1,"    ",4);
    }

    if(board[2][2]==8){
        SetAPen(rp, 2);
        SetBPen(rp, 9);
        Move(rp1,136,103);
        Text(rp1,"    ",4);
        Move(rp1,136,111);
        Text(rp1," 8  ",4);
        Move(rp1,136,119);
        Text(rp1,"    ",4);
    }

    if(board[2][2]==16){
        SetAPen(rp, 9);
        SetBPen(rp, 10);
        Move(rp1,136,103);
        Text(rp1,"    ",4);
        Move(rp1,136,111);
        Text(rp1," 16 ",4);
        Move(rp1,136,119);
        Text(rp1,"    ",4);
    }

    if(board[2][2]==32){
        SetAPen(rp, 2);
        SetBPen(rp, 5);
        Move(rp1,136,103);
        Text(rp1,"    ",4);
        Move(rp1,136,111);
        Text(rp1," 32 ",4);
        Move(rp1,136,119);
        Text(rp1,"    ",4);
    }

    if(board[2][2]==64){
        SetAPen(rp, 9);
        SetBPen(rp, 6);
        Move(rp1,136,103);
        Text(rp1,"    ",4);
        Move(rp1,136,111);
        Text(rp1," 64  ",4);
        Move(rp1,136,119);
        Text(rp1,"    ",4);
    }

    if(board[2][2]==128){
        SetAPen(rp, 12);
        SetBPen(rp, 14);
        Move(rp1,136,103);
        Text(rp1,"    ",4);
        Move(rp1,136,111);
        Text(rp1,"128 ",4);
        Move(rp1,136,119);
        Text(rp1,"    ",4);
    }

    if(board[2][2]==256){
        SetAPen(rp, 11);
        SetBPen(rp, 4);
        Move(rp1,136,103);
        Text(rp1,"    ",4);
        Move(rp1,136,111);
        Text(rp1,"256 ",4);
        Move(rp1,136,119);
        Text(rp1,"    ",4);
    }

    if(board[2][2]==512){
        SetAPen(rp, 6);
        SetBPen(rp, 13);
        Move(rp1,136,103);
        Text(rp1,"    ",4);
        Move(rp1,136,111);
        Text(rp1,"512 ",4);
        Move(rp1,136,119);
        Text(rp1,"    ",4);
    }

    if(board[2][2]==1024){
        SetAPen(rp, 14);
        SetBPen(rp, 1);
        Move(rp1,136,103);
        Text(rp1,"    ",4);
        Move(rp1,136,111);
        Text(rp1,"1024");
        Move(rp1,136,119);
        Text(rp1,"    ",4);
    }

    if(board[2][2]==2048){
        SetAPen(rp, 1);
        SetBPen(rp, 2);
        Move(rp1,136,103);
        Text(rp1,"    ",4);
        Move(rp1,136,111);
        Text(rp1,"2048");
        Move(rp1,136,119);
        Text(rp1,"    ",4);
    }

    //Board12
    if(board[2][3]==0){
        SetAPen(rp, 0);
        SetBPen(rp, 15);
        Move(rp1,169,103);
        Text(rp1,"    ",4);
        Move(rp1,169,111);
        Text(rp1,"    ",4);
        Move(rp1,169,119);
        Text(rp1,"    ",4);
    }

    if(board[2][3]==2){
        SetAPen(rp, 14);
        SetBPen(rp, 12);
        Move(rp1,169,103);
        Text(rp1,"    ",4);
        Move(rp1,169,111);
        Text(rp1," 2  ",4);
        Move(rp1,169,119);
        Text(rp1,"    ",4);
    }

    if(board[2][3]==4){
        SetAPen(rp, 4);
        SetBPen(rp, 11);
        Move(rp1,169,103);
        Text(rp1,"    ",4);
        Move(rp1,169,111);
        Text(rp1," 4  ",4);
        Move(rp1,169,119);
        Text(rp1,"    ",4);
    }

    if(board[2][3]==8){
        SetAPen(rp, 2);
        SetBPen(rp, 9);
        Move(rp1,169,103);
        Text(rp1,"    ",4);
        Move(rp1,169,111);
        Text(rp1," 8  ",4);
        Move(rp1,169,119);
        Text(rp1,"    ",4);
    }

    if(board[2][3]==16){
        SetAPen(rp, 9);
        SetBPen(rp, 10);
        Move(rp1,169,103);
        Text(rp1,"    ",4);
        Move(rp1,169,111);
        Text(rp1," 16 ",4);
        Move(rp1,169,119);
        Text(rp1,"    ",4);
    }

    if(board[2][3]==32){
        SetAPen(rp, 2);
        SetBPen(rp, 5);
        Move(rp1,169,103);
        Text(rp1,"    ",4);
        Move(rp1,169,111);
        Text(rp1," 32 ",4);
        Move(rp1,169,119);
        Text(rp1,"    ",4);
    }

    if(board[2][3]==64){
        SetAPen(rp, 9);
        SetBPen(rp, 6);
        Move(rp1,169,103);
        Text(rp1,"    ",4);
        Move(rp1,169,111);
        Text(rp1," 64  ",4);
        Move(rp1,169,119);
        Text(rp1,"    ",4);
    }

    if(board[2][3]==128){
        SetAPen(rp, 12);
        SetBPen(rp, 14);
        Move(rp1,169,103);
        Text(rp1,"    ",4);
        Move(rp1,169,111);
        Text(rp1,"128 ",4);
        Move(rp1,169,119);
        Text(rp1,"    ",4);
    }

    if(board[2][3]==256){
        SetAPen(rp, 11);
        SetBPen(rp, 4);
        Move(rp1,169,103);
        Text(rp1,"    ",4);
        Move(rp1,169,111);
        Text(rp1,"256 ",4);
        Move(rp1,169,119);
        Text(rp1,"    ",4);
    }

    if(board[2][3]==512){
        SetAPen(rp, 6);
        SetBPen(rp, 13);
        Move(rp1,169,103);
        Text(rp1,"    ",4);
        Move(rp1,169,111);
        Text(rp1,"512 ",4);
        Move(rp1,169,119);
        Text(rp1,"    ",4);
    }

    if(board[2][3]==1024){
        SetAPen(rp, 14);
        SetBPen(rp, 1);
        Move(rp1,169,103);
        Text(rp1,"    ",4);
        Move(rp1,169,111);
        Text(rp1,"1024");
        Move(rp1,169,119);
        Text(rp1,"    ",4);
    }

    if(board[2][3]==2048){
        SetAPen(rp, 1);
        SetBPen(rp, 2);
        Move(rp1,169,103);
        Text(rp1,"    ",4);
        Move(rp1,169,111);
        Text(rp1,"2048");
        Move(rp1,169,119);
        Text(rp1,"    ",4);
    }
//Board Finisher
    SetAPen(rp, 0);
    SetBPen(rp, 0);
    Move(rp1,202,103);
    Text(rp1,"    ",4);
    Move(rp1,202,111);
    Text(rp1,"    ",4);
    Move(rp1,202,119);
    Text(rp1,"    ",4);
    Move(rp1,202,120);
    Text(rp1,"    ",4);

    //Board13
    if(board[3][0]==0){
        SetAPen(rp, 0);
        SetBPen(rp, 15);
        Move(rp1,70,128);
        Text(rp1,"    ",4);
        Move(rp1,70,136);
        Text(rp1,"    ",4);
        Move(rp1,70,144);
        Text(rp1,"    ",4);
    }

    if(board[3][0]==2){
        SetAPen(rp, 14);
        SetBPen(rp, 12);
        Move(rp1,70,128);
        Text(rp1,"    ",4);
        Move(rp1,70,136);
        Text(rp1," 2  ",4);
        Move(rp1,70,144);
        Text(rp1,"    ",4);
    }

    if(board[3][0]==4){
        SetAPen(rp, 4);
        SetBPen(rp, 11);
        Move(rp1,70,128);
        Text(rp1,"    ",4);
        Move(rp1,70,136);
        Text(rp1," 4  ",4);
        Move(rp1,70,144);
        Text(rp1,"    ",4);
    }

    if(board[3][0]==8){
        SetAPen(rp, 2);
        SetBPen(rp, 9);
        Move(rp1,70,128);
        Text(rp1,"    ",4);
        Move(rp1,70,136);
        Text(rp1," 8  ",4);
        Move(rp1,70,144);
        Text(rp1,"    ",4);
    }

    if(board[3][0]==16){
        SetAPen(rp, 9);
        SetBPen(rp, 10);
        Move(rp1,70,128);
        Text(rp1,"    ",4);
        Move(rp1,70,136);
        Text(rp1," 16 ",4);
        Move(rp1,70,144);
        Text(rp1,"    ",4);
    }

    if(board[3][0]==32){
        SetAPen(rp, 2);
        SetBPen(rp, 5);
        Move(rp1,70,128);
        Text(rp1,"    ",4);
        Move(rp1,70,136);
        Text(rp1," 32 ",4);
        Move(rp1,70,144);
        Text(rp1,"    ",4);
    }

    if(board[3][0]==64){
        SetAPen(rp, 9);
        SetBPen(rp, 6);
        Move(rp1,70,128);
        Text(rp1,"    ",4);
        Move(rp1,70,136);
        Text(rp1," 64  ",4);
        Move(rp1,70,144);
        Text(rp1,"    ",4);
    }

    if(board[3][0]==128){
        SetAPen(rp, 12);
        SetBPen(rp, 14);
        Move(rp1,70,128);
        Text(rp1,"    ",4);
        Move(rp1,70,136);
        Text(rp1,"128 ",4);
        Move(rp1,70,144);
        Text(rp1,"    ",4);
    }

    if(board[3][0]==256){
        SetAPen(rp, 11);
        SetBPen(rp, 4);
        Move(rp1,70,128);
        Text(rp1,"    ",4);
        Move(rp1,70,136);
        Text(rp1,"256 ",4);
        Move(rp1,70,144);
        Text(rp1,"    ",4);
    }

    if(board[3][0]==512){
        SetAPen(rp, 6);
        SetBPen(rp, 13);
        Move(rp1,70,128);
        Text(rp1,"    ",4);
        Move(rp1,70,136);
        Text(rp1,"512 ",4);
        Move(rp1,70,144);
        Text(rp1,"    ",4);
    }

    if(board[3][0]==1024){
        SetAPen(rp, 14);
        SetBPen(rp, 1);
        Move(rp1,70,128);
        Text(rp1,"    ",4);
        Move(rp1,70,136);
        Text(rp1,"1024");
        Move(rp1,70,144);
        Text(rp1,"    ",4);
    }

    if(board[3][0]==2048){
        SetAPen(rp, 1);
        SetBPen(rp, 2);
        Move(rp1,70,128);
        Text(rp1,"    ",4);
        Move(rp1,70,136);
        Text(rp1,"2048");
        Move(rp1,70,144);
        Text(rp1,"    ",4);
    }
    //Board14
    if(board[3][1]==0){
        SetAPen(rp, 0);
        SetBPen(rp, 15);
        Move(rp1,103,128);
        Text(rp1,"    ",4);
        Move(rp1,103,136);
        Text(rp1,"    ",4);
        Move(rp1,103,144);
        Text(rp1,"    ",4);
    }

    if(board[3][1]==2){
        SetAPen(rp, 14);
        SetBPen(rp, 12);
        Move(rp1,103,128);
        Text(rp1,"    ",4);
        Move(rp1,103,136);
        Text(rp1," 2  ",4);
        Move(rp1,103,144);
        Text(rp1,"    ",4);
    }

    if(board[3][1]==4){
        SetAPen(rp, 4);
        SetBPen(rp, 11);
        Move(rp1,103,128);
        Text(rp1,"    ",4);
        Move(rp1,103,136);
        Text(rp1," 4  ",4);
        Move(rp1,103,144);
        Text(rp1,"    ",4);
    }

    if(board[3][1]==8){
        SetAPen(rp, 2);
        SetBPen(rp, 9);
        Move(rp1,103,128);
        Text(rp1,"    ",4);
        Move(rp1,103,136);
        Text(rp1," 8  ",4);
        Move(rp1,103,144);
        Text(rp1,"    ",4);
    }

    if(board[3][1]==16){
        SetAPen(rp, 9);
        SetBPen(rp, 10);
        Move(rp1,103,128);
        Text(rp1,"    ",4);
        Move(rp1,103,136);
        Text(rp1," 16 ",4);
        Move(rp1,103,144);
        Text(rp1,"    ",4);
    }

    if(board[3][1]==32){
        SetAPen(rp, 2);
        SetBPen(rp, 5);
        Move(rp1,103,128);
        Text(rp1,"    ",4);
        Move(rp1,103,136);
        Text(rp1," 32 ",4);
        Move(rp1,103,144);
        Text(rp1,"    ",4);
    }

    if(board[3][1]==64){
        SetAPen(rp, 9);
        SetBPen(rp, 6);
        Move(rp1,103,128);
        Text(rp1,"    ",4);
        Move(rp1,103,136);
        Text(rp1," 64  ",4);
        Move(rp1,103,144);
        Text(rp1,"    ",4);
    }

    if(board[3][1]==128){
        SetAPen(rp, 12);
        SetBPen(rp, 14);
        Move(rp1,103,128);
        Text(rp1,"    ",4);
        Move(rp1,103,136);
        Text(rp1,"128 ",4);
        Move(rp1,103,144);
        Text(rp1,"    ",4);
    }

    if(board[3][1]==256){
        SetAPen(rp, 11);
        SetBPen(rp, 4);
        Move(rp1,103,128);
        Text(rp1,"    ",4);
        Move(rp1,103,136);
        Text(rp1,"256 ",4);
        Move(rp1,103,144);
        Text(rp1,"    ",4);
    }

    if(board[3][1]==512){
        SetAPen(rp, 6);
        SetBPen(rp, 13);
        Move(rp1,103,128);
        Text(rp1,"    ",4);
        Move(rp1,103,136);
        Text(rp1,"512 ",4);
        Move(rp1,103,144);
        Text(rp1,"    ",4);
    }

    if(board[3][1]==1024){
        SetAPen(rp, 14);
        SetBPen(rp, 1);
        Move(rp1,103,128);
        Text(rp1,"    ",4);
        Move(rp1,103,136);
        Text(rp1,"1024");
        Move(rp1,103,144);
        Text(rp1,"    ",4);
    }

    if(board[3][1]==2048){
        SetAPen(rp, 1);
        SetBPen(rp, 2);
        Move(rp1,103,128);
        Text(rp1,"    ",4);
        Move(rp1,103,136);
        Text(rp1,"2048");
        Move(rp1,103,144);
        Text(rp1,"    ",4);
    }

    //Board15
    if(board[3][2]==0){
        SetAPen(rp, 0);
        SetBPen(rp, 15);
        Move(rp1,136,128);
        Text(rp1,"    ",4);
        Move(rp1,136,136);
        Text(rp1,"    ",4);
        Move(rp1,136,144);
        Text(rp1,"    ",4);
    }

    if(board[3][2]==2){
        SetAPen(rp, 14);
        SetBPen(rp, 12);
        Move(rp1,136,128);
        Text(rp1,"    ",4);
        Move(rp1,136,136);
        Text(rp1," 2  ",4);
        Move(rp1,136,144);
        Text(rp1,"    ",4);
    }

    if(board[3][2]==4){
        SetAPen(rp, 4);
        SetBPen(rp, 11);
        Move(rp1,136,128);
        Text(rp1,"    ",4);
        Move(rp1,136,136);
        Text(rp1," 4  ",4);
        Move(rp1,136,144);
        Text(rp1,"    ",4);
    }

    if(board[3][2]==8){
        SetAPen(rp, 2);
        SetBPen(rp, 9);
        Move(rp1,136,128);
        Text(rp1,"    ",4);
        Move(rp1,136,136);
        Text(rp1," 8  ",4);
        Move(rp1,136,144);
        Text(rp1,"    ",4);
    }

    if(board[3][2]==16){
        SetAPen(rp, 9);
        SetBPen(rp, 10);
        Move(rp1,136,128);
        Text(rp1,"    ",4);
        Move(rp1,136,136);
        Text(rp1," 16 ",4);
        Move(rp1,136,144);
        Text(rp1,"    ",4);
    }

    if(board[3][2]==32){
        SetAPen(rp, 2);
        SetBPen(rp, 5);
        Move(rp1,136,128);
        Text(rp1,"    ",4);
        Move(rp1,136,136);
        Text(rp1," 32 ",4);
        Move(rp1,136,144);
        Text(rp1,"    ",4);
    }

    if(board[3][2]==64){
        SetAPen(rp, 9);
        SetBPen(rp, 6);
        Move(rp1,136,128);
        Text(rp1,"    ",4);
        Move(rp1,136,136);
        Text(rp1," 64  ",4);
        Move(rp1,136,144);
        Text(rp1,"    ",4);
    }

    if(board[3][2]==128){
        SetAPen(rp, 12);
        SetBPen(rp, 14);
        Move(rp1,136,128);
        Text(rp1,"    ",4);
        Move(rp1,136,136);
        Text(rp1,"128 ",4);
        Move(rp1,136,144);
        Text(rp1,"    ",4);
    }

    if(board[3][2]==256){
        SetAPen(rp, 11);
        SetBPen(rp, 4);
        Move(rp1,136,128);
        Text(rp1,"    ",4);
        Move(rp1,136,136);
        Text(rp1,"256 ",4);
        Move(rp1,136,144);
        Text(rp1,"    ",4);
    }

    if(board[3][2]==512){
        SetAPen(rp, 6);
        SetBPen(rp, 13);
        Move(rp1,136,128);
        Text(rp1,"    ",4);
        Move(rp1,136,136);
        Text(rp1,"512 ",4);
        Move(rp1,136,144);
        Text(rp1,"    ",4);
    }

    if(board[3][2]==1024){
        SetAPen(rp, 14);
        SetBPen(rp, 1);
        Move(rp1,136,128);
        Text(rp1,"    ",4);
        Move(rp1,136,136);
        Text(rp1,"1024");
        Move(rp1,136,144);
        Text(rp1,"    ",4);
    }

    if(board[3][2]==2048){
        SetAPen(rp, 1);
        SetBPen(rp, 2);
        Move(rp1,136,128);
        Text(rp1,"    ",4);
        Move(rp1,136,136);
        Text(rp1,"2048");
        Move(rp1,136,144);
        Text(rp1,"    ",4);
    }
    //Board16
    if(board[3][3]==0){
        SetAPen(rp, 0);
        SetBPen(rp, 15);
        Move(rp1,169,128);
        Text(rp1,"    ",4);
        Move(rp1,169,136);
        Text(rp1,"    ",4);
        Move(rp1,169,144);
        Text(rp1,"    ",4);
    }

    if(board[3][3]==2){
        SetAPen(rp, 14);
        SetBPen(rp, 12);
        Move(rp1,169,128);
        Text(rp1,"    ",4);
        Move(rp1,169,136);
        Text(rp1," 2  ",4);
        Move(rp1,169,144);
        Text(rp1,"    ",4);
    }

    if(board[3][3]==4){
        SetAPen(rp, 4);
        SetBPen(rp, 11);
        Move(rp1,169,128);
        Text(rp1,"    ",4);
        Move(rp1,169,136);
        Text(rp1," 4  ",4);
        Move(rp1,169,144);
        Text(rp1,"    ",4);
    }

    if(board[3][3]==8){
        SetAPen(rp, 2);
        SetBPen(rp, 9);
        Move(rp1,169,128);
        Text(rp1,"    ",4);
        Move(rp1,169,136);
        Text(rp1," 8  ",4);
        Move(rp1,169,144);
        Text(rp1,"    ",4);
    }

    if(board[3][3]==16){
        SetAPen(rp, 9);
        SetBPen(rp, 10);
        Move(rp1,169,128);
        Text(rp1,"    ",4);
        Move(rp1,169,136);
        Text(rp1," 16 ",4);
        Move(rp1,169,144);
        Text(rp1,"    ",4);
    }

    if(board[3][3]==32){
        SetAPen(rp, 2);
        SetBPen(rp, 5);
        Move(rp1,169,128);
        Text(rp1,"    ",4);
        Move(rp1,169,136);
        Text(rp1," 32 ",4);
        Move(rp1,169,144);
        Text(rp1,"    ",4);
    }

    if(board[3][3]==64){
        SetAPen(rp, 9);
        SetBPen(rp, 6);
        Move(rp1,169,128);
        Text(rp1,"    ",4);
        Move(rp1,169,136);
        Text(rp1," 64  ",4);
        Move(rp1,169,144);
        Text(rp1,"    ",4);
    }

    if(board[3][3]==128){
        SetAPen(rp, 12);
        SetBPen(rp, 14);
        Move(rp1,169,128);
        Text(rp1,"    ",4);
        Move(rp1,169,136);
        Text(rp1,"128 ",4);
        Move(rp1,169,144);
        Text(rp1,"    ",4);
    }

    if(board[3][3]==256){
        SetAPen(rp, 11);
        SetBPen(rp, 4);
        Move(rp1,169,128);
        Text(rp1,"    ",4);
        Move(rp1,169,136);
        Text(rp1,"256 ",4);
        Move(rp1,169,144);
        Text(rp1,"    ",4);
    }

    if(board[3][3]==512){
        SetAPen(rp, 6);
        SetBPen(rp, 13);
        Move(rp1,169,128);
        Text(rp1,"    ",4);
        Move(rp1,169,136);
        Text(rp1,"512 ",4);
        Move(rp1,169,144);
        Text(rp1,"    ",4);
    }

    if(board[3][3]==1024){
        SetAPen(rp, 14);
        SetBPen(rp, 1);
        Move(rp1,169,128);
        Text(rp1,"    ",4);
        Move(rp1,169,136);
        Text(rp1,"1024");
        Move(rp1,169,144);
        Text(rp1,"    ",4);
    }

    if(board[3][3]==2048){
        SetAPen(rp, 1);
        SetBPen(rp, 2);
        Move(rp1,169,128);
        Text(rp1,"    ",4);
        Move(rp1,169,136);
        Text(rp1,"2048");
        Move(rp1,169,144);
        Text(rp1,"    ",4);
    }
//Board Finisher
    SetAPen(rp, 0);
    SetBPen(rp, 0);
    Move(rp1,202,128);
    Text(rp1,"    ",4);
    Move(rp1,202,136);
    Text(rp1,"    ",4);
    Move(rp1,202,144);
    Text(rp1,"    ",4);
    Move(rp1,202,146);
    Text(rp1,"    ",4);
}
main() {
    IntuitionBase = OpenLibrary("intuition.library", 0);
    if (IntuitionBase == NULL) {
        printf("intuitionbase error!\n");
        return (1);
    }

    GfxBase = OpenLibrary("graphics.library", 0);
    if (GfxBase == NULL) {
        printf("gfxbase error!\n");
        CloseLibrary(IntuitionBase);
        return (1);
    }

    scr = OpenScreen(&nw);
    if (scr == NULL) {
        printf("screen error!\n");
        CloseLibrary(GfxBase);
        CloseLibrary(IntuitionBase);
        return (1);
    }
    rp = &(scr->RastPort);
    SetAPen(rp, 1);
    Move(rp, 130, 40);
    Text(rp, "2048", 4);
    Move(rp, 20, 40);
    Text(rp, "Score: ", 7);
    Move(rp, 235, 40);
    Text(rp, "Move: ", 6);
    //Borders of board
    SetAPen(rp, 1);
    SetBPen(rp, 1);
    Move(rp, 69, 52);
    Text(rp, "                 ", 17);
    Move(rp, 69, 53);
    Text(rp, "                 ", 17);
    Move(rp, 69, 61);
    Text(rp, "                 ", 17);
    Move(rp, 69, 69);
    Text(rp, "                 ", 17);
    Move(rp, 69, 77);
    Text(rp, "                 ", 17);
    Move(rp, 69, 85);
    Text(rp, "                 ", 17);
    Move(rp, 69, 93);
    Text(rp, "                 ", 17);
    Move(rp, 69, 101);
    Text(rp, "                 ", 17);
    Move(rp, 69, 109);
    Text(rp, "                 ", 17);
    Move(rp, 69, 117);
    Text(rp, "                 ", 17);
    Move(rp, 69, 125);
    Text(rp, "                 ", 17);
    Move(rp, 69, 133);
    Text(rp, "                 ", 17);
    Move(rp, 69, 141);
    Text(rp, "                 ", 17);
    Move(rp, 69, 145);
    Text(rp, "                 ", 17);
    startBoard();
    updateScore();
    updateBoard();
    updateMoveNumber();
    while (1) {
        if (*joy & 2) {//right
            backupBoard();
            fillSpaceRight();
            applyRight();
            fillSpaceRight();
            if (full() && apocalypse) {
                response = -1;
            }
            else if (blockMoves()) {
                spawn();
            }
            else
                response = 0;
            updateScore();
            updateBoard();
            updateMoveNumber();
            Delay(20);
        }
        if (*joy & 512) {//left
            backupBoard();
            fillSpaceLeft();
            applyLeft();
            fillSpaceLeft();
            if (full() && apocalypse) {
                response = -1;
            }
            else if (blockMoves()) {
                spawn();
            }
            else
                response = 0;
            updateScore();
            updateBoard();
            updateMoveNumber();
            Delay(20);
        }
        if ((*joy >> 1 ^ *joy) & 1) {//down
            backupBoard();
            fillSpaceDown();
            applyDown();
            fillSpaceDown();
            if (full() && apocalypse) {
                response = -1;
            }
            else if (blockMoves()) {
                spawn();
            }
            else
                response = 0;
            updateScore();
            updateBoard();
            updateMoveNumber();
            Delay(20);
        }
        if ((*joy >> 1 ^ *joy) & 256) {//up
            backupBoard();
            fillSpaceUp();
            applyUp();
            fillSpaceUp();
            if (full() && apocalypse) {
                response = -1;
            }
            else if (blockMoves()) {
                spawn();
            }
            else
                response = 0;
            updateScore();
            updateBoard();
            updateMoveNumber();
            Delay(20);
        }
        if (!(*cia & 64 * PORT)) {
            CloseScreen(scr);
            CloseLibrary(GfxBase);
            CloseLibrary(IntuitionBase);
            return (0);
        }
    }
}
