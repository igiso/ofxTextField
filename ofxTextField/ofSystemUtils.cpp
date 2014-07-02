
#include "ofConstants.h"
#include "ofSystemUtils.h"
#include "ofFileUtils.h"
#include "ofLog.h"
#include "ofUtils.h"
#include "ofAppRunner.h"

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
	// ofSystemUtils.cpp is configured to build as
	// objective-c++ so as able to use Cocoa dialog panels
	// This is done with this compiler flag
	//		-x objective-c++
	// http://www.yakyak.org/viewtopic.php?p=1475838&sid=1e9dcb5c9fd652a6695ac00c5e957822#p1475838

	#include <Cocoa/Cocoa.h>
#endif

#ifdef TARGET_WIN32
#include <locale>
#include <sstream>
#include <string>

std::string convertWideToNarrow( const wchar_t *s, char dfault = '?',
                      const std::locale& loc = std::locale() )
{
  std::ostringstream stm;

  while( *s != L'\0' ) {
    stm << std::use_facet< std::ctype<wchar_t> >( loc ).narrow( *s++, dfault );
  }
  return stm.str();
}

std::wstring convertNarrowToWide( const std::string& as ){
    // deal with trivial case of empty string
    if( as.empty() )    return std::wstring();

    // determine required length of new string
    size_t reqLength = ::MultiByteToWideChar( CP_UTF8, 0, as.c_str(), (int)as.length(), 0, 0 );

    // construct new string of required length
    std::wstring ret( reqLength, L'\0' );

    // convert old string to new string
    ::MultiByteToWideChar( CP_UTF8, 0, as.c_str(), (int)as.length(), &ret[0], (int)ret.length() );

    // return new string ( compiler should optimize this away )
    return ret;
}

#endif

#if defined( TARGET_OSX )
static void restoreAppWindowFocus(){
	NSWindow * appWindow = (NSWindow *)ofGetCocoaWindow();
	if(appWindow) {
		[appWindow makeKeyAndOrderFront:nil];
	}
}
#endif

#if defined( TARGET_LINUX ) && defined (OF_USING_GTK)
#include <gtk/gtk.h>
#include "ofGstUtils.h"

static void initGTK(){
	static bool initialized = false;
	if(!initialized){
		ofGstUtils::startGstMainLoop();
	    gdk_threads_init();
	int argc=0; char **argv = NULL;
	gtk_init (&argc, &argv);
		initialized = true;
	}

}

static string gtkFileDialog(GtkFileChooserAction action,string windowTitle,string defaultName=""){
	initGTK();
	string results;

	const gchar* button_name = "";
	switch(action){
	case GTK_FILE_CHOOSER_ACTION_OPEN:
		button_name = GTK_STOCK_OPEN;
		break;
	case GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER:
		button_name = GTK_STOCK_SELECT_ALL;
		break;
	case GTK_FILE_CHOOSER_ACTION_SAVE:
		button_name = GTK_STOCK_SAVE;
		break;
	default:
		return "";
		break;
	}

	GtkWidget *dialog = gtk_file_chooser_dialog_new (windowTitle.c_str(),
						  NULL,
						  action,
						  button_name, GTK_RESPONSE_ACCEPT,
						  GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
						  NULL);

	gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog),defaultName.c_str());

	gdk_threads_enter();
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
		results = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
	}
	gtk_widget_destroy (dialog);
	gdk_threads_leave();
	//gtk_dialog_run(GTK_DIALOG(dialog));
	//startGTK(dialog);
	return results;
}

#endif
#ifdef TARGET_ANDROID
#include "ofxAndroidUtils.h"
#endif

//------------------------------------------------------------------------------
ofFileDialogResult::ofFileDialogResult(){
	filePath = "";
	fileName = "";
	bSuccess = false;
}

//------------------------------------------------------------------------------
string ofFileDialogResult::getName(){
	return fileName;
}

//------------------------------------------------------------------------------
string ofFileDialogResult::getPath(){
	return filePath;
}


//------------------------------------------------------------------------------
void ofSystemAlertDialog(string errorMessage){


	#ifdef TARGET_WIN32
		// we need to convert error message to a wide char message.
		// first, figure out the length and allocate a wchar_t at that length + 1 (the +1 is for a terminating character)
		int length = strlen(errorMessage.c_str());
		wchar_t * widearray = new wchar_t[length+1];
		memset(widearray, 0, sizeof(wchar_t)*(length+1));
		// then, call mbstowcs:
		// http://www.cplusplus.com/reference/clibrary/cstdlib/mbstowcs/
		mbstowcs(widearray, errorMessage.c_str(), length);
		// launch the alert:
		MessageBoxW(NULL, widearray, L"alert", MB_OK);
		// clear the allocated memory:
		delete widearray;
	#endif


	#ifdef TARGET_OSX
		NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
		NSAlert *alertDialog = [NSAlert alertWithMessageText:[NSString stringWithUTF8String:errorMessage.c_str()]
											   defaultButton:nil
											 alternateButton:nil
												 otherButton:nil
								   informativeTextWithFormat:@""];
		[alertDialog runModal];
		restoreAppWindowFocus();
		[pool drain];
	#endif

	#if defined( TARGET_LINUX ) && defined (OF_USING_GTK)
		initGTK();
		GtkWidget* dialog = gtk_message_dialog_new (NULL, (GtkDialogFlags) 0, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "%s", errorMessage.c_str());

		gdk_threads_enter();
		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);
		gdk_threads_leave();
	#endif

	#ifdef TARGET_ANDROID
		ofxAndroidAlertBox(errorMessage);
	#endif
}

//----------------------------------------------------------------------------------------
#ifdef TARGET_WIN32
//---------------------------------------------------------------------
static int CALLBACK loadDialogBrowseCallback(
  HWND hwnd,
  UINT uMsg,
  LPARAM lParam,
  LPARAM lpData
){
    string defaultPath = *(string*)lpData;
    if(defaultPath!="" && uMsg==BFFM_INITIALIZED){
		wchar_t         wideCharacterBuffer[MAX_PATH];
		wcscpy(wideCharacterBuffer, convertNarrowToWide(ofToDataPath(defaultPath)).c_str());
        SendMessage(hwnd,BFFM_SETSELECTION,1,(LPARAM)wideCharacterBuffer);
    }

	return 0;
}
//----------------------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------

// OS specific results here.  "" = cancel or something bad like can't load, can't save, etc...
ofFileDialogResult ofSystemLoadDialog(string windowTitle, bool bFolderSelection, string defaultPath){

	ofFileDialogResult results;

	//----------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------       OSX
	//----------------------------------------------------------------------------------------
#ifdef TARGET_OSX

	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	NSOpenPanel * loadDialog = [NSOpenPanel openPanel];
	[loadDialog setAllowsMultipleSelection:NO];
	[loadDialog setCanChooseDirectories:bFolderSelection];
	[loadDialog setResolvesAliases:YES];

	if(!windowTitle.empty()) {
		[loadDialog setTitle:[NSString stringWithUTF8String:windowTitle.c_str()]];
	}

	if(!defaultPath.empty()) {
		NSString * s = [NSString stringWithUTF8String:defaultPath.c_str()];
		s = [[s stringByExpandingTildeInPath] stringByResolvingSymlinksInPath];
		NSURL * defaultPathUrl = [NSURL fileURLWithPath:s];
		[loadDialog setDirectoryURL:defaultPathUrl];
	}

	NSInteger buttonClicked = [loadDialog runModal];
	restoreAppWindowFocus();

	if(buttonClicked == NSFileHandlingPanelOKButton) {
		NSURL * selectedFileURL = [[loadDialog URLs] objectAtIndex:0];
		results.filePath = string([[selectedFileURL path] UTF8String]);
	}
	[pool drain];

#endif
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------   windoze
	//----------------------------------------------------------------------------------------
#ifdef TARGET_WIN32
	wstring windowTitleW;
	windowTitleW.assign(windowTitle.begin(), windowTitle.end());

	if (bFolderSelection == false){

        OPENFILENAME ofn;

		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		HWND hwnd = WindowFromDC(wglGetCurrentDC());
		ofn.hwndOwner = hwnd;
#ifdef __MINGW32_VERSION
		char szFileName[MAX_PATH];
        memset(szFileName,0,260);
		if(defaultPath!=""){
            strcpy(szFileName,ofToDataPath(defaultPath).c_str());
		}

		ofn.lpstrFilter = "All\0";
		ofn.lpstrFile = szFileName;
#else // Visual Studio
		wchar_t szFileName[MAX_PATH];
		wchar_t szTitle[MAX_PATH];
		if(defaultPath!=""){
			wcscpy_s(szFileName,convertNarrowToWide(ofToDataPath(defaultPath)).c_str());
		}else{
		    //szFileName = L"";
			memset(szFileName,  0, sizeof(szFileName));
		}

		if (windowTitle != "") {
			wcscpy_s(szTitle, convertNarrowToWide(windowTitle).c_str());
			ofn.lpstrTitle = szTitle;
		} else {
			ofn.lpstrTitle = NULL;
		}

		ofn.lpstrFilter = L"All\0";
		ofn.lpstrFile = szFileName;
#endif
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
		ofn.lpstrDefExt = 0;
        
#ifdef __MINGW32_VERSION
		ofn.lpstrTitle = windowTitle.c_str();
#else
		ofn.lpstrTitle = windowTitleW.c_str();
#endif 

		if(GetOpenFileName(&ofn)) {
#ifdef __MINGW32_VERSION
			results.filePath = string(szFileName);
#else
			results.filePath = convertWideToNarrow(szFileName);
#endif

		}

	} else {

		BROWSEINFOW      bi;
		wchar_t         wideCharacterBuffer[MAX_PATH];
		wchar_t			wideWindowTitle[MAX_PATH];
		LPITEMIDLIST    pidl;
		LPMALLOC		lpMalloc;

		if (windowTitle != "") {
			wcscpy(wideWindowTitle, convertNarrowToWide(windowTitle).c_str());
		} else {
			wcscpy(wideWindowTitle, L"Select Directory");
		}

		// Get a pointer to the shell memory allocator
		if(SHGetMalloc(&lpMalloc) != S_OK){
			//TODO: deal with some sort of error here?
		}
		bi.hwndOwner        =   NULL;
		bi.pidlRoot         =   NULL;
		bi.pszDisplayName   =   wideCharacterBuffer;
		bi.lpszTitle        =   wideWindowTitle;
		bi.ulFlags          =   BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
		bi.lpfn             =   &loadDialogBrowseCallback;
		bi.lParam           =   (LPARAM) &defaultPath;
		bi.lpszTitle        =   windowTitleW.c_str();

		if(pidl = SHBrowseForFolderW(&bi)){
			// Copy the path directory to the buffer
			if(SHGetPathFromIDListW(pidl,wideCharacterBuffer)){
				results.filePath = convertWideToNarrow(wideCharacterBuffer);
			}
			lpMalloc->Free(pidl);
		}
		lpMalloc->Release();
	}

	//----------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------   windoze
	//----------------------------------------------------------------------------------------
#endif




	//----------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------   linux
	//----------------------------------------------------------------------------------------
#if defined( TARGET_LINUX ) && defined (OF_USING_GTK)
		if(bFolderSelection) results.filePath = gtkFileDialog(GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,windowTitle,defaultPath);
		else results.filePath = gtkFileDialog(GTK_FILE_CHOOSER_ACTION_OPEN,windowTitle,defaultPath);
#endif
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------



	if( results.filePath.length() > 0 ){
		results.bSuccess = true;
		results.fileName = ofFilePath::getFileName(results.filePath);
	}

	return results;
}



ofFileDialogResult ofSystemSaveDialog(string defaultName, string messageName){

	ofFileDialogResult results;

	//----------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------       OSX
	//----------------------------------------------------------------------------------------
#ifdef TARGET_OSX

	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	NSSavePanel * saveDialog = [NSSavePanel savePanel];
	[saveDialog setMessage:[NSString stringWithUTF8String:messageName.c_str()]];
	[saveDialog setNameFieldStringValue:[NSString stringWithUTF8String:defaultName.c_str()]];

	NSInteger buttonClicked = [saveDialog runModal];
	restoreAppWindowFocus();

	if(buttonClicked == NSFileHandlingPanelOKButton){
		results.filePath = string([[[saveDialog URL] path] UTF8String]);
	}
	[pool drain];

#endif
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------   windoze
	//----------------------------------------------------------------------------------------
#ifdef TARGET_WIN32


	wchar_t fileName[MAX_PATH] = L"";
	char * extension;
	OPENFILENAMEW ofn;
    memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	HWND hwnd = WindowFromDC(wglGetCurrentDC());
	ofn.hwndOwner = hwnd;
	ofn.hInstance = GetModuleHandle(0);
	ofn.nMaxFileTitle = 31;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = L"All Files (*.*)\0*.*\0";
	ofn.lpstrDefExt = L"";	// we could do .rxml here?
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
	ofn.lpstrTitle = L"Select Output File";

	if (GetSaveFileNameW(&ofn)){
		results.filePath = convertWideToNarrow(fileName);
	}

#endif
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------


	//----------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------   linux
	//----------------------------------------------------------------------------------------
#if defined( TARGET_LINUX ) && defined (OF_USING_GTK)

	results.filePath = gtkFileDialog(GTK_FILE_CHOOSER_ACTION_SAVE, messageName,defaultName);

#endif
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------

	if( results.filePath.length() > 0 ){
		results.bSuccess = true;
		results.fileName = ofFilePath::getFileName(results.filePath);
	}

	return results;
}

#ifdef TARGET_WIN32
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    //switch(msg)
    //{
    //    case WM_CLOSE:
    //        DestroyWindow(hwnd);
    //    break;
    //    case WM_DESTROY:
    //        PostQuitMessage(0);
    //    break;
    //    default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    //}
}
#endif


string ofSystemTextBoxDialog(string question, string text){
#if defined( TARGET_LINUX ) && defined (OF_USING_GTK)
	initGTK();
	GtkWidget* dialog = gtk_message_dialog_new (NULL, (GtkDialogFlags) 0, GTK_MESSAGE_QUESTION, (GtkButtonsType) GTK_BUTTONS_OK_CANCEL, "%s", question.c_str() );
	GtkWidget* content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
	GtkWidget* textbox = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(textbox),text.c_str());
	gtk_container_add (GTK_CONTAINER (content_area), textbox);
	gdk_threads_enter();
	gtk_widget_show_all (dialog);
	if(gtk_dialog_run (GTK_DIALOG (dialog))==GTK_RESPONSE_OK){
		text = gtk_entry_get_text(GTK_ENTRY(textbox));
	}
	gtk_widget_destroy (dialog);
	gdk_threads_leave();
#endif

#ifdef TARGET_OSX
	// create alert dialog
	NSAlert *alert = [[[NSAlert alloc] init] autorelease];
	[alert addButtonWithTitle:@"OK"];
	[alert addButtonWithTitle:@"Cancel"];
	[alert setMessageText:[NSString stringWithCString:question.c_str()
											 encoding:NSUTF8StringEncoding]];
	// create text field
	NSTextField* label = [[NSTextField alloc] initWithFrame:NSRectFromCGRect(CGRectMake(0,0,300,40))];
	[label setStringValue:[NSString stringWithCString:text.c_str()
											 encoding:NSUTF8StringEncoding]];
	// add text field to alert dialog
	[alert setAccessoryView:label];
	NSInteger returnCode = [alert runModal];
	// if OK was clicked, assign value to text
	if ( returnCode == NSAlertFirstButtonReturn )
		text = [[label stringValue] UTF8String];
#endif

#ifdef TARGET_WIN32
    // we need to convert error message to a wide char message.
    // first, figure out the length and allocate a wchar_t at that length + 1 (the +1 is for a terminating character)

	WNDCLASSEX wc;
	MSG Msg;

	//we have to do this because mingw wants non wide strings and vs wants wide strings
    #ifdef _MSC_VER

        #define TMP_STR_CONVERT LPCWSTR

		const LPCWSTR g_szClassName = L"myWindowClass\0";

		//Step 1: Registering the Window Class
		wc.cbSize        = sizeof(WNDCLASSEX);
		wc.style         = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc   = WndProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = GetModuleHandle(0);
		wc.lpszClassName = g_szClassName;
		wc.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
		wc.lpszMenuName  = NULL;
		wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
		wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
		if(!RegisterClassEx(&wc)){
			DWORD err=GetLastError();
			if ((err==ERROR_CLASS_ALREADY_EXISTS)){
                ; // we are ok
                // http://stackoverflow.com/questions/5791996/re-registering-user-defined-window-class-c
            } else {
			MessageBox(NULL, L"Window Registration Failed!\0", L"Error!\0",
				MB_ICONEXCLAMATION | MB_OK);
			return text;
		}
		}

		HWND dialog = CreateWindowEx(WS_EX_DLGMODALFRAME,
			g_szClassName,
			convertNarrowToWide(question).c_str(),
			WS_POPUP | WS_CAPTION | DS_MODALFRAME | WS_SYSMENU,
			CW_USEDEFAULT, CW_USEDEFAULT, 240, 140,
			WindowFromDC(wglGetCurrentDC()), NULL, GetModuleHandle(0),NULL);

		if(dialog == NULL)
		{
			
			MessageBox(NULL,L"Window Creation Failed!\0", L"Error!\0",
				MB_ICONEXCLAMATION | MB_OK);
			return text;
			
		}

		EnableWindow(WindowFromDC(wglGetCurrentDC()), FALSE);
		HWND hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT\0", convertNarrowToWide(text).c_str(),
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			10, 10, 210, 40, dialog, (HMENU)101, GetModuleHandle(NULL), NULL);


		HWND okButton = CreateWindowEx(WS_EX_CLIENTEDGE, L"BUTTON\0", L"OK\0",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			10, 60, 60, 30, dialog, (HMENU)IDOK, GetModuleHandle(NULL), NULL);

		HWND cancelButton = CreateWindowEx(WS_EX_CLIENTEDGE, L"BUTTON\0", L"Cancel\0",
			WS_CHILD | WS_VISIBLE,
			80, 60, 60, 30, dialog, (HMENU)IDCANCEL, GetModuleHandle(NULL), NULL);

		SetFocus( hEdit );

		ShowWindow(dialog, SW_SHOWNORMAL);
		bool bFirstEmpty = true;
		while (true){
			 if (!PeekMessageW( &Msg, 0, 0, 0, PM_REMOVE )){
				 if (bFirstEmpty){
					 // ShowWindow the first time the queue goes empty
					 ShowWindow( dialog, SW_SHOWNORMAL );
					 bFirstEmpty = FALSE;
				 }
				 if (!(GetWindowLongW( dialog, GWL_STYLE ) & DS_NOIDLEMSG)){
					 // No message present -> send ENTERIDLE and wait
					 SendMessageW( WindowFromDC(wglGetCurrentDC()), WM_ENTERIDLE, MSGF_DIALOGBOX, (LPARAM)dialog );
				 }
				 GetMessageW( &Msg, 0, 0, 0 );
			 }

			 if (Msg.message == WM_QUIT){
				 PostQuitMessage( Msg.wParam );
				 if (!IsWindow( dialog )){
					EnableWindow(WindowFromDC(wglGetCurrentDC()), TRUE);
					return text;
				 }
				 break;
			 }

			 if (!IsWindow( dialog )){
				EnableWindow(WindowFromDC(wglGetCurrentDC()), TRUE);
				return text;
			 }

			 TranslateMessage( &Msg );
			 DispatchMessageW( &Msg );

			 if((Msg.hwnd == okButton && Msg.message==WM_LBUTTONUP) || (Msg.message==WM_KEYUP && Msg.wParam==13)){
				 break;
			 }else if((Msg.hwnd == cancelButton && Msg.message==WM_LBUTTONUP) ||  (Msg.message==WM_KEYUP && Msg.wParam==27)){
				 EnableWindow(WindowFromDC(wglGetCurrentDC()), TRUE);
				 DestroyWindow(dialog);
				 return text;
			 }

			 if (!IsWindow( dialog )){
				EnableWindow(WindowFromDC(wglGetCurrentDC()), TRUE);
				return text;
			 }

			 if (bFirstEmpty && Msg.message == WM_TIMER){
				 ShowWindow( dialog, SW_SHOWNORMAL );
				 bFirstEmpty = FALSE;
			 }
		 }

		 char buf[16384];
		 GetDlgItemTextA( dialog, 101, buf, 16384 );
		 text = buf;

		 DestroyWindow(dialog);
		 EnableWindow(WindowFromDC(wglGetCurrentDC()), TRUE);

	#else

		const LPCSTR g_szClassName = "myWindowClass\0";

		//Step 1: Registering the Window Class
		wc.cbSize        = sizeof(WNDCLASSEX);
		wc.style         = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc   = WndProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = GetModuleHandle(0);
		wc.lpszClassName = g_szClassName;
		wc.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
		wc.lpszMenuName  = NULL;
		wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
		wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
		if(!RegisterClassEx(&wc))
		{

		    DWORD err=GetLastError();
            if ((err==ERROR_CLASS_ALREADY_EXISTS)){
                ; // we are ok
                // http://stackoverflow.com/questions/5791996/re-registering-user-defined-window-class-c
            } else {
			MessageBox(NULL, "Window Registration Failed!\0", "Error!\0",
				MB_ICONEXCLAMATION | MB_OK);
			return text;
		}


		}

		HWND dialog = CreateWindowEx(WS_EX_DLGMODALFRAME,
			g_szClassName,
			question.c_str(),
			WS_POPUP | WS_CAPTION | DS_MODALFRAME | WS_SYSMENU,
			CW_USEDEFAULT, CW_USEDEFAULT, 240, 140,
			WindowFromDC(wglGetCurrentDC()), NULL, GetModuleHandle(0),NULL);

		if(dialog == NULL)
		{
			MessageBox(NULL, "Window Creation Failed!\0", "Error!\0",
				MB_ICONEXCLAMATION | MB_OK);
			return text;
		}

		EnableWindow(WindowFromDC(wglGetCurrentDC()), FALSE);
		HWND hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT\0", text.c_str(),
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			10, 10, 210, 40, dialog, (HMENU)101, GetModuleHandle(NULL), NULL);


		HWND okButton = CreateWindowEx(WS_EX_CLIENTEDGE, "BUTTON\0", "OK\0",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			10, 60, 60, 30, dialog, (HMENU)IDOK, GetModuleHandle(NULL), NULL);

		HWND cancelButton = CreateWindowEx(WS_EX_CLIENTEDGE, "BUTTON\0", "Cancel\0",
			WS_CHILD | WS_VISIBLE,
			80, 60, 60, 30, dialog, (HMENU)IDCANCEL, GetModuleHandle(NULL), NULL);

		SetFocus( hEdit );

		ShowWindow(dialog, SW_SHOWNORMAL);
		bool bFirstEmpty = true;
		while (true){
			 if (!PeekMessageW( &Msg, 0, 0, 0, PM_REMOVE )){
				 if (bFirstEmpty){
					 // ShowWindow the first time the queue goes empty
					 ShowWindow( dialog, SW_SHOWNORMAL );
					 bFirstEmpty = FALSE;
				 }
				 if (!(GetWindowLongW( dialog, GWL_STYLE ) & DS_NOIDLEMSG)){
					 // No message present -> send ENTERIDLE and wait
					 SendMessageW( WindowFromDC(wglGetCurrentDC()), WM_ENTERIDLE, MSGF_DIALOGBOX, (LPARAM)dialog );
				 }
				 GetMessageW( &Msg, 0, 0, 0 );
			 }

			 if (Msg.message == WM_QUIT){
				 PostQuitMessage( Msg.wParam );
				 if (!IsWindow( dialog )){
					EnableWindow(WindowFromDC(wglGetCurrentDC()), TRUE);
					return text;
				 }
				 break;
			 }

			 if (!IsWindow( dialog )){
				EnableWindow(WindowFromDC(wglGetCurrentDC()), TRUE);
				return text;
			 }

			 TranslateMessage( &Msg );
			 DispatchMessageW( &Msg );

			 if((Msg.hwnd == okButton && Msg.message==WM_LBUTTONUP) || (Msg.message==WM_KEYUP && Msg.wParam==13)){
				 break;
			 }else if((Msg.hwnd == cancelButton && Msg.message==WM_LBUTTONUP) ||  (Msg.message==WM_KEYUP && Msg.wParam==27)){
				 EnableWindow(WindowFromDC(wglGetCurrentDC()), TRUE);
				 DestroyWindow(dialog);
				 return text;
			 }

			 if (!IsWindow( dialog )){
				EnableWindow(WindowFromDC(wglGetCurrentDC()), TRUE);
				return text;
			 }

			 if (bFirstEmpty && Msg.message == WM_TIMER){
				 ShowWindow( dialog, SW_SHOWNORMAL );
				 bFirstEmpty = FALSE;
			 }
		 }

		 char buf[16384];
		 GetDlgItemTextA( dialog, 101, buf, 16384 );
		 text = buf;

		 DestroyWindow(dialog);
		 EnableWindow(WindowFromDC(wglGetCurrentDC()), TRUE);

	#endif


#endif

#ifdef TARGET_ANDROID
     ofxAndroidAlertTextBox(question,text);
#endif

	return text;
}




///--------------------------------------------------------------------------
///--------------------------------------------------------------------------
///TEXTBOX FOR OF
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
    
    ~obj_ofT_(){
        wal = NULL;
        uiView = NULL;
        myTextView = NULL;
        myTextField=NULL;
    }
    ofNsWindow *wal;
    NSView * uiView;
    NSTextView *	myTextView;
    NSTextField*	myTextField;
    
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
        
    }
    
    
    wchar_t wstr2[16384]= L"EDIT\0";
    
    
    EnableWindow(WindowFromDC(wglGetCurrentDC()), FALSE);
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
                                    WS_CHILD | WS_VISIBLE | WS_TABSTOP ,
                                    x, y, w, h,WindowFromDC(wglGetCurrentDC()), (HMENU)101, GetModuleHandle(NULL), NULL);
        
        
    }
    
    SetFocus( hEdit );
    
    ShowWindow(hEdit, SW_SHOWNORMAL);
    if(showingScrolBar&&isMultiline)
        ShowScrollBar(hEdit,1,TRUE);
    
    bool bFirstEmpty = true;
    DestroyWindow(dialog);
    EnableWindow(WindowFromDC(wglGetCurrentDC()), TRUE);
    SetFocus( WindowFromDC(wglGetCurrentDC()));
#endif
    
    
#ifdef TARGET_OSX
    
    ofNsWindow *wal;
    NSView* uiView;
    NSTextView *	myTextView;
    NSTextField *	myTextField;
    
    quantity_ofBoxes++;
    NSDictionary *info = [[NSBundle mainBundle] infoDictionary];
    
    NSString *bundleName = [NSString stringWithFormat:@"%@", [info objectForKey:@"CFBundleExecutable"]];
    
    standardAppName = [bundleName UTF8String];
    
    NSRect Srect =    [[NSScreen mainScreen] frame];
    
    NSRect rect =    NSMakeRect(x,y,w,h);;
    
    
    NSArray * allWindows = [NSApp windows];
    
    
    
    
    uiView = [[[NSView alloc] initWithFrame:rect]autorelease];
    uiView.wantsLayer = YES;
    
    
    wal = [[[ofNsWindow alloc]initWithContentRect:rect
                                        styleMask:NSBorderlessWindowMask
                                          backing:NSBackingStoreBuffered
                                            defer:NO]autorelease];
    
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
        
    }else{
        myTextField = [[[NSTextField alloc] initWithFrame:rect]autorelease];
        
        [myTextField setStringValue:[NSString stringWithCString:text.c_str()
                                                       encoding:NSUTF8StringEncoding]];
        
        [myTextField setBezeled:YES];
        [myTextField setEditable:YES];
        [myTextField setEnabled:YES];
        [wal setContentView:myTextField];
        
    }
    
    
    
    [wal setLevel:NSNormalWindowLevel];
    [wal makeKeyAndOrderFront:wal];
    [wal orderFront:NSApp];
    
    
    
    
    for(NSWindow * aWindow in allWindows)
    {
        if([aWindow.miniwindowTitle isEqual: [NSString stringWithCString:standardAppName.c_str()]]){
            continue;
        }else{
            [wal setReleasedWhenClosed:NO];
            appWindow.size.height = [aWindow frame].size.height;
            appWindow.size.width =[aWindow frame].size.width;
            appWindow.origin.x = [aWindow frame].origin.x;
            appWindow.origin.y =[aWindow frame].origin.y;
            NSRect rectofT =  NSMakeRect(appWindow.origin.x+x,(appWindow.origin.y+appWindow.size.height-20)-(y+h),w,h);
            [wal setFrame:rectofT display:YES];
            [aWindow addChildWindow:wal ordered:NSWindowAbove];
            pointerToWindow = aWindow;
        }
    }
    
    
    
    if(isMultiline)
        pointer = new obj_ofT_(wal,uiView,myTextView);
    else
        pointer = new obj_ofT_(wal,uiView,myTextField);
    
    
    
    
    
    
#endif
    
}
ofTextField::ofTextField(){
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
        
        
        [aWindow removeChildWindow:pointer->wal];
        
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
    
    
    
    if([(NSString *)[activeApp objectForKey:@"NSApplicationName"]isEqual:[NSString stringWithCString:standardAppName.c_str()]]){
        
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
            SetWindowPos( hEdit, 0,  x, y, w, h, SWP_NOZORDER  );
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
        
        if (!isMultiline)
            text = [[pointer->myTextField stringValue] UTF8String];
        else
            text = [[pointer->myTextView string] UTF8String];
        
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



