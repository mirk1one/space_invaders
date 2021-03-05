#include <windows.h>								// Header File For Windows
#include <stdio.h>									// Header File For Standard Input/Output
#include <gl\gl.h>									// Header File For The OpenGL32 Library
#include <gl\glu.h>									// Header File For The GLu32 Library

#include "Game.h"
#include "Spaceship.h"
#include "resource.h"								// risorse (ID dei tasti del menu ed icone)
#include "SoundSystem.h"

//  LIBRERIE OPENGL e multimendia
//	OpenGL libraries
#pragma comment( lib, "opengl32.lib" )				// Search For OpenGL32.lib While Linking
#pragma comment( lib, "glu32.lib" )					// Search For GLu32.lib While Linking
//#pragma comment( lib, "winmm.lib" )				// Search For WinMM Library While Linking

class GameModel Data;
class SpaceshipModel Spaceship;
class SoundSystemModel SoundSystem;

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Dichiarazione di WndProc

///////////////////////////////////////////////////////////
//  kill the window
GLvoid KillGLWindow(GLvoid)									// Properly Kill The Window
{
	if (Data.fullscreen)									// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL, 0);						// If So Switch Back To The Desktop
		ShowCursor(TRUE);									// Show Mouse Pointer
	}

	if (Data.hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL, NULL)) {					// Are We Able To Release The DC And RC Contexts?
			MessageBox(NULL, "Release Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}
		if (!wglDeleteContext(Data.hRC)) {					// Are We Able To Delete The RC?
			MessageBox(NULL, "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}
		Data.hRC = NULL;									// Set RC To NULL
	}

	if (Data.hDC && !ReleaseDC(Data.hWnd, Data.hDC)) {		// Are We Able To Release The DC
		MessageBox(NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		Data.hDC = NULL;									// Set DC To NULL
	}

	if (Data.hWnd && !DestroyWindow(Data.hWnd)) {			// Are We Able To Destroy The Window?
		MessageBox(NULL, "Could Not Release hWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		Data.hWnd = NULL;									// Set hWnd To NULL
	}

	if (!UnregisterClass("SpaceInvaders", Data.hInstance))	// Are We Able To Unregister Class
	{
		MessageBox(NULL, "Could Not Unregister Class.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		Data.hInstance = NULL;								// Set hInstance To NULL
	}
}
///////////////////////////////////////////////////////////
//  create the window
/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
 *	title			- Title To Appear At The Top Of The Window				*
 *	width			- Width Of The GL Window Or Fullscreen Mode				*
 *	height			- Height Of The GL Window Or Fullscreen Mode			*
 *	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
 *	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/

BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;									// Holds The Results After Searching For A Match
	WNDCLASS	wc;												// Windows Class Structure
	DWORD		dwExStyle;										// Window Extended Style
	DWORD		dwStyle;										// Window Style
	RECT		WindowRect;										// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left = (long)0;									// Set Left Value To 0
	WindowRect.right = (long)width;								// Set Right Value To Requested Width
	WindowRect.top = (long)0;									// Set Top Value To 0
	WindowRect.bottom = (long)height;							// Set Bottom Value To Requested Height

	Data.fullscreen = fullscreenflag;							// Set The Global Fullscreen Flag

	Data.hInstance = GetModuleHandle(NULL);						// Grab An Instance For Our Window
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;				// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc = (WNDPROC)WndProc;							// WndProc Handles Messages
	wc.cbClsExtra = 0;											// No Extra Window Data
	wc.cbWndExtra = 0;											// No Extra Window Data
	wc.hInstance = Data.hInstance;								// Set The Instance
	// Carica l'icona del gioco
	wc.hIcon = LoadIcon(Data.hInstance, MAKEINTRESOURCE(IDI_ICON_GAME));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);					// Load The Arrow Pointer
	wc.hbrBackground = NULL;									// No Background Required For GL
	wc.lpszMenuName = NULL;										// We Don't Want A Menu
	wc.lpszClassName = "SpaceInvaders";							// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL, "Failed To Register The Window Class.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;											// Return FALSE
	}

	if (Data.fullscreen)										// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth = width;					// Selected Screen Width
		dmScreenSettings.dmPelsHeight = height;					// Selected Screen Height
		dmScreenSettings.dmBitsPerPel = bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL, "The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?", "NeHe GL", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
			{
				Data.fullscreen = FALSE;						// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL, "Program Will Now Close.", "ERROR", MB_OK | MB_ICONSTOP);
				return FALSE;									// Return FALSE
			}
		}
	}

	if (Data.fullscreen)											// Are We Still In Fullscreen Mode?
	{
		dwExStyle = WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle = WS_POPUP;											// Windows Style
		ShowCursor(FALSE);											// Hide Mouse Pointer
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;				// Window Extended Style
		dwStyle = (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);	// Windows Style (NO RESIZE WINDOW!)
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(Data.hWnd = CreateWindowEx(dwExStyle,						// Extended Style For The Window
		"SpaceInvaders",											// Class Name
		title,														// Window Title
		dwStyle |													// Defined Window Style
		WS_CLIPSIBLINGS |											// Required Window Style
		WS_CLIPCHILDREN,											// Required Window Style
		0, 0,														// Window Position
		WindowRect.right - WindowRect.left,							// Calculate Window Width
		WindowRect.bottom - WindowRect.top,							// Calculate Window Height
		NULL,														// No Parent Window
		NULL,														// No Menu
		Data.hInstance,												// Instance
		NULL)))														// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();												// Reset The Display
		MessageBox(NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;												// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd =				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};

	if (!(Data.hDC = GetDC(Data.hWnd)))				// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Create A GL Device Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(PixelFormat = ChoosePixelFormat(Data.hDC, &pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!SetPixelFormat(Data.hDC, PixelFormat, &pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(Data.hRC = wglCreateContext(Data.hDC)))			// Are We Able To Get A Rendering Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!wglMakeCurrent(Data.hDC, Data.hRC))				// Try To Activate The Rendering Context
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	ShowWindow(Data.hWnd, SW_SHOW);					// Show The Window
	SetForegroundWindow(Data.hWnd);					// Slightly Higher Priority
	SetFocus(Data.hWnd);							// Sets Keyboard Focus To The Window
	Data.ReSizeGLScene(width, height);				// Set Up Our Perspective GL Screen

	if (!Data.InitGL())								// Initialize Our Newly Created GL Window
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Initialization Failed.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	return TRUE;									// Success
}


///////////////////////////////////////////////////////////
// callback function
LRESULT CALLBACK WndProc(HWND	hWnd,					// Handle For This Window
	UINT	uMsg,										// Message For This Window
	WPARAM	wParam,										// Additional Message Information
	LPARAM	lParam)										// Additional Message Information
{
	switch (uMsg)										// Check For Windows Messages
	{
		case WM_ACTIVATE:								// Watch For Window Activate Message
		{
			if (!HIWORD(wParam))						// Check Minimization State
			{
				Data.active = TRUE;
			}											// Program Is Active
			else { Data.active = FALSE; }				// Program Is No Longer Active
			return 0;								    // Return To The Message Loop
		}

		case WM_MOUSEMOVE:

		case WM_SYSCOMMAND:								// Intercept System Commands
		{
			switch (wParam)								// Check System Calls
			{
			case SC_SCREENSAVE:							// Screensaver Trying To Start?
			case SC_MONITORPOWER:						// Monitor Trying To Enter Powersave?
				return 0;								// Prevent From Happening
			}
			break;										// Exit
		}

		case WM_CLOSE:									// Did We Receive A Close Message?
		{	
			PostQuitMessage(0);							// Send A Quit Message
			return 0;									// Jump Back
		}

		case WM_KEYDOWN:								// Is A Key Being Held Down?
		{	
			Data.keys[wParam] = TRUE;					// If So, Mark It As TRUE
			return 0;									// Jump Back
		}

		case WM_KEYUP:									// Has A Key Been Released?
		{	
			Data.keys[wParam] = FALSE;					// If So, Mark It As FALSE
			return 0;									// Jump Back
		}

		case WM_SIZE:									// Resize The OpenGL Window
		{	
			Data.ReSizeGLScene(LOWORD(lParam), HIWORD(lParam));  // LoWord=Width, HiWord=Height
			return 0;									// Jump Back
		}
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

///////////////////////////////////////////////////////////
//  Main program
int WINAPI WinMain(HINSTANCE	hInstance,			// Instance
	HINSTANCE	hPrevInstance,						// Previous Instance
	LPSTR		lpCmdLine,							// Command Line Parameters
	int			nCmdShow)							// Window Show State
{
	MSG		msg;									// Windows Message Structure
	BOOL	done = FALSE;							// Bool Variable To Exit Loop

	// Ask The User Which Screen Mode They Prefer
	// Init of the default button depending on Data.fullscreen
	unsigned long Def = MB_DEFBUTTON1;
	if (!Data.fullscreen) Def = MB_DEFBUTTON2;
	/*
	  if (MessageBox(NULL,"Would You Like To Run In Fullscreen Mode?",
		"Start FullScreen?",MB_YESNO|MB_ICONQUESTION | Def) == IDNO)
		{
			Data.fullscreen=FALSE;					// Windowed Mode
		}
	*/
	Data.fullscreen = false;						// removed the boring request...

	  // Create Our OpenGL Window
	if (!CreateGLWindow("Space Invaders", 640, 480, 16, Data.fullscreen))
	{
		return 0;									// Quit If Window Was Not Created
	}

	// Inizializzo i suoni
	if (!SoundSystem.InitilizeSounds())				// Qua inizializzo i suoni, se qualcosa va storto termino
		return 0;

	// Eseguo il suono di gioco
	SoundSystem.PlayGameSound();					// Eseguo il suono di background

	while (!done)									// Loop That Runs While done=FALSE
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message == WM_QUIT)				// Have We Received A Quit Message?
			{
				done = TRUE;							// If So done=TRUE
			}
			else									// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		}
		else										// If There Are No Messages
		{
			// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
			if ((Data.active && !Data.DrawGLScene()) || Data.keys[VK_ESCAPE])	// Active?  Was There A Quit Received?
			{
				done = TRUE;						// ESC or DrawGLScene Signalled A Quit
			}
			else									// Not Time To Quit, Update Screen
			{
				SwapBuffers(Data.hDC);				// Swap Buffers (Double Buffering)
			}

			//  Removed the F1 key: no fullscreen!
			/*
			if (Data.keys[VK_F1])					// Is F1 Being Pressed?
			{
				Data.keys[VK_F1]=FALSE;				// If So Make Key FALSE
				KillGLWindow();						// Kill Our Current Window
				Data.fullscreen=!Data.fullscreen;	// Toggle Fullscreen / Windowed Mode
				// Recreate Our OpenGL Window
				if (!CreateGLWindow("Basic 2D game skeleton",640,480,16,Data.fullscreen))
				{
					return 0;						// Quit If Window Was Not Created
				}
			}*/

			// Barra SPACE premuta e mi trovo nel livello di gioco -> sparo con la navicella
			if (Data.keys[VK_SPACE] && Data.sceneType == SceneType::GameLevel)
			{
				// Key diventa FALSE
				Data.keys[VK_SPACE] = FALSE;
				// Creo lo sparo per la navicella e lo salvo
				Data.shots.push_back(Spaceship.MakeShot());
				// Eseguo il suono dello sparo
				SoundSystem.PlayFire();
			}

			// Freccia dx premuta -> muovo navicella a dx
			if (Data.keys[VK_RIGHT])
			{
				Data.keys[VK_SPACE] = FALSE;
				if (Spaceship.px <= FLOAT_DX_BORDER)
					Spaceship.px += FLOAT_SPACESHIP_MOVE;
			}
			
			// Freccia sx premuta -> muovo navicella a sx
			if (Data.keys[VK_LEFT])
			{
				Data.keys[VK_LEFT];
				if (Spaceship.px >= FLOAT_SX_BORDER)
					Spaceship.px -= FLOAT_SPACESHIP_MOVE;
			}

			// Invio premuto -> passo a schermata gioco
			// Schermata menù iniz. -> passo a lv 1 gioco
			// Schermata fine livello -> se ho perso, posso rigiocare livello 1
			//						  -> se ho vinto, passo a livello successivo
			//						  -> se ho finito il gioco, posso ricominciarlo
			if (Data.keys[VK_RETURN])
			{
				// Se mi trovo nel menù iniziale -> passo a schermata livello
				if (Data.sceneType == SceneType::OpenMenu)
				{
					// Key diventa FALSE
					Data.keys[VK_RETURN] = FALSE;
					// Passo alla finestra di gioco
					Data.sceneType = SceneType::GameLevel;
				}
				// Altrimenti mi trovo in schermata di fine livello -> posso rigiocare
				else if(Data.sceneType == SceneType::EndLevel)
				{
					// Key diventa FALSE
					Data.keys[VK_RETURN] = FALSE;
					// Passo alla finestra di gioco
					Data.sceneType = SceneType::GameLevel;
					// Se ho perso oppure ho vinto l'ultimo livello -> rigioco il livello 1
					if (!Data.winLevel || Data.level == N_LEVELS)
						Data.SetLevelConfig(N_LIFES, 1, 0, 3, START_RANDOM_TIMES);
					// Altrimenti -> passo a livello successivo e gioco
					// Il livello successivo avrà:
					// - stesso numero di vite
					// - mi porto dietro il punteggio totale fatto dai livelli precedenti
					// - gli alieni aumenteranno di 2 per ogni riga (+1 a sx e +1 a dx)
					// - gli alieni aumenteranno la frequenza di lancio degli spari
					else
					{
						++Data.level;
						Data.SetLevelConfig(Data.lifes, Data.level, Data.scores, Data.nAliensPerRow + 2, START_RANDOM_TIMES - (10000 * Data.level));
					}
				}
			}
		}
	}

	// Shutdown
	KillGLWindow();									// Kill The Window
	return (msg.wParam);							// Exit The Program
}
