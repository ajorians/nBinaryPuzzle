#include "GameDrawer.h"

extern "C"
{
#include "SDL/SDL_gfxPrimitives.h"
}

#include "Metrics.h"
#include "Defines.h"

GameDrawer::GameDrawer(SDL_Surface* pSurface, BinaryLib binary, Metrics& metrics)
: m_pSurface(pSurface), m_Binary(binary), m_pBoardMetrics(&metrics)
{
   m_pFont = nSDL_LoadFont(NSDL_FONT_THIN, 0/*R*/, 0/*G*/, 0/*B*/);
   m_pFontPermanent = nSDL_LoadFont(NSDL_FONT_VGA, 49/*R*/, 79/*G*/, 79/*B*/);
}

GameDrawer::~GameDrawer()
{
   nSDL_FreeFont(m_pFont);
   nSDL_FreeFont(m_pFontPermanent);
}

void GameDrawer::Draw()
{
   boxRGBA(m_pSurface, m_pBoardMetrics->GetBoardLeft(), m_pBoardMetrics->GetBoardTop(), m_pBoardMetrics->GetBoardRight(), m_pBoardMetrics->GetBoardBottom(), GAME_BACKGROUND_R, GAME_BACKGROUND_G, GAME_BACKGROUND_B, 230);

   for(int nX=0; nX<=m_pBoardMetrics->GetWidth(); nX++) {
       vlineRGBA(m_pSurface, m_pBoardMetrics->GetLeftSpots() + nX*m_pBoardMetrics->GetPieceSize(), m_pBoardMetrics->GetTopSpots(), m_pBoardMetrics->GetBottomSpots(), 0, 0, 0, 255);
   }

   for(int nY=0; nY<=m_pBoardMetrics->GetHeight(); nY++) {
       hlineRGBA(m_pSurface, m_pBoardMetrics->GetLeftSpots(), m_pBoardMetrics->GetRightSpots(), m_pBoardMetrics->GetTopSpots() + nY*m_pBoardMetrics->GetPieceSize(), 0, 0, 0, 255);
   }

   for(int nX=0; nX<m_pBoardMetrics->GetWidth(); nX++)
      if( IsBinaryColDuplicate(m_Binary, nX) != BINARYLIB_ROW_UNIQUE ) {
         SDL_Rect rect;
         rect.w = m_pBoardMetrics->GetPieceSize();
         rect.h = m_pBoardMetrics->GetBottomSpots() - m_pBoardMetrics->GetTopSpots();
         rect.x = m_pBoardMetrics->GetXPos(nX);
         rect.y = m_pBoardMetrics->GetTopSpots();
         SDL_FillRect(m_pSurface, &rect, SDL_MapRGB(m_pSurface->format, 255, 255, 0));
      }

   for(int nY=0; nY<m_pBoardMetrics->GetHeight(); nY++)
      if( IsBinaryRowDuplicate(m_Binary, nY) != BINARYLIB_ROW_UNIQUE ) {
         SDL_Rect rect;
         rect.w = m_pBoardMetrics->GetRightSpots() - m_pBoardMetrics->GetLeftSpots();
         rect.h = m_pBoardMetrics->GetPieceSize();
         rect.x = m_pBoardMetrics->GetLeftSpots();
         rect.y = m_pBoardMetrics->GetYPos(nY);
         SDL_FillRect(m_pSurface, &rect, SDL_MapRGB(m_pSurface->format, 255, 255, 0));
      }

   for(int nX=0; nX<m_pBoardMetrics->GetWidth(); nX++) {
       for(int nY=0; nY<m_pBoardMetrics->GetHeight(); nY++) {
           DrawCell(nX, nY);
       }
   }
}

void GameDrawer::DrawCell(int nX, int nY)
{
   int nValue = GetBinarySpotValue(m_Binary, nX, nY);
   if( nValue < 0 )
      return;

   if( IsBinarySpot3InRow(m_Binary, nX, nY) != BINARYLIB_NOT_MULTIPILE_INROW ) {
      SDL_Rect rect;
      rect.w = rect.h = m_pBoardMetrics->GetPieceSize();
      rect.x = m_pBoardMetrics->GetXPos(nX);
      rect.y = m_pBoardMetrics->GetYPos(nY);
      SDL_FillRect(m_pSurface, &rect, SDL_MapRGB(m_pSurface->format, 255, 255, 0));
   }

   char buffer[16];
   Puz_itoa(nValue, buffer, 16);
   int nIndent = (m_pBoardMetrics->GetPieceSize() - nSDL_GetStringWidth(m_pFont, buffer))/2;
   bool bPermanent = IsBinarySpotPermanent(m_Binary, nX, nY) == BINARYLIB_SPOT_PERMANENT;
   nSDL_DrawString(m_pSurface, bPermanent ? m_pFontPermanent : m_pFont, m_pBoardMetrics->GetXPos(nX) + nIndent, m_pBoardMetrics->GetYPos(nY) + m_pBoardMetrics->GetPieceSize()/2, buffer);
}



