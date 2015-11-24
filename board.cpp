//
// board.cpp
//

#include "board.h"

#include <stdexcept>

using namespace std;

int CountBitsU16(u16 num) {
	u16 mask;
	int cnt = 0;
	for (mask = 1; mask; mask <<= 1) {
		if (num & mask) cnt++;
	}
	return cnt;
}
int CountBitsU64(u64 num) {
	u64 mask;
	int cnt = 0;
	for (mask = 1; mask; mask <<= 1) {
		if (num & mask) cnt++;
	}
	return cnt;
}

square SquareWithFileAndRankChars(char file, char rank) {
	return 'h' - file + (rank - '1') * 8;
}
u8 FileIdxForSquare(square s) { return s % 8; }
u8 RankIdxForSquare(square s) { return s / 8; }
char FileCharForSquare(square s) {
    return 'h' - FileIdxForSquare(s);
}
char RankCharForSquare(square s) {
    return '1' + RankIdxForSquare(s);
}
bool HasSquareAbove(square s) { return s < 56; }
square SquareAbove(square s) { return s + 8; }
bool HasSquareBelow(square s) { return s > 7; }
square SquareBelow(square s) { return s - 8; }
bool HasSquareRight(square s) { return (s % 8) > 0; }
square SquareRight(square s) { return s - 1; }
bool HasSquareLeft(square s) { return (s % 8) < 7; }
square SquareLeft(square s) { return s + 1; }
bool HasSquareAboveRight(square s) { return s < sq_h8 && (s % 8) > 0; }
square SquareAboveRight(square s) { return s + 7; }
bool HasSquareAboveLeft(square s) { return s < sq_h8 && (s % 8) < 7; }
square SquareAboveLeft(square s) { return s + 9; }
bool HasSquareBelowRight(square s) { return s > sq_a1 && (s % 8) > 0; }
square SquareBelowRight(square s) { return s - 9; }
bool HasSquareBelowLeft(square s) { return s > sq_a1 && (s % 8) < 7; }
square SquareBelowLeft(square s) { return s - 7; }
bool HasSquareUUR(square s) { return s < sq_h7 && s % 8 > 0; }
square SquareUUR(square s) { return s + 15; }
bool HasSquareUUL(square s) { return s < sq_a6 && s % 8 < 7; }
square SquareUUL(square s) { return s + 17; }
bool HasSquareRRU(square s) { return s % 8 > 1 && s < sq_h8; }
square SquareRRU(square s) { return s + 6; }
bool HasSquareRRD(square s) { return s % 8 > 1 && s > sq_a1; }
square SquareRRD(square s) { return s - 10; }
bool HasSquareLLU(square s) { return s % 8 < 6 && s < sq_h8; }
square SquareLLU(square s) { return s + 10; }
bool HasSquareLLD(square s) { return s % 8 < 6 && s > sq_a1; }
square SquareLLD(square s) { return s - 6; }
bool HasSquareDDR(square s) { return s > sq_h3 && s % 8 > 0; }
square SquareDDR(square s) { return s - 17; }
bool HasSquareDDL(square s) { return s > sq_a2 && s % 8 < 7; }
square SquareDDL(square s) { return s - 15; }
#define MIN(a,b) (a < b ? a : b)
#define MAX(a,b) (a > b ? a : b)
int StepsToCorner(square s) {
    int ri = RankIdxForSquare(s);
    int fi = FileIdxForSquare(s);
    return MAX(MIN(7 - ri, ri), MIN(7 - fi, fi));
}

color OppositeColor(color c) {
	return 1^c;
}

bool IsPieceChar(char c) {
	return c == pawn || c == rook || c == knight || c == bishop || c == queen || c == king;
}
bool IsPromotionChar(char c) {
    return c == rook || c == knight || c == bishop || c == queen;
}
char PieceCharForIdx(int i) {
	if (i >= nopiece) return ' ';
	if (i >= 16) i -= 16; // Make all pieces white
	switch (i) {
	case white_p1 ... white_p8: return pawn;
	case white_r1 ... white_r2: return rook;
	case white_n1 ... white_n2: return knight;
	case white_b1 ... white_b2: return bishop;
	case white_q: return queen;
	case white_k: return king;
	}
	throw runtime_error("Bad piece index");
}
int IdxForPieceChar(char pc, color c) {
    switch (pc) {
    case 'p': return c == white ? white_p1 : black_p1;
    case 'R': return c == white ? white_r1 : black_r1;
    case 'N': return c == white ? white_n1 : black_n1;
    case 'B': return c == white ? white_b1 : black_b1;
    case 'Q': return c == white ? white_q  : black_q;
    case 'K': return c == white ? white_k  : black_k;
    default: throw runtime_error("bad piece char");
    }
}
char PromotionCharForIdx(int idx) {
    switch (idx) {
    case 0: return 'N';
    case 1: return 'B';
    case 2: return 'R';
    case 3: return 'Q';
    default: throw runtime_error("invalid promotion index");
    }
}
int IdxForPromotionChar(char pc) {
    switch (pc) {
    case 'N': return 0;
    case 'B': return 1;
    case 'R': return 2;
    case 'Q': return 3;
    default: throw runtime_error("invalid promotion char");
    }
}
bool IsFileChar(char c) {
	return c >= 'a' && c <= 'h';
}
bool IsRankChar(char c) {
	return c >= '1' && c <= '8';
}
