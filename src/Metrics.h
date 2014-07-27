#ifndef METRICS_H
#define METRICS_H

extern "C"
{
        #include <os.h>
}

#define INDICATOR_PORTION	(0.5)

class Metrics
{
public:
   Metrics(int nWidthScreen, int nHeightScreen);
   bool SetDimensions(int nWidth, int nHeight);//TODO: Rename; call it DetermineDemensions?
   int GetWidth() const;
   int GetHeight() const;
   int GetXPos(int nBoardX) const;
   int GetYPos(int nBoardY) const;
   int GetPieceSize() const;

   int GetBoardLeft() const;
   int GetBoardRight() const;
   int GetBoardTop() const;
   int GetBoardBottom() const;

   int GetLeftSpots() const;
   int GetRightSpots() const;
   int GetTopSpots() const;
   int GetBottomSpots() const;

protected:
   int m_nWidthScreen, m_nHeightScreen;
   int m_nWidth;
   int m_nHeight;
   int m_nPieceSize;
   int m_nLeftSpots;
   int m_nTopSpots;
};

#endif

