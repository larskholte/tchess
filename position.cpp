//
// pos.cpp
//

#include "position.h"

#include <string.h>
#include <stdexcept>
#include <sstream>

#include "config.h"

using namespace std;

//
// Position
//
Position Position::initialPosition() {
	Position p;
	// White
	p._positions[white_p1] = 15;
	p._positions[white_p2] = 14;
	p._positions[white_p3] = 13;
	p._positions[white_p4] = 12;
	p._positions[white_p5] = 11;
	p._positions[white_p6] = 10;
	p._positions[white_p7] = 9;
	p._positions[white_p8] = 8;
	p._positions[white_r1] = 7;
	p._positions[white_r2] = 0;
	p._positions[white_n1] = 6;
	p._positions[white_n2] = 1;
	p._positions[white_b1] = 5;
	p._positions[white_b2] = 2;
	p._positions[white_q] = 4;
	p._positions[white_k] = 3;
	p._boardmasks[white] = 0x000000000000ffff;
	// Black
	p._positions[black_p1] = 55;
	p._positions[black_p2] = 54;
	p._positions[black_p3] = 53;
	p._positions[black_p4] = 52;
	p._positions[black_p5] = 51;
	p._positions[black_p6] = 50;
	p._positions[black_p7] = 49;
	p._positions[black_p8] = 48;
	p._positions[black_r1] = 63;
	p._positions[black_r2] = 56;
	p._positions[black_n1] = 62;
	p._positions[black_n2] = 57;
	p._positions[black_b1] = 61;
	p._positions[black_b2] = 58;
	p._positions[black_q] = 60;
	p._positions[black_k] = 59;
	p._boardmasks[black] = 0xffff000000000000;
	p._promotions = 0;
	return p;
}
int Position::value() const {
	switch (matingSequence()) {
	case MSWhiteR: case MSWhiteQ: return WhiteMate + 3 - StepsToCorner(squareForPiece(black_k));
	case MSBlackR: case MSBlackQ: return BlackMate - 3 + StepsToCorner(squareForPiece(white_k));
	}
	int val = numForPieceChar('p',white)*PawnWeight - numForPieceChar('p',black)*PawnWeight +
			numForPieceChar('N',white)*KnightWeight - numForPieceChar('N',black)*KnightWeight +
			numForPieceChar('B',white)*BishopWeight - numForPieceChar('B',black)*BishopWeight +
			numForPieceChar('R',white)*RookWeight - numForPieceChar('R',black)*RookWeight +
			numForPieceChar('Q',white)*QueenWeight - numForPieceChar('Q',black)*QueenWeight;
	val *= PiecesWeight;
	int pres, cov, prot;
	pcp(&pres,&cov,&prot);
	val += pres*PressureWeight;
	val += cov*CoverageWeight;
	val += prot*ProtectionWeight;
	return val;
}
color Position::squareColor(square s) const {
	if (_boardmasks[white] & ((u64)1 << s)) return white;
	if (_boardmasks[black] & ((u64)1 << s)) return black;
	return emptycolor;
}
bool Position::squareEmpty(square s) const {
	return squareColor(s) == emptycolor;
}
piece Position::pieceForSquareAndColor(square s, color c) const {
	int start = c * 16;
	for (int i = 0; i < 16; i++) {
		if (_positions[start+i] == s) return start+i;
		if ((_positions[start+i] & ~PromotionMask) == s) {
			// i < 8
			int shift = 2*i + start;
			u32 mask = 3 << shift;
			return IdxForPieceChar(PromotionCharForIdx((_promotions & mask) >> shift),c);
		}
	}
	return nopiece;
}
square Position::squareForPiece(piece p) const {
	return _positions[p];
}
bool Position::inCheck(color c) const {
	square ks = squareForPiece(c == white ? white_k : black_k);
	color oc = OppositeColor(c);
	square cs;
	piece p;
	char pc;
	// NOTE: could make more efficient
	// Check pawn moves
	if (c == white) {
		if (HasSquareAboveRight(ks)) {
			cs = SquareAboveRight(ks);
			p = pieceForSquareAndColor(cs,oc);
			if (PieceCharForIdx(p) == pawn) return true;
		}
		if (HasSquareAboveLeft(ks)) {
			cs = SquareAboveLeft(ks);
			p = pieceForSquareAndColor(cs,oc);
			if (PieceCharForIdx(p) == pawn) return true;
		}
	} else {
		if (HasSquareBelowRight(ks)) {
			cs = SquareBelowRight(ks);
			p = pieceForSquareAndColor(cs,oc);
			if (PieceCharForIdx(p) == pawn) return true;
		}
		if (HasSquareBelowLeft(ks)) {
			cs = SquareBelowLeft(ks);
			p = pieceForSquareAndColor(cs,oc);
			if (PieceCharForIdx(p) == pawn) return true;
		}
	}
	// Check knight's moves
	if (HasSquareUUR(ks)) {
		cs = SquareUUR(ks);
		p = pieceForSquareAndColor(cs,oc);
		if (PieceCharForIdx(p) == knight) return true;
	}
	if (HasSquareUUL(ks)) {
		cs = SquareUUL(ks);
		p = pieceForSquareAndColor(cs,oc);
		if (PieceCharForIdx(p) == knight) return true;
	}
	if (HasSquareRRU(ks)) {
		cs = SquareRRU(ks);
		p = pieceForSquareAndColor(cs,oc);
		if (PieceCharForIdx(p) == knight) return true;
	}
	if (HasSquareRRD(ks)) {
		cs = SquareRRD(ks);
		p = pieceForSquareAndColor(cs,oc);
		if (PieceCharForIdx(p) == knight) return true;
	}
	if (HasSquareLLU(ks)) {
		cs = SquareLLU(ks);
		p = pieceForSquareAndColor(cs,oc);
		if (PieceCharForIdx(p) == knight) return true;
	}
	if (HasSquareLLD(ks)) {
		cs = SquareLLD(ks);
		p = pieceForSquareAndColor(cs,oc);
		if (PieceCharForIdx(p) == knight) return true;
	}
	if (HasSquareDDR(ks)) {
		cs = SquareDDR(ks);
		p = pieceForSquareAndColor(cs,oc);
		if (PieceCharForIdx(p) == knight) return true;
	}
	if (HasSquareDDL(ks)) {
		cs = SquareDDL(ks);
		p = pieceForSquareAndColor(cs,oc);
		if (PieceCharForIdx(p) == knight) return true;
	}
	// Check ranks and files
	color sc;
	cs = ks;
	while (HasSquareAbove(cs)) {
		cs = SquareAbove(cs);
		sc = squareColor(cs);
		if (sc == emptycolor) continue;
		if (sc == c) break; // Piece of same color
		p = pieceForSquareAndColor(cs,oc);
		pc = PieceCharForIdx(p);
		if (pc == rook || pc == queen || (pc == king && cs == SquareAbove(ks))) return true;
		break;
	}
	cs = ks;
	while (HasSquareBelow(cs)) {
		cs = SquareBelow(cs);
		sc = squareColor(cs);
		if (sc == emptycolor) continue;
		if (sc == c) break; // Piece of same color
		p = pieceForSquareAndColor(cs,oc);
		pc = PieceCharForIdx(p);
		if (pc == rook || pc == queen || (pc == king && cs == SquareBelow(ks))) return true;
		break;
	}
	cs = ks;
	while (HasSquareRight(cs)) {
		cs = SquareRight(cs);
		sc = squareColor(cs);
		if (sc == emptycolor) continue;
		if (sc == c) break; // Piece of same color
		p = pieceForSquareAndColor(cs,oc);
		pc = PieceCharForIdx(p);
		if (pc == rook || pc == queen || (pc == king && cs == SquareRight(ks))) return true;
		break;
	}
	cs = ks;
	while (HasSquareLeft(cs)) {
		cs = SquareLeft(cs);
		sc = squareColor(cs);
		if (sc == emptycolor) continue;
		if (sc == c) break; // Piece of same color
		p = pieceForSquareAndColor(cs,oc);
		pc = PieceCharForIdx(p);
		if (pc == rook || pc == queen || (pc == king && cs == SquareLeft(ks))) return true;
		break;
	}
	// Check diagonals
	cs = ks;
	while (HasSquareAboveRight(cs)) {
		cs = SquareAboveRight(cs);
		sc = squareColor(cs);
		if (sc == emptycolor) continue;
		if (sc == c) break; // Piece of same color
		p = pieceForSquareAndColor(cs,oc);
		pc = PieceCharForIdx(p);
		if (pc == bishop || pc == queen || (pc == king && cs == SquareAboveRight(ks))) return true;
		break;
	}
	cs = ks;
	while (HasSquareAboveLeft(cs)) {
		cs = SquareAboveLeft(cs);
		sc = squareColor(cs);
		if (sc == emptycolor) continue;
		if (sc == c) break; // Piece of same color
		p = pieceForSquareAndColor(cs,oc);
		pc = PieceCharForIdx(p);
		if (pc == bishop || pc == queen || (pc == king && cs == SquareAboveLeft(ks))) return true;
		break;
	}
	cs = ks;
	while (HasSquareBelowRight(cs)) {
		cs = SquareBelowRight(cs);
		sc = squareColor(cs);
		if (sc == emptycolor) continue;
		if (sc == c) break; // Piece of same color
		p = pieceForSquareAndColor(cs,oc);
		pc = PieceCharForIdx(p);
		if (pc == bishop || pc == queen || (pc == king && cs == SquareBelowRight(ks))) return true;
		break;
	}
	cs = ks;
	while (HasSquareBelowLeft(cs)) {
		cs = SquareBelowLeft(cs);
		sc = squareColor(cs);
		if (sc == emptycolor) continue;
		if (sc == c) break; // Piece of same color
		p = pieceForSquareAndColor(cs,oc);
		pc = PieceCharForIdx(p);
		if (pc == bishop || pc == queen || (pc == king && cs == SquareBelowLeft(ks))) return true;
		break;
	}
	return false;
}
int Position::numForColor(color c) const {
	return CountBitsU64(_boardmasks[c]);
}
int Position::numForColorInFile(color c, char file) const {
	square s = SquareWithFileAndRankChars(file,'1');
	int num = 0;
	while (1) {
		if (squareColor(s) == c) num++;
		if (!HasSquareAbove(s)) break;
		s = SquareAbove(s);
	}
	return num;
}
int Position::numForPieceChar(char pc, color c) const {
	int start = 16*c, num = 2;
	switch (pc) {
	case pawn: num = 8; break;
	case rook: start += white_r1; break;
	case knight: start += white_n1; break;
	case bishop: start += white_b1; break;
	case queen: start += white_q; num = 1; break;
	case king: start += white_k; num = 1; break;
	default: throw runtime_error("Bad piece char");
	}
	int cnt = 0;
	for (int i = 0; i < num; i++) {
		if (!(_positions[start+i] & TakenMask)) cnt++;
	}
	start = c*16;
	if (IsPromotionChar(pc)) {
		for (int i = 0; i < 8; i++) {
			if ((_positions[start+i] & PromotionMask) && !(_positions[i] & TakenMask)) {
				int shift = 2*i + start;
				u32 mask = 3 << shift;
				if (pc == PromotionCharForIdx((_promotions & mask) >> shift)) cnt++;
			}
		}
	}
	return cnt;
}
char Position::promotionCharForPromotedPawn(piece p) const {
	int shift = p >= 16 ? 2*(p-8) : 2*p;
	u32 mask = 3 << shift;
	return PromotionCharForIdx((_promotions & mask) >> shift);
}
void Position::movePiece(piece p, square tosquare) {
	square fromsquare = _positions[p];
	_positions[p] = tosquare | (fromsquare & PromotionMask);
	color c = (p >= 16);
	_boardmasks[c] = (_boardmasks[c] & ~((u64)1 << fromsquare)) | ((u64)1 << tosquare);
}
void Position::takePiece(piece p, square tosquare) {
	square fromsquare = _positions[p];
	_positions[p] = tosquare | (fromsquare & PromotionMask);
	color c = (p >= 16);
	_boardmasks[c] = (_boardmasks[c] & ~((u64)1 << (fromsquare & ~PromotionMask))) | ((u64)1 << tosquare);
	_boardmasks[OppositeColor(c)] &= ~((u64)1 << tosquare);
	if (c == white) { // Take black piece
		for (int i = 16; i < 32; i++) if ((_positions[i] & ~PromotionMask) == tosquare) _positions[i] |= TakenMask;
	} else { // Take white piece
		for (int i = 0; i < 16; i++) if ((_positions[i] & ~PromotionMask) == tosquare) _positions[i] |= TakenMask;
	}
}
void Position::promotePawn(piece p, char pc) {
	_positions[p] |= PromotionMask;
	int shift = p > 8 ? 2*(p - 8) : 2*p;
	u32 mask = 3 << shift;
	_promotions = (_promotions & ~mask) | (IdxForPromotionChar(pc) << shift);
}
bool Position::operator ==(const Position &p) const {
	return memcmp(this,&p,sizeof(Position)) == 0;
}
Move Position::moveToPositionForColor(const Position &pos, color c) const {
	color oc = OppositeColor(c);
	u64 ourChanges = _boardmasks[c] ^ pos._boardmasks[c];
	u64 ourVacancies = ourChanges & ~pos._boardmasks[c];
	if (CountBitsU64(ourVacancies) == 2) {
		// Castle
		Move m;
		if ((ourChanges & 0x0f) == 0x0f && c == white) m.special = smCastleKingside;
		else if ((ourChanges & 0xb8) == 0xb8 && c == white) m.special = smCastleQueenside;
		else if ((ourChanges & 0x0f00000000000000) == 0x0f00000000000000 && c == black) m.special = smCastleKingside;
		else if ((ourChanges & 0xb800000000000000) == 0xb800000000000000 && c == black) m.special = smCastleQueenside;
		else throw runtime_error("no move to result in position");
		return m;
	} else if (CountBitsU64(ourVacancies) != 1) {
		throw runtime_error("no move to result in position");
	}
	u64 dest = ourChanges & pos._boardmasks[c];
	Move m;
	square vacated = squareForBoardmask(ourVacancies);
	m.piecechar = PieceCharForIdx(pieceForSquareAndColor(vacated,c));
	m.file = FileCharForSquare(vacated);
	m.rank = RankCharForSquare(vacated);
	u64 theirChanges = _boardmasks[oc] ^ pos._boardmasks[oc];
	u64 theirVacancies = theirChanges & ~pos._boardmasks[oc];
	m.takes = !!theirVacancies;
	m.to = squareForBoardmask(dest);
	char tochar = PieceCharForIdx(pos.pieceForSquareAndColor(m.to,c));
	if (m.piecechar != tochar) {
		m.promotionchar = tochar;
	}
	if (pos.inCheck(oc)) m.check = 1;
	return m;
}
square Position::squareForBoardmask(u64 bm) {
	if (bm == 0) return nosquare;
	square s = 0;
	for (; 1; s++) {
		if (bm & 1) return s;
		bm >>= 1;
	}
}
string Position::stringForFileLabels() {
	return "     a   b   c   d   e   f   g   h     ";
}
string Position::stringForTopBorder() {
	return "    -------------------------------    ";
}
string Position::stringForBottomBorder() {
	return Position::stringForTopBorder();
}
string Position::stringForRankSeparator() {
	return "   |---+---+---+---+---+---+---+---|   ";
}
string Position::stringForRank(int rank) const {
	square s = rank*8 - 1;
	piece p = nopiece;
	color c = emptycolor;
	stringstream ss;
	ss << ' ' << rank;
	for (int j = 0; j < 8; j++) {
		c = squareColor(s);
		if (c != emptycolor) p = pieceForSquareAndColor(s,c);
		else p = nopiece;
		ss << " | \033[";
		switch (c) {
			case white: ss << "39"; break;
			case black: ss << "31"; break; // Actually red
			default: ss << "39"; // Reset foreground to default
		}
		ss << 'm' << PieceCharForIdx(p);
		ss << "\033[0m"; // Reset to default
		s--;
	}
	ss << " | " << rank << ' ';
	return ss.str();
}
ms Position::matingSequence() const {
	if (numForColor(white) == 1) { // White king alone
		if (numForPieceChar('Q',black)) return MSBlackQ;
		if (numForPieceChar('R',black)) return MSBlackR;
	} else if (numForColor(black) == 1) { // Black king alone
		if (numForPieceChar('Q',white)) return MSWhiteQ;
		if (numForPieceChar('R',white)) return MSWhiteR;
	}
	return MSNone;
}
// Pressure, coverage, protection
void Position::pcp(int *pres, int *cov, int *prot) const {
	int wpres, wcov, wprot;
	pcpFor(white,&wpres,&wcov,&wprot);
	int bpres, bcov, bprot;
	pcpFor(black,&bpres,&bcov,&bprot);
	*pres = wpres - bpres;
	*cov = wcov - bcov;
	*prot = wprot - bprot;
}
void Position::pcpFor(color c, int *pres, int *cov, int *prot) const {
	color oc = OppositeColor(c);
	piece p = c*16;
	square s;
	// pawn pcp
	if (c == white) {
		for (; !(p & 8); p++) {
			if (_positions[p] & TakenMask) {
				continue;
			}
			s = squareForPiece(p);
			if (HasSquareAboveRight(s)) {
				if (squareColor(SquareAboveRight(s)) == oc) (*pres)++;
				else if (squareColor(SquareAboveRight(s)) == c) (*prot)++;
				else (*cov)++;
			}
			if (HasSquareAboveLeft(s)) {
				if (squareColor(SquareAboveLeft(s)) == oc) (*pres)++;
				else if (squareColor(SquareAboveLeft(s)) == c) (*prot)++;
				else (*cov)++;
			}
		}
	} else {
		for (; !(p & 8); p++) {
			if (_positions[p] & TakenMask) {
				continue;
			}
			s = squareForPiece(p);
			if (HasSquareBelowRight(s)) {
				if (squareColor(SquareBelowRight(s)) == oc) (*pres)++;
				else if (squareColor(SquareBelowRight(s)) == c) (*prot)++;
				else (*cov)++;
			}
			if (HasSquareBelowLeft(s)) {
				if (squareColor(SquareBelowLeft(s)) == oc) (*pres)++;
				else if (squareColor(SquareBelowLeft(s)) == c) (*prot)++;
				else (*cov)++;
			}
		}
	}
	// rook pcp
	for (; !(p & 2); p++) {
		if (!(_positions[p] & TakenMask)) {
			rookPcp(c,squareForPiece(p),pres,cov,prot);
		}
	}
	// knight pcp
	for (; p & 2; p++) {
		if (!(_positions[p] & TakenMask)) {
			knightPcp(c,squareForPiece(p),pres,cov,prot);
		}
	}
	// bishop pcp
	for (; !(p & 2); p++) {
		if (!(_positions[p] & TakenMask)) {
			bishopPcp(c,squareForPiece(p),pres,cov,prot);
		}
	}
	// queen pressure
	if (!(_positions[p] & TakenMask)) {
		rookPcp(c,squareForPiece(p),pres,cov,prot);
		bishopPcp(c,squareForPiece(p),pres,cov,prot);
	}
	p++;
	// TODO: king pressure
}
void Position::rookPcp(color c, square rs, int *pres, int *cov, int *prot) const {
	color oc = OppositeColor(c);
	square s = rs;
	while (HasSquareAbove(s)) {
		s = SquareAbove(s);
		if (squareColor(s) == oc) {
			(*pres)++;
			break;
		} else if (squareColor(s) == c) {
			(*prot)++;
			break;
		}
		(*cov)++;
	}
	s = rs;
	while (HasSquareBelow(s)) {
		s = SquareBelow(s);
		if (squareColor(s) == oc) {
			(*pres)++;
			break;
		} else if (squareColor(s) == c) {
			(*prot)++;
			break;
		}
		(*cov)++;
	}
	s = rs;
	while (HasSquareRight(s)) {
		s = SquareRight(s);
		if (squareColor(s) == oc) {
			(*pres)++;
			break;
		} else if (squareColor(s) == c) {
			(*prot)++;
			break;
		}
		(*cov)++;
	}
	s = rs;
	while (HasSquareLeft(s)) {
		s = SquareLeft(s);
		if (squareColor(s) == oc) {
			(*pres)++;
			break;
		} else if (squareColor(s) == c) {
			(*prot)++;
			break;
		}
		(*cov)++;
	}
}
void Position::bishopPcp(color c, square bs, int *pres, int *cov, int *prot) const {
	color oc = OppositeColor(c);
	square s = bs;
	while (HasSquareAboveRight(s)) {
		s = SquareAboveRight(s);
		if (squareColor(s) == oc) {
			(*pres)++;
			break;
		} else if (squareColor(s) == c) {
			(*prot)++;
			break;
		}
		(*cov)++;
	}
	s = bs;
	while (HasSquareBelowRight(s)) {
		s = SquareBelowRight(s);
		if (squareColor(s) == oc) {
			(*pres)++;
			break;
		} else if (squareColor(s) == c) {
			(*prot)++;
			break;
		}
		(*cov)++;
	}
	s = bs;
	while (HasSquareAboveLeft(s)) {
		s = SquareAboveLeft(s);
		if (squareColor(s) == oc) {
			(*pres)++;
			break;
		} else if (squareColor(s) == c) {
			(*prot)++;
			break;
		}
		(*cov)++;
	}
	s = bs;
	while (HasSquareBelowLeft(s)) {
		s = SquareBelowLeft(s);
		if (squareColor(s) == oc) {
			(*pres)++;
			break;
		} else if (squareColor(s) == c) {
			(*prot)++;
			break;
		}
		(*cov)++;
	}
}
void Position::knightPcp(color c, square ns, int *pres, int *cov, int *prot) const {
	color oc = OppositeColor(c);
	square s = ns;
	color sc;
	if (HasSquareUUR(s)) {
		s = SquareUUR(s);
		sc = squareColor(s);
		if (sc == oc) {
			(*pres)++;
		} else if (sc == c) {
			(*prot)++;
		} else {
			(*cov)++;
		}
	}
	if (HasSquareUUL(s)) {
		s = SquareUUL(s);
		sc = squareColor(s);
		if (sc == oc) {
			(*pres)++;
		} else if (sc == c) {
			(*prot)++;
		} else {
			(*cov)++;
		}
	}
	if (HasSquareDDR(s)) {
		s = SquareDDR(s);
		sc = squareColor(s);
		if (sc == oc) {
			(*pres)++;
		} else if (sc == c) {
			(*prot)++;
		} else {
			(*cov)++;
		}
	}
	if (HasSquareDDL(s)) {
		s = SquareDDL(s);
		sc = squareColor(s);
		if (sc == oc) {
			(*pres)++;
		} else if (sc == c) {
			(*prot)++;
		} else {
			(*cov)++;
		}
	}
	if (HasSquareRRU(s)) {
		s = SquareRRU(s);
		sc = squareColor(s);
		if (sc == oc) {
			(*pres)++;
		} else if (sc == c) {
			(*prot)++;
		} else {
			(*cov)++;
		}
	}
	if (HasSquareRRD(s)) {
		s = SquareRRD(s);
		sc = squareColor(s);
		if (sc == oc) {
			(*pres)++;
		} else if (sc == c) {
			(*prot)++;
		} else {
			(*cov)++;
		}
	}
	if (HasSquareLLU(s)) {
		s = SquareLLU(s);
		sc = squareColor(s);
		if (sc == oc) {
			(*pres)++;
		} else if (sc == c) {
			(*prot)++;
		} else {
			(*cov)++;
		}
	}
	if (HasSquareLLD(s)) {
		s = SquareLLD(s);
		sc = squareColor(s);
		if (sc == oc) {
			(*pres)++;
		} else if (sc == c) {
			(*prot)++;
		} else {
			(*cov)++;
		}
	}
}
ostream& operator<<(ostream &out,const Position &pos) {
	out << "     a   b   c   d   e   f   g   h\n" << "    -------------------------------\n";
	square s = sq_a8;
	color c = emptycolor;
	piece p = nopiece;
	for (int i = 8; i > 0; i--) {
		out << ' ' << i;
		for (int j = 0; j < 8; j++) {
			c = pos.squareColor(s);
			if (c != emptycolor) p = pos.pieceForSquareAndColor(s,c);
			else p = nopiece;
			out << " | \033[";
			switch (c) {
				case white: out << "39"; break;
				case black: out << "31"; break; // Actually red
				default: out << "39"; // Reset foreground to default
			}
			out << 'm';
			out << PieceCharForIdx(p);
			out << "\033[0m"; // Reset to default
			s--;
		}
		out << " | " << i << "\n";
		if (i > 1) out << "   |---+---+---+---+---+---+---+---|\n";
		else       out << "    -------------------------------\n";
	}
	out << "     a   b   c   d   e   f   g   h\n";
	return out;
}
