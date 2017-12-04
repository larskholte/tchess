//
// node.h
//

#ifndef node_h
#define node_h

#include "position.h"
#include "config.h"
#include "board.h"
#include "move.h"

#include <atomic>
#include <iostream>
#include <stdexcept>
#include <list>

using namespace std;

class BadMove : public runtime_error {
public:
	BadMove(const char*);
};
class IllegalMove : public BadMove {
public:
	IllegalMove();
};
class AmbiguousMove : public BadMove {
public:
	AmbiguousMove();
};

struct Move;

class Node {
public:
	Node(int depth); // Initial node
	~Node();
	const Node *parent() const;
	void grow(int depth);
	bool grown() const;
	int evaluate();
	int value() const;
	int intrinsicValue() const;
	void predict(float priority);
	float priority() const;
	void heal();
	int numChildren() const;
	color getColor() const;
	const Node *inheritor() const;
	bool whiteCanCastleKingside() const;
	bool whiteCanCastleQueenside() const;
	bool blackCanCastleKingside() const;
	bool blackCanCastleQueenside() const;
	int movesWithoutPawnMove() const;
	int terminationStatus() const;
	Move moveToNode(const Node*) const;
	Move moveToOnlyChild() const;
	const Node *onlyChild() const;
	const Position& pos() const;
	Node* applyMove(Move);
	Node *playFor(color, Move*);
	friend ostream& operator<<(ostream&,const Node&);
	void printWithChildren(ostream&,int depth) const;
	void deleteChildrenExcept(Node*);
	void printGame(ostream&,std::list<Move>&) const;
private:
	typedef u16 SpecialType;
	Node(Node *parent, const Position& p, color turn, int depth,SpecialType);
	void addPosition(const Position&,int depth,SpecialType);
	void addAllMoves(piece,square*,int depth,SpecialType);
	void pawnMoves(square,square*);
	void rookMoves(square,square*);
	void knightMoves(square,square*);
	void bishopMoves(square,square*);
	void queenMoves(square,square*);
	void kingMoves(square,square*);
	void movesForPiece(piece,square*);
	bool moveCompatible(Move m,const Position&);
	int _value;
	Node *_parent;
	Position _pos;
	vector<Node*> _children;
	float _priority;
	SpecialType _special;
	SpecialType nextSpecialWithMovesWithoutPawnMove(int);
	SpecialType nextSpecial();
#define GrownMask 0x10
#define WhiteCanCastleKingsideMask 0x01
#define WhiteCanCastleQueensideMask 0x02
#define BlackCanCastleKingsideMask 0x04
#define BlackCanCastleQueensideMask 0x08
#define MovesWithoutPawnMoveMask 0x7e0
#define MovesWithoutPawnMoveShift 5
	color _color;
};
ostream& operator<<(ostream&,const Node&);

extern atomic_size_t nodeCount;
extern Node* terms[MaxNumTerminals+2];
extern int numTerms;
void AddTerminal(Node*);

#endif // node_h
