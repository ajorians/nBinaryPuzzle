#include "Options.h"
#include "Defines.h"

Options::Options(SDL_Surface* pScreen, Config* pConfig)
: m_pScreen(pScreen), m_nOptionsIndex(0), m_pConfig(pConfig)
{
	m_pFont = nSDL_LoadFont(NSDL_FONT_TINYTYPE, 0, 0, 0);
	nSDL_SetFontSpacing(m_pFont, 0, 2);
}

Options::~Options()
{
	nSDL_FreeFont(m_pFont);
}

bool Options::Loop()
{
	//Handle keypresses
	if( PollEvents() == false )
		return false;
	
	UpdateDisplay();
	
	return true;
}

bool Options::PollEvents()
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
						return false;
					break;

					case SDLK_SPACE:
                                        case SDLK_RETURN:
                                        case SDLK_LCTRL:
						ToggleCurrentOption();
                                                break;

					case SDLK_UP:
                                        case SDLK_8:
                                                Move(OP_Up);
                                                break;

                                        case SDLK_DOWN:
                                        case SDLK_2:
                                                Move(OP_Down);
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

void Options::ToggleCurrentOption()
{
	if( m_nOptionsIndex == 0 ) {
		m_pConfig->SetSelectorMovementAnimate(!m_pConfig->GetSelectorMovementAnimate());
	} else if( m_nOptionsIndex == 1 ) {
                m_pConfig->SetBackgroundMoves(!m_pConfig->GetBackgroundMoves());
        }
}

void Options::Move(Option_Direction eDirection)
{
	if( eDirection == OP_Down && m_nOptionsIndex < 1 ) {
		m_nOptionsIndex++;
	} else if( eDirection == OP_Up && m_nOptionsIndex > 0 ) {
		m_nOptionsIndex--;
	}
}

#define ANIMATE_SELECTOR_Y	(65)
#define BACKGROUND_MOVES_Y	(105)
void Options::UpdateDisplay()
{
	//Draw background
	SDL_FillRect(m_pScreen, NULL, SDL_MapRGB(m_pScreen->format, 153, 153, 255));
	nSDL_DrawString(m_pScreen, m_pFont, (SCREEN_WIDTH-nSDL_GetStringWidth(m_pFont, "Options:"))/2, 15, "Options:");

	char buffer[256];
	strcpy(buffer, "Animate Selector Movement: ");

	if( m_pConfig->GetSelectorMovementAnimate() ) {
		strcat(buffer, "ON");
	} else {
		strcat(buffer, "OFF");
	}

	strcat(buffer,
"\n\
Whether the selector slides to\n\
the new spot or just moves there");
	nSDL_DrawString(m_pScreen, m_pFont, 12, ANIMATE_SELECTOR_Y, buffer);

	if( m_nOptionsIndex == 0 ) {
		if( is_classic ) {
	                draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, 255, 255, 255), 7, ANIMATE_SELECTOR_Y-5, 312, 45, 1);
	        } else {
	                draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, 255, 0, 0), 7, ANIMATE_SELECTOR_Y-5, 312, 45, 1);
	        }
	}

	strcpy(buffer, "Moving Background: ");

        if( m_pConfig->GetBackgroundMoves() ) {
                strcat(buffer, "ON");
        } else {
                strcat(buffer, "OFF");
        }

        strcat(buffer,
"\n\
CX Version only.  Specifies whether\n\
the background slides or not");
        nSDL_DrawString(m_pScreen, m_pFont, 12, BACKGROUND_MOVES_Y, buffer);

        if( m_nOptionsIndex == 1 ) {
                if( is_classic ) {
                        draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, 255, 255, 255), 7, BACKGROUND_MOVES_Y-5, 312, 45, 1);
                } else {
                        draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, 255, 0, 0), 7, BACKGROUND_MOVES_Y-5, 312, 45, 1);
                }
        }

	SDL_UpdateRect(m_pScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

}




