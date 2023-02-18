#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d2d1.h>
#include <vector>

class GraphicsLayer;

class Game
{
public:
    Game(HWND windowHandle);
    ~Game();

    void CreateResources();
    void ReleaseResources();

    void Reset();

    void OnKeyDown(char keyPressed);
    void OnMouseMove(int x, int y);
    void OnResize(int x, int y);

    void Update(float deltaTime);
    void Draw();

private:
    bool m_ResourcesAreValid = false;
    GraphicsLayer* m_pGraphicsLayer = nullptr;
    
    ID2D1SolidColorBrush* m_pBrushRed = nullptr;
    ID2D1SolidColorBrush* m_pBrushGreen = nullptr;
    ID2D1SolidColorBrush* m_pBrushBlue = nullptr;

    IDWriteTextFormat* m_pTextFormatArial = nullptr;
    ID2D1Bitmap* m_pTestImage = nullptr;

    float m_PositionX = 500;
    float m_PositionY = 200;
    float m_DirX = 0.7071f;
    float m_DirY = 0.7071f;
};
