#include "Help.h"

BinaryHelp::BinaryHelp(SDL_Surface* pScreen)
: m_pScreen(pScreen)
{
	m_pFont = nSDL_LoadFont(NSDL_FONT_VGA, 0, 0, 0);
}

BinaryHelp::~BinaryHelp()
{
	nSDL_FreeFont(m_pFont);
}

bool BinaryHelp::Loop()
{
	//Handle keypresses
	if( PollEvents() == false )
		return false;
	
	UpdateDisplay();
	
	return true;
}

bool BinaryHelp::PollEvents()
{
	SDL_Event event;
	
	/* Poll for events. SDL_PollEvent() returns 0 when there are no  */
	/* more events on the event queue, our while loop will exit when */
	/* that occurs.                                                  */
	while( SDL_PollEvent( &event ) )
	{
		/* We are only worried about SDL_KEYDOWN and SDL_KEYUP events */
		switch( event.type )
		{
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) 
				{
					case SDLK_ESCAPE:
					case SDLK_RETURN:
					case SDLK_SPACE:
						return false;
					break;
					
					default:
						break;
				}

				break;
			
			//Called when the mouse moves
			case SDL_MOUSEMOTION:
				break;
			
			case SDL_KEYUP:
				break;
			
			default:
				break;
		}
	}
	return true;
}

void BinaryHelp::UpdateDisplay()
{
	SDL_FillRect(m_pScreen, NULL, SDL_MapRGB(m_pScreen->format, 153, 153, 255));

	nSDL_DrawString(m_pScreen, m_pFont, 15, 20, 
"Binary puzzle is a puzzle game.\n\
Here are the rules:\n\
\n\
1. Each cell must contain either\n\
a zero or a one.\n\
\n\
2. There cannot be three or more\n\
consecutive ones or consecutive zeroes\n\
in any row or column\n\
\n\
3. Each row and each column must\n\
contain an equal number of zeroes\n\
and ones.\n\
\n\
4. Each row is unique and each column\n\
is unique\n\
\n\
Also have fun!");		
	
	SDL_UpdateRect(m_pScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}




