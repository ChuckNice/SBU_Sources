#ifndef _GLWINDOW_H
#define _GLWINDOW_H

#include <windows.h>
#include <ctime>

class GLWindow
{
public:
    GLWindow(HINSTANCE hInstance); //default constructor
    virtual ~GLWindow();
	
    bool create(int width, int height, int bpp, bool fullscreen);
    void destroy();

	bool isRunning();
    void processEvents(float dt);

    void swapBuffers() { SwapBuffers(m_hdc); }

    float getElapsedSeconds();

private:
    bool m_isRunning; //Is the window still running?
    bool m_isFullscreen;
	float m_lastTime;

    HWND m_hwnd; //Window handle
    HGLRC m_hglrc; //Rendering context
    HDC m_hdc; //Device context
    RECT m_windowRect; //Window bounds
    HINSTANCE m_hinstance; //Application instance
    WNDCLASSEX m_windowClass;

    void setupPixelFormat(void);

private:
	static LRESULT CALLBACK StaticWndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif
