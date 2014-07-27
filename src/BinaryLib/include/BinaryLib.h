#ifndef BINARYLIB_H_INCLUDED
#define BINARYLIB_H_INCLUDED

typedef void* BinaryLib;

#define BINARYLIB_OK			(0)
#define BINARYLIB_BADARGUMENT		(-1)
#define BINARYLIB_OUT_OF_MEMORY		(-2)
#define BINARYLIB_NOT_PUZZLE_LEVEL	(-3)
#define BINARYLIB_UNKNOWN_VERSION	(-4)

#define BINARYLIB_SPOT_PERMANENT	(1)
#define BINARYLIB_SPOT_VARIABLE		(0)

#define BINARYLIB_MULTIPLE_INROW	(1)
#define BINARYLIB_MULTIPLE_INCOL	(2)
#define BINARYLIB_NOT_MULTIPILE_INROW	(0)

#define BINARYLIB_ROW_NOT_UNIQUE	(0)
#define BINARYLIB_ROW_UNIQUE		(1)

#define BINARYLIB_SOLVED		(0)
#define BINARYLIB_NOTSOLVED_EMPTYSPOTS  (-1)
#define BINARYLIB_NOTSOLVED_NONUNIQUEC  (-2)
#define BINARYLIB_NOTSOLVED_NONUNIQUER  (-3)
#define BINARYLIB_NOTSOLVED_3MOREINROW  (-4)

#define BINARYLIB_CANNOT_UNDO		(1)
#define BINARYLIB_CANNOT_REDO		(1)

#define BINARYLIB_EMPTY			(1)
#define BINARYLIB_NOT_EMPTY		(0)

//////////////////////////////////////////////
//Initalization/Error checking/Mode functions
//////////////////////////////////////////////
int BinaryLibCreate(BinaryLib* api, const char* pstrFile );
int BinaryLibFree(BinaryLib* api);

int BinaryLibPersist(BinaryLib api, char** ppstr);
int BinaryLibFreePersist(char* pstr);

int GetBinaryLibError(BinaryLib api);
void ClearBinaryLibError(BinaryLib api);

//////////////////////////////////////////////
//BinaryLib related functions
//////////////////////////////////////////////
int GetBinaryBoardWidth(BinaryLib api);
int GetBinaryBoardHeight(BinaryLib api);
int IsBinarySpotPermanent(BinaryLib api, int nX, int nY);
int GetBinarySpotValue(BinaryLib api, int nX, int nY);
int SetBinarySpotValue(BinaryLib api, int nX, int nY, int nValue);
int IsBinarySpot3InRow(BinaryLib api, int nX, int nY);
int IsBinaryRowDuplicate(BinaryLib api, int nRow);
int IsBinaryColDuplicate(BinaryLib api, int nCol);
int IsBinarySolved(BinaryLib api);
int IsBinaryEmpty(BinaryLib api);
int SetBinaryToSolved(BinaryLib api);
int BinaryUndo(BinaryLib api);
int BinaryRedo(BinaryLib api);
int BinaryAuto(BinaryLib api);

#endif //BINARYLIB_H_INCLUDED
