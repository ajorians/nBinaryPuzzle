#include "Config.h"
#include "Defines.h"

#define SELECTOR_ANIMATE_DEFAULT        (true)
#define BACKGROUND_MOVES_DEFAULT	(true)

#define STRINGIFY_CONFIG_ITEM(x) #x

#define READ_BOOL_CONFIG_VAR(identifier) if( strcmp(strName, STRINGIFY_CONFIG_ITEM(Use##identifier) ) == 0 ) {\
   if( strcmp("0", GetValue(m_Archive, "Settings", i)) == 0 ) {\
      m_##identifier = false;\
   } else {\
      m_##identifier = true;\
  }\
}

#define WRITE_BOOL_CONFIG_VAR(identifier) UpdateArchiveEntry(m_Archive, "Settings", STRINGIFY_CONFIG_ITEM(Use##identifier), m_##identifier ? "1" : "0", NULL);

#define READ_INT_CONFIG_VAR(identifier) if( strcmp(strName, STRINGIFY_CONFIG_ITEM(Use##identifier) ) == 0 ) {\
   m_##identifier = atoi( GetValue(m_Archive, "Settings", i));\
}

#define WRITE_INT_CONFIG_VAR(identifier) Puz_itoa(m_##identifier, buffer, 8);\
   UpdateArchiveEntry(m_Archive, "Settings", STRINGIFY_CONFIG_ITEM(Use##identifier), buffer, NULL);

#define READ_STRING_CONFIG_VAR(identifier) if( strcmp(strName, STRINGIFY_CONFIG_ITEM(Current##identifier) ) == 0 ) {\
   strcpy(m_##identifier, GetValue(m_Archive, "Settings", i));\
}

#define WRITE_STRING_CONFIG_VAR(identifier) UpdateArchiveEntry(m_Archive, "Settings", STRINGIFY_CONFIG_ITEM(Current##identifier), m_##identifier, NULL);

Config::Config()
: m_SelectorMovementAnimate(SELECTOR_ANIMATE_DEFAULT), m_BackgroundMoves(BACKGROUND_MOVES_DEFAULT), m_CurrentLevel(0)
{
   ArchiveCreate(&m_Archive);

   OpenArchiveFile(m_Archive, "BinarySettings.tns");

   int nSettings = GetNumberArchives(m_Archive, "Settings");
   char strName[MAX_NAME_LENGTH];
   for(int i=0; i<nSettings; i++) {
      strcpy(strName, GetName(m_Archive, "Settings", i));

      READ_BOOL_CONFIG_VAR(SelectorMovementAnimate);
      READ_BOOL_CONFIG_VAR(BackgroundMoves);
      READ_INT_CONFIG_VAR(CurrentLevel);
      READ_STRING_CONFIG_VAR(LevelData);
   }
}

Config::~Config()
{
   ArchiveSetBatchMode(m_Archive, ARCHIVE_ENABLE_BATCH);
   WRITE_BOOL_CONFIG_VAR(SelectorMovementAnimate);
   WRITE_BOOL_CONFIG_VAR(BackgroundMoves);
   char buffer[8];
   WRITE_INT_CONFIG_VAR(CurrentLevel);
   WRITE_STRING_CONFIG_VAR(LevelData);
   ArchiveSetBatchMode(m_Archive, ARCHIVE_DISABLE_BATCH);

   ArchiveFree(&m_Archive);
}

bool Config::BeatLevel(int nLevelNumber, int& nCompletionTime) const
{
   char buffer[8];
   Puz_itoa(nLevelNumber, buffer, 8);

   int nLevelsBeat = GetNumberArchives(m_Archive, "LevelsCompleted");
   char strName[MAX_NAME_LENGTH];
   bool bCompleted = false;
   for(int i=0; i<nLevelsBeat; i++) {
      strcpy(strName, GetName(m_Archive, "LevelsCompleted", i));
      if( strcmp(strName, buffer) == 0 ) {
         if( strcmp("1", GetValue(m_Archive, "LevelsCompleted", i)) == 0 ) {
            bCompleted = true;
            break;
         } else {
            break;
         }
      }
   }

   int nTimeCompletions = GetNumberArchives(m_Archive, "CompletionTime");
   for(int i=0; i<nTimeCompletions; i++) {
      strcpy(strName, GetName(m_Archive, "CompletionTime", i));
      if( strcmp(strName, buffer) == 0 ) {
         nCompletionTime = atoi(GetValue(m_Archive, "CompletionTime", i));
         break;
      }
   }

   return bCompleted;
}

void Config::SetBeatLevel(int nLevelNumber, int nTimeElapsed)
{
   char buffer[8];
   Puz_itoa(nLevelNumber, buffer, 8);
   UpdateArchiveEntry(m_Archive, "LevelsCompleted", buffer, "1", NULL);

   bool bWriteCompletionTime = false;
   int nCompletionTime = -1;
   if( BeatLevel(nLevelNumber, nCompletionTime) ) {
      if( nTimeElapsed < nCompletionTime ) {
         bWriteCompletionTime = true;
      }
   }
   else {
      bWriteCompletionTime = true;
   }

   char buffer2[8];
   Puz_itoa(nTimeElapsed, buffer2, 8);
   UpdateArchiveEntry(m_Archive, "CompletionTime", buffer, buffer2, NULL);
}

