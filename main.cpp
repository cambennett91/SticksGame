/*
Title: ITC226 Assessment 2 2009
Code Modified by: Cameron Bennett, 11418586
Date of Completion: 24/10/2009
Description: Creates a game of "NIM" where the object is to force the opposing player to take the last stick from a collection.
Uses a resource and file for the menu's.
*/
#include <windows.h>
#include <ctime>
#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include "header.h"
//used for getting a handle on the childwindows (buttons) and dialog box's
HINSTANCE handle;
// used for the menu
static TCHAR szAppName[16];
//Menu handle - so we can access the menu's
static HMENU hMenu;
//fires every second for timer display and change of turn
const int TIMER1 = 1;
// Self explanitory
static char player_one_name[15] = "Player One",player_two_name[15] = "Player Two";
// These hold the difficulty of the game, and if the game is paused
static BOOL pause_yn, easy_yn, medium_yn, hard_yn, quit_yn, aiPlayer_yn,;
// Start player number, and integers to hold the max turn time, current time left
static int currentPlayer, timeRemaining = 30, turnTime = 30;
// For registering the button commands
static int iChildID,wNote,hChild;
// Where to start to draw sticks
static const int XPOSITION = 20,YPOSITION = 80;
// Number of sticks
static int currentSticks, totalSticks, difference = 0;
// The dialog boxes for the player's names, and the new round time
BOOL CALLBACK Diag (HWND,UINT,WPARAM,LPARAM);
BOOL CALLBACK Timer (HWND,UINT,WPARAM,LPARAM);

void TakeSticks(HWND hwnd,int numberTaken);
void Pause(HWND hwnd); // Toggles the game between paused and unpaused
void Unpause(HWND hwnd); // Toggles the game between paused and unpaused
void Reset(HWND hwnd); // Start a new game
void Winner(HWND hwnd); // tests for winnner
void SwitchPlayer(HWND hwnd); // changes the current player
void AIControlEasy(HWND hwnd); // controls the AI
void AIControlMedium(HWND hwnd); // controls the AI
void AIControlHard(HWND hwnd); // controls the AI

//#region Create a window
//##############################################################################
/*
A starting shell for a Win32 Windows Application
Original obtained from DevC++.
Modified by serverside (May 2007), Cam Bennett (Oct 2009)
*/
/* Declare Windows procedure */
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
/* Make the class name into a global variable */
char szClassName[ ] = "WindowsApp";

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nFunsterStil)
{
    HWND hwnd; /* This is the handle for our window */
    MSG messages; /* Here messages to the application are saved */
    WNDCLASSEX wincl; /* Data structure for the windowclass */
    hMenu = LoadMenu(hThisInstance,TEXT("MyMenu"));
    handle = hThisInstance;
    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WndProc; /* This function is called by windows */
    wincl.style = CS_DBLCLKS; /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);
    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (hThisInstance, MAKEINTRESOURCE (ICON_STICK1)) ;
    wincl.hIconSm = LoadIcon (handle, MAKEINTRESOURCE (ICON_STICK3));
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = "Generic"; /* "Generic" defined in RC file*/
    wincl.cbClsExtra = 0; /* No extra bytes after the window class */
    wincl.cbWndExtra = 0; /* structure or the window instance */
    /* Use Windows's default color as the background of the window */
    wincl.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    
    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0, /* Extended possibilites for variation */
           szClassName, /* Classname */
           "NIM Game", /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT, /* Windows decides the position */
           CW_USEDEFAULT, /* where the window ends up on the screen */
           520, /* The programs width */
           300, /* and height in pixels */
           HWND_DESKTOP, /* The window is a child-window to desktop */
           LoadMenu(hThisInstance,"Generic"), /* MENU!! woo */
           hThisInstance, /* Program Instance handler */
           NULL /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nFunsterStil);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}
//##############################################################################
//#endregion

/* This function is called by the Windows function DispatchMessage() */
LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // Handle to a device context, for drawing
    HDC hdc;
    // Struct that we do our painting through, and setting the background colour
    PAINTSTRUCT ps;
    HBRUSH bgBrush = CreateSolidBrush(RGB(10,255,100));
    // Character buffers for printing out our text to the screen
    static char buffer1[20],buffer2[20],buffer3[20];
    // Holds the window dinemsions, and a small rect for the time
    RECT windowRect;
    // Icon handle for the program icon
    static HICON IconHandle;
    // Bitmap for the images of the sticks
    HBITMAP hbmp;
    // Temp member to determine if there will be any AI
    int temp;
    /* Message Handler */
    switch (message)
    {
        //-----------------------------------------------------------------------------------------------------------------------
        case WM_CREATE:
             hMenu = GetMenu(hwnd);
             IconHandle = LoadIcon(handle,MAKEINTRESOURCE(ICON_STICK2));
             GetClientRect(hwnd,&windowRect);
             // Seed the random number generator with the time in milliseconds
             srand(time(0));
             // Starting number of sticks = 10>x<15 inclusive
             totalSticks = (rand()%6) + 9;
             currentSticks = totalSticks;
             // Does Player 1 or Player 2 go first?
             currentPlayer = rand()%2;
             // These will be the "Take 1 stick", "Take 2 sticks" etc buttons.
             CreateWindow("button","1",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,20,50,20,20,hwnd,(HMENU)101,handle,NULL);
             CreateWindow("button","2",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,50,50,20,20,hwnd,(HMENU)102,handle,NULL);
             CreateWindow("button","3",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,80,50,20,20,hwnd,(HMENU)103,handle,NULL);
             temp = MessageBox(hwnd,"Take either 1, 2, or 3 sticks\nForce your opponent to take the final stick.\nA turn has a time limit of 20seconds\nDo you want Player 2 to be an AI player?","Instructions",MB_YESNO | MB_ICONINFORMATION);
             // MessageBox returns 6 for a "Yes" and 7 for a "No"
             if(temp == 6) // if AI exists, check the menu item
             {
                 aiPlayer_yn = TRUE;
                 easy_yn = TRUE;medium_yn = FALSE;hard_yn = FALSE;
                 CheckMenuItem(hMenu,AI_EASY,MF_CHECKED);
             }
             else {
                 aiPlayer_yn = FALSE;
                 easy_yn = FALSE;medium_yn = FALSE;hard_yn = FALSE;
                 CheckMenuItem(hMenu,AI_HUMAN,MF_CHECKED);
             }
             // Start the timer for 1sec intervals
             SetTimer(hwnd,TIMER1,1000,NULL);
        return 0;
        //-----------------------------------------------------------------------------------------------------------------------
        case WM_PAINT:
             hdc = BeginPaint( hwnd, &ps );
             // Paint the background
             GetClientRect(hwnd,&windowRect);
             FillRect(hdc,&windowRect,bgBrush);
             // Instructions
             Rectangle(hdc,399,9,399+90,9+18);
             TextOut(hdc,400,10,"Time Left:",10);
             TextOut(hdc,470,10,buffer1,wsprintf(buffer1,"%i",timeRemaining));
             Rectangle(hdc,399,29,399+90,29+18);
             TextOut(hdc,400,30,"Sticks Left:",12);
             TextOut(hdc,470,30,buffer2,wsprintf(buffer2,"%i",currentSticks));
             Rectangle(hdc,9,29,9+200,29+18);
             TextOut(hdc,10,30,"Press 1,2,3 or choose a value",29);
             Rectangle(hdc,9,9,9+140,9+18);
             TextOut(hdc,10,10,"Turn:",5);
             // Indicate who's turn it is
             if(currentPlayer == 0) // Player 1's turn
                  TextOut(hdc,50,10,player_one_name,wsprintf(buffer3,"%s",player_one_name));
             if(currentPlayer == 1 && !aiPlayer_yn) // Player 2's Turn
                  TextOut(hdc,50,10,player_two_name,wsprintf(buffer3,"%s",player_two_name));
             if(currentPlayer != 0 && aiPlayer_yn) // AI Players Turn
                  TextOut(hdc,50,10,"AI Player",9);
             // Indicate the AI difficulty
             if(aiPlayer_yn) {
                 if(easy_yn)
                     TextOut(hdc,200,10,"AI - Easy",9);
                 else if(medium_yn)
                     TextOut(hdc,200,10,"AI - Medium",11);
                 else if(hard_yn)
                     TextOut(hdc,200,10,"AI - Hard",9);
             }
             // Log to say who JUST took sticks
             if(difference != 0){
                 if(currentPlayer == 1)
                     TextOut(hdc,120,170,"Player One",11); // just took sticks
                 if(aiPlayer_yn && currentPlayer == 0)
                     TextOut(hdc,135,170,"AI Player",10); // just took sticks
                 if(!aiPlayer_yn && currentPlayer == 0)
                     TextOut(hdc,120,170,"Player Two",11); // just took sticks
                 if(currentSticks != totalSticks)
                     TextOut(hdc,200,170,buffer3,wsprintf(buffer3,"took %i sticks last turn",difference));
             }
             // Paused
             if(pause_yn){
                 Rectangle(hdc,229,49,229+82,49+18);
                 TextOut(hdc,230,50," --PAUSED-- ",12);
             }
             // Draw the sticks
             for(int i = 0;i < currentSticks;i++)
                 DrawIconEx(hdc,XPOSITION+(i*35),YPOSITION,IconHandle,35,70,NULL,NULL,DI_NORMAL);
             
             EndPaint( hwnd, &ps );
             return 0;
        //-----------------------------------------------------------------------------------------------------------------------
        case WM_KEYDOWN:
            switch(wParam)
            {
            // Exit the program on Escape Key
            case VK_ESCAPE:
                PostQuitMessage(0);
                break;
            }
             return 0;
        //-----------------------------------------------------------------------------------------------------------------------
        case WM_CHAR:
            {
                if (!pause_yn) //if game is und, allow removal of sticks
                {
                    if(currentSticks == 0) // if no sticks left and a button is pressed, reset
                        Reset(hwnd);
                    // The "1" Key is pressed, take 1 stick
                    if(wParam == '1')
                        TakeSticks(hwnd,1);
                    // The "2" Key is pressed, take 2 sticks
                    else if(wParam == '2') {
                        if(currentSticks < 2) {
                            hdc = GetDC(hwnd);
                            TextOut(hdc,200,170,"There arent enough sticks",25);
                            ReleaseDC(hwnd,hdc);
                        }
                        else
                            TakeSticks(hwnd,2);
                    }
                    // The "3" Key is pressed, take 3 sticks
                    else if(wParam == '3') {
                        if(currentSticks < 3) {
                            hdc = GetDC(hwnd);
                            TextOut(hdc,200,170,"There arent enough sticks",25);
                            ReleaseDC(hwnd,hdc);
                        }
                        else
                            TakeSticks(hwnd,3);
                    }
             } // END of the !paused check
             InvalidateRect(hwnd,NULL,TRUE); // refreshes so timer is continuously updated
             }
             return 0;
        //-----------------------------------------------------------------------------------------------------------------------
        case WM_TIMER:
             switch(wParam)
             {
                 if(!pause_yn){
                 case TIMER1:
                     {
                         // If someone took too long taking sticks
                         if(timeRemaining == 0 && currentSticks > 0) {
                             currentSticks = 0; //this is to stop the counter and end the game
                             Winner(hwnd);
                         }
                         // If AI exists, it's their turn, and there are sticks left to take
                         if(aiPlayer_yn && currentPlayer == 1 && currentSticks > 0)
                         {
                             if(easy_yn)
                                 AIControlEasy(hwnd);
                             else if(medium_yn)
                                 AIControlMedium(hwnd);
                             else if(hard_yn)
                                 AIControlHard(hwnd);
                         }
                         // Else just decrease the time remaining
                         else if(timeRemaining >= 1 && currentSticks > 0)
                             timeRemaining--;
                         InvalidateRect(hwnd,NULL,TRUE);
                         break;
                     } // END TIMER1
             } // END If(!paused)
             }// END switch(wParam)
             break;
        //-----------------------------------------------------------------------------------------------------------------------
        case WM_COMMAND:
            hMenu = GetMenu(hwnd);
            iChildID = LOWORD(wParam);
            wNote = HIWORD(wParam);
            hChild = lParam;
            // If a menu is being accessed
            if(hChild == 0)
            {
                switch(LOWORD(wParam))
                {
                    case FILE_NEW:
                        Reset(hwnd);
                        return 0;
                    case FILE_PAUSE:
                        if(pause_yn)Unpause(hwnd);
                        if(!pause_yn)Pause(hwnd);
                        return 0;
                    case FILE_EXIT:
                        Pause(hwnd);
                        quit_yn = MessageBox(hwnd,"Do you really want to quit?","Dont leave",MB_ICONQUESTION | MB_YESNO);
                        if(quit_yn == IDYES)
                            DestroyWindow(hwnd);
                        else
                            Unpause(hwnd);
                        return 0;
                    case EDIT_TIMER:
                        Pause(hwnd);
                        timeRemaining = turnTime = (DialogBoxParam(handle,MAKEINTRESOURCE(TIMER_BOX),hwnd,Timer,(LPARAM)&turnTime)) + 1; // if 5, make it 5 + the zero count instead of starting at 4
                        Unpause(hwnd);
                        return 0;
                    case EDIT_NAMES:
                        Pause(hwnd);
                        DialogBox(handle,MAKEINTRESOURCE(DIALOG_BOX),hwnd,Diag);
                        Unpause(hwnd);
                        return 0;
                    case AI_EASY:
                        easy_yn = TRUE;
                        medium_yn = FALSE;
                        hard_yn = FALSE;
                        aiPlayer_yn = TRUE;
                        Reset(hwnd);
                        return 0;
                    case AI_MEDIUM:
                        easy_yn = FALSE;
                        medium_yn = TRUE;
                        hard_yn = FALSE;
                        aiPlayer_yn = TRUE;
                        Reset(hwnd);
                        return 0;
                    case AI_HARD:
                        easy_yn = FALSE;
                        medium_yn = FALSE;
                        hard_yn = TRUE;
                        aiPlayer_yn = TRUE;
                        Reset(hwnd);
                        return 0;
                    case AI_HUMAN:
                        easy_yn = FALSE;
                        medium_yn = FALSE;
                        hard_yn = FALSE;
                        aiPlayer_yn = FALSE;
                        Reset(hwnd);
                        return 0;
                    case HELP_RULES:
                        Pause(hwnd);
                        MessageBox(hwnd,"The aim is to force your opponent to take the final stick.\n"
                        "Players take it in turns to take either 1, 2, or 3 sticks\n"
                        "You have 20 seconds to pick before your turn is skipped","Instructions",MB_OK);
                        Unpause(hwnd);
                        return 0;
                    case HELP_ABOUT:
                        Pause(hwnd);
                        MessageBox(hwnd,"This Game was created for Assessment 2 of ITC226 2009\n"
                        "Author: Cameron Bennett, 11418586\n"
                        "Code template borrowed from Errol Chopping 2009","About",MB_OK);
                        Unpause(hwnd);
                        return 0;
                } // END of the LOWORD(wParam) switch
            } // END menu access
            // If a button is being pressed instead
            else {
                //if game is unpaused, allow removal of sticks
                if (!pause_yn){
                if(iChildID == 101)//One Stick
                    TakeSticks(hwnd,1);
                else if(iChildID == 102)//Two Sticks
                    TakeSticks(hwnd,2);
                else if(iChildID == 103)//Three Sticks
                    TakeSticks(hwnd,3);
                }// end of the "unpause?" IF statement
                SetFocus(hwnd);
            } // END of the "else"
             
            InvalidateRect(hwnd,NULL,TRUE); // refreshes so timer is continuously updated
            return 0;
            
        //-----------------------------------------------------------------------------------------------------------------------
        case WM_SIZE:
             GetClientRect(hwnd,&windowRect);
             InvalidateRect(hwnd,NULL,TRUE);
             return 0;
        //-----------------------------------------------------------------------------------------------------------------------
        case WM_DESTROY:
            PostQuitMessage(0);
        return 0;
        // For messages that we don't deal with
        default:
            return DefWindowProc (hwnd, message, wParam, lParam);
    }
    return 0;
} // END of the callback proc function
/* Take sticks from the pile */
void TakeSticks(HWND hwnd,int numberTaken) {
    difference = numberTaken;
    currentSticks -= difference;
    if(currentSticks <= 0)
        Winner(hwnd);
    else
        SwitchPlayer(hwnd);
}
/* Change which player gets the next turn. */
void SwitchPlayer(HWND hwnd) {
    KillTimer(hwnd,TIMER1);
    if(currentPlayer == 0) // Player 1 becomes Player 2/AI
        currentPlayer = 1;
    else if(currentPlayer != 0) // Player 2/AI becomes Player 1
        currentPlayer = 0;
    // Restart the turn timer again
    SetTimer(hwnd,TIMER1,1000,NULL);
    timeRemaining = turnTime;
}
/* AI control, hard difficulty */
void AIControlHard(HWND hwnd) {
    int diff;
    if(currentSticks == 1 || currentSticks == 2 ||
    currentSticks == 5 || currentSticks == 6 ||
    currentSticks == 9 || currentSticks == 10 ||
    currentSticks == 13 || currentSticks == 14 ||
    currentSticks == 17 || currentSticks == 18 ||
    currentSticks == 21 || currentSticks == 22)
        TakeSticks(hwnd,1);
    
    else if(currentSticks == 3 || currentSticks == 7 ||
    currentSticks == 11 || currentSticks == 15 ||
    currentSticks == 19 || currentSticks == 23)
        TakeSticks(hwnd,2);
        
    else if(currentSticks == 4 || currentSticks == 8 || currentSticks == 12 ||
    currentSticks == 16 || currentSticks == 20 || currentSticks == 24)
        TakeSticks(hwnd,3);
}
/* AI control, medium difficulty */
void AIControlMedium(HWND hwnd) {
    int diff;
    // If there are more than 8 sticks, randomly take 1/2/3
    if(currentSticks >= 9) {
        diff = ((rand()%3) + 1); // diff = 1 || 2 || 3
        TakeSticks(hwnd,diff);
    }
    // If there are 4>x<9 sticks, randomly take 1 or 2
    if(currentSticks < 9 && currentSticks > 4) {
        diff = ((rand()%2) + 1); // diff = 1 || 2
        TakeSticks(hwnd,diff);
    }
    // If there are less than 5 left, take such that there is 1 left
    if(currentSticks <= 4 && currentSticks != 1) {
        diff = currentSticks-1;
        TakeSticks(hwnd,diff);
    }
    // Take the last stick
    if(currentSticks == 1) {
        diff = 1;
        TakeSticks(hwnd,1);
    }
}
/* AI control, easy difficulty */
void AIControlEasy(HWND hwnd) {
    // Randomly take 1, 2, or 3 sticks
    int diff = ((rand()%3) + 1);
    TakeSticks(hwnd,diff);
}
/* Toggle the game being paused/unpaused */
void Unpause(HWND hwnd) {
    pause_yn = FALSE;
    CheckMenuItem(hMenu,FILE_PAUSE,MF_UNCHECKED);
    SetTimer(hwnd,TIMER1,1000,NULL);
    InvalidateRect(hwnd,NULL,TRUE); // Redraw everything
}

/* Toggle the game being paused/unpaused */
void Pause(HWND hwnd) {
    pause_yn = !pause_yn;
    CheckMenuItem(hMenu,FILE_PAUSE,MF_CHECKED);
    KillTimer(hwnd,TIMER1);
    InvalidateRect(hwnd,NULL,TRUE); // Redraw everything
}
/* Displays a message box once the game is over*/
void Winner(HWND hwnd) {
     if(currentPlayer == 0 && !aiPlayer_yn) // Player 1 Wins over Player 2
         MessageBox(hwnd,"Player Two is the winner","Game Over",MB_OK | MB_ICONEXCLAMATION);
     if(currentPlayer == 1 && !aiPlayer_yn) // Player 2 wins over Player 1
         MessageBox(hwnd,"Player One is the winner","Game Over",MB_OK | MB_ICONEXCLAMATION);
     if(currentPlayer == 1 && aiPlayer_yn) // Player wins over AI
         MessageBox(hwnd,"Player One is the winner","Game Over",MB_OK | MB_ICONEXCLAMATION);
     if(currentPlayer == 0 && aiPlayer_yn) // AI wins over player
         MessageBox(hwnd,"The AI wins\nBetter luck next time","Game Over",MB_OK | MB_ICONEXCLAMATION);
}
/* Start a new game */
void Reset(HWND hwnd) {
    Pause(hwnd);
    currentSticks = totalSticks = (rand()%6) + 9;
    difference = 0; // 0 Sticks taken since last turn
    currentPlayer = rand()%2;
    hMenu = GetMenu(hwnd);
    CheckMenuItem(hMenu,AI_EASY,MF_UNCHECKED);
    CheckMenuItem(hMenu,AI_MEDIUM,MF_UNCHECKED);
    CheckMenuItem(hMenu,AI_HARD,MF_UNCHECKED);
    CheckMenuItem(hMenu,AI_HUMAN,MF_UNCHECKED);
    if(aiPlayer_yn && easy_yn)
        CheckMenuItem(hMenu,AI_EASY,MF_CHECKED);
    else if(aiPlayer_yn && medium_yn)
        CheckMenuItem(hMenu,AI_MEDIUM,MF_CHECKED);
    else if(aiPlayer_yn && hard_yn)
        CheckMenuItem(hMenu,AI_HARD,MF_CHECKED);
    else
        CheckMenuItem(hMenu,AI_HUMAN,MF_CHECKED);
    
    timeRemaining = turnTime;
    Unpause(hwnd);
    InvalidateRect(hwnd,NULL,TRUE); // Redraw everything
}
/* Create a small window for setting the turn length */
BOOL CALLBACK Timer (HWND timerhwnd,UINT msg,WPARAM wParam,LPARAM lParam) {
    static int buffer;
switch( msg )
{
case WM_INITDIALOG:
return TRUE;

case WM_COMMAND:
switch( LOWORD( wParam ) )
{
case TIMER_OK:
             buffer = GetDlgItemInt(timerhwnd,TIMER_EDIT,NULL,FALSE);
             EndDialog(timerhwnd,buffer);
             return TRUE;
}
return TRUE;	
}
return FALSE;
}
/* Create a small window for the name change function */
BOOL CALLBACK Diag(HWND diaghwnd,UINT msg,WPARAM wParam,LPARAM lParam) {
    static char buffer[15];
switch( msg )
{
case WM_INITDIALOG:
return TRUE;

case WM_COMMAND:
switch( LOWORD( wParam ) )
{
        case WM_INITDIALOG:
             return TRUE;
case DLG_PLAYER_ONE_CHANGE:
GetDlgItemText(diaghwnd,DLG_PLAYER_ONE_NAME,player_one_name,15);
            break;
case DLG_PLAYER_TWO_CHANGE:
GetDlgItemText(diaghwnd,DLG_PLAYER_TWO_NAME,player_two_name,15);
            break;
        case DLG_CLOSE:
             EndDialog(diaghwnd,1);
             break;
}
return TRUE;	
}
return FALSE;
}
