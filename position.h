//
// position.h
// Chess position
//

#ifndef position_h
#define position_h

#include <vector>
#include <iostream>

#include "board.h"
#include "move.h"

using namespace std;

//
// Position
//
class Position {
public:
	static Position initialPosition();
	// + Favors white
	// - Favors black
	int value() const;
	color squareColor(square) const;
	bool squareEmpty(square) const;
	piece pieceForSquareAndColor(square,color) const;
	square squareForPiece(piece) const;
	int numForColor(color) const;
	int numForColorInFile(color,char file) const;
	int numForPieceChar(char,color) const;
	char promotionCharForPromotedPawn(piece p) const;
	bool inCheck(color) const;
	void movePiece(piece p, square to);
	void takePiece(piece p, square to);
	void promotePawn(piece p, char);
	bool operator==(const Position&) const;
	Move moveToPositionForColor(const Position&,color) const;
	static square squareForBoardmask(u64);
	static string stringForFileLabels();
	static string stringForTopBorder();
	static string stringForBottomBorder();
	static string stringForRankSeparator();
	string stringForRank(int) const;
	ms matingSequence() const;
private:
	// Pressure, coverage, protection
	void pcp(int *pres, int *cov, int *prot) const;
	void pcpFor(color,int *pres, int *cov, int *prot) const;
	void rookPcp(color,square rs,int *pres, int *cov, int *prot) const;
	void bishopPcp(color,square rs,int *pres, int *cov, int *prot) const;
	void knightPcp(color c, square ns, int *pres, int *cov, int *prot) const;
	void kingPcp(color c, square ks, int *pres, int *cov, int *prot) const;
	square _positions[32];
	u64 _boardmasks[2];
	u32 _promotions;
	friend class Node;
};
ostream& operator<<(ostream&,const Position&);

#endif // position_h
