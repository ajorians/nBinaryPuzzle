#ifndef MENU_H
#define MENU_H

//#define SHOW_LEVEL_DIMENSIONS//No real point since you choose the size
//#define SHOW_PREVIEW

extern "C"
{
	#include <os.h>
	#include "SDL/SDL.h"
}

#include "Config.h"

class MainMenu
{
public:
	MainMenu(SDL_Surface* pScreen, Config* pConfig);
	~MainMenu();

	bool Loop();

	bool ShouldQuit() const;
	bool ShowShowOptions() const;
	bool ShouldShowHelp() const;
	const char* GetLevel();
	int GetLevelNumber() const;//Note this is the combined level number!
	
protected:
	bool PollEvents();
	void UpdateDisplay();
	void UpdateLevelDimensions();
#ifdef SHOW_PREVIEW
	void FreePreviewSurface();
#endif

protected:
	SDL_Surface	*m_pScreen;//Does not own
	SDL_Surface 	*m_pBackground;
	nSDL_Font 	*m_pFont;
	enum MenuChoice{PlaySize, PlayDifficulty, PlayLevel, Play, Options, Help, Quit};
	MenuChoice 	m_eChoice;
	Config		*m_pConfig;
	enum LevelSize{size6x6, size8x8, size10x10, size12x12, size14x14};
	enum Difficulty{Easy, Medium, Hard, VeryHard};
	static LevelSize s_eLevelSize;
	static Difficulty s_eDifficulty;
	static int 	s_nLevel;
	bool		m_bBeatLevel;
	int		m_nCompletionTime;
	SDL_Surface	*m_pStar;
#ifdef SHOW_LEVEL_DIMENSIONS
	int 		m_nWidth;
	int 		m_nHeight;
#endif
#ifdef SHOW_PREVIEW
	SDL_Surface     *m_pPreview;
#endif
};

#endif
