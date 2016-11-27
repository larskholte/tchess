//
// node.cpp
//

#include "node.h"

#include "config.h"
#include "board.h"

#include <cmath>
#include <vector>

using namespace std;

BadMove::BadMove(const char *msg) : runtime_error(msg) { }
IllegalMove::IllegalMove() : BadMove("Illegal move") { }
AmbiguousMove::AmbiguousMove() : BadMove("Ambiguous move") { }

Node::Node(Node *parent, const Position& p, color turn, int depth, SpecialType special) :
		_value(0), _parent(parent), _pos(p), _children(0), _priority(0), _special(special),
		_color(turn) {
	grow(depth);
}
Node::Node(int depth) : _value(0), _parent(0),
		_pos(Position::initialPosition()), _children(0), _priority(0), _special(0),
		_color(white) {
	grow(depth);
}
Node::~Node() {
	for (size_t i = 0; i < _children.size(); i++) {
		delete _children[i];
	}
	nodeCount -= _children.size();
}
const Node* Node::parent() const { return _parent; }
void Node::addPosition(const Position &pos, int depth, SpecialType special) {
	// Can't put yourself in check
	if (pos.inCheck(_color)) return;
	Node *node = new Node(this,pos,OppositeColor(_color),depth-1,special);
	if (node == 0) throw runtime_error("failed to allocate node");
	_children.push_back(node);
}
void Node::addAllMoves(piece p, square *squares, int depth, u16 special) {
	// Find all legal moves and add as children
	Position pos;
	for (int j = 0; squares[j] != nosquare; j++) {
		pos = _pos;
		if (squares[j] & TakesMask) {
			pos.takePiece(p,squares[j] & ~TakesMask);
		} else {
			pos.movePiece(p,squares[j]);
		}
		addPosition(pos,depth,special);
	}
}
void Node::grow(int depth) {
	if (depth == 0) return;
	if (grown()) {
		for (size_t i = 0; i < _children.size(); i++) {
			_children[i]->grow(depth-1);
		}
		return;
	}
	_special |= GrownMask;
	if (movesWithoutPawnMove() >= MaxMovesWithoutPawnMove) {
		return;
	}
	_children.reserve(StandardCapacity);
	// Generate all valid moves
	square squares[29];
	piece p;
	Position pos;
	// Castling
	if (_color == white) {
		// White castle
		if (whiteCanCastleKingside()) {
			if (_pos.squareEmpty(sq_f1) &&
				_pos.squareEmpty(sq_g1) &&
				_pos.pieceForSquareAndColor(sq_h1,_color) == white_r2) {
				pos = _pos;
				pos.movePiece(white_k,sq_g1);
				pos.movePiece(white_r2,sq_f1);
				addPosition(pos,depth,nextSpecial() |
						WhiteCanCastleKingsideMask |
						WhiteCanCastleQueensideMask);
			}
		}
		if (whiteCanCastleQueenside()) {
			if (_pos.squareEmpty(sq_d1) &&
				_pos.squareEmpty(sq_c1) &&
				_pos.squareEmpty(sq_b1) &&
				_pos.pieceForSquareAndColor(sq_a1,_color) == white_r1) {
				pos = _pos;
				pos.movePiece(white_k,sq_c1);
				pos.movePiece(white_r1,sq_d1);
				addPosition(pos,depth,nextSpecial() |
						WhiteCanCastleKingsideMask |
						WhiteCanCastleQueensideMask);
			}
		}
		p = 0;
	} else {
		// Black castle
		if (blackCanCastleKingside()) {
			if (_pos.squareEmpty(sq_f8) &&
				_pos.squareEmpty(sq_g8) &&
				_pos.pieceForSquareAndColor(sq_h8,_color) == black_r2) {
				pos = _pos;
				pos.movePiece(black_k,sq_g8);
				pos.movePiece(black_r2,sq_f8);
				addPosition(pos,depth,nextSpecial() |
						BlackCanCastleKingsideMask |
						BlackCanCastleQueensideMask);
			}
		}
		if (blackCanCastleQueenside()) {
			if (_pos.squareEmpty(sq_d8) &&
				_pos.squareEmpty(sq_c8) &&
				_pos.squareEmpty(sq_b8) &&
				_pos.pieceForSquareAndColor(sq_a8,_color) == black_r1) {
				pos = _pos;
				pos.movePiece(black_k,sq_c8);
				pos.movePiece(black_r1,sq_d8);
				addPosition(pos,depth,nextSpecial() |
						BlackCanCastleKingsideMask |
						BlackCanCastleQueensideMask);
			}
		}
		p = 16;
	}
	// Generate pawn moves
	for (; !(p & 8); p++) {
		square ps = _pos.squareForPiece(p);
		if (ps & TakenMask) continue;
		if (ps & PromotionMask) {
			// Promoted pawns
			char pc = _pos.promotionCharForPromotedPawn(p);
			switch (pc) {
			case 'N': knightMoves(ps & ~PromotionMask,squares); break;
			case 'B': bishopMoves(ps & ~PromotionMask,squares); break;
			case 'R': rookMoves(ps & ~PromotionMask,squares); break;
			case 'Q': queenMoves(ps & ~PromotionMask,squares); break;
			default: throw runtime_error("bad promotion char");
			}
			addAllMoves(p,squares,depth,nextSpecial());
			continue;
		}
		pawnMoves(ps,squares);
		for (int i = 0; squares[i] != nosquare; i++) {
			Position pos = _pos;
			if (squares[i] & TakesMask) {
				pos.takePiece(p,squares[i] & ~TakesMask);
			} else {
				pos.movePiece(p,squares[i]);
			}
			int ri = RankIdxForSquare(squares[i] & 0x3f);
			if (ri == 0 || ri == 7) {
				// Promotion on last ranks
				Position pos2 = pos;
				pos2.promotePawn(p,'N');
				addPosition(pos2,depth,nextSpecialWithMovesWithoutPawnMove(0));
				pos2 = pos;
				pos2.promotePawn(p,'B');
				addPosition(pos2,depth,nextSpecialWithMovesWithoutPawnMove(0));
				pos2 = pos;
				pos2.promotePawn(p,'R');
				addPosition(pos2,depth,nextSpecialWithMovesWithoutPawnMove(0));
				pos2 = pos;
				pos2.promotePawn(p,'Q');
				addPosition(pos2,depth,nextSpecialWithMovesWithoutPawnMove(0));
			} else {
				addPosition(pos,depth,nextSpecialWithMovesWithoutPawnMove(0));
			}
		}
		int ri = RankIdxForSquare(ps);
		if (ri == 3 && _color == black && _parent) {
			// TODO: en passant
			if (HasSquareLeft(ps)) {
				char lpc = PieceCharForIdx(_pos.pieceForSquareAndColor(SquareLeft(ps),white));
				char plpc = PieceCharForIdx(_parent->_pos.pieceForSquareAndColor(SquareLeft(ps),white));
				if (lpc == pawn && plpc != pawn) { // Pawn moved into left square

				}
			}
		} else if (ri == 4 && _color == white && _parent) {

		}
	}
	// Generate rook moves
	// Queen's rook
	square rs = _pos.squareForPiece(p);
	if (!(rs & TakenMask)) {
		rookMoves(rs,squares);
		if (_color == white) {
			addAllMoves(p,squares,depth,nextSpecial() & ~WhiteCanCastleQueensideMask);
		} else {
			addAllMoves(p,squares,depth,nextSpecial() & ~BlackCanCastleQueensideMask);
		}
	}
	p++;
	// King's rook
	rs = _pos.squareForPiece(p);
	if (!(rs & TakenMask)) {
		rookMoves(rs,squares);
		if (_color == white) {
			addAllMoves(p,squares,depth,nextSpecial() & ~WhiteCanCastleKingsideMask);
		} else {
			addAllMoves(p,squares,depth,nextSpecial() & ~BlackCanCastleKingsideMask);
		}
	}
	p++;
	// Generate knight moves
	for (; p & 2; p++) {
		square ns = _pos.squareForPiece(p);
		if (ns & TakenMask) continue;
		knightMoves(ns,squares);
		addAllMoves(p,squares,depth,nextSpecial());
	}
	// Generate bishop moves
	for (; !(p & 2); p++) {
		square bs = _pos.squareForPiece(p);
		if (bs & TakenMask) continue;
		bishopMoves(bs,squares);
		addAllMoves(p,squares,depth,nextSpecial());
	}
	// Generate queen moves
	square qs = _pos.squareForPiece(p);
	if (!(qs & TakenMask)) {
		queenMoves(qs,squares);
		addAllMoves(p,squares,depth,nextSpecial());
	}
	p++;
	// Generate king moves
	square ks = _pos.squareForPiece(p);
	kingMoves(ks,squares);
	addAllMoves(p,squares,depth,nextSpecial() |
			(_color == white ?
			 WhiteCanCastleKingsideMask | WhiteCanCastleQueensideMask :
			 BlackCanCastleKingsideMask | BlackCanCastleQueensideMask));
	_children.shrink_to_fit();
	nodeCount += _children.size();
}
bool Node::grown() const {
	return _special & GrownMask;
}
int Node::evaluate() {
	if (!grown()) {
		_value = intrinsicValue();
	} else if (_color == white) {
		if (_children.size() == 0) {
			if (_pos.inCheck(white)) {
				return _value = BlackMate;
			}
			return Stalemate;
		}
		int maxval = _children[0]->evaluate();
		for (size_t i = 1; i < _children.size(); i++) {
			int val = _children[i]->evaluate();
			if (val > maxval) maxval = val;
		}
		_value = maxval;
	} else {
		if (_children.size() == 0) {
			if (_pos.inCheck(black)) {
				return _value = WhiteMate;
			}
			return Stalemate;
		}
		int minval = _children[0]->evaluate();
		for (size_t i = 1; i < _children.size(); i++) {
			int val = _children[i]->evaluate();
			if (val < minval) minval = val;
		}
		_value = minval;
	}
	return _value;
}
int Node::value() const { return _value; }
// Returns prediction weight for value
float WeightForValue(int value) {
	return expf((float)value/PredictionSpread);
}
void Node::predict(float priority) {
	_priority = priority;
	if (_children.size() == 0) {
		if (!grown()) {
			AddTerminal(this);
		}
		return;
	}
	float weightsum = 0;
	if (_color == white) {
		// Sum prediction weights
		for (size_t i = 0; i < _children.size(); i++) {
			weightsum += WeightForValue(_children[i]->_value);
		}
		// Predict child nodes
		for (size_t i = 0; i < _children.size(); i++) {
			_children[i]->predict(WeightForValue(_children[i]->_value)/weightsum*_priority);
		}
	} else {
		// Sum prediction weights
		for (size_t i = 0; i < _children.size(); i++) {
			weightsum += WeightForValue(-_children[i]->_value);
		}
		// Predict child nodes
		for (size_t i = 0; i < _children.size(); i++) {
			_children[i]->predict(WeightForValue(-_children[i]->_value)/weightsum*_priority);
		}
	}
}
float Node::priority() const { return _priority; }
/* void Node::heal() {
	// This node has just had children calculated, and its value may have
	// changed. Its parent's value may also have changed. Consequently, its
	// probability estimate may also have changed. The game tree must be
	// "healed".
	int val = _value;
	evaluate();
	if (!_parent) {
		predict(_priority);
		return;
	}
	if (val == _value) { // Values healed
		predict(_priority);
	} else {
		_parent->heal();
	}
} */
int Node::numChildren() const {
	return _children.size();
}
color Node::getColor() const { return _color; }
const Node* Node::inheritor() const {
	if (_children.size() == 0) return this;
	Node *max = _children[0], *min = _children[0];
	for (size_t i = 1; i < _children.size(); i++) {
		if (_children[i]->_value > max->_value) max = _children[i];
		if (_children[i]->_value < min->_value) min = _children[i];
	}
	Node *inheritor;
	if (_color == white) {
		inheritor = max;
	} else {
		inheritor = min;
	}
	return inheritor->inheritor();
}
bool Node::whiteCanCastleKingside() const {
	return !(_special & WhiteCanCastleKingsideMask) && !(_pos.squareForPiece(white_r2) & TakenMask);
}
bool Node::whiteCanCastleQueenside() const {
	return !(_special & WhiteCanCastleKingsideMask);
}
bool Node::blackCanCastleKingside() const {
	return !(_special & BlackCanCastleKingsideMask);
}
bool Node::blackCanCastleQueenside() const {
	return !(_special & BlackCanCastleKingsideMask);
}
int Node::movesWithoutPawnMove() const {
	return (_special & MovesWithoutPawnMoveMask) >> MovesWithoutPawnMoveShift;
}
int Node::terminationStatus() const {
	if (!grown()) return TSNotGrown;
	if (_children.size()) return TSNotTerminal;
	if (_pos.inCheck(white)) return TSBlackMate;
	if (_pos.inCheck(black)) return TSWhiteMate;
	return TSStaleMate;
}
Move Node::moveToNode(const Node *node) const {
	if (!grown()) throw runtime_error("can only calculate moves from grown nodes");
	vector<Move> moves;
	Move move;
	for (size_t i = 0; i < _children.size(); i++) {
		if (_children[i] == node) {
			move = _pos.moveToPositionForColor(node->_pos,_color);
		} else {
			moves.push_back(_pos.moveToPositionForColor(_children[i]->_pos,_color));
		}
	}
	bool savefile = 0, saverank = 0;
	for (size_t i = 0; i < moves.size(); i++) {
		if (moves[i].to == move.to &&
			moves[i].piecechar == move.piecechar &&
			moves[i].promotionchar == move.promotionchar) {
			savefile = 1;
			if (moves[i].file == move.file) saverank = 1;
		}
	}
	if (move.piecechar == pawn) move.piecechar = 0;
	if (!savefile) {
		move.file = 0;
		move.rank = 0;
	} else if (!saverank) {
		move.rank = 0;
	}
	return move;
}
Move Node::moveToOnlyChild() const {
	return moveToNode(onlyChild());
}
const Node* Node::onlyChild() const {
	if (_children.size() != 1) throw runtime_error("number of children not 1");
	return _children[0];
}
void Node::deleteChildrenExcept(Node *inheritor) {
	for (size_t i = 0; i < _children.size(); i++) {
		if (_children[i] != inheritor) delete _children[i];
	}
	_children.clear();
	_children.push_back(inheritor);
}
void Node::printGame(ostream &out) const {
	const Node *node = this;
	while (node->_parent) node = node->_parent;
	out << _pos.stringForFileLabels();
	if (node != this) out << "       White     Black\n";
	else out << '\n';
	out << _pos.stringForTopBorder();
	if (node != this) out << "       --------- ---------\n";
	else out << '\n';
	int moveno = 1;
	for (int i = 0; i < 17 || node != this; i++) {
		if (i % 2 == 0 && i < 15) {
			out << _pos.stringForRank(8-i/2);
		} else if (i/2 < 8 - 1 && i < 15) {
			out << _pos.stringForRankSeparator();
		} else if (i == 15) {
			out << _pos.stringForBottomBorder();
		} else if (i == 16) {
			out << _pos.stringForFileLabels();
		} else {
            out << "                                       ";
		}
		if (node != this) {
			out << "   " << to_string(moveno) << '.' << string(3-to_string(moveno).length(),' ');
			Move m = node->moveToOnlyChild();
			string ms = string(m);
			out << ms;
			out << string(10-ms.length(),' ');
			node = node->onlyChild();
			if (node != this) {
				m = node->moveToOnlyChild();
				out << m;
				node = node->onlyChild();
			}
			moveno++;
		}
		out << '\n';
	}
}
int Node::intrinsicValue() const {
	return _pos.value();
}
void Node::pawnMoves(square ps, square *squares) {
	int i = 0;
	square cs;
	if (_color == white) {
		cs = SquareAbove(ps);
		if (_pos.squareColor(cs) == emptycolor) {
			squares[i++] = cs;
			if (RankIdxForSquare(ps) == 1) {
				// Double-step from starting rank
				cs = SquareAbove(cs);
				if (_pos.squareColor(cs) == emptycolor) squares[i++] = cs;
			}
		}
		// Captures
		if (HasSquareAboveRight(ps)) {
			cs = SquareAboveRight(ps);
			if (_pos.squareColor(cs) == OppositeColor(_color)) squares[i++] = cs | TakesMask;
		}
		if (HasSquareAboveLeft(ps)) {
			cs = SquareAboveLeft(ps);
			if (_pos.squareColor(cs) == OppositeColor(_color)) squares[i++] = cs | TakesMask;
		}
	} else { // Black
		cs = SquareBelow(ps);
		if (_pos.squareColor(cs) == emptycolor) {
			squares[i++] = cs;
			if (RankIdxForSquare(ps) == 6) {
				// Double-step from starting rank
				cs = SquareBelow(cs);
				if (_pos.squareColor(cs) == emptycolor) squares[i++] = cs;
			}
		}
		// Captures
		if (HasSquareBelowRight(ps)) {
			cs = SquareBelowRight(ps);
			if (_pos.squareColor(cs) == OppositeColor(_color)) squares[i++] = cs | TakesMask;
		}
		if (HasSquareBelowLeft(ps)) {
			cs = SquareBelowLeft(ps);
			if (_pos.squareColor(cs) == OppositeColor(_color)) squares[i++] = cs | TakesMask;
		}
	}
	squares[i++] = nosquare;
}
void Node::rookMoves(square rs, square *squares) {
	int i = 0;
	square cs; // Current square
	if (HasSquareAbove(rs)) { // Move up
		cs = SquareAbove(rs);
		while (1) {
			color c = _pos.squareColor(cs);
			if (c == emptycolor) {
				squares[i++] = cs;
				if (!HasSquareAbove(cs)) break;
				cs = SquareAbove(cs);
			} else if (c != _color) { // Take piece
				squares[i++] = cs | TakesMask;
				break;
			} else break;
		}
	}
	if (HasSquareBelow(rs)) { // Move up
		cs = SquareBelow(rs);
		while (1) {
			color c = _pos.squareColor(cs);
			if (c == emptycolor) {
				squares[i++] = cs;
				if (!HasSquareBelow(cs)) break;
				cs = SquareBelow(cs);
			} else if (c != _color) { // Take piece
				squares[i++] = cs | TakesMask;
				break;
			} else break;
		}
	}
	if (HasSquareRight(rs)) { // Move right
		cs = SquareRight(rs);
		while (1) {
			color c = _pos.squareColor(cs);
			if (c == emptycolor) {
				squares[i++] = cs;
				if (!HasSquareRight(cs)) break;
				cs = SquareRight(cs);
			} else if (c != _color) { // Take piece
				squares[i++] = cs | TakesMask;
				break;
			} else break;
		}
	}
	if (HasSquareLeft(rs)) { // Move left
		cs = SquareLeft(rs);
		while (1) {
			color c = _pos.squareColor(cs);
			if (c == emptycolor) {
				squares[i++] = cs;
				if (!HasSquareLeft(cs)) break;
				cs = SquareLeft(cs);
			} else if (c != _color) { // Take piece
				squares[i++] = cs | TakesMask;
				break;
			} else break;
		}
	}
	squares[i++] = nosquare;
}
void Node::knightMoves(square ns, square *squares) {
	int i = 0;
	square cs;
	color c;
	if (HasSquareUUR(ns)) {
		cs = SquareUUR(ns);
		c = _pos.squareColor(cs);
		if (c == emptycolor) squares[i++] = cs;
		else if (c != _color) squares[i++] = cs | TakesMask;
	}
	if (HasSquareUUL(ns)) {
		cs = SquareUUL(ns);
		c = _pos.squareColor(cs);
		if (c == emptycolor) squares[i++] = cs;
		else if (c != _color) squares[i++] = cs | TakesMask;
	}
	if (HasSquareRRU(ns)) {
		cs = SquareRRU(ns);
		c = _pos.squareColor(cs);
		if (c == emptycolor) squares[i++] = cs;
		else if (c != _color) squares[i++] = cs | TakesMask;
	}
	if (HasSquareRRD(ns)) {
		cs = SquareRRD(ns);
		c = _pos.squareColor(cs);
		if (c == emptycolor) squares[i++] = cs;
		else if (c != _color) squares[i++] = cs | TakesMask;
	}
	if (HasSquareLLU(ns)) {
		cs = SquareLLU(ns);
		c = _pos.squareColor(cs);
		if (c == emptycolor) squares[i++] = cs;
		else if (c != _color) squares[i++] = cs | TakesMask;
	}
	if (HasSquareLLD(ns)) {
		cs = SquareLLD(ns);
		c = _pos.squareColor(cs);
		if (c == emptycolor) squares[i++] = cs;
		else if (c != _color) squares[i++] = cs | TakesMask;
	}
	if (HasSquareDDR(ns)) {
		cs = SquareDDR(ns);
		c = _pos.squareColor(cs);
		if (c == emptycolor) squares[i++] = cs;
		else if (c != _color) squares[i++] = cs | TakesMask;
	}
	if (HasSquareDDL(ns)) {
		cs = SquareDDL(ns);
		c = _pos.squareColor(cs);
		if (c == emptycolor) squares[i++] = cs;
		else if (c != _color) squares[i++] = cs | TakesMask;
	}
	squares[i++] = nosquare;
}
void Node::bishopMoves(square bs, square *squares) {
	int i = 0;
	square cs; // Current square
	if (HasSquareAboveRight(bs)) { // Move up and to the right
		cs = SquareAboveRight(bs);
		while (1) {
			color c = _pos.squareColor(cs);
			if (c == emptycolor) {
				squares[i++] = cs;
				if (!HasSquareAboveRight(cs)) break;
				cs = SquareAboveRight(cs);
			} else if (c != _color) { // Take piece
				squares[i++] = cs | TakesMask;
				break;
			} else break;
		}
	}
	if (HasSquareAboveLeft(bs)) { // Move up and to the left
		cs = SquareAboveLeft(bs);
		while (1) {
			color c = _pos.squareColor(cs);
			if (c == emptycolor) {
				squares[i++] = cs;
				if (!HasSquareAboveLeft(cs)) break;
				cs = SquareAboveLeft(cs);
			} else if (c != _color) { // Take piece
				squares[i++] = cs | TakesMask;
				break;
			} else break;
		}
	}
	if (HasSquareBelowRight(bs)) { // Move down and to the right
		cs = SquareBelowRight(bs);
		while (1) {
			color c = _pos.squareColor(cs);
			if (c == emptycolor) {
				squares[i++] = cs;
				if (!HasSquareBelowRight(cs)) break;
				cs = SquareBelowRight(cs);
			} else if (c != _color) { // Take piece
				squares[i++] = cs | TakesMask;
				break;
			} else break;
		}
	}
	if (HasSquareBelowLeft(bs)) { // Move down and to the left
		cs = SquareBelowLeft(bs);
		while (1) {
			color c = _pos.squareColor(cs);
			if (c == emptycolor) {
				squares[i++] = cs;
				if (!HasSquareBelowLeft(cs)) break;
				cs = SquareBelowLeft(cs);
			} else if (c != _color) { // Take piece
				squares[i++] = cs | TakesMask;
				break;
			} else break;
		}
	}
	squares[i++] = nosquare;
}
void Node::queenMoves(square qs, square *squares) {
	rookMoves(qs,squares);
	for (; *squares != nosquare; squares++);
	bishopMoves(qs,squares);
}
void Node::kingMoves(square ks, square *squares) {
	int i = 0;
	square cs;
	color c;
	// NOTE: could make more efficient
	if (HasSquareAboveRight(ks)) {
		cs = SquareAboveRight(ks);
		c = _pos.squareColor(cs);
		if (c == emptycolor) {
			squares[i++] = cs;
		} else if (c != _color) {
			squares[i++] = cs | TakesMask;
		}
	}
	if (HasSquareRight(ks)) {
		cs = SquareRight(ks);
		c = _pos.squareColor(cs);
		if (c == emptycolor) {
			squares[i++] = cs;
		} else if (c != _color) {
			squares[i++] = cs | TakesMask;
		}
	}
	if (HasSquareBelowRight(ks)) {
		cs = SquareBelowRight(ks);
		c = _pos.squareColor(cs);
		if (c == emptycolor) {
			squares[i++] = cs;
		} else if (c != _color) {
			squares[i++] = cs | TakesMask;
		}
	}
	if (HasSquareBelow(ks)) {
		cs = SquareBelow(ks);
		c = _pos.squareColor(cs);
		if (c == emptycolor) {
			squares[i++] = cs;
		} else if (c != _color) {
			squares[i++] = cs | TakesMask;
		}
	}
	if (HasSquareBelowLeft(ks)) {
		cs = SquareBelowLeft(ks);
		c = _pos.squareColor(cs);
		if (c == emptycolor) {
			squares[i++] = cs;
		} else if (c != _color) {
			squares[i++] = cs | TakesMask;
		}
	}
	if (HasSquareLeft(ks)) {
		cs = SquareLeft(ks);
		c = _pos.squareColor(cs);
		if (c == emptycolor) {
			squares[i++] = cs;
		} else if (c != _color) {
			squares[i++] = cs | TakesMask;
		}
	}
	if (HasSquareAboveLeft(ks)) {
		cs = SquareAboveLeft(ks);
		c = _pos.squareColor(cs);
		if (c == emptycolor) {
			squares[i++] = cs;
		} else if (c != _color) {
			squares[i++] = cs | TakesMask;
		}
	}
	if (HasSquareAbove(ks)) {
		cs = SquareAbove(ks);
		c = _pos.squareColor(cs);
		if (c == emptycolor) {
			squares[i++] = cs;
		} else if (c != _color) {
			squares[i++] = cs | TakesMask;
		}
	}
	squares[i++] = nosquare;
}
Node* Node::applyMove(Move m) {
	if (!grown()) {
		throw runtime_error("must move in grown node");
	}
	// Find the child(ren) compatible with the given move
	vector<Node*> compats;
	compats.reserve(3);
	for (size_t i = 0; i < _children.size(); i++) {
		if (moveCompatible(m,_children[i]->_pos)) compats.push_back(_children[i]);
	}
	if (compats.size() == 0) throw IllegalMove();
	if (compats.size() > 1) throw AmbiguousMove();
	Node *inheritor = compats[0];
	// Delete the other children
	//deleteChildrenExcept(inheritor);
	return inheritor;
}
Node* Node::playFor(color c) {
	if (_children.size() == 0) {
		throw runtime_error("can't play terminal node");
	}
	// TODO: special instructions for rook mate, queen mate, double bishop mate?
	Node *max = _children[0], *min = _children[0];
	for (size_t i = 1; i < _children.size(); i++) {
		if (_children[i]->_value > max->_value) max = _children[i];
		if (_children[i]->_value < min->_value) min = _children[i];
	}
	Node *inheritor;
	if (c == white) {
		inheritor = max;
	} else {
		inheritor = min;
	}
	deleteChildrenExcept(inheritor);
	return inheritor;
}
ostream& operator<<(ostream &out, const Node &n) {
	out << n._pos;
	return out;
}
void Node::printWithChildren(ostream &out, int depth) const {
	out << this << '\n';
	out << *this;
	out << "Intrinsic value: " << intrinsicValue() << '\n';
	out << "Inherited value: " << _value << '\n';
	out << "Turn: " << (_color == white ? "White" : "Black" ) << '\n';
	if (_children.size() == 0 || depth == 0) {
		return;
	}
	out << "Children:\n";
	for (size_t i = 0; i < _children.size(); i++) {
		out << _children[i] << "\n";
	}
	out << '\n';
	for (size_t i = 0; i < _children.size(); i++) {
		_children[i]->printWithChildren(out,depth-1);
	}
	out << '\n';
}
bool Node::moveCompatible(Move m, const Position &pos) {
	// Given that the given position is the result of a legal move from this
	// node's position, determine whether the given move (which may or may not
	// be legal) describes the change.
	
	// Castling
	if (m.special & smCastleKingside) {
		piece kp = _color == white ? white_k : black_k;
		piece rp = _color == white ? white_r2 : black_r2;
		return pos.squareForPiece(kp) != _pos.squareForPiece(kp) &&
			pos.squareForPiece(rp) != _pos.squareForPiece(rp);
	}
	if (m.special & smCastleQueenside) {
		piece kp = _color == white ? white_k : black_k;
		piece rp = _color == white ? white_r1 : black_r1;
		return pos.squareForPiece(kp) != _pos.squareForPiece(kp) &&
			pos.squareForPiece(rp) != _pos.squareForPiece(rp);
	}
	// If this is marked as a capture, a piece of the opposite color must have
	// If this is marked as a capture, a piece of the opposite color must have
	// been captured.
	if (m.takes &&
			_pos.numForColor(OppositeColor(_color)) !=
			pos.numForColor(OppositeColor(_color)) + 1) {
		return false;
	}
	// If a file is given, a piece of the current color must have moved from that file.
	if (m.file && _pos.numForColorInFile(_color,m.file) != pos.numForColorInFile(_color,m.file) + 1) return false;
	// If a file and rank are given, a piece of the current color must have moved from that square.
	if (m.file && m.rank) {
		square s = SquareWithFileAndRankChars(m.file,m.rank);
		if (_pos.squareColor(s) != _color || pos.squareColor(s) == _color) return false;
	}
	// Not moving is not moving
	piece p = pos.pieceForSquareAndColor(m.to,_color);
	if (_pos.pieceForSquareAndColor(m.to,_color) == p) return false;
	// Promotion
	if (m.promotionchar) {
		int before = _pos.numForPieceChar(m.promotionchar,_color);
		int after = pos.numForPieceChar(m.promotionchar,_color);
		return after == before + 1 && PieceCharForIdx(pos.pieceForSquareAndColor(m.to,_color)) == m.promotionchar;
	}
	// You must have moved the right piece
	if (m.piecechar != PieceCharForIdx(p)) return false;
	return true;
}
Node::SpecialType Node::nextSpecialWithMovesWithoutPawnMove(int moves) {
	return (_special & ~(MovesWithoutPawnMoveMask|GrownMask)) | ((moves << MovesWithoutPawnMoveShift) & MovesWithoutPawnMoveMask);
}
Node::SpecialType Node::nextSpecial() {
	return nextSpecialWithMovesWithoutPawnMove(movesWithoutPawnMove()+1);
}

Node* terms[MaxNumTerminals+2];
int numTerms = 0;
void AddTerminal(Node *term) {
	// Insert new element
	terms[++numTerms] = term;
	int i = numTerms/2; // parent
	int j = numTerms; // child
	while (i != j && i > 0) {
		if (terms[i]->priority() > terms[j]->priority()) {
			Node *temp = terms[i];
			terms[i] = terms[j];
			terms[j] = temp;
		}
		j = i;
		i /= 2;
	}
	if (numTerms > MaxNumTerminals) {
		terms[1] = terms[MaxNumTerminals];
		int i = 1;
		int j = 2;
		while (j < MaxNumTerminals) {
			if (terms[i]->priority() > terms[j]->priority()) {
				if (terms[j]->priority() > terms[j+1]->priority()) {
					j = j + 1;
				}
				Node *temp = terms[i];
				terms[i] = terms[j];
				terms[j] = temp;
			}
			i = j;
			j *= 2;
		}
		numTerms--;
	}
}
