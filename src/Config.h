#ifndef CONFIG_H
#define CONFIG_H

extern "C"
{
	#include <os.h>
	#include "ArchiveLib/ArchiveLib.h"
}

#define MAKE_CONFIG_ITEM(type, name) \
public: \
   type Get##name() const { return m_##name; }\
   void Set##name(type val) { m_##name = val; }\
protected: \
   type m_##name;

class Config
{
public:
   Config();
   ~Config();

   MAKE_CONFIG_ITEM(bool, SelectorMovementAnimate);
   MAKE_CONFIG_ITEM(bool, BackgroundMoves);
   MAKE_CONFIG_ITEM(int,  CurrentLevel);
public:
   char m_LevelData[2048];

public:
   bool BeatLevel(int nLevelNumber, int& nCompletionTime) const;
   void SetBeatLevel(int nLevelNumber, int nTimeElapsed);

protected:
   ArchiveLib	m_Archive;
};

#endif

