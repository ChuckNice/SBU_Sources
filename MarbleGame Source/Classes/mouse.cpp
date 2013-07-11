#ifdef __APPLE__
#ifndef MARBLE_IOS_BUILD
#include <SDL/SDL.h>
#endif
#endif

#include "mouse.h"
#include "global.h"
#include <list>

// singleton getter for mouse
Mouse* s_Mouse()
{
	static Mouse s_mouse;
	return &s_mouse;
}

Mouse::Mouse() :
m_mouseLocked(false)
{
	// clear buttons
	for (int i = 0; i < 2; ++i) {
		m_buttonState[i] = false;
		m_lastButtonState[i] = false;
	}
}

#ifdef MARBLE_IOS_BUILD
float ios_x = 0.0f;
float ios_y = 0.0f;
#endif

void Mouse::getMousePos(int& x, int& y)
{
#ifdef _WIN32
	POINT point;
	GetCursorPos(&point);
	x = point.x;
	y = point.y;
#elif defined __APPLE__
    #ifndef MARBLE_IOS_BUILD
        SDL_GetMouseState(&x, &y);
    #else
        x = ios_x;
        y = ios_y;
    #endif
#endif
}

void Mouse::getRelativeMousePos(int& x, int& y)
{
	x = (int)m_relativeMousePosX;
	y = (int)m_relativeMousePosY;
}


void Mouse::setMousePos(int x, int y)
{
#ifdef _WIN32
	SetCursorPos(x, y);
#elif defined __APPLE__
    #ifndef MARBLE_IOS_BUILD
        SDL_WarpMouse(x, y);
    #else
        ios_x = x;
        ios_y = y;
    #endif
#endif
}

void Mouse::showCursor(bool val)
{
#ifdef _WIN32
	ShowCursor(val);
#elif defined __APPLE__
    #ifndef MARBLE_IOS_BUILD
        if (val)
            SDL_ShowCursor(SDL_ENABLE);
        else
            SDL_ShowCursor(SDL_DISABLE);
    #endif
#endif
}

bool Mouse::isButtonPressed(int button)
{
	if (button < 2)
		return ( !m_lastButtonState[button] && m_buttonState[button] );
	return false;
}

bool Mouse::isButtonHeld(int button)
{
	if (button < 2)
		return ( m_lastButtonState[button] && m_buttonState[button] );
	return false;
}

// limit FPS or mouse moves to quick (also don't update on frist frame)
const float timestep_threshold = 1.0f / 60.0f;
float elapsed_time = 0.0f;
bool firstFrame = true;

void Mouse::update(float dt)
{
	memcpy(m_lastButtonState, m_buttonState, sizeof(bool) * 2);
#ifdef _WIN32
	m_buttonState[0] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) ? true : false;
	m_buttonState[1] = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) ? true : false;
#elif defined __APPLE__
    #ifndef MARBLE_IOS_BUILD
        Uint8 mouseState = SDL_GetMouseState(NULL, NULL);
        m_buttonState[0] = (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) ? true : false;
        m_buttonState[1] = (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) ? true : false;
    #endif
#endif

	/*
	This next section of code allows for really smooth mouse movement no matter
	what the frame rate (well... within reason). The last 10 positions
	are stored in a list. Each frame a new relative position is added and the oldest is removed.
	These positions are then added together using a weighting value. The most recent position
	has the most weight, whereas the oldest as the lowest weighting. This value is then averaged
	to produce the new relative mouse position.
	*/
	elapsed_time += dt;
	if (elapsed_time > timestep_threshold)
	{
		static std::list<std::pair<float, float> > mousePositionHistory;

        if (m_mouseLocked) {
            int x, y;
            getMousePos(x, y);
            GLint viewport[4];
            glGetIntegerv(GL_VIEWPORT, viewport);
            
            if ( !firstFrame ) {
                //Add the current mouse position - starting position
                mousePositionHistory.push_front(std::make_pair((float)x - (viewport[2] / 2), (float)y - (viewport[3] / 2)));
                if (mousePositionHistory.size() > 10)
                {
                    //Make sure only the last 10 positions are stored
                    mousePositionHistory.pop_back();
                }
                
                m_relativeMousePosX = 0.0f;
                m_relativeMousePosY = 0.0f;
                float weight = 1.0f;
                
                //Calculate a weighted average
                for (std::list<std::pair<float, float> >::iterator i = mousePositionHistory.begin();
                     i != mousePositionHistory.end(); ++i)
                {
                    m_relativeMousePosX += (*i).first * weight;
                    m_relativeMousePosY += (*i).second * weight;
                    weight *= 0.5f;
                }
                m_relativeMousePosX /= 10.0f;
                m_relativeMousePosY /= 10.0f;
            }
            firstFrame = false;
            
            //Put the mouse in the middle of the screen
            setMousePos(viewport[2] / 2, viewport[3] / 2);
            showCursor(false);
        }

		elapsed_time = 0.0f;
	}
}