//
// board.h
//

#ifndef board_h
#define board_h

#include <inttypes.h>
#include <climits>
#include <atomic>

using namespace std;

//
// integers
//
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
int CountBitsU16(u16);
int CountBitsU64(u64);

//
// node termination status
//
#define TSNotGrown 0
#define TSNotTerminal 1
#define TSWhiteMate 2
#define TSBlackMate 3
#define TSStaleMate 4

//
// mating sequence
//
typedef int ms;
#define MSNone 0x00
#define MSWhiteR 0x01
#define MSWhiteQ 0x02
#define MSBlackR 0x81
#define MSBlackQ 0x82

//
// square
//
typedef u8 square;
#define sq_a1 7
#define sq_a2 15
#define sq_a6 47
#define sq_a8 63
#define sq_b1 6
#define sq_b8 62
#define sq_c1 5
#define sq_c8 61
#define sq_d1 4
#define sq_d8 60
#define sq_f1 2
#define sq_f8 58
#define sq_g1 1
#define sq_g8 57
#define sq_h1 0
#define sq_h3 16
#define sq_h7 48
#define sq_h8 56
#define TakenMask 0x80
#define TakesMask TakenMask
#define PromotionMask 0x40
#define nosquare 64
square SquareWithFileAndRankChars(char file, char rank);
u8 FileIdxForSquare(square);
u8 RankIdxForSquare(square);
char FileCharForSquare(square);
char RankCharForSquare(square);
bool HasSquareAbove(square);
square SquareAbove(square);
bool HasSquareBelow(square);
square SquareBelow(square);
bool HasSquareRight(square);
square SquareRight(square);
bool HasSquareLeft(square);
square SquareLeft(square);
bool HasSquareAboveRight(square);
square SquareAboveRight(square);
bool HasSquareAboveLeft(square);
square SquareAboveLeft(square);
bool HasSquareBelowRight(square);
square SquareBelowRight(square);
bool HasSquareBelowLeft(square);
square SquareBelowLeft(square);
bool HasSquareUUR(square);
square SquareUUR(square);
bool HasSquareUUL(square);
square SquareUUL(square);
bool HasSquareRRU(square);
square SquareRRU(square);
bool HasSquareRRD(square);
square SquareRRD(square);
bool HasSquareLLU(square);
square SquareLLU(square);
bool HasSquareLLD(square);
square SquareLLD(square);
bool HasSquareDDR(square);
square SquareDDR(square);
bool HasSquareDDL(square);
square SquareDDL(square);
int StepsToCorner(square);

//
// color
//
typedef u8 color;
#define white 0
#define black 1
#define emptycolor 2
color OppositeColor(color);

//
// piece
//
typedef u8 piece;
#define white_p1 0
#define white_p2 1
#define white_p3 2
#define white_p4 3
#define white_p5 4
#define white_p6 5
#define white_p7 6
#define white_p8 7
#define white_r1 8
#define white_r2 9
#define white_n1 10
#define white_n2 11
#define white_b1 12 // Dark square
#define white_b2 13 // Light square
#define white_q 14
#define white_k 15
#define black_p1 16
#define black_p2 17
#define black_p3 18
#define black_p4 19
#define black_p5 20
#define black_p6 21
#define black_p7 22
#define black_p8 23
#define black_r1 24
#define black_r2 25
#define black_n1 26
#define black_n2 27
#define black_b1 28 // Light square
#define black_b2 29 // Dark square
#define black_q 30
#define black_k 31
#define nopiece 255

//
// piece char
//
#define pawn 'p'
#define rook 'R'
#define knight 'N'
#define bishop 'B'
#define queen 'Q'
#define king 'K'
bool IsPieceChar(char);
bool IsPromotionChar(char);
char PieceCharForIdx(int);
int IdxForPieceChar(char,color);
char PromotionCharForIdx(int);
int IdxForPromotionChar(char);

//
// file and rank
//
bool IsFileChar(char);
bool IsRankChar(char);

#endif // board_h
