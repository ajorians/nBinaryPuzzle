#ifndef HELP_H
#define HELP_H

extern "C"
{
	#include <os.h>
	#include "SDL/SDL.h"
}

class BinaryHelp
{
public:
	BinaryHelp(SDL_Surface* pScreen);
	~BinaryHelp();

	bool Loop();
	
protected:
	bool PollEvents();
	void UpdateDisplay();

protected:
	SDL_Surface	*m_pScreen;//Does not own
	nSDL_Font	*m_pFont;
};

#endif
