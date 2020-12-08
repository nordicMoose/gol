extern "C"
{
    #include "application.h"
    #include "keycodes.h"
}
#include <cstdio>
#include <vector>
#include <algorithm>

//constants
constexpr int windown_w = 1000;
constexpr int windown_h = 1000;

constexpr int boardscale = 1;
constexpr int boardH = windown_h / boardscale;
constexpr int boardW = windown_w / boardscale;

constexpr w_Color DEAD = 0xFFFFFFFF;
constexpr w_Color ALIVE = 0;
constexpr w_Color SELECT = 0xFF0000FF;
constexpr w_Color SELECT2 = 0xFFFFB6C1;

struct Rect
{
    int x1 = 0;
    int x2 = 0;
    int y1 = 0;
    int y2 = 0;
};

struct V2i
{
    int x = 0,y = 0;
    
    bool operator == (const V2i& p) const{
        return p.x == x && p.y == y;
    }
    bool operator != (const V2i& p) const{
        return !(*this==p);
    }
};

using Pos = V2i;

namespace g
{
    template<class T>
    constexpr T max (T v1, T v2)
    {
        if(v1>v2) 
            return v1;
        else
            return v2;
    }

    template<class T>
    constexpr T min (T v1, T v2)
    {
        if(v1<v2) 
            return v1;
        else
            return v2;
    }
}

//timer to control simulation speed
w_Timer* gameSpeedTimer;

//title timer, update title every 1 seconds
w_Timer* titletimer;

char titlebuffer[150];      //buffer for the whole title
char pauseStateTitle[10];   //buffer for pause state text
char boardmodeTitle[15];    //buffer for boardmode state text
char drawmodeTitle[15];     //buffer for drawmode state text

bool paused;        //is the game paused; 1=paused, 0=not paused
char boardmode;     //clear board or not
char drawmode;      //line or dot draw

//zoom bounds
const int maxzoom = 12;
const int minzoom = 1;

int zoomvalue;
int zoomvalue2; //zoomvalue * 2

//camera
Pos camerapos;
Rect cameraRect;
Pos mousedrag;

Pos drawpoint; //previous mouse pos. used for drawing lines
char drawing;   //drawing status

//current simulation speed
float gameSpeed;

//selection and clipboard
char selecting;
bool selmoved;
bool newsel;
Pos selection1;
Pos selection2;
Pos tmpsel;
std::vector<w_Color> selectiontmp(boardW * boardH,DEAD);
std::vector<w_Color> clipboard;
V2i clipboard_size;
bool has_clipboard_data;

//currently alive cells. keep a list so no need to go through all of the cells every frame
std::vector<Pos> alive1;
std::vector<Pos> alive2;

//cell boards
std::vector<w_Color> board1(boardW * boardH,DEAD);
std::vector<w_Color> board2(boardW * boardH,DEAD);

//array for keeping track of already checked cells
//is cleared each frame
bool* checked = 0;

//func
void DragCamera();
void SetZoom();
void SetCameraPos();
void ClampCamera();
void SetCameraRect();
void SetGameSpeed();
void DoGame();
void CheckCell(int x,int y);
void DoDraw();
void SetBoardCell(std::vector<w_Color>& _board, int x, int y, w_Color value);
void DrawLineCells(std::vector<w_Color>& _board, int x1, int y1, int x2, int y2, w_Color color);
int GetNeighbors(const w_Color* _board, unsigned x, unsigned y);
void DrawBoard();
Pos WindowToDrawPoint(Pos);
Pos WindowToCameraPoint(Pos);
void DoGameMode2();
void CheckCellMode2(int x,int y);
void Select();
Pos GetMousePos();
void DrawSelectionArea();
void ClearSelectionLine();
void CopySelection();
void PasteClipboard();
bool IsInSelectionTmp(unsigned x, unsigned y);

void Start()
{
    //set drawsize. is same as windowsize if boardscale == 1,
    //so this could be put into a constexpr if, but really it's not an issue
    SetDrawSize(boardW,boardH);

    //limit fps here, and when paused too
    //reduces cpu usage when paused
    w_SetFPSLimit(250);
    
    //init variables
    paused      = true;
    boardmode   = 1;
    drawmode    = 1;
    gameSpeed   = 0;    //0 = unlimited
    zoomvalue   = 1;
    drawing     = -1;
    selecting   = -1;
    selmoved    = false;
    newsel      = false;
    zoomvalue2  = zoomvalue*2;
    has_clipboard_data = false;

    //init camera rect to be the size of the screen
    cameraRect.x1 = 0;
    cameraRect.x2 = GetDrawWidth() - 1;
    cameraRect.y1 = 0;
    cameraRect.y2 = GetDrawHeight() - 1;

    //init camera pos to the middle of screen
    camerapos.x = GetDrawWidth() / 2;
    camerapos.y = GetDrawHeight() / 2;

    sprintf(pauseStateTitle, "PAUSED |");
    sprintf(boardmodeTitle, "BOARDM: 1 |");
    sprintf(drawmodeTitle,"DRAWM: DOT |");

    //allocate checked board.
    checked = new bool[boardW*boardH];

    //reserve space for these, so there is no runtime allocation cost
    alive1.reserve(boardW * boardH);
    alive2.reserve(boardW * boardH);

    clipboard.reserve(boardW * boardH);

    //create and init fps timer
    titletimer = CreateTimer();
    titletimer->enabled = 1;
    titletimer->time = 1;       //set time to 1 so that title is updated immideately

    //create and enable game speed timer
    gameSpeedTimer = CreateTimer();
    gameSpeedTimer->enabled = 1;

    //use manual blit rect
    SetUserBlitRectState(1);
    SetUserBlitDestRect(0,0,GetWindowWidth(),GetWindowHeight());

    //free original screenbuffer because it's not used
    free(Dev_GetScreenBuffer());

    //fill all boards with white cells, leave board1 as the screenbuffer
    Dev_SetScreenBuffer(board2.data());
    FillScreen(DEAD);
    Dev_SetScreenBuffer(board1.data());
    FillScreen(DEAD);
}

void Update()
{
    if(!paused)
    {
        if(gameSpeed == 0 || gameSpeedTimer->time >= gameSpeed)
        {
            gameSpeedTimer->time = 0;
            if(boardmode)
                DoGame();
            else
                DoGameMode2();
        }
    }
    else
    {
        Select();
        DoDraw();
    }
    
    DrawBoard();
    SetZoom();
    DragCamera();
    SetGameSpeed();

    //change pause state
    if(GetKeyDown(VK_SPACE))
    {
        paused = !paused;

        if(paused)
        {
            w_SetFPSLimit(250);

            pauseStateTitle[0] = 'P';
            boardmodeTitle[0] = 'B';
            drawmodeTitle[0] = 'D';
        }
        else
        {
            if(selecting != -1)
                ClearSelectionLine();
            selecting = -1;

            w_SetFPSLimit(0);

            pauseStateTitle[0] = '\0';
            boardmodeTitle[0] = '\0';
            drawmodeTitle[0] = '\0';
        }
            
        titletimer->time = 1;
    }

    //draw title with info, every 1 seconds
    if(titletimer->time >= 1)
    {
        titletimer->time = 0;
        sprintf(titlebuffer,"%s -- %s %s %s SPEED: %1.1f | FPS:%4.1f",GetWindowTitle(), pauseStateTitle, boardmodeTitle, drawmodeTitle, 5.0-gameSpeed, 1.0 / GetDeltatime());
        SetWindowTitle(titlebuffer);
    }

    //exit on esc
    if(GetKeyDown(VK_ESCAPE))
        ExitApplication();

    //force redraw, because I'm not always
    //using builtin drawing functions
    DisplayDirect();
}

//clear the selection outline
void ClearSelectionLine()
{
    if(!newsel)
    {
        Pos s1 = {clampi(g::min(selection1.x,selection2.x),0,GetDrawWidth()-1),clampi(g::min(selection1.y,selection2.y),0,GetDrawHeight()-1)};
        Pos s2 = {clampi(g::max(selection1.x,selection2.x),0,GetDrawWidth()-1),clampi(g::max(selection1.y,selection2.y),0,GetDrawHeight()-1)};

        for(int i = s1.x; i < s2.x+1; ++i)
        {  
            board1[s1.y * GetDrawWidth() + i] = selectiontmp[s1.y * GetDrawWidth() + i];
            board1[s2.y * GetDrawWidth() + i] = selectiontmp[s2.y * GetDrawWidth() + i];
        }
    
        for(int i = s1.y+1; i < s2.y; ++i)
        {
            board1[i * GetDrawWidth() + s1.x] = selectiontmp[i * GetDrawWidth() + s1.x];
            board1[i * GetDrawWidth() + s2.x] = selectiontmp[i * GetDrawWidth() + s2.x];
        }
    }
}

//draw the selection outline
void DrawSelectionArea()
{
    static int offset = 0;
    static w_Color sel_color[2] { SELECT2, SELECT };

    if(selecting != -1)
    {
        //offset is used to animate the selection line
        if(++offset >= 1200)
            offset = 0;
        int off10 = offset/10;

        //if selection has changed (aka mouse moved), 
        //clear old line and store cells which the new line will go over
        if(selmoved)
        {
            ClearSelectionLine();

            selection2 = tmpsel;

            Pos s1 = {clampi(g::min(selection1.x,selection2.x),0,GetDrawWidth()-1),clampi(g::min(selection1.y,selection2.y),0,GetDrawHeight()-1)};
            Pos s2 = {clampi(g::max(selection1.x,selection2.x),0,GetDrawWidth()-1),clampi(g::max(selection1.y,selection2.y),0,GetDrawHeight()-1)};

            for(int i = s1.x; i < s2.x+1; ++i)
            {  
                selectiontmp[s1.y * GetDrawWidth() + i] = board1[s1.y * GetDrawWidth() + i];
                selectiontmp[s2.y * GetDrawWidth() + i] = board1[s2.y * GetDrawWidth() + i];
            }
            for(int i = s1.y+1; i < s2.y; ++i)
            {
                selectiontmp[i * GetDrawWidth() + s1.x] = board1[i * GetDrawWidth() + s1.x];
                selectiontmp[i * GetDrawWidth() + s2.x] = board1[i * GetDrawWidth() + s2.x];
            }

            newsel = false;
            selmoved = false;
        }

        Pos s1 = {clampi(g::min(selection1.x,selection2.x),0,GetDrawWidth()-1),clampi(g::min(selection1.y,selection2.y),0,GetDrawHeight()-1)};
        Pos s2 = {clampi(g::max(selection1.x,selection2.x),0,GetDrawWidth()-1),clampi(g::max(selection1.y,selection2.y),0,GetDrawHeight()-1)};

        //draw the selection lines

        //hor lines
        for(int i = s1.x; i < s2.x+1; ++i)
        {
            //draw dashed line. use color array with 2 elements to avoid an if statement
            //there is little to no performance gain, but the code looks nicer
            board1[s1.y * GetDrawWidth() + i] = sel_color[(i+off10) % 8 > 2];
            board1[s2.y * GetDrawWidth() + i] = sel_color[(unsigned)(i-off10) % 8 > 2];
            
        }

        //ver lines
        for(int i = s1.y+1; i < s2.y; ++i)
        {
            board1[i * GetDrawWidth() + s1.x] = sel_color[(unsigned)(i-off10) % 8 > 2];
            board1[i * GetDrawWidth() + s2.x] = sel_color[(i+off10) % 8 > 2];
        }
    }
}

//copy selected area to clipboard
void CopySelection()
{
    Pos s1 = {clampi(g::min(selection1.x,selection2.x),0,GetDrawWidth()-1),clampi(g::min(selection1.y,selection2.y),0,GetDrawHeight()-1)};
    Pos s2 = {clampi(g::max(selection1.x,selection2.x),0,GetDrawWidth()-1),clampi(g::max(selection1.y,selection2.y),0,GetDrawHeight()-1)};

    has_clipboard_data = true;
    clipboard_size = {s2.x-s1.x+1,s2.y-s1.y+1};

    clipboard.clear();
    ClearSelectionLine();
    for(int i = s1.y; i < s2.y+1; ++i)
    {
        for(int j = s1.x; j < s2.x+1; ++j)
        {
            clipboard.push_back(board1[i * GetDrawWidth() + j]);
        }
    };
}

//paste clipboard data to board
void PasteClipboard()
{
    Pos mpos = WindowToCameraPoint(GetMousePos());

    int w = clampi(cameraRect.x1+mpos.x + clipboard_size.x,0,GetDrawWidth()-1) - clampi(cameraRect.x1+mpos.x,0,GetDrawWidth()-1);
    int h = clampi(cameraRect.y1+mpos.y + clipboard_size.y,0,GetDrawHeight()-1) - clampi(cameraRect.y1+mpos.y,0,GetDrawHeight()-1);

    for(int i = 0; i < h; ++i)
    {
        for(int j = 0; j < w; ++j)
        {
            board1[(mpos.y+i) * GetDrawWidth() + mpos.x+j] = clipboard[i * clipboard_size.x + j];

            if(clipboard[i * clipboard_size.x + j] == ALIVE)
                alive1.push_back({ mpos.x+j,mpos.y+i});

            if(IsInSelectionTmp(mpos.x+j,mpos.y+i))
            {
                selectiontmp[(mpos.y+i) * GetDrawWidth() + mpos.x+j] = clipboard[i * clipboard_size.x + j];   
            }
        }
    }
}

//selecion stuff
void Select()
{
    //copy selection on ctr+C
    if(selecting > 0 && GetKeyDown(VK_C) && GetKey(VK_CONTROL))
        CopySelection();

    //paste on ctrl+V if clipboard has data
    //content will be placed such that topleft is on mouse cursor
    if(has_clipboard_data && GetKeyDown(VK_V) && GetKey(VK_CONTROL))
        PasteClipboard();

    //deselect on ctrl+D
    if(GetKeyDown(VK_D) && GetKey(VK_CONTROL))
    {
        ClearSelectionLine();
        selecting = -1;
        return;
    }

    //start selection on S down
    if(GetKeyDown(VK_S))
    {
        if(selecting == -1)
            newsel = true;
        ClearSelectionLine();
        newsel = true;
        selecting = 0;
        selection1 = WindowToCameraPoint(GetMousePos());
    }

    //end select when S is lifted
    if(GetKeyUp(VK_S) && selecting != -1)
    {
        selecting = 1;
        Pos p = WindowToCameraPoint(GetMousePos());
        if(p != selection2)
        {
            selmoved = true;
            tmpsel = p;
        }
    }

    //change selection when S is held
    if(GetKey(VK_S) && selecting == 0)
    {
        Pos p = WindowToCameraPoint(GetMousePos());
        if(p != selection2)
        {
            selmoved = true;
            tmpsel = p;
        }
    }

    //draw selection outline if something is selected
    if(selecting != -1)
        DrawSelectionArea();
}

//drag camera on mouse3
void DragCamera()
{
    //start drag on keydown
    if(GetKeyDown(VK_MBUTTON))
    {
        mousedrag = WindowToDrawPoint(GetMousePos());
    }
    
    //do the drag on key held
    if(GetKey(VK_MBUTTON))
    {
        Pos npos = WindowToDrawPoint(GetMousePos());

        //move in x dir
        if(abs(npos.x - mousedrag.x) >= zoomvalue)
        {
            camerapos.x -= ((npos.x - mousedrag.x) / abs(npos.x - mousedrag.x)) * abs(npos.x - mousedrag.x) / zoomvalue;
            mousedrag.x = npos.x;
            ClampCamera();
            SetCameraRect();
        }

        //move in y dir
        if(abs(npos.y - mousedrag.y) >= zoomvalue)
        {
            camerapos.y -= ((npos.y - mousedrag.y) / abs(npos.y - mousedrag.y)) * abs(npos.y - mousedrag.y) / zoomvalue;
            mousedrag.y = npos.y;
            ClampCamera();
            SetCameraRect();
        }
    }
}

//zoom camera on mouse scroll
void SetZoom()
{
    if(GetScroll() > 0 && zoomvalue < maxzoom)
    {
        zoomvalue++;
        zoomvalue2 = zoomvalue*2;
        SetCameraPos();
        SetCameraRect();
    }

    if(GetScroll() < 0 && zoomvalue > minzoom)
    {
        zoomvalue--;
        zoomvalue2 = zoomvalue*2;
        ClampCamera();
        SetCameraRect();
    }
}

//move camera to mouse pos
void SetCameraPos()
{
    Pos pos = GetMousePos();
    pos = WindowToDrawPoint(pos);

    pos.x = clampi(pos.x, 0, GetDrawWidth());
    pos.y = clampi(pos.y, 0, GetDrawHeight());
    
    pos.x -= GetDrawWidth() / 2;
    pos.y -= GetDrawHeight() / 2;
    
    pos.x = pos.x / zoomvalue;
    pos.y = pos.y / zoomvalue;

    int zm = ((zoomvalue - 1) * 2);

    camerapos.x = clampi(camerapos.x + pos.x, 
                    camerapos.x - GetDrawWidth() / zm + GetDrawWidth() / zoomvalue2, 
                    camerapos.x + GetDrawWidth() / zm - GetDrawWidth() / zoomvalue2);
    camerapos.y = clampi(camerapos.y + pos.y, 
                    camerapos.y - GetDrawHeight() / zm + GetDrawHeight() / zoomvalue2, 
                    camerapos.y + GetDrawHeight() / zm - GetDrawHeight() / zoomvalue2);
}

//clamp camera pos so that camera rect stays within window area
void ClampCamera()
{
    camerapos.x = clampi(camerapos.x, 
                        GetDrawWidth() / zoomvalue2, 
                        GetDrawWidth() - GetDrawWidth() / zoomvalue2);
    camerapos.y = clampi(camerapos.y, 
                        GetDrawHeight() / zoomvalue2, 
                        GetDrawHeight() - GetDrawHeight() / zoomvalue2);
}

//update camera rect to be in accordance with camera pos
void SetCameraRect()
{
    cameraRect.x1 = camerapos.x - GetDrawWidth()  / zoomvalue2;
    cameraRect.x2 = camerapos.x + GetDrawWidth()  / zoomvalue2;
    cameraRect.y1 = camerapos.y - GetDrawHeight() / zoomvalue2;
    cameraRect.y2 = camerapos.y + GetDrawHeight() / zoomvalue2;
}

//set game speed on number keys
void SetGameSpeed()
{
    if(GetKeyDown(VK_0))
        gameSpeed = 0;
    if(GetKeyDown(VK_9))
        gameSpeed = 0.1;
    if(GetKeyDown(VK_8))
        gameSpeed = 0.2;
    if(GetKeyDown(VK_7))
        gameSpeed = 0.5;
    if(GetKeyDown(VK_6))
        gameSpeed = 0.7;
    if(GetKeyDown(VK_5))
        gameSpeed = 1;
    if(GetKeyDown(VK_4))
        gameSpeed = 1.5;
    if(GetKeyDown(VK_3))
        gameSpeed = 2.0;
    if(GetKeyDown(VK_2))
        gameSpeed = 3.0;
    if(GetKeyDown(VK_1))
        gameSpeed = 4.0;
}

//chek if a cell should be alive or dead
void CheckCell(int x, int y)
{
    int n = GetNeighbors(board1.data(),x,y);

    if(board1[y * boardW + x] == DEAD)
    {
        if(n == 3)
        {
            board2[y * boardW + x] = ALIVE;
            alive2.push_back({x,y});
        }
    }
    else
    {
        if(n == 3 || n == 2)
        {
            board2[y * boardW + x] = ALIVE;
            alive2.push_back({x,y});
        }
    }
}

//iterate through all alive cells, and their neighbors
void DoGame()
{
    memset(checked,0,boardW*boardH*sizeof(bool));

    for(auto& it : alive1)
    {
        if(!board1[it.y * boardW + it.x])
        {
            int mx = clampi(it.x + 2, 0, boardW);
            int my = clampi(it.y + 2, 0, boardH);

            //check a 3x3 area centered on it.x,it.y
            for(int _x = clampi(it.x - 1, 0, boardW - 1); _x < mx; ++_x)
            {
                for(int _y = clampi(it.y - 1, 0, boardH - 1); _y < my; ++_y)
                {
                    if(!checked[(_y) * boardW + _x])
                    {
                        CheckCell(_x, _y);
                        checked[(_y) * boardW + _x] = 1;
                    }
                }
            }
        }

        checked[it.y * boardW + it.x] = 1;
    }

    std::swap(board1, board2);
    std::swap(alive1, alive2);

    alive2.clear();

    //I don't have a fill function for arrays, so use a lil hack to fill it with screen fill 
    Dev_SetScreenBuffer(board2.data());
    FillScreen(DEAD);

    Dev_SetScreenBuffer(board1.data());
}

//cell check for mode2
void CheckCellMode2(int x, int y)
{
    int n = GetNeighbors(board1.data(),x,y);

    if(board1[y * boardW + x])
    {
        if(n == 3)
        {
            board2[y * boardW + x] = ALIVE;
            alive2.push_back({x,y});
        }
    }
    else
    {
        if(n == 3 || n == 2)
        {
            board2[y * boardW + x] = ALIVE;
            alive2.push_back({x,y});
        }
    }
}

//made this while trying to make gol work.
//it's not gol, but it looks cool so I kept it
void DoGameMode2()
{
    memset(checked,0,boardW*boardH*sizeof(bool));

    int i = 0; 
    for(auto& it : alive1)
    {
        ++i;
        if(!board1[it.y * boardW + it.x])
        {
            int mx = clampi(it.x + 2, 0, boardW);
            int my = clampi(it.y + 2, 0, boardH);

            for(int _x = clampi(it.x - 1, 0, boardW - 1); _x < mx; ++_x)
            {
                for(int _y = clampi(it.y - 1, 0, boardH - 1); _y < my; ++_y)
                {
                    if(!checked[(_y) * boardW + _x])
                    {
                        CheckCellMode2(_x, _y);
                        checked[(_y) * boardW + _x] = 1;
                    }
                }
            }
        }
        checked[it.y * boardW + it.x] = 1;
    }

    memcpy(board1.data(),board2.data(),boardW*boardH*sizeof(w_Color));

    alive1 = alive2;

    alive2.clear();
    
    Dev_SetScreenBuffer(board1.data());
}

//convert window space point to draw space point
Pos WindowToDrawPoint(Pos p)
{
    Pos rp;
    rp.x = (p.x / (float)GetWindowWidth())  * GetDrawWidth();
    rp.y = (p.y / (float)GetWindowHeight()) * GetDrawHeight();
    return rp;
}

//convert window space point to camera space point
Pos WindowToCameraPoint(Pos p)
{
    int scale = cameraRect.x2 - cameraRect.x1;
    return {cameraRect.x1+(int)(((float)p.x / (float)GetWindowWidth()) * scale),cameraRect.y1+(int)(((float)p.y / (float)GetWindowHeight()) * scale)};
}

//wrapper for GetMousePosition
Pos GetMousePos()
{
    w_LPoint p;
    if(!GetMousePosition(&p))
        return {0,0};
    return {p.x,p.y};
}

//check if a point overlaps the selection graphic
bool IsInSelectionTmp(unsigned x, unsigned y)
{
    if((x == selection1.x && y >= selection1.y && y <= selection2.y) ||
       (x == selection2.x && y >= selection1.y && y <= selection2.y) ||
       (y == selection1.y && x >= selection1.x && x <= selection2.x) ||
       (y == selection2.y && x >= selection1.x && x <= selection2.x))
    {
        return true;  
    }
    else
    {
        return false;
    }
}

//user drawing
void DoDraw()
{
    //change boardmode on m
    if(GetKeyDown(VK_M))
    {
        boardmode = !boardmode;

        if(boardmode)
            sprintf(boardmodeTitle, "BOARDM: 1 |");
        else
            sprintf(boardmodeTitle, "BOARDM: 0 |");
        
        titletimer->time = 1;
    }

    //change drawmode (line or dot) on d
    if(GetKeyDown(VK_D) && !GetKey(VK_CONTROL))
    {
        drawmode = !drawmode;

        if(drawmode)
            sprintf(drawmodeTitle, "DRAWM: LINE |");
        else
            sprintf(drawmodeTitle, "DRAWM: DOT |");
        
        titletimer->time = 1;
    }

    //clear screen on ctrl + x, or del
    if((GetKeyDown(VK_X) && GetKey(VK_CONTROL)) || GetKeyDown(VK_DELETE))
    {
        if(selecting > 0)
        {
            Pos s1 = {clampi(g::min(selection1.x,selection2.x),0,GetDrawWidth()-1),clampi(g::min(selection1.y,selection2.y),0,GetDrawHeight()-1)};
            Pos s2 = {clampi(g::max(selection1.x,selection2.x),0,GetDrawWidth()-1),clampi(g::max(selection1.y,selection2.y),0,GetDrawHeight()-1)};

            ClearSelectionLine();
            for(int i = s1.y; i < s2.y+1; ++i)
            {
                for(int j = s1.x; j < s2.x+1; ++j)
                {
                    board1[i * GetDrawWidth() + j] = DEAD;
                    board2[i * GetDrawWidth() + j] = DEAD;
                    selectiontmp[i * GetDrawWidth() + j] = DEAD;
                }
            };

            std::remove_if(alive1.begin(),alive1.end(), 
            [&](const Pos& p)
            {
                return p.x >= s1.x && p.x <= s2.x && p.y >= s1.y && p.y <= s2.y;
            });
        }
        else
        {
            Dev_SetScreenBuffer(selectiontmp.data());
            FillScreen(DEAD);
            Dev_SetScreenBuffer(board2.data());
            FillScreen(DEAD);
            Dev_SetScreenBuffer(board1.data());
            FillScreen(DEAD);
            alive1.clear();
        }
    }
    
    //draw on mouse1, erase on mouse2
    if(GetKeyDown(VK_LBUTTON) || GetKeyDown(VK_RBUTTON))
    {
        if(drawing == -1)
        {
            drawing = 0;
            if(GetKeyDown(VK_LBUTTON))
                drawing = 1;

            drawpoint = GetMousePos();
        }
    }
    else if((GetKey(VK_LBUTTON) && drawing == 1) || (GetKey(VK_RBUTTON) && drawing == 0))
    {
        Pos pos = GetMousePos();

        
        Pos diff = {abs(drawpoint.x-pos.x),abs(drawpoint.y-pos.y)};

        if(pos.x >= 0 && pos.y >= 0 && pos.x < GetWindowWidth() && pos.y < GetWindowHeight())
        {
            int scale = cameraRect.x2 - cameraRect.x1;
            int x = ((float)pos.x / (float)GetWindowWidth())  * scale;
            int y = ((float)pos.y / (float)GetWindowHeight()) * scale;

            //draw straight if shift is held
            if(GetKey(VK_SHIFT))
            {
                if(diff.x > diff.y)
                    y = ((float)drawpoint.y / (float)GetWindowHeight()) * scale;
                else
                    x = ((float)drawpoint.x / (float)GetWindowWidth())  * scale;
            }

            //draw dots or lines based on drawmode
            if(!drawmode && 
              ((board1[(cameraRect.y1 + y)*boardW+cameraRect.x1 + x] == DEAD) || 
               (selecting != -1 && IsInSelectionTmp(cameraRect.x1+x,cameraRect.y1+y) && 
                selectiontmp[(cameraRect.y1 + y) * boardW + cameraRect.x1 + x] == DEAD)))
            {
                SetBoardCell(board1, 
                            cameraRect.x1 + x, 
                            cameraRect.y1 + y, 
                            drawing?ALIVE:DEAD);

                //if we draw over selection outline, draw into selectiontmp
                if(selecting != -1)
                {
                    if(IsInSelectionTmp(cameraRect.x1+x,cameraRect.y1+y))
                    {
                        selectiontmp[(cameraRect.y1 + y) * boardW + cameraRect.x1 + x] = drawing?ALIVE:DEAD;   
                    }
                }    
            }
            else
            {
                int x2 = ((float)drawpoint.x / (float)GetWindowWidth())  * scale;
                int y2 = ((float)drawpoint.y / (float)GetWindowHeight()) * scale;

                //draw straight if shift is held
                if(GetKey(VK_SHIFT))
                {
                    if(diff.x > diff.y)
                        y2 = y;
                    else
                        x2 = x;
                }

                DrawLineCells(board1,
                                cameraRect.x1 + x,
                                cameraRect.y1 + y,
                                cameraRect.x1 + x2,
                                cameraRect.y1 + y2,
                                drawing?ALIVE:DEAD);
            }
        }

        if(!GetKey(VK_SHIFT))
            drawpoint = pos;
    }
    else
    {
        drawing = -1;
    }
}

//draw the board to screen
void DrawBoard()
{
    //board1 is already set as the screenbuffer, 
    //but gotta still set the blit rect every frame because camera can move
    SetUserBlitSrcRect(cameraRect.x1,cameraRect.y1,cameraRect.x2-cameraRect.x1,cameraRect.y2-cameraRect.y1);
}

//get alive neighbors of a cell using the for-loop method
int GetNeighbors(const w_Color* _board, unsigned x, unsigned y)
{
    int count = 0;

    int mx = clampi(x + 2, 0, boardW);
    int my = clampi(y + 2, 0, boardH);

    for(int _x = clampi(x - 1, 0, boardW - 1); _x < mx; ++_x)
    {
        for(int _y = clampi(y - 1, 0, boardH - 1); _y < my; ++_y)
        {
            if(_x != x || _y != y)
                count += _board[_y* boardW + _x] != DEAD;
        }
    }
    return count;
}

//set a cell in board
void SetBoardCell(std::vector<w_Color>& _board, int x, int y, w_Color value)
{
    if(x >= boardW && y >= boardH)
        return;
    
    if(!value)
        alive1.push_back({x,y});
    _board[y * boardW + x] = value;
}

//draw a 1 pixel wide line
void DrawLineCells(std::vector<w_Color>& _board, int x1, int y1, int x2, int y2, w_Color color)
{
    x1 = clampi(x1, 0, GetDrawWidth() - 1);
    x2 = clampi(x2, 0, GetDrawWidth() - 1);
    y1 = clampi(y1, 0, GetDrawHeight() - 1);
    y2 = clampi(y2, 0, GetDrawHeight() - 1);

    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = abs(y2 - y1), sy = y1 < y2 ? 1 : -1; 
    int err = (dx > dy ? dx : -dy) / 2, e2;

    while(1)
    {
        if(color == ALIVE &&
        ((board1[y1*boardW+x1] == DEAD) || (selecting != -1 && IsInSelectionTmp(x1,y1) && selectiontmp[y1 * boardW + x1] == DEAD))) 
            alive1.push_back({x1,y1});
        _board[y1 * GetDrawWidth() + x1] = color;

        //selection outline thing
        if(selecting != -1)
        {
            if(IsInSelectionTmp(x1,y1))
            {
                selectiontmp[y1 * boardW + x1] = drawing?ALIVE:DEAD;   
            }
        }

        if (x1 == x2 && y1 == y2)
            break;

        e2 = err;

        if (e2 > -dx) 
        {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dy) 
        { 
            err += dx; 
            y1 += sy; 
        }
    }
}

int main(int argc, char** argv)
{
    w_ApplicationInit(argc,argv,windown_w,windown_h,"Game of Life");
    
    SetRenderMode(W_RENDERER_SW);
    w_RegisterUpdate(Update);
    w_RegisterStart(Start);

    return w_ApplicationStart();
}