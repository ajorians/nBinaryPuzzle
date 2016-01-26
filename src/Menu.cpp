#include "Menu.h"
#include "MainMenuGraphic.h"
#include "BinaryStarGraphic.h"
#include "Defines.h"
#ifdef SHOW_LEVEL_DIMENSIONS
extern "C"
{
        #include "BinaryLib/BinaryLib.h"
}
#include "Metrics.h"
#include "GameDrawer.h"
#endif
#include "BinaryLevels.h"

#define MENU_FONT		(NSDL_FONT_VGA)//NSDL_FONT_THIN
#ifdef SHOW_PREVIEW
#define PREVIEW_SURFACE_SIZE	(72)
#endif

#define CLASSIC_NSPIRE		(is_classic)

MainMenu::LevelSize MainMenu::s_eLevelSize = MainMenu::size6x6;
MainMenu::Difficulty MainMenu::s_eDifficulty = MainMenu::Easy;
int MainMenu::s_nLevel = 0;

MainMenu::MainMenu(SDL_Surface* pScreen, Config* pConfig)
: m_pScreen(pScreen), m_eChoice(Play), m_pConfig(pConfig), m_bBeatLevel(false), m_nCompletionTime(-1)
#ifdef SHOW_LEVEL_DIMENSIONS
, m_nWidth(-1), m_nHeight(-1)
#ifdef SHOW_PREVIEW
, m_pPreview(NULL)
#endif
#endif
{
	m_pBackground = nSDL_LoadImage(image_BinaryMainMenu);
	m_pStar = nSDL_LoadImage(image_BinaryStar);
	SDL_SetColorKey(m_pStar, SDL_SRCCOLORKEY, SDL_MapRGB(m_pStar->format, 0, 0, 0));

	if( CLASSIC_NSPIRE ) {
		m_pFont = nSDL_LoadFont(MENU_FONT, 0/*R*/, 0/*G*/, 0/*B*/);
	} else {
		m_pFont = nSDL_LoadFont(MENU_FONT, 255/*R*/, 255/*G*/, 255/*B*/);
	}

#if 0
	if( !g_nRelocatedGlobals ) {
        	nl_relocdata((unsigned*)g_Levels, sizeof(g_Levels)/sizeof(g_Levels[0]));
        	g_nRelocatedGlobals = 1;
   	}
#endif

	UpdateLevelDimensions();
}

MainMenu::~MainMenu()
{
#ifdef SHOW_LEVEL_DIMENSIONS
#ifdef SHOW_PREVIEW
	FreePreviewSurface();
#endif
#endif
	SDL_FreeSurface(m_pBackground);
	nSDL_FreeFont(m_pFont);
}

bool MainMenu::Loop()
{
	//Handle keypresses
	if( PollEvents() == false )
		return false;
	
	UpdateDisplay();

	sleep(5);
	
	return true;
}

bool MainMenu::ShouldQuit() const
{
	return m_eChoice == Quit;
}

bool MainMenu::ShowShowOptions() const
{
	return m_eChoice == Options;
}

bool MainMenu::ShouldShowHelp() const
{
	return m_eChoice == Help;
}

const char* MainMenu::GetLevel()
{
   return g_Levels[GetLevelNumber()-1];
}

#ifdef ALLOW_ALL_LEVELS
#define LEVEL_MULTIPLIER		(10)
#else
#define LEVEL_MULTIPLIER	(1)
#endif

int MainMenu::GetLevelNumber() const
{
   int nIndex = 0;
   if( s_eLevelSize == size8x8 )
      nIndex = 40*LEVEL_MULTIPLIER;
   else if( s_eLevelSize == size10x10 )
      nIndex = 80*LEVEL_MULTIPLIER;
   else if( s_eLevelSize == size12x12 )
      nIndex = 120*LEVEL_MULTIPLIER;
   else if( s_eLevelSize == size14x14 )
      nIndex = 160*LEVEL_MULTIPLIER;

   if( s_eDifficulty == Medium )
      nIndex += 10*LEVEL_MULTIPLIER;
   else if( s_eDifficulty == Hard )
      nIndex += 20*LEVEL_MULTIPLIER;
   else if( s_eDifficulty == VeryHard )
      nIndex += 30*LEVEL_MULTIPLIER;

   nIndex += s_nLevel;
   return nIndex+1;//For the save data
}

bool MainMenu::PollEvents()
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
				printf( "Key press detected\n" );
				switch (event.key.keysym.sym) 
				{
					case SDLK_ESCAPE:
						fprintf(stderr, "Hit Escape!n");
						m_eChoice = Quit;
					return false;
					break;
					
					case SDLK_UP:
					case SDLK_8:
						if( m_eChoice == Help )
							m_eChoice = Options;
						else if( m_eChoice == Options )
							m_eChoice = PlayLevel;
						else if( m_eChoice == PlayLevel )
							m_eChoice = PlayDifficulty;
						else if( m_eChoice == PlayDifficulty )
							m_eChoice = PlaySize;
						else if( m_eChoice == PlaySize )
							m_eChoice = Play;
						break;
					
					case SDLK_DOWN:
					case SDLK_2:
						if( m_eChoice == Play )
							m_eChoice = PlaySize;
						else if( m_eChoice == PlaySize )
							m_eChoice = PlayDifficulty;
						else if( m_eChoice == PlayDifficulty )
							m_eChoice = PlayLevel;
						else if( m_eChoice == PlayLevel )
							m_eChoice = Options;
						else if( m_eChoice == Options )
							m_eChoice = Help;
						break;

					case SDLK_LEFT:
                                        case SDLK_4:
						if( m_eChoice == PlaySize ) {
							if( s_eLevelSize == size6x6 )
								s_eLevelSize = size14x14;
							else if( s_eLevelSize == size8x8 )
								s_eLevelSize = size6x6;
							else if( s_eLevelSize == size10x10 )
                                                                s_eLevelSize = size8x8;
							else if( s_eLevelSize == size12x12 )
                                                                s_eLevelSize = size10x10;
							else if( s_eLevelSize == size14x14 )
                                                                s_eLevelSize = size12x12;
							s_nLevel = 0;
						} else if( m_eChoice == PlayDifficulty ) {
							if( s_eDifficulty == Easy )
								s_eDifficulty = VeryHard;
							else if( s_eDifficulty == Medium )
                                                                s_eDifficulty = Easy;
							else if( s_eDifficulty == Hard )
                                                                s_eDifficulty = Medium;
							else if( s_eDifficulty == VeryHard )
                                                                s_eDifficulty = Hard;
							s_nLevel = 0;
						} else if( m_eChoice == PlayLevel ) {
							s_nLevel--;
							if( s_nLevel < 0 )
								s_nLevel = (10*LEVEL_MULTIPLIER)-1;
						}
						UpdateLevelDimensions();
						break;

					case SDLK_RIGHT:
                                        case SDLK_6:
						if( m_eChoice == PlaySize ) {
                                                        if( s_eLevelSize == size6x6 )
                                                                s_eLevelSize = size8x8;
                                                        else if( s_eLevelSize == size8x8 )
                                                                s_eLevelSize = size10x10;
                                                        else if( s_eLevelSize == size10x10 )
                                                                s_eLevelSize = size12x12;
                                                        else if( s_eLevelSize == size12x12 )
                                                                s_eLevelSize = size14x14;
                                                        else if( s_eLevelSize == size14x14 )
                                                                s_eLevelSize = size6x6;
                                                        s_nLevel = 0;
                                                } else if( m_eChoice == PlayDifficulty ) {
                                                        if( s_eDifficulty == Easy )
                                                                s_eDifficulty = Medium;
                                                        else if( s_eDifficulty == Medium )
                                                                s_eDifficulty = Hard;
                                                        else if( s_eDifficulty == Hard )
                                                                s_eDifficulty = VeryHard;
                                                        else if( s_eDifficulty == VeryHard )
                                                                s_eDifficulty = Easy;
                                                        s_nLevel = 0;
                                                } else if( m_eChoice == PlayLevel ) {
							s_nLevel++;
							if( s_nLevel >= (10*LEVEL_MULTIPLIER) )
								s_nLevel = 0;
						}
						UpdateLevelDimensions();
						break;
					
					case SDLK_SPACE:
					case SDLK_RETURN:
					case SDLK_LCTRL:
						if( m_eChoice == Play || m_eChoice == Options || m_eChoice == Help )
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
				printf( "Key release detected\n" );
				break;
			
			default:
				break;
		}
	}
	return true;
}

#define PLAY_POS			(62)
#define PLAY_SIZE_POS			(92)
#define PLAY_DIFFICULTY_POS		(PLAY_SIZE_POS+14)
#define PLAY_LEVEL_POS			(PLAY_DIFFICULTY_POS+14)
#ifdef SHOW_LEVEL_DIMENSIONS
#define PLAY_DIMENSION_POS              (PLAY_LEVEL_POS+14)
#define PLAY_COMPLETION_POS		(PLAY_DIMENSION_POS+14)
#else
#define PLAY_COMPLETION_POS             (PLAY_LEVEL_POS+14)
#endif

#define INDICATOR_SIZE_WIDTH		(80)
#define INDICATOR_DIFFICULTY_WIDTH	(137)
#define INDICATOR_LEVEL_WIDTH		(87)

#define INDICATOR_OFFSET		(5)
#define INDICATOR_HEIGHT		(18)
#define OPTIONS_POS			(189)
#define HELP_POS			(210)

void MainMenu::UpdateDisplay()
{
	//Draw background
	if( CLASSIC_NSPIRE ) {
		SDL_FillRect(m_pScreen, NULL, SDL_MapRGB(m_pScreen->format, 255, 255, 255));
		nSDL_DrawString(m_pScreen, m_pFont, SCREEN_WIDTH/2-30, 15, "nBinary Puzzle");
		nSDL_DrawString(m_pScreen, m_pFont, SCREEN_WIDTH/2-14, 75, "Play");
		nSDL_DrawString(m_pScreen, m_pFont, SCREEN_WIDTH/2-30, 195, "Options");
		nSDL_DrawString(m_pScreen, m_pFont, SCREEN_WIDTH/2-25, 215, "Help");
	} else {
		SDL_BlitSurface(m_pBackground, NULL, m_pScreen, NULL);
	}

	char buffer[16];
	strcpy(buffer, "Size: ");
	if( s_eLevelSize == size6x6 )
		strcat(buffer, "6x6");
	else if( s_eLevelSize == size8x8 )
                strcat(buffer, "8x8");
	else if( s_eLevelSize == size10x10 )
                strcat(buffer, "10x10");
	else if( s_eLevelSize == size12x12 )
                strcat(buffer, "12x12");
	else if( s_eLevelSize == size14x14 )
                strcat(buffer, "14x14");
	int nLeftLevelSize = (SCREEN_WIDTH-nSDL_GetStringWidth(m_pFont, buffer))/2;
	nSDL_DrawString(m_pScreen, m_pFont, nLeftLevelSize, PLAY_SIZE_POS, buffer);

	strcpy(buffer, "Difficulty: ");
	if( s_eDifficulty == Easy )
                strcat(buffer, "Easy");
        else if( s_eDifficulty == Medium )
                strcat(buffer, "Medium");
        else if( s_eDifficulty == Hard )
                strcat(buffer, "Hard");
        else if( s_eDifficulty == VeryHard )
                strcat(buffer, "Very Hard");
        int nLeftDifficulty = (SCREEN_WIDTH-nSDL_GetStringWidth(m_pFont, buffer))/2;
        nSDL_DrawString(m_pScreen, m_pFont, nLeftDifficulty, PLAY_DIFFICULTY_POS, buffer);

	strcpy(buffer, "Level ");
        Puz_itoa(s_nLevel+1, buffer + 6, 16-6);
	int nLeftLevelName = (SCREEN_WIDTH-nSDL_GetStringWidth(m_pFont, buffer))/2;
	nSDL_DrawString(m_pScreen, m_pFont, nLeftLevelName, PLAY_LEVEL_POS, buffer);

#ifdef SHOW_LEVEL_DIMENSIONS
	if( m_nWidth > 0 && m_nHeight > 0 ) {
		char bufMessage[16];
		char buf[8];
		strcpy(bufMessage, "(");
		Puz_itoa(m_nWidth, buf, 8);
		strcat(bufMessage, buf);
		strcat(bufMessage, ", ");
		Puz_itoa(m_nHeight, buf, 8);
		strcat(bufMessage, buf);
		strcat(bufMessage, ")");
		nSDL_DrawString(m_pScreen, m_pFont, (SCREEN_WIDTH-nSDL_GetStringWidth(m_pFont, bufMessage))/2, PLAY_DIMENSION_POS, bufMessage);
	}
#endif

#ifdef SHOW_PREVIEW
	SDL_Rect dstPreview;
	dstPreview.w = dstPreview.h = PREVIEW_SURFACE_SIZE;
	dstPreview.x = SCREEN_WIDTH/2 + 62 + 5;
	dstPreview.y = 55;
	SDL_BlitSurface(m_pPreview, NULL, m_pScreen, &dstPreview);
#endif

	if( m_bBeatLevel ) {
		SDL_Rect rectStar;
		rectStar.x = nLeftLevelName - 20;
		rectStar.y = PLAY_LEVEL_POS-5;
		rectStar.w = 16;
		rectStar.h = 16;
		SDL_BlitSurface(m_pStar, NULL, m_pScreen, &rectStar);
		if( m_nCompletionTime >= 0 ) {
			char bufMessage[32];
			char buf[8];
			strcpy(bufMessage, "Completed in ");
			Puz_itoa(m_nCompletionTime, buf, 8);
			strcat(bufMessage, buf);
			strcat(bufMessage, " seconds");
			/*SDL_Rect rectLevelTime;
		        rectLevelTime.w = nSDL_GetStringWidth(m_pFont, bufMessage) + 3;
		        rectLevelTime.h = nSDL_GetStringHeight(m_pFont, bufMessage);
		        rectLevelTime.x = (SCREEN_WIDTH-nSDL_GetStringWidth(m_pFont, bufMessage))/2;
		        rectLevelTime.y = 116;
		        SDL_FillRect(m_pScreen, &rectLevelTime, SDL_MapRGB(m_pScreen->format, 255, 255, 255));*/
			nSDL_DrawString(m_pScreen, m_pFont, (SCREEN_WIDTH-nSDL_GetStringWidth(m_pFont, bufMessage))/2, PLAY_COMPLETION_POS, bufMessage);
		}
	}
	
	if( m_eChoice == Play )
		draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, 255, 0, 0), SCREEN_WIDTH/2-40, PLAY_POS, 80, 25, 1);
	else if( m_eChoice == PlaySize )
		draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, 255, 0, 0), SCREEN_WIDTH/2-(INDICATOR_SIZE_WIDTH/2), PLAY_SIZE_POS-INDICATOR_OFFSET, INDICATOR_SIZE_WIDTH, INDICATOR_HEIGHT, 1);
	else if( m_eChoice == PlayDifficulty )
		draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, 255, 0, 0), SCREEN_WIDTH/2-(INDICATOR_DIFFICULTY_WIDTH/2), PLAY_DIFFICULTY_POS-INDICATOR_OFFSET, INDICATOR_DIFFICULTY_WIDTH, INDICATOR_HEIGHT, 1);
	else if( m_eChoice == PlayLevel )
		draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, 255, 0, 0), SCREEN_WIDTH/2-(INDICATOR_LEVEL_WIDTH/2), PLAY_LEVEL_POS-INDICATOR_OFFSET, INDICATOR_LEVEL_WIDTH, INDICATOR_HEIGHT, 1);
	else if( m_eChoice == Options )
		draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, 255, 0, 0), 114, OPTIONS_POS, 93, 24, 1);
	else if( m_eChoice == Help )
		draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, 255, 0, 0), 130, HELP_POS, 60, 25, 1);
	
	SDL_UpdateRect(m_pScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

#ifdef SHOW_LEVEL_DIMENSIONS
#ifdef SHOW_PREVIEW
void MainMenu::FreePreviewSurface()
{
   if( m_pPreview ) {
      SDL_FreeSurface(m_pPreview);
      m_pPreview = NULL;
   }
}
#endif
#endif

void MainMenu::UpdateLevelDimensions()
{
	m_bBeatLevel = m_pConfig->BeatLevel(GetLevelNumber(), m_nCompletionTime);
#ifdef SHOW_LEVEL_DIMENSIONS
	BinaryLib tow;
        BinaryLibCreate(&tow, GetLevel());
        m_nWidth = GetBinaryBoardWidth(tow);
        m_nHeight = GetBinaryBoardHeight(tow);

#ifdef SHOW_PREVIEW
	FreePreviewSurface();
	m_pPreview = SDL_CreateRGBSurface(SDL_SWSURFACE, PREVIEW_SURFACE_SIZE, PREVIEW_SURFACE_SIZE, 16, 0, 0, 0, 0);
	Metrics met(PREVIEW_SURFACE_SIZE, PREVIEW_SURFACE_SIZE);
	met.SetDimensions(m_nWidth, m_nHeight);
	
	GameDrawer drawer(m_pPreview, tow, met);
	drawer.Draw();
#endif

        BinaryLibFree(&tow);
#endif	
}


