#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Windowsx.h>
#include <string>
#include <assert.h>
#include "Utility.h"
#include "Game.h"
#include "GraphicsLayer.h"

#pragma comment( lib, "d2d1.lib" )
#pragma comment( lib, "dwrite.lib" )
#pragma comment( lib, "windowscodecs.lib" )

// Most of this code is taken from the Direct2D tutorial.
// https://learn.microsoft.com/en-us/windows/win32/direct2d/direct2d-quickstart
// https://github.com/microsoft/Windows-classic-samples/tree/main/Samples/Win7Samples/multimedia/Direct2D/SimpleDirect2DApplication

GraphicsLayer::GraphicsLayer(HWND windowHandle)
    : m_WindowHandle( windowHandle )
{
    CreateDeviceIndependentResources();
    CreateDeviceResources();
}

GraphicsLayer::~GraphicsLayer()
{
    DiscardDeviceResources();
    m_pD2DFactory->Release();
    m_pDWriteFactory->Release();
}

void GraphicsLayer::CreateDeviceIndependentResources()
{
    HRESULT hr;

    // Create a Direct2D factory.
    hr = D2D1CreateFactory( D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory );
    assert( SUCCEEDED(hr) );

    // Create WIC factory, we need this to load images from disk.
    hr = CoCreateInstance( CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, reinterpret_cast<void**>(&m_pWICFactory) );
    assert( SUCCEEDED(hr) );

    // Create a DirectWrite factory, we need this for font creation.
    hr = DWriteCreateFactory( DWRITE_FACTORY_TYPE_SHARED, __uuidof(m_pDWriteFactory), reinterpret_cast<IUnknown**>(&m_pDWriteFactory) );
    assert( SUCCEEDED(hr) );
}

void GraphicsLayer::CreateDeviceResources()
{
    if( m_pRenderTarget != nullptr )
        return;

    RECT rc;
    GetClientRect( m_WindowHandle, &rc );
    D2D1_SIZE_U size = D2D1::SizeU( rc.right - rc.left, rc.bottom - rc.top );

    // Create a Direct2D render target.
    D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties = D2D1::RenderTargetProperties();
    D2D1_HWND_RENDER_TARGET_PROPERTIES hwndRenderTargetProperties = D2D1::HwndRenderTargetProperties( m_WindowHandle, size );
    HRESULT hr = m_pD2DFactory->CreateHwndRenderTarget( renderTargetProperties, hwndRenderTargetProperties, &m_pRenderTarget );
    assert( SUCCEEDED(hr) );
}

void GraphicsLayer::DiscardDeviceResources()
{
    SafeRelease( m_pRenderTarget );
}

bool GraphicsLayer::OnResize(int x, int y)
{
    if( m_pRenderTarget != nullptr )
    {
        HRESULT hr = m_pRenderTarget->Resize( D2D1::SizeU(x,y) );

        if( hr == D2DERR_RECREATE_TARGET )
        {
            // Not tested. Not sure how to invalidate the render target.
            DiscardDeviceResources();
            CreateDeviceResources();
            return true; // Tell the caller to rebuild resources.
        }
    }

    return false;
}

IDWriteTextFormat* GraphicsLayer::LoadFont(const wchar_t* fontName, float fontSize)
{
    HRESULT hr;

    IDWriteTextFormat* pTextFormat = nullptr;

    // Create a DirectWrite text format object.
    hr = m_pDWriteFactory->CreateTextFormat( fontName, nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
                                             fontSize, L"", &pTextFormat );
    assert( SUCCEEDED(hr) );

    // Center the text horizontally and vertically.
    //pTextFormat->SetTextAlignment( DWRITE_TEXT_ALIGNMENT_CENTER );
    //pTextFormat->SetParagraphAlignment( DWRITE_PARAGRAPH_ALIGNMENT_CENTER );

    return pTextFormat;
}

ID2D1Bitmap* GraphicsLayer::LoadImageFromDisk(const wchar_t* filename)
{
    HRESULT hr = S_OK;

    // Load the file from disk and decode the format.
    IWICBitmapDecoder* pDecoder = nullptr;
    hr = m_pWICFactory->CreateDecoderFromFilename( filename, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pDecoder );
    assert( SUCCEEDED(hr) );

    // Create the initial frame.
    IWICBitmapFrameDecode* pSource = nullptr;
    hr = pDecoder->GetFrame( 0, &pSource );
    assert( SUCCEEDED(hr) );

    // Setup to convert the image format to 32bppPBGRA (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
    IWICFormatConverter* pConverter = nullptr;
    hr = m_pWICFactory->CreateFormatConverter( &pConverter );
    assert( SUCCEEDED(hr) );

    hr = pConverter->Initialize( pSource, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeMedianCut );
    assert( SUCCEEDED(hr) );

    // Create a Direct2D bitmap from the WIC bitmap.
    ID2D1Bitmap* pBitmap;
    hr = m_pRenderTarget->CreateBitmapFromWicBitmap( pConverter, nullptr, &pBitmap );

    SafeRelease( pDecoder );
    SafeRelease( pSource );
    SafeRelease( pConverter );

    return pBitmap;
}

bool GraphicsLayer::BeginRender()
{
    // Rebuild the RenderTarget if the window has been resized.
    assert( m_pRenderTarget != nullptr );

    if( !(m_pRenderTarget->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED) )
    {
        m_pRenderTarget->BeginDraw();
        return true;
    }

    return false;
} 

bool GraphicsLayer::EndRender()
{
    HRESULT hr = m_pRenderTarget->EndDraw();

    if( hr == D2DERR_RECREATE_TARGET )
    {
        // Not tested. Not sure how to invalidate the render target.
        DiscardDeviceResources();
        CreateDeviceResources();
        return true; // Tell the caller to rebuild resources.
    }

    return false;
}
