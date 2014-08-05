
///--------------------------------------------------------------------------
///--------------------------------------------------------------------------
///TEXTBOX FOR OF add the following code inside ofSystemUtils.h
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
enum ofTextField_Allingment{
    ofTextField_Alling_LEFT=0,
    ofTextField_Alling_RIGHT=2,
    ofTextField_Alling_CENTER=1
};
#endif
#ifdef TARGET_OSX
enum ofTextField_Allingment{
    ofTextField_Alling_LEFT=0,
    ofTextField_Alling_RIGHT=1,
    ofTextField_Alling_CENTER=2
};
class obj_ofT_;
extern int quantity_ofBoxes;
#endif


class ofTextField{
    string text,question, standardAppName;
    bool isCreated,isMultiline,isPassword,isHiding,isDrawing,isCentered;
    void create(int x, int y,int w,int h);
#ifdef TARGET_WIN32
    
    HWND hEdit;
    HWND MainWindow;
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
    ofTextField_Allingment TextDirection_;
public:
    bool activeApp();
    bool isActive();
    ofTextField();
    ~ofTextField();
    void draw(int x, int y,int w,int h);
    string getText();
    bool showScrollBar(bool showing = true);
    bool setPassWordMode(bool passwrdmd = true);
    bool setMultiline(bool multiln=true);
    void hide();
    void show();
    bool getIsHiding();
    void hideIfNotDrawing();
    void setText(string dtext="");
    void setTextDir(ofTextField_Allingment direction= ofTextField_Alling_CENTER);
};