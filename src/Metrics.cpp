#include "Metrics.h"
extern "C"
{
        #include "BinaryLib/BinaryLib.h"
}

Metrics::Metrics(int nWidthScreen, int nHeightScreen)
: m_nWidthScreen(nWidthScreen), m_nHeightScreen(nHeightScreen)
{
}

bool Metrics::SetDimensions(int nWidth, int nHeight)
{
   m_nWidth = nWidth;
   m_nHeight = nHeight;

   int nPieceWidth = m_nWidthScreen/((double)nWidth);
   int nPieceHeight = m_nHeightScreen/((double)nHeight);

   m_nPieceSize = nPieceWidth < nPieceHeight ? nPieceWidth : nPieceHeight;

   int nBoardWidth = m_nPieceSize * nWidth;
   int nBoardHeight = m_nPieceSize * nHeight;

   m_nTopSpots = (m_nHeightScreen - nBoardHeight) / 2;
   m_nLeftSpots = (m_nWidthScreen - nBoardWidth) / 2;

   return true;
}

int Metrics::GetWidth() const { return m_nWidth; }
int Metrics::GetHeight() const { return m_nHeight; }

int Metrics::GetXPos(int nBoardX) const
{
   return m_nLeftSpots + nBoardX*m_nPieceSize;
}

int Metrics::GetYPos(int nBoardY) const
{
   return m_nTopSpots + nBoardY*m_nPieceSize;
}

int Metrics::GetPieceSize() const
{
   return m_nPieceSize;
}

int Metrics::GetBoardLeft() const
{
   return GetLeftSpots();
}

int Metrics::GetBoardRight() const
{
   return GetRightSpots();
}

int Metrics::GetBoardTop() const
{
   return GetTopSpots();
}

int Metrics::GetBoardBottom() const
{
   return GetBottomSpots();
}

int Metrics::GetLeftSpots() const
{
   return m_nLeftSpots;
}

int Metrics::GetRightSpots() const
{
   return m_nLeftSpots + m_nWidth * m_nPieceSize;
}

int Metrics::GetTopSpots() const
{
   return m_nTopSpots;
}

int Metrics::GetBottomSpots() const
{
   return m_nTopSpots + m_nHeight * m_nPieceSize;
}


