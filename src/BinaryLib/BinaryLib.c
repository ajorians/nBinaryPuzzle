//Public domain :)

#include <os.h>
#include "include/BinaryLib.h"
#include "Defines.h"

#define SOLVE_PRINT     //printf

struct BinaryItem
{
   int m_nValue;
   int m_nPermanent;
};

struct BinaryBoard
{
   int m_nWidth;
   int m_nHeight;
   struct BinaryItem* m_pItems;
};

struct BinaryItem* GetAt(struct BinaryBoard* pBoard, int nX, int nY)
{
   if( nX < 0 || nY < 0 || (nX >= pBoard->m_nWidth) || (nY >= pBoard->m_nHeight) ) {
      printf("Accessing non-existant element %d,%d\n", nX, nY);
      return NULL;
   }

   return pBoard->m_pItems + (pBoard->m_nWidth * nY + nX);
}

struct BinaryBoard* CopyBoard(struct BinaryBoard* pOrig)
{
   struct BinaryBoard* pBoard = malloc(sizeof(struct BinaryBoard));
   if( pBoard == NULL )
      return NULL;

   pBoard->m_nWidth = pOrig->m_nWidth;
   pBoard->m_nHeight = pOrig->m_nHeight;
   pBoard->m_pItems = malloc(pBoard->m_nWidth*pBoard->m_nHeight*sizeof(struct BinaryItem));
   if( pBoard->m_pItems == NULL ) {//Out of memory
      free(pBoard);
      pBoard = NULL;
      return NULL;
   }
   int nX=0, nY=0;
   for(nX=0; nX<pOrig->m_nWidth; nX++) {
      for(nY=0; nY<pOrig->m_nHeight; nY++) {
         struct BinaryItem* pItemOrig = GetAt(pOrig, nX, nY);
         struct BinaryItem* pItem = GetAt(pBoard, nX, nY);
         pItem->m_nValue = pItemOrig->m_nValue;
         pItem->m_nPermanent = pItemOrig->m_nPermanent;
      }
   }

   return pBoard;
}

void DeleteBoard(struct BinaryBoard* pBoard)
{
   free(pBoard->m_pItems);
   pBoard->m_pItems = NULL;
   free(pBoard);
}

struct BinaryAction
{
   int m_nX;
   int m_nY;
   int m_nValue;
   struct BinaryAction* m_pNext;
};

struct BinarySolution
{
   int m_nX;
   int m_nY;
   int m_nValue;
   struct BinarySolution* m_pNext;
};

struct BinaryPuzzle
{
   struct BinaryBoard* m_pBoard;
   char* m_pstrFile;
   struct BinaryAction* m_pUndoActions;
   struct BinaryAction* m_pRedoActions;
   struct BinarySolution* m_pSolution;
   int m_nLastError;
};

void AddSolutionItem(struct BinaryPuzzle* pT, int nX, int nY, int nValue)
{
   if( nX < 0 || nY < 0 ) return;
   struct BinarySolution* pSol = malloc(sizeof(struct BinarySolution));
   pSol->m_nX = nX;
   pSol->m_nY = nY;
   pSol->m_nValue = nValue;
   pSol->m_pNext = NULL;
   if( pT->m_pSolution == NULL ) {
      pT->m_pSolution = pSol;
   } else {
      struct BinarySolution* pCurrent = pT->m_pSolution;
      while(pCurrent->m_pNext != NULL) {
         pCurrent = pCurrent->m_pNext;
      }
      pCurrent->m_pNext = pSol;
   }
}

void ClearSolution(BinaryLib api)
{
   DEBUG_FUNC_NAME;
   
   struct BinaryPuzzle* pT = (struct BinaryPuzzle*)api;

   struct BinarySolution* pCurrent = pT->m_pSolution;
   while(pCurrent != NULL) {
      struct BinarySolution* pTemp = pCurrent;
      pCurrent = pCurrent->m_pNext;
      free(pTemp);
      pTemp = NULL;
      pT->m_pSolution = pCurrent;
   }
}

int BinaryLoadBoard(BinaryLib api, char* pstrFile)
{
   DEBUG_FUNC_NAME;

   struct BinaryPuzzle* pT = (struct BinaryPuzzle*)api;

   if( pT->m_pBoard != NULL ) {
      free(pT->m_pBoard);
      pT->m_pBoard = NULL;
   }

   pT->m_pBoard = malloc(sizeof(struct BinaryBoard));
   if( pT->m_pBoard == NULL ){//Out of memory
      free(pT);
      pT = NULL;
      return BINARYLIB_OUT_OF_MEMORY;
   }
   pT->m_pBoard->m_pItems = NULL;

   if( strstr(pstrFile, "BinaryPuzzle ") != pstrFile ) {//Binary puzzle file version check
      free(pT->m_pBoard);
      pT->m_pBoard = NULL;
      free(pT);
      pT = NULL;
      return BINARYLIB_NOT_PUZZLE_LEVEL;
   }

   if( strstr(pstrFile, "1 ") != (pstrFile + strlen("BinaryPuzzle ")) ) {//Version check
      free(pT->m_pBoard);
      pT->m_pBoard = NULL;
      free(pT);
      pT = NULL;
      return BINARYLIB_UNKNOWN_VERSION;
   }

   char* pstr = pstrFile + strlen("BinaryPuzzle 1 ");
   char buffer[16];
   int nSpotInBuffer = 0;

   int nWidth = -1, nHeight = -1;
   int nX = -1, nY = -1, nValue = -1;
   while(pstr != '\0') {
      char ch = *pstr; pstr++;
      if( isdigit(ch) ) {
         buffer[nSpotInBuffer++] = ch;
      }
      else {
         if( !isspace(ch) )
            break;
         buffer[nSpotInBuffer] = '\0';
         nSpotInBuffer = 0;
         nValue = atoi(buffer);

         if( nWidth < 0 ) {
            nWidth = nValue;
            pT->m_pBoard->m_nWidth = nWidth;
         }
         else if( nHeight < 0 ) {
            nHeight = nValue;
            pT->m_pBoard->m_nHeight = nHeight;
            pT->m_pBoard->m_pItems = malloc(nWidth*nHeight*sizeof(struct BinaryItem));

            if( pT->m_pBoard->m_pItems == NULL ) {//Out of memory
               free(pT->m_pBoard);
               pT->m_pBoard = NULL;
               free(pT);
               pT = NULL;
               return BINARYLIB_OUT_OF_MEMORY;
            }

            int x,y;
            for(x=0; x<nWidth; x++)
               for(y=0; y<nHeight; y++) {
                  struct BinaryItem* pSpot = GetAt(pT->m_pBoard, x, y);
		  pSpot->m_nValue = -1;
                  pSpot->m_nPermanent = 0;
		}

         }
         else {
            if( nX <= -1 ) {
               nX = nValue;
            }
            else if( nY <= -1 ) {
               nY = nValue;
            }
            else {
               struct BinaryItem* pSpot = GetAt(pT->m_pBoard, nX, nY);
               pSpot->m_nValue = nValue;
               pSpot->m_nPermanent = 1;

               nX = nY = -1;
            }

            //Maybe check if has more numbers than it should?
         }
      }
   }
   if( nX >= 0 && nY >= 0 ) {
      buffer[nSpotInBuffer] = '\0';
      int nValue = atoi(buffer);
      struct BinaryItem* pSpot = GetAt(pT->m_pBoard, nX, nY);
      pSpot->m_nValue = nValue;
      pSpot->m_nPermanent = 1;
   }

   pT->m_pSolution = NULL;
   pstr = strstr(pstrFile, "Solution ");
   if( pstr == NULL ) {
      //Should never happen!
   }
   pstr = pstr + strlen("Solution ");
   int nSolX = 0, nSolY = 0, nSolVal = -1;
   while(pstr != '\0') {
      char ch = *pstr; pstr++;
      if( isdigit(ch) ) {
         buffer[nSpotInBuffer++] = ch;
      }
      else {
         if( !isspace(ch) || ch == '\r' || ch == '\n' || ch == '\0' )
            break;
         buffer[nSpotInBuffer] = '\0';
         nSpotInBuffer = 0;
         int nValue = atoi(buffer);

         {
            nSolVal = nValue;
            AddSolutionItem(pT, nSolX, nSolY, nSolVal);
            nSolY++;
            if( nSolY >= nHeight ) {
               nSolX++; nSolY = 0;
            }
         }
      }
   }
   buffer[nSpotInBuffer] = '\0';
   nValue = atoi(buffer);
   nSolVal = nValue;
   AddSolutionItem(pT, nSolX, nSolY, nSolVal);

   pstr = strstr(pstrFile, "Progress ");
   if( pstr != NULL ) {
      pstr = pstr + strlen("Progress ");
      int nProX = -1, nProY = -1, nProVal = -1;
      while(pstr != '\0') {
         char ch = *pstr; pstr++;
         if( isdigit(ch) ) {
            buffer[nSpotInBuffer++] = ch;
         }
         else {
            if( !isspace(ch) || ch == '\r' || ch == '\n' || ch == '\0' )
               break;
            buffer[nSpotInBuffer] = '\0';
            nSpotInBuffer = 0;
            int nValue = atoi(buffer);

            if( nProX < 0 ) {
               nProX = nValue;
            }
            else if( nProY < 0 ) {
               nProY = nValue;
            } else {
               nProVal = nValue;
               struct BinaryItem* pSpot = GetAt(pT->m_pBoard, nProX, nProY);
               pSpot->m_nValue = nProVal;
               pSpot->m_nPermanent = 0;
               nProX = -1;
               nProY = -1;
               nProVal = -1;
            }
         }
      }
      buffer[nSpotInBuffer] = '\0';
      nValue = atoi(buffer);
      nProVal = nValue;
      struct BinaryItem* pSpot = GetAt(pT->m_pBoard, nProX, nProY);
      pSpot->m_nValue = nProVal;
      pSpot->m_nPermanent = 0;
   }

   return BINARYLIB_OK;
}

int BinaryLibCreate(BinaryLib* api, const char* pstrFile)
{
   DEBUG_FUNC_NAME;

   struct BinaryPuzzle* pT = malloc(sizeof(struct BinaryPuzzle));
   if( pT == NULL ){//Out of memory
      return BINARYLIB_OUT_OF_MEMORY;
   }

   pT->m_pstrFile = pstrFile;
   pT->m_pBoard = NULL;

   int nRet = BinaryLoadBoard((BinaryLib)pT, pstrFile);
   if( nRet != BINARYLIB_OK )
      return nRet;

   pT->m_pUndoActions = NULL;
   pT->m_pRedoActions = NULL;
   pT->m_nLastError = BINARYLIB_OK;

   *api = pT;

   return BINARYLIB_OK;
}

void ClearUndos(BinaryLib api)
{
   DEBUG_FUNC_NAME;

   struct BinaryPuzzle* pT = (struct BinaryPuzzle*)api;

   struct BinaryAction* pCurrent = pT->m_pUndoActions;
   while(pCurrent != NULL) {
      struct BinaryAction* pTemp = pCurrent;
      pCurrent = pCurrent->m_pNext;
      free(pTemp);
      pTemp = NULL;
      pT->m_pUndoActions = pCurrent;
   }

}

void ClearRedos(BinaryLib api)
{
   DEBUG_FUNC_NAME;

   struct BinaryPuzzle* pT = (struct BinaryPuzzle*)api;

   struct BinaryAction* pCurrent = pT->m_pRedoActions;
   while(pCurrent != NULL) {
      struct BinaryAction* pTemp = pCurrent;
      pCurrent = pCurrent->m_pNext;
      free(pTemp);
      pTemp = NULL;
      pT->m_pRedoActions = pCurrent;
   }

}

void AddUndo(BinaryLib api, int nX, int nY, int nValue)
{
   DEBUG_FUNC_NAME;

   struct BinaryPuzzle* pT = (struct BinaryPuzzle*)api;

   struct BinaryAction* pAction = malloc(sizeof(struct BinaryAction));
   if( pAction == NULL ) {
      DEBUG_MSG("Out of memory: AddUndo\n");
   //Assume allocated
   }

   pAction->m_nX = nX;
   pAction->m_nY = nY;
   pAction->m_nValue = nValue;

   struct BinaryAction* pRoot = pT->m_pUndoActions;
   pAction->m_pNext = pRoot;
   pT->m_pUndoActions = pAction;
}

void AddRedo(BinaryLib api, int nX, int nY, int nValue)
{
   DEBUG_FUNC_NAME;

   struct BinaryPuzzle* pT = (struct BinaryPuzzle*)api;

   struct BinaryAction* pAction = malloc(sizeof(struct BinaryAction));
   if( pAction == NULL ) {
      DEBUG_MSG("Out of memory: AddRedo\n");
      //Assume allocated
   }

   pAction->m_nX = nX;
   pAction->m_nY = nY;
   pAction->m_nValue = nValue;

   struct BinaryAction* pRoot = pT->m_pRedoActions;
   pAction->m_pNext = pRoot;
   pT->m_pRedoActions = pAction;
}

int BinaryLibFree(BinaryLib* api)
{
   DEBUG_FUNC_NAME;

   struct BinaryPuzzle* pT = *api;

   ClearUndos(*api);
   ClearRedos(*api);
   ClearSolution(*api);

   DeleteBoard(pT->m_pBoard);
   pT->m_pBoard = NULL;
   free(pT);
   pT = NULL;

   *api = NULL;
   return BINARYLIB_OK;
}

int BinaryLibPersist(BinaryLib api, char** ppstr)
{
   DEBUG_FUNC_NAME;

   struct BinaryPuzzle* pT = (struct BinaryPuzzle*)api;

   *ppstr = (char*)malloc(2048);
   if( *ppstr == NULL )
      return BINARYLIB_OUT_OF_MEMORY;

   char* pstr = *ppstr;

   char buffer[16];

   strcpy(pstr, "BinaryPuzzle 1 ");
   sprintf(buffer, "%d ", pT->m_pBoard->m_nWidth);
   strcat(pstr, buffer);
   sprintf(buffer, "%d ", pT->m_pBoard->m_nHeight);
   strcat(pstr, buffer);

   int x=0, y=0;
   for(x=0; x<GetBinaryBoardWidth(api); x++) {
      for(y=0; y<GetBinaryBoardHeight(api); y++) {
         if( IsBinarySpotPermanent(api, x, y) == BINARYLIB_SPOT_PERMANENT ) {
            sprintf(buffer, "%d ", x);
            strcat(pstr, buffer);
            sprintf(buffer, "%d ", y);
            strcat(pstr, buffer);
            sprintf(buffer, "%d ", GetBinarySpotValue(api, x, y));
            strcat(pstr, buffer);
         }
      }
   }

   strcat(pstr, "Solution ");
   struct BinarySolution* pCurrent = pT->m_pSolution;
   while(pCurrent != NULL) {
      sprintf(buffer, "%d ", pCurrent->m_nValue);
      strcat(pstr, buffer);
      pCurrent = pCurrent->m_pNext;
   }

   strcat(pstr, "Progress ");
   for(x=0; x<GetBinaryBoardWidth(api); x++) {
      for(y=0; y<GetBinaryBoardHeight(api); y++) {
         if( IsBinarySpotPermanent(api, x, y) != BINARYLIB_SPOT_PERMANENT ) {//This is just for efficency
            if( GetBinarySpotValue(api, x, y) >= 0 ) {
               sprintf(buffer, "%d ", x);
               strcat(pstr, buffer);
               sprintf(buffer, "%d ", y);
               strcat(pstr, buffer);
               sprintf(buffer, "%d ", GetBinarySpotValue(api, x, y));
               strcat(pstr, buffer);
            }
         }
      }
   }
   strcat(pstr, "\0");

   return BINARYLIB_OK;
}

int BinaryLibFreePersist(char* pstr)
{
   DEBUG_FUNC_NAME;

   free(pstr);
   return BINARYLIB_OK;
}

int GetBinaryLibError(BinaryLib api)
{
   DEBUG_FUNC_NAME;

   struct BinaryPuzzle* pT = (struct BinaryPuzzle*)api;
   return pT->m_nLastError;
}

void ClearBinaryLibError(BinaryLib api)
{
   DEBUG_FUNC_NAME;

   struct BinaryPuzzle* pT = (struct BinaryPuzzle*)api;
   pT->m_nLastError = BINARYLIB_OK;
}

//BinaryLib related functions
int GetBoardWidth(struct BinaryBoard* pBoard)
{
   return pBoard->m_nWidth;
}

int GetBinaryBoardWidth(BinaryLib api)
{
   DEBUG_FUNC_NAME;

   struct BinaryPuzzle* pT = (struct BinaryPuzzle*)api;

   return GetBoardWidth(pT->m_pBoard);
}

int GetBoardHeight(struct BinaryBoard* pBoard)
{
   return pBoard->m_nHeight;
}

int GetBinaryBoardHeight(BinaryLib api)
{
   DEBUG_FUNC_NAME;

   struct BinaryPuzzle* pT = (struct BinaryPuzzle*)api;

   return GetBoardHeight(pT->m_pBoard);
}

int IsBinarySpotPermanent(BinaryLib api, int nX, int nY)
{
   DEBUG_FUNC_NAME;

   struct BinaryPuzzle* pT = (struct BinaryPuzzle*)api;

   struct BinaryItem* pSpot = GetAt(pT->m_pBoard, nX, nY);
   return pSpot->m_nPermanent == 1 ? BINARYLIB_SPOT_PERMANENT : BINARYLIB_SPOT_VARIABLE;
}

int GetBoardSpotValue(struct BinaryBoard* pBoard, int nX, int nY)
{
   return GetAt(pBoard, nX, nY)->m_nValue;
}

int GetBinarySpotValue(BinaryLib api, int nX, int nY)
{
   DEBUG_FUNC_NAME;

   struct BinaryPuzzle* pT = (struct BinaryPuzzle*)api;

   return GetBoardSpotValue(pT->m_pBoard, nX, nY);
}

void SetBoardSpotValue(struct BinaryBoard* pBoard, int nX, int nY, int nValue)
{
   GetAt(pBoard, nX, nY)->m_nValue = nValue;
}

int SetBinarySpotValue(BinaryLib api, int nX, int nY, int nValue)
{
   DEBUG_FUNC_NAME;

   if( IsBinarySpotPermanent(api, nX, nY) == BINARYLIB_SPOT_PERMANENT )
      return BINARYLIB_OK;//Could use a different return value

   struct BinaryPuzzle* pT = (struct BinaryPuzzle*)api;

   ClearRedos(api);

   int nOldValue = GetAt(pT->m_pBoard, nX, nY)->m_nValue;
   AddUndo(api, nX, nY, nOldValue);
   SetBoardSpotValue(pT->m_pBoard, nX, nY, nValue);

   return BINARYLIB_OK;
}

int Is3InRow(struct BinaryBoard* pBoard, int nX, int nY)
{
   int nValue = GetBoardSpotValue(pBoard, nX, nY);
   if( nValue < 0 )
      return BINARYLIB_NOT_MULTIPILE_INROW;

   int nCount = 0;

   int x;
   for(x=nX-2; x<=(nX + 2); x++) {
      if( (x < 0) || (x >= GetBoardWidth(pBoard)) )
         continue;
      int val = GetBoardSpotValue(pBoard, x, nY);
      if( val < 0 ) {
         nCount = 0;
         continue;
      }
      if( nValue == val ) {
         nCount++;
      } else {
         nCount = 0;
         continue;
      }
      if( nCount >= 3 )
         return BINARYLIB_MULTIPLE_INROW;
   }

   nCount = 0;

   int y;
   for(y=nY-2; y<=(nY + 2); y++) {
      if( (y < 0) || (y >= GetBoardHeight(pBoard)) )
         continue;
      int val = GetBoardSpotValue(pBoard, nX, y);
      if( val < 0 ) {
         nCount = 0;
         continue;
      }
      if( nValue == val ) {
         nCount++;
      } else {
         nCount = 0;
         continue;
      }
      if( nCount >= 3 )
         return BINARYLIB_MULTIPLE_INCOL;
   }

   return BINARYLIB_NOT_MULTIPILE_INROW;
}

int IsBinarySpot3InRow(BinaryLib api, int nX, int nY)
{
   DEBUG_FUNC_NAME;

   struct BinaryPuzzle* pT = (struct BinaryPuzzle*)api;

   return Is3InRow(pT->m_pBoard, nX, nY);
}

int IsRowDuplicate(struct BinaryBoard* pBoard, int nRow)
{
   int nY;
   for(nY=0; nY<GetBoardHeight(pBoard); nY++) {
      if( nY == nRow )
         continue;
      int nX;
      int nSame = 1;
      for(nX=0; nX<GetBoardWidth(pBoard); nX++) {
         int nRowVal = GetBoardSpotValue(pBoard, nX, nRow);
         if( nRowVal < 0 )
            return BINARYLIB_ROW_UNIQUE;

         int nVal = GetBoardSpotValue(pBoard, nX, nY);
         if( (nVal < 0) || (nRowVal != nVal) ) {
            nSame = 0;
            break;
         }
      }

      if( nSame == 1 )
         return BINARYLIB_ROW_NOT_UNIQUE;
   }

   return BINARYLIB_ROW_UNIQUE;
}

int IsBinaryRowDuplicate(BinaryLib api, int nRow)
{
   DEBUG_FUNC_NAME;

   struct BinaryPuzzle* pT = (struct BinaryPuzzle*)api;

   return IsRowDuplicate(pT->m_pBoard, nRow);
}

int IsColDuplicate(struct BinaryBoard* pBoard, int nCol)
{
   int nX;
   for(nX=0; nX<GetBoardWidth(pBoard); nX++) {
      if( nX == nCol )
         continue;
      int nY;
      int nSame = 1;
      for(nY=0; nY<GetBoardHeight(pBoard); nY++) {
         int nColVal = GetBoardSpotValue(pBoard, nCol, nY);
         if( nColVal < 0 )
            return BINARYLIB_ROW_UNIQUE;

         int nVal = GetBoardSpotValue(pBoard, nX, nY);
         if( (nVal < 0) || (nColVal != nVal) ) {
            nSame = 0;
            break;
         }
      }

      if( nSame == 1 )
         return BINARYLIB_ROW_NOT_UNIQUE;
   }

   return BINARYLIB_ROW_UNIQUE;
}

int IsBinaryColDuplicate(BinaryLib api, int nCol)
{
   DEBUG_FUNC_NAME;

   struct BinaryPuzzle* pT = (struct BinaryPuzzle*)api;

   return IsColDuplicate(pT->m_pBoard, nCol);
}

int IsBroken(struct BinaryBoard* pBoard)
{
   int nX = 0, nY = 0;
   for(nX = 0; nX<GetBoardWidth(pBoard); nX++)
      for(nY = 0; nY<GetBoardHeight(pBoard); nY++) {
         if( Is3InRow(pBoard, nX, nY ) != BINARYLIB_NOT_MULTIPILE_INROW ) {
            SOLVE_PRINT("3 in row\n");
            return BINARYLIB_NOTSOLVED_3MOREINROW;
         }
      }

   for(nX = 0; nX<GetBoardWidth(pBoard); nX++)
      if( IsColDuplicate(pBoard, nX) == BINARYLIB_ROW_NOT_UNIQUE ) {
         SOLVE_PRINT("Column is duplicate\n");
         return BINARYLIB_NOTSOLVED_NONUNIQUEC;
      }

   for(nY = 0; nY<GetBoardHeight(pBoard); nY++)
      if( IsRowDuplicate(pBoard, nY) == BINARYLIB_ROW_NOT_UNIQUE ) {
         SOLVE_PRINT("Row is duplicate\n");
         return BINARYLIB_NOTSOLVED_NONUNIQUER;
      }

   return 0;
}

int IsBinarySolved(BinaryLib api)
{
   DEBUG_FUNC_NAME;

   struct BinaryPuzzle* pT = (struct BinaryPuzzle*)api;

   int nX = 0, nY = 0;
   for(nX = 0; nX<GetBinaryBoardWidth(api); nX++)
      for(nY = 0; nY<GetBinaryBoardHeight(api); nY++) {
         if( GetBinarySpotValue(api, nX, nY) < 0 )
            return BINARYLIB_NOTSOLVED_EMPTYSPOTS;
      }

   int nRet = IsBroken(pT->m_pBoard);
   return nRet == 0 ? BINARYLIB_SOLVED : nRet;
}

int GetNumEmptySpots(BinaryLib api)
{
   int nCount = 0;
   struct BinaryPuzzle* pT = (struct BinaryPuzzle*)api;

   int nX = 0, nY = 0;
   for(nX = 0; nX<GetBinaryBoardWidth(api); nX++)
      for(nY = 0; nY<GetBinaryBoardHeight(api); nY++)
         if( GetBinarySpotValue(api, nX, nY) < 0 )
            nCount++;

   return nCount;
}

int IsBinaryEmpty(BinaryLib api)
{
   DEBUG_FUNC_NAME;

   int nX = 0, nY = 0;
   for(nX = 0; nX<GetBinaryBoardWidth(api); nX++)
      for(nY = 0; nY<GetBinaryBoardHeight(api); nY++)
         if( GetBinarySpotValue(api, nX, nY) >=0 && IsBinarySpotPermanent(api, nX, nY) == BINARYLIB_SPOT_VARIABLE )
            return BINARYLIB_NOT_EMPTY;

   return BINARYLIB_EMPTY;
}

int SetBinaryToSolved(BinaryLib api)
{
   DEBUG_FUNC_NAME;

   struct BinaryPuzzle* pT = (struct BinaryPuzzle*)api;
   struct BinaryBoard* pBoard = pT->m_pBoard;

   struct BinarySolution* pCurrent = pT->m_pSolution;
   while(pCurrent != NULL) {
      GetAt(pT->m_pBoard, pCurrent->m_nX, pCurrent->m_nY)->m_nValue = pCurrent->m_nValue;
      pCurrent = pCurrent->m_pNext;
   }

   return BINARYLIB_OK;
}

int BinaryUndo(BinaryLib api)
{
   DEBUG_FUNC_NAME;

   struct BinaryPuzzle* pT = (struct BinaryPuzzle*)api;
   struct BinaryBoard* pBoard = pT->m_pBoard;

   struct BinaryAction* pRoot = pT->m_pUndoActions;
   if( pRoot == NULL )
      return BINARYLIB_CANNOT_UNDO;

   pT->m_pUndoActions = pRoot->m_pNext;
   AddRedo(api, pRoot->m_nX, pRoot->m_nY, GetBinarySpotValue(api, pRoot->m_nX, pRoot->m_nY));
   GetAt(pBoard, pRoot->m_nX, pRoot->m_nY)->m_nValue = pRoot->m_nValue;

   free(pRoot);
   pRoot = NULL;

   return BINARYLIB_OK;
}

int BinaryRedo(BinaryLib api)
{
   DEBUG_FUNC_NAME;

   struct BinaryPuzzle* pT = (struct BinaryPuzzle*)api;
   struct BinaryBoard* pBoard = pT->m_pBoard;

   struct BinaryAction* pRoot = pT->m_pRedoActions;
   if( pRoot == NULL )
      return BINARYLIB_CANNOT_REDO;

   pT->m_pRedoActions = pRoot->m_pNext;
   AddUndo(api, pRoot->m_nX, pRoot->m_nY, GetBinarySpotValue(api, pRoot->m_nX, pRoot->m_nY));
   GetAt(pBoard, pRoot->m_nX, pRoot->m_nY)->m_nValue = pRoot->m_nValue;

   free(pRoot);
   pRoot = NULL;

   return BINARYLIB_OK;
}

#define BROKEN			(-1)
#define ALLOCATE_PROBLEM	(-2)
#define REACHED_RECURSIVE_MAX	(-3)
#define NO_MOVE_FOUND		(-4)

int RecursiveCheckBoard(struct BinaryBoard* pBoard, int nLevel, int nLevelMax)
{
   SOLVE_PRINT("Doing an auto with board %p\n", pBoard);
   //Check if in a bad state!  If so return bad!
   if( IsBroken(pBoard) != 0 ) {
      SOLVE_PRINT("Is Broken :(\n");
      return BROKEN;
   }

   if( nLevel >= nLevelMax )
      return REACHED_RECURSIVE_MAX;

   int nX, nY;
   for(nX=0; nX<GetBoardWidth(pBoard); nX++) {
      for(nY=0; nY<GetBoardHeight(pBoard); nY++) {
         if( GetBoardSpotValue(pBoard, nX, nY) >= 0 )
            continue;

         if( nX > 1 ) {
            int nLeft2 = GetBoardSpotValue(pBoard, nX-2, nY);
            int nLeft1 = GetBoardSpotValue(pBoard, nX-1, nY);
            if( nLeft2 >=0 && nLeft1 >= 0 && (nLeft2 == nLeft1) ) {
               SetBoardSpotValue(pBoard, nX, nY, nLeft1 == 0 ? 1 : 0);
               SOLVE_PRINT("Left2 are the same\n");
               return BINARYLIB_OK;
            }
         }

         if( nY > 1 ) {
            int nUp2 = GetBoardSpotValue(pBoard, nX, nY-2);
            int nUp1 = GetBoardSpotValue(pBoard, nX, nY-1);
            if( nUp2 >=0 && nUp1 >= 0 && (nUp2 == nUp1) ) {
               SetBoardSpotValue(pBoard, nX, nY, nUp1 == 0 ? 1 : 0);
               SOLVE_PRINT("Upper 2 are the same\n");
               return BINARYLIB_OK;
            }
         }

         if( nX > 0 && nX < (GetBoardWidth(pBoard)-1) ) {
            int nLeft1 = GetBoardSpotValue(pBoard, nX-1, nY);
            int nRight1 = GetBoardSpotValue(pBoard, nX+1, nY);
            if( nLeft1 >=0 && nRight1 >= 0 && (nLeft1 == nRight1) ) {
               SetBoardSpotValue(pBoard, nX, nY, nLeft1 == 0 ? 1 : 0);
               SOLVE_PRINT("Left and right are the same\n");
               return BINARYLIB_OK;
            }
         }

         if( nY > 0 && nY < (GetBoardHeight(pBoard)-1) ) {
            int nUp1 = GetBoardSpotValue(pBoard, nX, nY-1);
            int nDown1 = GetBoardSpotValue(pBoard, nX, nY+1);
            if( nUp1 >=0 && nDown1 >= 0 && (nUp1 == nDown1) ) {
               SetBoardSpotValue(pBoard, nX, nY, nUp1 == 0 ? 1 : 0);
               SOLVE_PRINT("Up and down are the same\n");
               return BINARYLIB_OK;
            }
         }

         if( nX < (GetBoardWidth(pBoard)-2) ) {
            int nRight2 = GetBoardSpotValue(pBoard, nX+2, nY);
            int nRight1 = GetBoardSpotValue(pBoard, nX+1, nY);
            if( nRight2 >=0 && nRight1 >= 0 && (nRight2 == nRight1) ) {
               SetBoardSpotValue(pBoard, nX, nY, nRight1 == 0 ? 1 : 0);
               SOLVE_PRINT("Right2 are the same\n");
               return BINARYLIB_OK;
            }
         }

         if( nY < (GetBoardHeight(pBoard)-2) ) {
            int nDown2 = GetBoardSpotValue(pBoard, nX, nY+2);
            int nDown1 = GetBoardSpotValue(pBoard, nX, nY+1);
            if( nDown2 >=0 && nDown1 >= 0 && (nDown2 == nDown1) ) {
               SetBoardSpotValue(pBoard, nX, nY, nDown1 == 0 ? 1 : 0);
               SOLVE_PRINT("Down2 are the same\n");
               return BINARYLIB_OK;
            }
         }
      }
   }

   //This here checks for 1 missing value on a given row/column
   for(nX=0; nX<GetBoardWidth(pBoard); nX++) {
      int nCount = 0;
      int x=0, y=0, n=0;
      for(nY=0; nY<GetBoardHeight(pBoard); nY++) {
         int nVal = GetBoardSpotValue(pBoard, nX, nY);
         if( nVal >= 0 ) {
            nCount++;
            if( nVal==0 ) n++;
            else if( nVal==1) n--;
         } else {
            x = nX; y = nY;
         }
      }
      if( nCount == (GetBoardHeight(pBoard)-1) ) {
         SetBoardSpotValue(pBoard, x, y, n<0 ? 0 : 1);
         SOLVE_PRINT("Only 1 missing value in column\n");
         return BINARYLIB_OK;
      }
   }

   for(nY=0; nY<GetBoardHeight(pBoard); nY++) {
      int nCount = 0;
      int x=0, y=0, n=0;
      for(nX=0; nX<GetBoardWidth(pBoard); nX++) {
         int nVal = GetBoardSpotValue(pBoard, nX, nY);
         if( nVal >= 0 ) {
            nCount++;
            if( nVal==0 ) n++;
            else if( nVal==1) n--;
         } else {
            x = nX; y = nY;
         }
      }
      if( nCount == (GetBoardWidth(pBoard)-1) ) {
         SetBoardSpotValue(pBoard, x, y, n<0 ? 0 : 1);
         SOLVE_PRINT("Only 1 missing value in row\n");
         return BINARYLIB_OK;
      }
   }

   //This here checks if all 0s or all 1s for a row/column are met and will supply in the missing values
    for(nX=0; nX<GetBoardWidth(pBoard); nX++) {
      int nCount = 0, n0s = 0, n1s = 0;
      for(nY=0; nY<GetBoardHeight(pBoard); nY++) {
         int nVal = GetBoardSpotValue(pBoard, nX, nY);
         if( nVal >= 0 ) {
            if( nVal == 0 ) n0s++;
            else if( nVal == 1 ) n1s++;
         } else {
            nCount++;
         }
      }
      if( nCount > 0 ) {//There is a gap
         if( n0s == (GetBoardHeight(pBoard)/2) ) {
            for(nY=0; nY<GetBoardHeight(pBoard); nY++) {
               if( GetBoardSpotValue(pBoard, nX, nY) < 0 ) {
                  SetBoardSpotValue(pBoard, nX, nY, 1);
                  SOLVE_PRINT("Filled in missing 1 in Col\n");
                  return BINARYLIB_OK;
               }
            }
         }
         if( n1s == (GetBoardHeight(pBoard)/2) ) {
            for(nY=0; nY<GetBoardHeight(pBoard); nY++) {
               if( GetBoardSpotValue(pBoard, nX, nY) < 0 ) {
                  SetBoardSpotValue(pBoard, nX, nY, 0);
                  SOLVE_PRINT("Filled in missing 0 in Col\n");
                  return BINARYLIB_OK;
               }
            }
         }
      }
   }

   for(nY=0; nY<GetBoardHeight(pBoard); nY++) {
      int nCount = 0, n0s = 0, n1s = 0;
      for(nX=0; nX<GetBoardWidth(pBoard); nX++) {
         int nVal = GetBoardSpotValue(pBoard, nX, nY);
         if( nVal >= 0 ) {
            if( nVal == 0 ) n0s++;
            else if( nVal == 1 ) n1s++;
         } else {
            nCount++;
         }
      }
      if( nCount > 0 ) {//There is a gap
         if( n1s == (GetBoardWidth(pBoard)/2) ) {
            for(nX=0; nX<GetBoardWidth(pBoard); nX++) {
               if( GetBoardSpotValue(pBoard, nX, nY) < 0 ) {
                  SetBoardSpotValue(pBoard, nX, nY, 0);
                  SOLVE_PRINT("Filled in missing 0 in row\n");
                  return BINARYLIB_OK;
               }
            }
         }
         if( n0s == (GetBoardWidth(pBoard)/2) ) {
            for(nX=0; nX<GetBoardWidth(pBoard); nX++) {
               if( GetBoardSpotValue(pBoard, nX, nY) < 0 ) {
                  SetBoardSpotValue(pBoard, nX, nY, 1);
                  SOLVE_PRINT("Filled in missing 1 in row\n");
                  return BINARYLIB_OK;
               }
            }
         }
      }
   }

   //Calls recursive to see if reaches fails.
   struct BinaryBoard* pBoardCopy = CopyBoard(pBoard);
   if( pBoard == NULL ) {
      printf("Memory issue\n");
      return ALLOCATE_PROBLEM;
   }
   for(nX=0; nX<GetBoardWidth(pBoardCopy); nX++) {
      for(nY=0; nY<GetBoardHeight(pBoardCopy); nY++) {
         if( GetBoardSpotValue(pBoardCopy, nX, nY) < 0 ) {
            SetBoardSpotValue(pBoardCopy, nX, nY, 0);
            int nCheck = BINARYLIB_OK;
            while(nCheck == BINARYLIB_OK) {
               nCheck = RecursiveCheckBoard(pBoardCopy, nLevel+1, nLevelMax);
            }
            if( nCheck == BROKEN ) {
               SetBoardSpotValue(pBoard, nX, nY, 1);
               SOLVE_PRINT("Supplied spot from recusive\n");
               DeleteBoard(pBoardCopy);
               return BINARYLIB_OK;
            } else {
               DeleteBoard(pBoardCopy);
               pBoardCopy = CopyBoard(pBoard);
               SOLVE_PRINT("Going to see if 1 is better test value\n");
               SetBoardSpotValue(pBoardCopy, nX, nY, 1);
               nCheck = BINARYLIB_OK;
               while(nCheck == BINARYLIB_OK) {
                  nCheck = RecursiveCheckBoard(pBoardCopy, nLevel+1, nLevelMax);
               }
               if( nCheck == BROKEN ) {
                  SetBoardSpotValue(pBoard, nX, nY, 0);
                  SOLVE_PRINT("Supplied spot (0) from recusive\n");
                  DeleteBoard(pBoardCopy);
                  return BINARYLIB_OK;
               } else {
                  DeleteBoard(pBoardCopy);
                  pBoardCopy = CopyBoard(pBoard);
                  SOLVE_PRINT("Going to find another spot to test with\n");
                  continue;
               }
            }
         }
      }
   }
   DeleteBoard(pBoardCopy);

   //If I get here means I wasn't able to find something to do
   SOLVE_PRINT("Wasn't able to find a move!\n");
   return NO_MOVE_FOUND;
}

int CheckIfTooMuchCalculations(int nLevel, int nEmptySpots)
{
   if( nLevel == 3 && nEmptySpots > 30 )
      return 1;
   if( nLevel >=4 && nEmptySpots > 20 )
      return 1;
   return 0;
}

int BinaryAuto(BinaryLib api)
{
   DEBUG_FUNC_NAME;

   struct BinaryPuzzle* pB = (struct BinaryPuzzle*)api;
   struct BinaryBoard* pBoard = pB->m_pBoard;

   const int nLevelMax = 5;
   int nRes;
   for(int i=1; i<=nLevelMax; i++) {
      printf("Checking recursive level %d\n", i);
      if( CheckIfTooMuchCalculations(i, GetNumEmptySpots(api)) == 1 ) {
         printf("Skipping as could take too long\n");
         continue;
      }
      nRes = RecursiveCheckBoard(pBoard, 0, i);
      if( nRes == REACHED_RECURSIVE_MAX || nRes == NO_MOVE_FOUND ) {
         continue;
      } else {
         break;
      }
   }

   printf("--Done\n");

   return nRes;
}
