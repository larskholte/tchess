//
// move.h
//

#ifndef move_h
#define move_h

#include "board.h"

#include <stdexcept>
#include <string>

using namespace std;

class BadMoveDesc : public runtime_error {
public:
	BadMoveDesc();
};

struct Move {
	Move();
	Move(const char*);
    Move(const string&);
	char piecechar;
	char file;
	char rank;
	u8 takes;
	square to;
#define smCastleKingside 1
#define smCastleQueenside 2
#define smOfferDraw 3
	u8 special;
    char promotionchar;
    bool check;
	friend ostream& operator<<(ostream&,const Move&);
    operator string() const;
};
ostream& operator<<(ostream&,const Move&);

#endif // move_h
