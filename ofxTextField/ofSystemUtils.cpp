

///--------------------------------------------------------------------------
///--------------------------------------------------------------------------
///TEXTBOX FOR OF add this code inside ofSystemUtils.cpp
///--------------------------------------------------------------------------
///--------------------------------------------------------------------------



#ifdef TARGET_OSX


@interface ofNsWindow : NSWindow

- (BOOL)canBecomeKeyWindow;

@end
@implementation ofNsWindow{
    
}
- (BOOL)canBecomeKeyWindow {
    return YES;
}
- (NSText *)fieldEditor:(BOOL)createWhenNeeded forObject:(id)anObject
{
    NSText* text = [super fieldEditor:createWhenNeeded forObject:anObject];
    if ([text isKindOfClass:[NSTextView class]])
        [(NSTextView *)text setUsesRuler:YES];
    return text;
}
@end


struct obj_ofT_{
public:
    obj_ofT_( ofNsWindow *wal_,
             NSView * uiView_,
             NSTextView *	myTextView_){
        
        wal = wal_;
        uiView = uiView_;
        myTextView = myTextView_;
        myTextField=NULL;
        
    }
    obj_ofT_( ofNsWindow *wal_,
             NSView * uiView_,
             NSTextField *	myTextField_){
        
        wal = wal_;
        uiView = uiView_;
        myTextField = myTextField_;
        myTextView = NULL;
    }
    obj_ofT_( ofNsWindow *wal_,
             NSView * uiView_,
             NSSecureTextField *	myPassField_){
        wal = wal_;
        uiView = uiView_;
        myPasswordField = myPassField_;
        myTextView = NULL;
    }
    ~obj_ofT_(){
        wal = NULL;
        uiView = NULL;
        myTextView = NULL;
        myTextField=NULL;
        myPasswordField=NULL;
    }
    ofNsWindow *wal;
    NSView * uiView;
    NSTextView *	myTextView;
    NSTextField*	myTextField;
    NSSecureTextField* myPasswordField;
    int id;
    
};
int quantity_ofBoxes;
extern "C" AXError _AXUIElementGetWindow(AXUIElementRef, CGWindowID* out);

#endif








#ifdef TARGET_WIN32



#define FAKE_WND_CLASS_NAME _T("TranFakeWnd")
#endif




void ofTextField::create(int x, int y,int w,int h){
#ifdef TARGET_WIN32
    memset(&wc, 0, sizeof(wc));
    int OF_CENTER_POSX_ = 0;
    int OF_CENTER_POSY_ = 0;
	const LPCWSTR g_szClassName = L"myWindowClass\0";
    
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = GetModuleHandle(0);
    
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon         = NULL;
    wc.lpszClassName = g_szClassName;
    
    wc.hIconSm       = NULL;
    
    
    if(!RegisterClassExW(&wc))
    {
        
        DWORD err=GetLastError();
        if ((err==ERROR_CLASS_ALREADY_EXISTS)){
            
            ;
            
        } else {
			MessageBoxW(NULL, L"Window Registration Failed!\0", L"Error!\0",
                        MB_ICONEXCLAMATION | MB_OK);
            //	return text;
		}
        
        
    }
    
    
    RECT rc;
    HWND dialog = CreateWindowExW(WS_EX_LAYERED | WS_OVERLAPPED,
                                  g_szClassName,
                                  convertNarrowToWide(question).c_str(),
                                  WS_VISIBLE | WS_OVERLAPPED,
                                  x, y, w, h,
                                  WindowFromDC(wglGetCurrentDC()), NULL, GetModuleHandle(0),NULL);
    
    GetWindowRect ( dialog, &rc ) ;
    
    
    OF_CENTER_POSX_ = (GetSystemMetrics(SM_CXSCREEN) - rc.right)/2;
    OF_CENTER_POSY_ = (GetSystemMetrics(SM_CYSCREEN) - rc.bottom)/2;
    
    
    if(dialog == NULL)
    {
        MessageBoxW(NULL, L"Window Creation Failed!\0", L"Error!\0",
                    MB_ICONEXCLAMATION | MB_OK);
        //   return text;
    }
    
    
    wchar_t wstr2[16384]= L"EDIT\0";
    
    
    // EnableWindow(WindowFromDC(wglGetCurrentDC()), FALSE);
    if(isMultiline)
        hEdit = CreateWindowExW(WS_EX_APPWINDOW, wstr2, convertNarrowToWide(text).c_str(),
                                WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_MULTILINE |ES_AUTOVSCROLL,
                                x, y, w, h,WindowFromDC(wglGetCurrentDC()), (HMENU)101, GetModuleHandle(NULL), NULL);
    else {
        if(isPassword)
            hEdit = CreateWindowExW(WS_EX_APPWINDOW, wstr2, convertNarrowToWide(text).c_str(),
                                    WS_CHILD | WS_VISIBLE | WS_TABSTOP |ES_PASSWORD,
                                    x, y, w, h,WindowFromDC(wglGetCurrentDC()), (HMENU)101, GetModuleHandle(NULL), NULL);
        
        else
            hEdit = CreateWindowExW(WS_EX_APPWINDOW, wstr2, convertNarrowToWide(text).c_str(),
                                    WS_CHILD | WS_VISIBLE |ES_AUTOHSCROLL |WS_TABSTOP | int(TextDirection_),
                                    x, y, w, h,WindowFromDC(wglGetCurrentDC()), (HMENU)101, GetModuleHandle(NULL), NULL);
        
        
    }
    
    
    
    ShowWindow(hEdit, SW_SHOWNORMAL);
    if(showingScrolBar&&isMultiline)
        ShowScrollBar(hEdit,1,TRUE);
    
    bool bFirstEmpty = true;
    DestroyWindow(dialog);
    //EnableWindow(WindowFromDC(wglGetCurrentDC()), TRUE);
    // SetFocus(WindowFromDC(wglGetCurrentDC()));
    MainWindow = ofGetWin32Window();
    
#endif
    
    
#ifdef TARGET_OSX
    
    ofNsWindow *wal;
    NSView* uiView;
    NSTextView *	myTextView;
    NSTextField *	myTextField;
    NSSecureTextField* myPasswordField;
    
    quantity_ofBoxes++;
    NSDictionary *info = [[NSBundle mainBundle] infoDictionary];
    NSString *bundleName = [NSString stringWithFormat:@"%@", [info objectForKey:@"CFBundleExecutable"]];
    standardAppName = [bundleName UTF8String];
    NSRect Srect =    [[NSScreen mainScreen] frame];
    NSRect rect =    NSMakeRect(x,y,w,h);
    NSArray * allWindows = [NSApp windows];
    uiView = [[[NSView alloc] initWithFrame:rect]autorelease];
    uiView.wantsLayer = YES;
    wal = [[[ofNsWindow alloc]initWithContentRect:rect
                                        styleMask:NSBorderlessWindowMask
                                          backing:NSBackingStoreBuffered
                                            defer:NO]autorelease];
    NSTextAlignment Allingment_ = NSTextAlignment(TextDirection_);
    
    if(!isPassword){
        if(isMultiline){
            myTextView = [[[NSTextView alloc] initWithFrame:rect]autorelease];
            [myTextView setString:[NSString stringWithCString:text.c_str()
                                                     encoding:NSUTF8StringEncoding]];
            [myTextView setEditable: YES];
            [myTextView setRichText:NO];
            NSScrollView *scrollview = [[[NSScrollView alloc]
                                         initWithFrame:rect]autorelease];
            NSSize contentSize = [scrollview contentSize];
            [scrollview setBorderType:NSGrooveBorder];
            [wal fieldEditor:YES forObject:myTextView];
            [scrollview setDocumentView:myTextView];
            [scrollview setHasVerticalScroller:YES];
            [scrollview setHasVerticalRuler:YES];
            [scrollview setAutohidesScrollers:NO];
            [scrollview setBorderType:NSBezelBorder];
            [wal setContentView:scrollview];
            [wal makeFirstResponder:myTextView];
        }else{
            myTextField = [[[NSTextField alloc] initWithFrame:rect]autorelease];
            [myTextField setStringValue:[NSString stringWithCString:text.c_str()
                                                           encoding:NSUTF8StringEncoding]];
            [myTextField setAlignment:Allingment_];
            [myTextField setBezeled:YES];
            [myTextField setEditable:YES];
            [myTextField setEnabled:YES];
            [wal setContentView:myTextField];
            [wal makeFirstResponder:myTextField];
        }
    }else{
        
        myPasswordField = [[[NSSecureTextField alloc] initWithFrame:rect]autorelease];
        [myPasswordField setStringValue:[NSString stringWithCString:text.c_str()
                                                           encoding:NSUTF8StringEncoding]];
        
        [myPasswordField setAlignment:Allingment_];
        [myPasswordField setBezeled:YES];
        [myPasswordField setEditable:YES];
        [myPasswordField setEnabled:YES];
        [wal setContentView:myPasswordField];
        [wal makeFirstResponder:myPasswordField];
        
    }
    
    
    [wal setLevel:NSNormalWindowLevel];
    [wal makeKeyAndOrderFront:wal];
    [wal orderFront:NSApp];
    NSWindow * aWindow = (NSWindow*)ofGetCocoaWindow();
    appWindow.size.height = [aWindow frame].size.height;
    appWindow.size.width =[aWindow frame].size.width;
    appWindow.origin.x = [aWindow frame].origin.x;
    appWindow.origin.y =[aWindow frame].origin.y;
    NSRect rectofT =  NSMakeRect(appWindow.origin.x+x,(appWindow.origin.y+appWindow.size.height-20)-(y+h),w,h);
    [wal setFrame:rectofT display:YES];
    [aWindow addChildWindow:wal ordered:NSWindowAbove];
    pointerToWindow = ofGetCocoaWindow();
    
    if(!isPassword){
        if(isMultiline)
            pointer = new obj_ofT_(wal,uiView,myTextView);
        else
            pointer = new obj_ofT_(wal,uiView,myTextField);
    }else pointer = new obj_ofT_(wal,uiView,myPasswordField);
    
    
    
    
    
    //  NSLog(@"%@",allWindows);
    
#endif
    
}
ofTextField::ofTextField(){
    TextDirection_ = ofTextField_Alling_LEFT;
    isCreated = false;
    posX=0,posY=0,width=0,height=0;
    showingScrolBar=false;
    isMultiline=false;//change this if you want multiline text as default
    isPassword =false;
    isHiding =false;
    isDrawing = true;
#ifdef TARGET_OSX
    pointerToWindow = NULL;
#endif
    
}
ofTextField::  ~ofTextField(){
    if(isCreated){
#ifdef TARGET_WIN32
        DestroyWindow(hEdit);
#endif
#ifdef TARGET_OSX
        NSWindow * aWindow = (NSWindow *)pointerToWindow;
        if ([[NSApp windows]count]>1){
            [aWindow removeChildWindow:pointer->wal];
        }
        
        delete pointer;
        pointerToWindow =NULL;
        
#endif
        isCreated=false;
    }
    
}

bool ofTextField::activeApp(){
    bool isactive=false;
#ifdef TARGET_OSX
    
    //get if of App is in focus (selected) (thought it might be usefull in some cases
    
    
    NSDictionary *activeApp = [[NSWorkspace sharedWorkspace] activeApplication];
    
    
    //This is the only place we should use Ascii encoding though UTF8 will also work
    if([(NSString *)[activeApp objectForKey:@"NSApplicationName"]isEqual:
        [NSString stringWithCString:standardAppName.c_str() encoding:NSASCIIStringEncoding]]){
        
        isactive = true;
        
        
        
        
    }else{
        
        isactive =false;
        
    }
    
    
    
#endif
    /*
     add pc part of code
     
     */
    return isactive;
    
    
}


bool ofTextField::isActive(){
    bool isTextboxActive =false;
#ifdef TARGET_WIN32
    TRACKMOUSEEVENT mouseOverTextbox;
    mouseOverTextbox.cbSize = sizeof(TRACKMOUSEEVENT);
    mouseOverTextbox.dwFlags = TME_HOVER;
    mouseOverTextbox.hwndTrack = this->hEdit;
    return TrackMouseEvent(&mouseOverTextbox);
#endif
    
    return isTextboxActive;
    
}


void ofTextField::draw(int x, int y,int w,int h){
    if(!isCreated){
        create(x,y,w,h);
        isCreated=true;
    }else{
        isDrawing=true;
        bool rePosisionTheTextBox = false;
#ifdef TARGET_OSX
        NSWindow * aWindow = (NSWindow *)pointerToWindow;
        if([aWindow frame].size.width!=appWindow.size.width||[aWindow frame].size.height!=appWindow.size.height)rePosisionTheTextBox=true;
        
#endif
        
        if(x!=posX||y!=posY||w!=width||h!=height||rePosisionTheTextBox){
#ifdef TARGET_WIN32
            SetWindowPos(hEdit, 0,  x, y, w, h, SWP_NOZORDER);
            if(isActive())SetFocus(hEdit); else SetFocus(MainWindow);
            
#endif
            
#ifdef TARGET_OSX
            
            appWindow.size.height = [aWindow frame].size.height;
            appWindow.size.width =[aWindow frame].size.width;
            appWindow.origin.x = [aWindow frame].origin.x;
            appWindow.origin.y =[aWindow frame].origin.y;
            
            
            
            NSRect Srect =    [[NSScreen mainScreen] frame];
            
            NSRect rectofT =  NSMakeRect(appWindow.origin.x+x,(appWindow.origin.y+appWindow.size.height-20)-(y+h),w,h);
            
            [pointer->wal setFrame:rectofT display:!isHiding animate:NO];
            [aWindow setMinSize:NSMakeSize(x+w, h+y+h)];
            
#endif
            posX=x,posY=y,width=w,height=h;
            
        }
    }
    
}

string ofTextField::getText(){
    if(isCreated){
#ifdef TARGET_WIN32
        wchar_t wstr[16384];
        GetWindowTextW( hEdit, wstr, 16384 );
        char mbstr2[16384];
        
        WideCharToMultiByte(CP_UTF8,0,wstr,16384,mbstr2,16384,NULL,NULL);
        text = mbstr2;
#endif
#ifdef TARGET_OSX
        if(!isPassword){
            if (!isMultiline)
                text = [[pointer->myTextField stringValue] UTF8String];
            else
                text = [[pointer->myTextView string] UTF8String];
        }else text = [[pointer->myPasswordField stringValue]UTF8String];
#endif
    }
    return text;
    
}
bool ofTextField::showScrollBar(bool showing){
#ifdef TARGET_WIN32
    ShowScrollBar(hEdit,101,showingScrolBar);
#endif
    showingScrolBar = showing;
    return showingScrolBar;
    
    
}
bool ofTextField::setMultiline(bool multln){
    showScrollBar(multln);
    
    
    isMultiline = multln;
    return isMultiline;
}

void ofTextField::hide()
{       if(!isHiding&&isCreated){
#ifdef TARGET_WIN32
    
    ShowWindow(hEdit, SW_HIDE);
    EnableWindow(hEdit,false);
#endif
    
#ifdef TARGET_OSX
    if (!isMultiline){
        [pointer->myTextField setHidden:YES];
        [pointer->wal setAlphaValue:0];
    }
    else{
        [pointer->myTextView setHidden:YES];
        [pointer->wal setAlphaValue:0];
        
    }
    
    
#endif
    isHiding = true;
    
}
}
bool ofTextField::getIsHiding(){
    return isHiding;
}
void ofTextField::show()
{
    
    if(isHiding&&isCreated){
        
#ifdef TARGET_WIN32
        
        ShowWindow(hEdit, SW_SHOWNORMAL);
        EnableWindow(hEdit,true);
#endif
        
#ifdef TARGET_OSX
        
        if (!isMultiline){
            [pointer->myTextField setHidden:NO];
            [pointer->wal setAlphaValue:1];
        }
        else{
            [pointer->myTextView setHidden:NO];
            [pointer->wal setAlphaValue:1];
            
        }
        
#endif
        isHiding = false;
    }
    
    
}
bool ofTextField::setPassWordMode(bool passwrdmd ){
    
    isPassword= passwrdmd;
    
    return isPassword;
}
void ofTextField::setText(string dtext){
    if(isCreated){
#ifdef TARGET_WIN32
        
        SetWindowTextW(hEdit,convertNarrowToWide(dtext).c_str());
#endif
#ifdef TARGET_OSX
        if (!isMultiline){
            [pointer->myTextField setStringValue:[NSString stringWithCString:dtext.c_str()
                                                                    encoding:NSUTF8StringEncoding]];
        }
        else{
            [pointer->myTextView setString:[NSString stringWithCString:dtext.c_str()
                                                              encoding:NSUTF8StringEncoding]];
            
        }
#endif
        
    }else{
        text =dtext;
    }
    //code needed
    
    
}
void ofTextField::setTextDir(ofTextField_Allingment direction){
    if(!isCreated){
        TextDirection_ = direction;
    }else cout<<"ofTField::Allingment Should Be called on Setup"<<endl;
}

///this is the weird part of the code
//since this textbox is drawing above openGL
//you might want to use this function to figure out
//how to hide it when you don't want it.
void ofTextField::hideIfNotDrawing(){
#ifdef TARGET_WIN32
    
    if(!isDrawing)hide();
    if(isDrawing){
        
        isDrawing=false;
    }
#endif
    
#ifdef TARGET_OSX
    
    //code needed
#endif
    
    
}






