# Chess Game (C++ & SFML)

## Description
A fully functional chess game built from scratch using C++ and SFML. It features piece validation, drag-and-drop movement, turn-based play, and game state detection (Check, Checkmate, Stalemate).

## Features
* **Drag & Drop:** Smooth visual movement for all pieces.
* **Move Validation:** Enforces rules for Pawn, Rook, Knight, Bishop, Queen, and King.
* **Visual Feedback:**
    * **Green Squares:** Valid moves.
    * **Red Squares:** Capture targets.
* **Game Rules:**
    * Turn-based system (White/Black).
    * Pawn Promotion (with graphical selection menu).
    * Check, Checkmate, and Stalemate detection.
* **UI:**
    * Score tracking on board
    * Board coordinates (1-8, a-h).

## How to Run
1.  Ensure you have Visual Studio and SFML configured.
2.  Place the `images` folder (containing wP.png, etc.) and `arial.ttf` in the same directory as the executable.
3.  Run the .exe file.

## Controls
* **Mouse Left Click:** Select and drag pieces.
* **Mouse Release:** Drop pieces to move.
