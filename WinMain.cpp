#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Windowsx.h>
#include <string>
#include "Utility.h"
#include "Game.h"

const int c_WindowWidth = 1280;
const int c_WindowHeight = 720;

Game* g_pGame = nullptr;

void Paint(HWND hWnd)
{
    g_pGame->Draw();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch( msg )
    {
    case WM_CREATE:
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

    case WM_KEYDOWN:
        {
            char keyPressed = (char)wParam;

            if( keyPressed == VK_ESCAPE )
            {
                PostQuitMessage( 0 );
                break;
            }

            g_pGame->OnKeyDown( keyPressed );
        }
        break;

    case WM_MOUSEMOVE:
        {
            int xPos = GET_X_LPARAM( lParam ); 
            int yPos = GET_Y_LPARAM( lParam );

            g_pGame->OnMouseMove( xPos, yPos );
        }
        break;

    case WM_SIZE:
        {
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);

            if( g_pGame )
            {
                g_pGame->OnResize( width, height );
            }
        }
        break;

    default:
        return DefWindowProc( hWnd, msg, wParam, lParam );
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int showCmd)
{
    CoInitialize( NULL ); // Needed?
    
    // Set up the properties of our window class.
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof( WNDCLASSEX );        // Set the size of the structure.
    wc.style = CS_HREDRAW | CS_VREDRAW;      // Redraw on move.
    wc.lpfnWndProc = WndProc;                // WndProc handles messages.
    wc.cbClsExtra = 0;                       // No extra window data.
    wc.cbWndExtra = 0;                       // No extra window data.
    wc.hInstance = hInstance;                // Set the instance.
    wc.hIcon = LoadIcon( 0, IDI_WINLOGO );   // Load the default icon.
    wc.hCursor = LoadCursor( 0, IDC_ARROW ); // Load the arrow pointer.
    wc.hbrBackground = 0;                    // No background required.
    wc.lpszMenuName = nullptr;               // We don't want a menu.
    wc.lpszClassName = "MyWindowClass";      // Set the class name.

    // Register our window class.
    if( RegisterClassEx( &wc ) == false )
    {
        MessageBox( 0, "Window class registration failed", "Application Error Message", 0 );
        return false;
    }

    // Calculate the window size needed to support our client area.
    DWORD dwStyle = WS_OVERLAPPEDWINDOW;
    DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
    RECT windowRect = { 0, 0, c_WindowWidth, c_WindowHeight };
    AdjustWindowRectEx( &windowRect, dwStyle, false, dwExStyle );

    // Create a window using the class defined above.
    HWND hWnd = CreateWindowEx( dwExStyle,     // Extended style for the window.
        "MyWindowClass",                       // Class name.
        "Window Title",                        // Window title.
        dwStyle,                               // Selected window styles.
        0, 0,                                  // Window position.
        windowRect.right - windowRect.left,    // Calculate adjusted window width.
        windowRect.bottom - windowRect.top,    // Calculate adjusted window height.
        nullptr,                               // No parent window.
        nullptr,                               // No menu.
        hInstance,                             // Instance.
        nullptr );                             // Pass nothing to WM_NCCREATE.

    // If the window creation failed for any reason, show a message.
    if( hWnd == nullptr )
    {
        MessageBoxA( 0, "CreateWindow failed", "Application Error Message", 0 );
        return false;
    }

    // Display the window.
    ShowWindow( hWnd, showCmd );
    UpdateWindow( hWnd );

    // Get the window device context.
    HDC hdc = GetDC( hWnd );

    g_pGame = new Game( hWnd );

    // Main windows message loop. Keep looping until a quit message comes in.
    MSG message;
    double previousTime = GetHighPrecisionTime();

    bool done = false;
    while( !done )
    {
        if( PeekMessage( &message, nullptr, 0, 0, PM_REMOVE ) )
        {
            if( message.message == WM_QUIT )
            {
                done = true;
            }
            else
            {
                // Send the message to WndProc.
                TranslateMessage( &message );
                DispatchMessage( &message );
            }
        }
        else
        {
            double currentTime = GetHighPrecisionTime();
            double deltaTime = currentTime - previousTime;
            previousTime = currentTime;

            g_pGame->Update( (float)deltaTime );

            Paint( hWnd );
        }
    }

    delete g_pGame;

    // Release the window device context.
    ReleaseDC( hWnd, hdc );

    // Truncate wParam in 64-bit mode to an int.
    return static_cast<int>( message.wParam );
}
