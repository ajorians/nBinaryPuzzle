extern "C"
{
	#include <os.h>
	#include "SDL/SDL.h"
	#include "ArchiveLib/ArchiveLib.h"
}

#include "Menu.h"
#include "Game.h"
#include "Config.h"
#include "Options.h"
#include "Startup.h"
#include "Help.h"

#define SCREEN_BIT_DEPTH        (16)
#define SCREEN_VIDEO_MODE (SDL_SWSURFACE|SDL_FULLSCREEN|SDL_HWPALETTE)

class GameObject
{
public:
   GameObject(Config* pConfig)
        : m_pConfig(pConfig), m_game(NULL), m_nGameNumber(0)
   {
      if( strlen(pConfig->m_LevelData) > 0 ) {
         printf("Level data: [%s]\n", pConfig->m_LevelData);
         BinaryLibCreate(&m_game, pConfig->m_LevelData);
         m_nGameNumber = pConfig->GetCurrentLevel();
      }
   }
   ~GameObject()
   {
      EndCurrentGame();
   }

   bool GameInProgress() {
      if( m_game == NULL )
         return false;
      return (IsBinarySolved(m_game) != BINARYLIB_SOLVED) && (IsBinaryEmpty(m_game) == BINARYLIB_NOT_EMPTY);
   }

   void PersistCurrentGame()
   {
      if( !GameInProgress() ) {
         strcpy(m_pConfig->m_LevelData, "");
         return;
      }

      if( m_nGameNumber < 0 ) {
         strcpy(m_pConfig->m_LevelData, "");
         return;
      }

      char* pstr = NULL;
      BinaryLibPersist(m_game, &pstr);
      printf("size of pstr: %d\n", strlen(pstr));
      printf("str: [%s]\n", pstr);
      strcpy(m_pConfig->m_LevelData, pstr);
      BinaryLibFreePersist(pstr);
      m_pConfig->SetCurrentLevel(m_nGameNumber);
   }

   void EndCurrentGame() {
      if( m_game ) {
        BinaryLibFree(&m_game);
        m_game = NULL;
      }
   }

   int CurrentGameNumber() {
      return m_nGameNumber;
   }

   BinaryLib StartGame(const char* pstrFile, int nNumber) {
      if( m_game && nNumber == m_nGameNumber )
         return m_game;

      return RestartGame(pstrFile, nNumber);
   }

   BinaryLib RestartGame(const char* pstrFile, int nNumber) {
      EndCurrentGame();

      BinaryLibCreate(&m_game, pstrFile);
      m_nGameNumber = nNumber;
      return m_game;
   }

protected:
   Config* m_pConfig;
   BinaryLib m_game;
   int m_nGameNumber;
};

void PlayGame(SDL_Surface* pScreen, BinaryLib lib, int nLevelNumber, Config* pConfig)
{
   bool bPlay = true;
   while(bPlay)
   {
      Game game(pScreen, lib, nLevelNumber, pConfig);
      /* Game loop */
      while(game.Loop()){}

      bPlay = false;
   }
}

int main(int argc, char *argv[]) 
{
	if (argc != 2) {
        	if (!config_file_already_written()) {
			write_config_file();
        	}
	}

	printf("Initializing SDL.\n");
	
	/* Initialize the SDL library (starts the event loop) */
    if ( SDL_Init(SDL_INIT_VIDEO ) < 0 )
	{
        fprintf(stderr,
                "Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }
	
	printf("SDL initialized.\n");
	
	SDL_Surface* pScreen = NULL;
	pScreen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BIT_DEPTH, SCREEN_VIDEO_MODE);

	ArchiveSetCurrentDirectory(argv[0]);
	Config config;
	GameObject gameobj(&config);

	if( pScreen == NULL )
	{
		fprintf(stderr, "Couldn't set %dx%dx%d video mode: %s\n", SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BIT_DEPTH, SDL_GetError());
		exit(1);
	}
	else
	{
		/* Hides the cursor */
		SDL_ShowCursor(SDL_DISABLE);

		int nLevelNumber = -1;
		char strLevelData[2048];//Big enough for a 18x18 puzzle with solution

                if( gameobj.GameInProgress() ) {
                   while(isKeyPressed(KEY_NSPIRE_ENTER)){}
                   if( 2 == show_msgbox_2b("Continue game?", "There is an unfinished game; would you like to continue that one or end the game?", "Continue", "End game") ) {
                      printf("ending game\n");
                      gameobj.EndCurrentGame();
                      strcpy(config.m_LevelData, "");
                      while(isKeyPressed(KEY_NSPIRE_ENTER)){}
                   } else {
                      while(isKeyPressed(KEY_NSPIRE_ENTER)){}
                      printf("Leveldata: [%s]\n", config.m_LevelData);
                      PlayGame(pScreen, gameobj.StartGame(config.m_LevelData, config.GetCurrentLevel()), config.GetCurrentLevel(), &config);
                   }
                }

		while(true)
		{
			bool bShowHelp = false, bShowOptions = false;
			SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

			if( argc != 2 )
			{
				MainMenu menu(pScreen, &config);
				while(menu.Loop()){}
				if( menu.ShouldQuit() )
					break;
				bShowHelp = menu.ShouldShowHelp();
				bShowOptions = menu.ShowShowOptions();
				strcpy(strLevelData, menu.GetLevel());
				nLevelNumber = menu.GetLevelNumber();
			}
			else
			{
				FILE *fp = fopen(argv[1], "r");
				if (!fp) { return 0; }
				struct stat filestat;
				if (stat(argv[1],&filestat) == -1) { fclose(fp); return 0; }

				fread(strLevelData, 1, filestat.st_size, fp);

				strLevelData[filestat.st_size] = 0;

    				fclose(fp);
			}
			
			if( bShowOptions ) {
				Options ops(pScreen, &config);
				while(ops.Loop()){}
				continue;
			}
			else if( bShowHelp )
			{
				BinaryHelp help(pScreen);
				while(help.Loop()){}
				continue;
			}
			else
			{
				if( gameobj.GameInProgress() && gameobj.CurrentGameNumber() == nLevelNumber ) {
                                        while(isKeyPressed(KEY_NSPIRE_ENTER)){}
                                        if( 2 == show_msgbox_2b("Continue game?", "There is an unfinished game; would you like to continue that one or start a new one?", "Continue", "New game") ) {
                                        gameobj.EndCurrentGame();
                                        }
                                }
                                else {
                                        gameobj.EndCurrentGame();
                                }
                                PlayGame(pScreen, gameobj.StartGame(strLevelData, nLevelNumber), nLevelNumber, &config);
			}

			if( argc == 2 )
				break;//Just play game and exit if playing a level.
		}
	}
	
	printf("Quitting SDL.\n");

    gameobj.PersistCurrentGame();
    
    /* Shutdown all subsystems */
    SDL_Quit();

    printf("Quitting...\n");

	return 0;
}
