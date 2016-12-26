//
// main.cpp
//

#include "board.h"
#include "node.h"
#include "move.h"

#include <atomic>
#include <iostream>
#include <string>
#include <cstring>
#include <stdexcept>
#include <queue>
#include <list>

using namespace std;

atomic_size_t nodeCount; // Total number of nodes
Node start(StandardDepth); // Initial position
Node *current = &start; // Current position
std::list<Move> moves;

string GetResponse(const string &msg) {
	cout << msg;
	string resp;
	getline(cin,resp);
	return resp;
}

int GetYesNo(const char *msg) {
	while (1) {
		string resp = GetResponse(msg);
		if (resp.length() == 0 && cin.eof()) return 2;
		if (resp[0] == 'y' || resp[0] == 'Y') return 1;
		if (resp[0] == 'n' || resp[0] == 'N') return 0;
		cout << "I didn't catch that. Please try again.\n";
	}
}

void Frame(const string &msg, char framechar='-') {
	size_t limit = msg.length() + 2;
	cout << "\n ";
	for (size_t i = 0; i < limit; i++) {
		cout << framechar;
	}
	cout << " \n| " << msg << " |\n";
	cout << ' ';
	for (size_t i = 0; i < limit; i++) {
		cout << framechar;
	}
	cout << " \n\n";
}

void PrintGame() {
	// Clear the screen and move cursor to top left
	cout << "\033[2J\033[H";
	Frame("                            tchess                              ");
	current->printGame(cout,moves);
	cout << '\n';
}
int main(int argc, char* argv[]) {
	color playfor = black;
	try {
	// Process command-line arguments
	int argstate = 0;
	for (int i = 1; i < argc; i++) {
		const char *arg = argv[i];
		switch (argstate) {
		case 0:
			if (strcmp(arg,"--white") == 0) {
				playfor = black;
			} else if (strcmp(arg,"--black") == 0) {
				playfor = white;
			} else {
				throw runtime_error("unrecognized command-line argument");
			}
			break;
		case 1:
			break;
		default:
			throw runtime_error("bad arg state");
		}
	}
	if (argstate != 0) {
		throw runtime_error("expected another command-line argument");
	}
	//
	// Main program loop
	//
	while (1) {
		Move m;
		string resp;
		while (1) {
			// Print the current position and game history
			PrintGame();
			// Check termination status
			int ts = current->terminationStatus();
			switch (ts) {
			case TSNotTerminal: break;
			case TSWhiteMate:
				Frame("Checkmate. White wins");
				return 0;
			case TSBlackMate:
				Frame("Checkmate. Black wins");
				return 0;
			case TSStaleMate:
				Frame("Stalemate");
				return 0;
			}
			if (current->getColor() == playfor) {
				// Play chess
				cout << "Thinking...\n";
				// Grow current node to minimum depth
				current->grow(MinimumDepth);
				// Use up all available memory
				while (nodeCount < MaxNumNodes) {
					// Re-evaluate entire tree
					current->evaluate();
					// Empty terminal node heap
					numTerms = 0;
					// Re-predict entire tree
					current->predict(1.0);
					if (numTerms == 0) break;
					// Grow terminal nodes
					for (int i = 1; i < numTerms+1; i++) {
						terms[i]->grow(StandardDepth);
					}
				}
				// Re-evaluate entire tree
				current->evaluate();
				// Make a move
				Move m;
				current = current->playFor(current->getColor(),&m);
				moves.push_back(m);
			} else {
				// Get user's move
				while (1) {
					resp = GetResponse(current->getColor() == white ? "White move: " : "Black move: ");
					if (cin.eof() || resp == "resign") {
						Frame(current->getColor() == white ? "White resigned" : "Black resigned");
						return 0;
					}
					if (resp == "draw") {
						if (current->value() >= DrawLevel) {
							Frame("Draw accepted");
							return 0;
						} else {
							Frame("Draw not accepted");
							continue;
						}
					}
					if (resp == "help") {
						cout << "Enter a move in algebraic notation or enter one of the following commands:\n";
						cout << "\tresign - resigns\n";
						cout << "\tdraw - offers a draw\n";
						continue;
					}
					try {
						m = Move(resp.c_str());
					} catch (BadMoveDesc &e) {
						cout << "Invalid move. Try again. ";
						continue;
					}
					try {
						// Apply move
						Node *inheritor = current->applyMove(m);
						Move m = current->moveToNode(inheritor);
						moves.push_back(m);
						current->deleteChildrenExcept(inheritor);
						current = inheritor;
						break;
					} catch (AmbiguousMove &e) {
						cout << "Ambiguous move. Try again. ";
						continue;
					} catch (BadMove &e) {
						cout << "Illegal move. Try again. ";
						continue;
					}
				}
			}
		}
	}
	} catch (exception &e) {
		cerr << "A problem occurred: " << e.what() << '\n';
		return 1;
	}
	return 0;
}
