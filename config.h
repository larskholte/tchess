//
// config.h
//

#ifndef config_h
#define config_h

#define StandardCapacity 40
#define StandardDepth 2
#define MinimumDepth 4 // plies
#define MaxNumNodes 100000
#define MaxNumTerminals 30
#define WhiteMate 8192
#define BlackMate (-WhiteMate)
#define Stalemate 0
#define PredictionSpread 120.0
#define MaxMovesWithoutPawnMove 50
#define DrawLevel 20

#define PawnWeight 1
#define KnightWeight 3
#define BishopWeight 3
#define RookWeight 5
#define QueenWeight 9

#define PiecesWeight 20
#define PressureWeight 2
#define CoverageWeight 2
#define ProtectionWeight 1

#endif // config_h
