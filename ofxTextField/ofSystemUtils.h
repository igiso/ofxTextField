#pragma once


class ofFileDialogResult{
	public:
		ofFileDialogResult();
		
		//TODO: only 1 file for now
		string getName();
		string getPath();
		
		string filePath;
		string fileName;
		bool bSuccess;
};

void ofSystemAlertDialog(string errorMessage);
ofFileDialogResult ofSystemLoadDialog(string windowTitle="", bool bFolderSelection = false, string defaultPath="");
ofFileDialogResult ofSystemSaveDialog(string defaultName, string messageName);
string ofSystemTextBoxDialog(string question, string text="");


///--------------------------------------------------------------------------
///--------------------------------------------------------------------------
///TEXTBOX FOR OF
///--------------------------------------------------------------------------
///--------------------------------------------------------------------------


#ifdef TARGET_WIN32

#include <winuser.h>
#include <commdlg.h>
#define _WIN32_DCOM

#include <windows.h>
#include <shlobj.h>
#include <tchar.h>
#include <stdio.h>
#endif



#ifdef TARGET_OSX
class obj_ofT_;
extern int quantity_ofBoxes;
#endif


class ofTextField{
    string text,question, standardAppName;
    bool isCreated,isMultiline,isPassword,isHiding,isDrawing,isCentered;
    void create(int x, int y,int w,int h);
#ifdef TARGET_WIN32
    
    HWND hEdit;
    WNDCLASSEXW wc;
	MSG Msg;
#endif
#ifdef TARGET_OSX
    CGRect appWindow;
    obj_ofT_ * pointer;
    void *pointerToWindow;
#endif
    
    int winPosx,winPosy;
	int posX,posY,width,height;
	bool showingScrolBar;
    bool showScrollBar(bool showing = true);
public:
    bool activeApp();
    ofTextField();
    ~ofTextField();
    void draw(int x, int y,int w,int h);//use in loop
    
    string getText();//use in loop
    bool setPassWordMode(bool passwrdmd = true);//call in setup() (pc only)
    bool setMultiline(bool multiln=true);//setup()
    void hide();//loop
    void show();//loop
    bool getIsHiding();//loop
    void hideIfNotDrawing();//dont' use this yet
    void setText(string dtext="");//setup or loop
    void setAllignCenter(bool center=true);//don't use this yet
};
