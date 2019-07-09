# tchess
Play chess in a text-based environment!

```
 ------------------------------------------------------------------ 
|                             tchess                               |
 ------------------------------------------------------------------ 

     a   b   c   d   e   f   g   h            White     Black
    -------------------------------           --------- ---------
 8 | R | N | B | Q | K | B | N | R | 8    1.  d4        d5
   |---+---+---+---+---+---+---+---|   
 7 | p | p | p |   | p | p | p | p | 7 
   |---+---+---+---+---+---+---+---|   
 6 |   |   |   |   |   |   |   |   | 6 
   |---+---+---+---+---+---+---+---|   
 5 |   |   |   | p |   |   |   |   | 5 
   |---+---+---+---+---+---+---+---|   
 4 |   |   |   | p |   |   |   |   | 4 
   |---+---+---+---+---+---+---+---|   
 3 |   |   |   |   |   |   |   |   | 3 
   |---+---+---+---+---+---+---+---|   
 2 | p | p | p |   | p | p | p | p | 2 
   |---+---+---+---+---+---+---+---|   
 1 | R | N | B | Q | K | B | N | R | 1 
    -------------------------------    
     a   b   c   d   e   f   g   h     

White move: 
```

Need I say more?

## Installation
Simply `make` in the project directory and it will build the executable.

## Gameplay
Enter your moves in algebraic notation. By default the user plays for white. You can play for black by using the `--black` command-line argument. Resign with `resign`. Offer a draw with `draw`.

A log of the game is saved by default to `tchess.log`. You can change the filename with the `--log` command-line argument.

The program understands most of the special moves including pawn promotion and castling, but does not understand *en passant*.

## How Does It Work?

tchess plays by building the "game tree" from the current position using the rules of chess. Then it selects the most likely outcomes using the minimax algorithm.
