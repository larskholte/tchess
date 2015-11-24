//
// move.cpp
//

#include "move.h"

#include <cctype>
#include <string>
#include <sstream>

using namespace std;

BadMoveDesc::BadMoveDesc() : runtime_error("Bad move descriptor") { }

static bool IsWhiteString(const char *str) {
	for (int i = 0; str[i] != 0; i++) if (!isspace(str[i])) return false;
	return true;
}
Move::Move() : piecechar(0), file(0), rank(0), takes(0), to(0), special(0), promotionchar(0), check(0) { }
Move::Move(const string &s) : Move(s.c_str()) { }
Move::Move(const char *msg) : Move() {
	// Skip leading whitespace
	for (; isspace(*msg); msg++);
	// Determine format
	if (IsFileChar(msg[0]) && // e4
			IsRankChar(msg[1]) &&
			IsWhiteString(msg+2)) {
		piecechar = pawn;
		to = SquareWithFileAndRankChars(msg[0],msg[1]);
	} else if (IsFileChar(msg[0]) && // e4=Q
			IsRankChar(msg[1]) &&
			msg[2] == '=' &&
			IsPromotionChar(msg[3]) &&
			IsWhiteString(msg+4)) {
		piecechar = pawn;
		to = SquareWithFileAndRankChars(msg[0],msg[1]);
        promotionchar = msg[3];
	} else if (msg[0] == 'x' && // xe4
			IsFileChar(msg[1]) &&
			IsRankChar(msg[2]) &&
			IsWhiteString(msg+3)) {
		piecechar = pawn;
		takes = 1;
		to = SquareWithFileAndRankChars(msg[1],msg[2]);
	} else if (msg[0] == 'x' && // xe4=Q
			IsFileChar(msg[1]) &&
			IsRankChar(msg[2]) &&
            msg[3] == '=' &&
			IsPromotionChar(msg[4]) &&
			IsWhiteString(msg+5)) {
		piecechar = pawn;
		takes = 1;
		to = SquareWithFileAndRankChars(msg[1],msg[2]);
        promotionchar = msg[4];
	} else if (IsFileChar(msg[0]) && // dxe4
            msg[1] == 'x' &&
			IsFileChar(msg[2]) &&
			IsRankChar(msg[3]) &&
			IsWhiteString(msg+4)) {
		piecechar = pawn;
        file = msg[0];
		takes = 1;
		to = SquareWithFileAndRankChars(msg[2],msg[3]);
	} else if (IsFileChar(msg[0]) && // dxe4=Q
            msg[1] == 'x' &&
			IsFileChar(msg[2]) &&
			IsRankChar(msg[3]) &&
            msg[4] == '=' &&
			IsPromotionChar(msg[5]) &&
			IsWhiteString(msg+6)) {
		piecechar = pawn;
        file = msg[0];
		takes = 1;
		to = SquareWithFileAndRankChars(msg[2],msg[3]);
        promotionchar = msg[5];
	} else if (IsFileChar(msg[0]) && // e4xd5
			IsRankChar(msg[1]) &&
			msg[2] == 'x' &&
			IsFileChar(msg[3]) &&
			IsRankChar(msg[4]) &&
			IsWhiteString(msg+5)) {
		piecechar = pawn;
		file = msg[0];
		rank = msg[1];
		takes = 1;
		to = SquareWithFileAndRankChars(msg[3],msg[4]);
	} else if (IsFileChar(msg[0]) && // e4xd5=Q
			IsRankChar(msg[1]) &&
			msg[2] == 'x' &&
			IsFileChar(msg[3]) &&
			IsRankChar(msg[4]) &&
            msg[5] == '=' &&
            IsPromotionChar(msg[6]) &&
			IsWhiteString(msg+7)) {
		piecechar = pawn;
		file = msg[0];
		rank = msg[1];
		takes = 1;
		to = SquareWithFileAndRankChars(msg[3],msg[4]);
        promotionchar = msg[6];
	} else if (IsPieceChar(msg[0]) &&	// Be5
			IsFileChar(msg[1]) &&
			IsRankChar(msg[2]) &&
			IsWhiteString(msg+3)) {
		piecechar = msg[0];
		to = SquareWithFileAndRankChars(msg[1],msg[2]);
	} else if (IsPieceChar(msg[0]) && // Bxe5
			msg[1] == 'x' &&
			IsFileChar(msg[2]) &&
			IsRankChar(msg[3]) &&
			IsWhiteString(msg+4)) {
		piecechar = msg[0];
		takes = 1;
		to = SquareWithFileAndRankChars(msg[2],msg[3]);
	} else if (IsPieceChar(msg[0]) && // Bde5
			IsFileChar(msg[1]) &&
			IsFileChar(msg[2]) &&
			IsRankChar(msg[3]) &&
			IsWhiteString(msg+4)) {
		piecechar = msg[0];
		file = msg[1];
		to = SquareWithFileAndRankChars(msg[2],msg[3]);
	} else if (IsPieceChar(msg[0]) && // Bdxe5
			IsFileChar(msg[1]) &&
			msg[2] == 'x' &&
			IsFileChar(msg[3]) &&
			IsRankChar(msg[4]) &&
			IsWhiteString(msg+5)) {
		piecechar = msg[0];
		file = msg[1];
		takes = 1;
		to = SquareWithFileAndRankChars(msg[3],msg[4]);
	} else if (IsPieceChar(msg[0]) && // Bd4e5
			IsFileChar(msg[1]) &&
			IsRankChar(msg[2]) &&
			IsFileChar(msg[3]) &&
			IsRankChar(msg[4]) &&
			IsWhiteString(msg+5)) {
		piecechar = msg[0];
		file = msg[1];
		rank = msg[2];
		to = SquareWithFileAndRankChars(msg[3],msg[4]);
	} else if (IsPieceChar(msg[0]) && // Bd4xe5
			IsFileChar(msg[1]) &&
			IsRankChar(msg[2]) &&
			msg[3] == 'x' &&
			IsFileChar(msg[4]) &&
			IsRankChar(msg[5]) &&
			IsWhiteString(msg+6)) {
		piecechar = msg[0];
		file = msg[1];
		rank = msg[2];
		takes = 1;
		to = SquareWithFileAndRankChars(msg[4],msg[5]);
	} else if (strncmp(msg,"0-0",3) == 0 && IsWhiteString(msg+3)) {
        special = smCastleKingside;
    } else if (strncmp(msg,"0-0-0",5) == 0 && IsWhiteString(msg+5)) {
        special = smCastleQueenside;
    }
    else {
		throw BadMoveDesc();
	}
}
Move::operator string() const {
    if (special & smCastleKingside) {
        return string("0-0");
    }
    if (special & smCastleQueenside) {
        return string("0-0-0");
    }
    stringstream ss;
    if (piecechar) ss << string(1,piecechar);
    if (file) ss << string(1,file);
    if (rank) ss << string(1,rank);
    if (takes) ss << string(1,'x');
    ss << string(1,FileCharForSquare(to)) << string(1,RankCharForSquare(to));
    if (promotionchar) {
        ss << string("=") << string(1,promotionchar);
    }
    if (check) ss << string(1,'+');
    return ss.str();
}
ostream& operator<<(ostream &out, const Move &move) {
    return out << string(move);
}
