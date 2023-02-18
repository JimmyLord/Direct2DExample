#pragma once

#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>

template<class Interface> inline void SafeRelease(Interface*& pInterfaceToRelease)
{
    if( pInterfaceToRelease != nullptr )
    {
        pInterfaceToRelease->Release();
        pInterfaceToRelease = nullptr;
    }
}

class GraphicsLayer
{
public:
    GraphicsLayer(HWND windowHandle);
    ~GraphicsLayer();

    bool OnResize(int x, int y); // Will return true if resources need to be rebuilt.

    ID2D1HwndRenderTarget* GetRenderTarget() { return m_pRenderTarget; }

    IDWriteTextFormat* LoadFont(const wchar_t* name, float size);
    ID2D1Bitmap* LoadImageFromDisk(const wchar_t* filename);

    bool BeginRender();
    bool EndRender(); // Will return true if resources need to be rebuilt.

protected:
    void CreateDeviceIndependentResources();
    void CreateDeviceResources();
    void DiscardDeviceResources();

protected:
    HWND m_WindowHandle = 0;
    
    ID2D1Factory* m_pD2DFactory = nullptr; // Needed for creating render targets.
    IWICImagingFactory* m_pWICFactory = nullptr; // Needed for loading bitmaps from files.
    IDWriteFactory* m_pDWriteFactory = nullptr; // Needed for creating text formats.
    
    ID2D1HwndRenderTarget* m_pRenderTarget = nullptr; // Needed for drawing.
};
