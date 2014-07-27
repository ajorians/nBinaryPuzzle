#ifndef GAMEDRAWER_H
#define GAMEDRAWER_H

extern "C"
{
        #include <os.h>
        #include "SDL/SDL.h"
        #include "BinaryLib/BinaryLib.h"
}

class Metrics;

class GameDrawer
{
public:
   GameDrawer(SDL_Surface* pSurface, BinaryLib binary, Metrics& metrics);
   ~GameDrawer();
   void Draw();

protected:
   void DrawCell(int nX, int nY);

protected:
   SDL_Surface	*m_pSurface;
   BinaryLib	m_Binary;
   Metrics	*m_pBoardMetrics;

   nSDL_Font    *m_pFont;
   nSDL_Font    *m_pFontPermanent;
};

#endif

