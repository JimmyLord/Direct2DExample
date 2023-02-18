#include <assert.h>
#include "Game.h"
#include "GraphicsLayer.h"
#include "Utility.h"

Game::Game(HWND windowHandle)
{
    m_pGraphicsLayer = new GraphicsLayer( windowHandle );
    
    srand( (int)GetHighPrecisionTime() );

    CreateResources();
    
    m_pTextFormatArial = m_pGraphicsLayer->LoadFont( L"Arial", 50.0f );
    m_pTestImage = m_pGraphicsLayer->LoadImageFromDisk( L"images/player_01.png" );
    
    Reset();
}

Game::~Game()
{
    ReleaseResources();
    
    m_pTextFormatArial->Release();
    m_pTestImage->Release();

    delete m_pGraphicsLayer;
}

void Game::CreateResources()
{
    assert( m_ResourcesAreValid == false );
    m_ResourcesAreValid = true;

    m_pGraphicsLayer->GetRenderTarget()->CreateSolidColorBrush( D2D1::ColorF( 255/255.0f,  40/255.0f,  40/255.0f ), &m_pBrushRed );
    m_pGraphicsLayer->GetRenderTarget()->CreateSolidColorBrush( D2D1::ColorF(  40/255.0f, 255/255.0f,  40/255.0f ), &m_pBrushGreen );
    m_pGraphicsLayer->GetRenderTarget()->CreateSolidColorBrush( D2D1::ColorF(  40/255.0f,  40/255.0f, 255/255.0f ), &m_pBrushBlue );
}

void Game::ReleaseResources()
{
    assert( m_ResourcesAreValid == true );
    m_ResourcesAreValid = false;

    SafeRelease( m_pBrushRed );
    SafeRelease( m_pBrushGreen );
    SafeRelease( m_pBrushBlue );
}

void Game::Reset()
{
    m_PositionX = 500;
    m_PositionY = 200;
    m_DirX = 0.7071f;
    m_DirY = 0.7071f;
}

void Game::OnKeyDown(char keyPressed)
{
    if( keyPressed == 'R' )
    {
        Reset();
    }
}

void Game::OnMouseMove(int x, int y)
{
}

void Game::OnResize(int x, int y)
{
    if( m_pGraphicsLayer->OnResize( x, y ) )
    {
        // The graphics layer was rebuilt, so we need to recreate our resources.
        // Not tested. Not sure how to invalidate the render target.
        ReleaseResources();
        CreateResources();
    }
}

void Game::Update(float deltaTime)
{
    float speed = 300;
    m_PositionX += m_DirX * speed * deltaTime;
    m_PositionY += m_DirY * speed * deltaTime;
    
    if( m_PositionX < 0 || m_PositionX > 1280-128 )
    {
        m_DirX *= -1;
    }
    if( m_PositionY < 0 || m_PositionY > 720-128 )
    {
        m_DirY *= -1;
    }
}

void Game::Draw()
{
    // Start rendering, this may fail if the window is minimized (or otherwise). So only draw if needed.
    if( m_pGraphicsLayer->BeginRender() )
    {
        // Clear the screen.
        m_pGraphicsLayer->GetRenderTarget()->Clear( D2D1::ColorF(0x000030) );

        // Draw an ellipse.
        D2D1_ELLIPSE ellipse = D2D1::Ellipse( D2D1::Point2F(200, 300), 100, 50 );
        m_pGraphicsLayer->GetRenderTarget()->FillEllipse( ellipse, m_pBrushBlue );

        // Draw a rounded rectangle.
        D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect( D2D1::RectF(1000, 400, 600, 600), 20, 20 );
        m_pGraphicsLayer->GetRenderTarget()->FillRoundedRectangle( roundedRect, m_pBrushRed );

        // Draw the sprite.
        m_pGraphicsLayer->GetRenderTarget()->DrawBitmap( m_pTestImage, D2D1::RectF(m_PositionX, m_PositionY, m_PositionX+128, m_PositionY+128) );

        // Draw some text.
        D2D1_RECT_F textRect = D2D1::RectF(600, 100, 800, 300);
        wchar_t text[] = L"Hello world!";
        m_pGraphicsLayer->GetRenderTarget()->DrawText( text, (UINT32)wcslen(text), m_pTextFormatArial, textRect, m_pBrushGreen );

        // Finalize the render.
        if( m_pGraphicsLayer->EndRender() )
        {
            // The graphics layer was rebuilt, so we need to recreate our resources.
            // Not tested. Not sure how to invalidate the render target.
            ReleaseResources();
            CreateResources();
        }
    }
}
