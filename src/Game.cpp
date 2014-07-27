#include "Game.h"
#ifdef USE_GRAPHIC_YOU_WIN
#include "YouWinGraphic.h"
#endif

extern "C"
{
#include "SDL/SDL_gfxPrimitives.h"
}

Game::Game(SDL_Surface* pScreen, BinaryLib binary, int nLevelNumber, Config* pConfig)
: m_pScreen(pScreen), m_Background(pScreen, pConfig)
#ifdef USE_GRAPHIC_YOU_WIN
, m_pWinGraphic(NULL)
#else
, m_YouWinMessage(pScreen)
#endif
, m_Drawer(pScreen, binary, m_BoardMetrics), m_Binary(binary), m_nLevelNumber(nLevelNumber), m_pConfig(pConfig), m_Selector(pScreen, pConfig, &m_BoardMetrics), m_bGameOver(false), m_BoardMetrics(SCREEN_WIDTH, SCREEN_HEIGHT)
{
	int nWidth = GetBinaryBoardWidth(m_Binary), nHeight = GetBinaryBoardHeight(m_Binary);

	m_BoardMetrics.SetDimensions(nWidth, nHeight);

	m_Timer.Start();
}

Game::~Game()
{
#ifdef USE_GRAPHIC_YOU_WIN
	if( m_pWinGraphic != NULL )
		SDL_FreeSurface(m_pWinGraphic);
#endif
}

bool Game::Loop()
{
	//Handle keypresses
	if( PollEvents() == false )
		return false;

	//Check for state changes
	if( CheckStateConditions() == false )
		return false;

	//Update screen
	UpdateDisplay();
	
	SDL_Delay(30);
	
	return true;
}

bool Game::CheckStateConditions()
{
   if( m_YouWinMessage.HasMessage() && ((!m_YouWinMessage.IsAnimating() && !m_YouWinMessage.IsStayDuration()) || m_YouWinMessage.FinishFast()) )
      return false;

   return true;
}

bool Game::PollEvents()
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
						fprintf(stderr, "Hit Escape!n");
					return false;
					break;

					case SDLK_RETURN:
                                        case SDLK_LCTRL:
                                        case SDLK_RCTRL:
						IncrementSpot();
                                        break;

					case SDLK_0:
                                                SetSpot(0);
                                        break;
                                        case SDLK_1:
						SetSpot(1);
					break;
					
					case SDLK_UP:
                                                Move(Up);
        	                                break;
                                        
                                        case SDLK_DOWN:
                                                Move(Down);
	                                        break;

					case SDLK_RIGHT:
						Move(Right);
						break;
					
					case SDLK_LEFT:
						Move(Left);
						break;

					case SDLK_PLUS:
						Redo();
						break;

					case SDLK_MINUS:
						Undo();
						break;

					case SDLK_TAB:
                                                if( !m_bGameOver ) SetBinaryToSolved(m_Binary);
                                                if( IsBinarySolved(m_Binary) == BINARYLIB_SOLVED ) {
                                                        m_bGameOver = true;
						}
                                                break;

					case SDLK_a:
						AutoMove();
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

void Game::DrawSelector()
{
   m_Selector.DrawSelector();
}

void Game::UpdateDisplay()
{
	//Draw background
	m_Background.DrawBackground();

	m_Drawer.Draw();

	if( !m_YouWinMessage.HasMessage() /*&& !m_Pieces.Animate()*/ )
	{
		DrawSelector();
		SDL_UpdateRect(m_pScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	}

#ifdef USE_GRAPHIC_YOU_WIN
	if( m_bGameOver ) {
		if( m_pWinGraphic == NULL ) {
		m_pWinGraphic = nSDL_LoadImage(image_HitoriYouWin);
                SDL_SetColorKey(m_pWinGraphic, SDL_SRCCOLORKEY, SDL_MapRGB(m_pWinGraphic->format, 255, 255, 255));
		}

		SDL_Rect rectWin;
		rectWin.x = (SCREEN_WIDTH - m_pWinGraphic->w)/2;
		rectWin.y = (SCREEN_HEIGHT - m_pWinGraphic->h)/2;
		rectWin.w = m_pWinGraphic->w;
		rectWin.h = m_pWinGraphic->h;
	
		SDL_BlitSurface(m_pWinGraphic, NULL, m_pScreen, &rectWin);	
	}
#else
	if( m_YouWinMessage.HasMessage() && !m_YouWinMessage.Animate() )
#endif
        {
                SDL_UpdateRect(m_pScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        }
}

void Game::Move(Direction eDirection)
{
	if( m_bGameOver )
		return;

	m_Selector.Move(eDirection);	
}

void Game::SetSpot(int nValue)
{
   if( m_bGameOver )
      return;

   if( nValue > 1 || nValue < -1 )
      return;

   int nX = m_Selector.GetCurrentX(), nY = m_Selector.GetCurrentY();
   SetBinarySpotValue(m_Binary, nX, nY, nValue);

   CheckGameOver();
}

void Game::IncrementSpot()
{
   if( m_bGameOver )
      return;

   int nX = m_Selector.GetCurrentX(), nY = m_Selector.GetCurrentY();
   int nValue = GetBinarySpotValue(m_Binary, nX, nY);
   nValue++;
   if( nValue > 1 )
      nValue = -1;
   SetSpot(nValue);
}

void Game::Undo()
{
   if( m_bGameOver )
      return;

   BinaryUndo(m_Binary);
}

void Game::Redo()
{
   if( m_bGameOver )
      return;

   BinaryRedo(m_Binary);
}

void Game::AutoMove()
{
   if( m_bGameOver )
      return;

   BinaryAuto(m_Binary);

   CheckGameOver();
}

void Game::CheckGameOver()
{
   if( IsBinarySolved(m_Binary) == BINARYLIB_SOLVED ) {
       m_bGameOver = true;
       m_Timer.Stop();
#ifndef USE_GRAPHIC_YOU_WIN
       m_YouWinMessage.CreateMessage("You Win!!!\n******");
#endif
       if( m_nLevelNumber >= 0 ) {
           m_pConfig->SetBeatLevel(m_nLevelNumber, m_Timer.GetElapsed());
       }
   }
}
