#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

using namespace std;

// GLOBAL VARIABLES 
const int SIZE = 8;
char board[SIZE][SIZE];
bool whiteTurn = true;
bool gameOver = false;
bool isKingInCheck = false; // NEW: Track check state
int whiteScore = 0;
int blackScore = 0;
string statusMsg = "";

// FUNCTION PROTOTYPES 
// piece validation
bool isValidPawnMove(int sx, int sy, int dx, int dy);
bool isValidBishopMove(int sx, int sy, int dx, int dy);
bool isValidRookMove(int sx, int sy, int dx, int dy);
bool isValidKnightMove(int sx, int sy, int dx, int dy);
bool isValidQueenMove(int sx, int sy, int dx, int dy);
bool isValidKingMove(int sx, int sy, int dx, int dy);

// Game Rule Prototypes
bool isCheck(bool whiteKing);
bool isCheckmate(bool whiteKing);
bool isStalemate(bool whiteKing);

// manager function
bool isValidMove(int sx, int sy, int dx, int dy);

// helper functions
void Capture_func(char capturedPiece);
void initializeBoard();
string getPieceName(char p);
int getTextureID(char p);
void resizeView(const sf::Window& window, sf::View& view);
void handlePromotion(int r, int c, sf::RenderWindow& window, const sf::Texture* textures, const float size);
int getPieceValue(char p);
void findKing(bool whiteKing, int& kr, int& kc);

// PIECE LOGIC FUNCTIONS 

// rook logic: straight lines
bool isValidRookMove(int sx, int sy, int dx, int dy) {
    // check straight line
    if (sx != dx && sy != dy) {
        return false;
    }

    // check vertical path
    if (sy == dy) {
        int step = (dx > sx) ? 1 : -1;
        for (int r = sx + step; r != dx; r += step) {
            if (board[r][sy] != ' ') {
                return false; // blocked
            }
        }
    }
    // check horizontal path
    else {
        int step = (dy > sy) ? 1 : -1;
        for (int c = sy + step; c != dy; c += step) {
            if (board[sx][c] != ' ') {
                return false; // blocked
            }
        }
    }
    return true;
}

// knight logic: L-shape jump
bool isValidKnightMove(int sx, int sy, int dx, int dy) {
    int rdiff = (dx > sx) ? dx - sx : sx - dx;
    int cdiff = (dy > sy) ? dy - sy : sy - dy;

    // check L-shape 
    if ((rdiff == 2 && cdiff == 1) || (rdiff == 1 && cdiff == 2)) {
        return true;
    }
    return false;
}

// bishop logic: diagonal lines
bool isValidBishopMove(int sx, int sy, int dx, int dy) {

    int rowdiff = dx - sx;
    int coldiff = dy - sy;

    int absrowdiff = (rowdiff > 0) ? rowdiff : -rowdiff; // absolute values to check equality of difference
    int abscoldiff = (coldiff > 0) ? coldiff : -coldiff;

    if (absrowdiff != abscoldiff)
    {
        return false; // not a diagonal move
    }

    int rowstep, colstep;

    if (rowdiff > 0) { // 1 for down and right ,-1 for up and left
        rowstep = 1;
    }
    else
        rowstep = -1;

    if (coldiff > 0) {
        colstep = 1;
    }
    else
        colstep = -1;

    int r = sx + rowstep;
    int c = sy + colstep;

    while (r != dx && c != dy) { // while destination reaches
        if (board[r][c] != ' ') {//path is bloacked
            return false;
        }
        r += rowstep;
        c += colstep;
    }
    return true;
}


// queen logic: rook + bishop
bool isValidQueenMove(int sx, int sy, int dx, int dy) {
    if (isValidRookMove(sx, sy, dx, dy)) {
        return true;
    }
    if (isValidBishopMove(sx, sy, dx, dy)) {
        return true;
    }
    return false;
}

// king logic: 1 step any direction
bool isValidKingMove(int sx, int sy, int dx, int dy) {
    int rdiff = (dx > sx) ? dx - sx : sx - dx;
    int cdiff = (dy > sy) ? dy - sy : sy - dy;

    if (rdiff <= 1 && cdiff <= 1) {
        return true;
    }
    return false;
}

// pawn logic: forward and capture
bool isValidPawnMove(int sx, int sy, int dx, int dy) {
    char piece = board[sx][sy];
    char dest = board[dx][dy];

    if (piece == 'P') { // white pawn
        if (sy == dy && dx == sx - 1 && dest == ' ') {
            return true;
        }
        if (sx == 6 && sy == dy && dx == sx - 2 && dest == ' ' && board[sx - 1][sy] == ' ') {
            return true;
        }
        if (dx == sx - 1 && (dy == sy - 1 || dy == sy + 1) && (dest >= 'a' && dest <= 'z')) {
            return true;
        }
    }
    else if (piece == 'p') { // black pawn
        if (sy == dy && dx == sx + 1 && dest == ' ') {
            return true;
        }
        if (sx == 1 && sy == dy && dx == sx + 2 && dest == ' ' && board[sx + 1][sy] == ' ') {
            return true;
        }
        if (dx == sx + 1 && (dy == sy - 1 || dy == sy + 1) && (dest >= 'A' && dest <= 'Z')) {
            return true;
        }
    }
    return false;
}

// checks turns and calls helpers
bool isValidMove(int sx, int sy, int dx, int dy) {
    char piece = board[sx][sy];
    char dest = board[dx][dy];

    if (piece == ' ') {
        return false;
    }

    // check turns
    if (whiteTurn && (piece >= 'a' && piece <= 'z')) {
        return false;
    }
    if (!whiteTurn && (piece >= 'A' && piece <= 'Z')) {
        return false;
    }

    // check friendly fire
    if (whiteTurn && (dest >= 'A' && dest <= 'Z')) {
        return false;
    }
    if (!whiteTurn && (dest >= 'a' && dest <= 'z')) {
        return false;
    }

    char p = (piece >= 'A' && piece <= 'Z') ? piece + 32 : piece;

    if (p == 'p') {
        return isValidPawnMove(sx, sy, dx, dy);
    }
    if (p == 'b') {
        return isValidBishopMove(sx, sy, dx, dy);
    }
    if (p == 'r') {
        return isValidRookMove(sx, sy, dx, dy);
    }
    if (p == 'n') {
        return isValidKnightMove(sx, sy, dx, dy);
    }
    if (p == 'q') {
        return isValidQueenMove(sx, sy, dx, dy);
    }
    if (p == 'k') {
        return isValidKingMove(sx, sy, dx, dy);
    }

    return false;
}

//  to find king location of white or black player
void findKing(bool whiteKing, int& kr, int& kc) { //for check/checkmate detection
  
    char target = 0;
    if (whiteKing) { target = 'K'; }
    else { target = 'k'; }

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            if (board[r][c] == target) {
                kr = r; kc = c;
                return;
            }
        }
    }
}

// check if king is under attack
bool isCheck(bool whiteKing) {
    int kr = -1;
    int kc = -1;
    findKing(whiteKing, kr, kc);

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            char p = board[r][c];
            if (p != ' ') {
                bool isWhitePiece = (p >= 'A' && p <= 'Z');
                if (isWhitePiece != whiteKing) {

                    char raw = (p >= 'A' && p <= 'Z') ? p + 32 : p;
                    bool hit = false;

                    if (raw == 'p') { hit = isValidPawnMove(r, c, kr, kc); }
                    else if (raw == 'b') { hit = isValidBishopMove(r, c, kr, kc); }
                    else if (raw == 'r') { hit = isValidRookMove(r, c, kr, kc); }
                    else if (raw == 'n') { hit = isValidKnightMove(r, c, kr, kc); }
                    else if (raw == 'q') { hit = isValidQueenMove(r, c, kr, kc); }
                    else if (raw == 'k') { hit = isValidKingMove(r, c, kr, kc); }

                    if (hit) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

// checkmate detection
bool isCheckmate(bool whiteKing) {
    if (!isCheck(whiteKing)) {
        return false;
    }

    for (int sr = 0; sr < 8; sr++) {
        for (int sc = 0; sc < 8; sc++) {
            char p = board[sr][sc];
            if (p != ' ') {
                bool isWhite = (p >= 'A' && p <= 'Z');
                if (isWhite == whiteKing) {
                    for (int dr = 0; dr < 8; dr++) {
                        for (int dc = 0; dc < 8; dc++) {
                            if (isValidMove(sr, sc, dr, dc)) {
                                // simulate move
                                char temp = board[dr][dc];
                                board[dr][dc] = board[sr][sc];
                                board[sr][sc] = ' ';

                                bool safe = !isCheck(whiteKing);

                                // undo move
                                board[sr][sc] = board[dr][dc];
                                board[dr][dc] = temp;

                                if (safe) {
                                    return false; // found a save
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return true; // no moves left
}

// stalemate detection
bool isStalemate(bool whiteKing) {
    if (isCheck(whiteKing)) {
        return false;
    }

    for (int sr = 0; sr < 8; sr++) {
        for (int sc = 0; sc < 8; sc++) {
            char p = board[sr][sc];
            if (p != ' ') {
                bool isWhite = (p >= 'A' && p <= 'Z');
                if (isWhite == whiteKing) {
                    for (int dr = 0; dr < 8; dr++) {
                        for (int dc = 0; dc < 8; dc++) {
                            if (isValidMove(sr, sc, dr, dc)) {
                                char temp = board[dr][dc];
                                board[dr][dc] = board[sr][sc];
                                board[sr][sc] = ' ';

                                bool check = isCheck(whiteKing);

                                board[sr][sc] = board[dr][dc];
                                board[dr][dc] = temp;

                                if (!check) {
                                    return false; // valid move exists
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return true; // no legal moves
}

// HELPER FUNCTIONS FOR SFML

void initializeBoard() {
    //Initializing with empty squares
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            board[r][c] = ' ';
        }
    }

    // Place black pieces(lowercase)
    board[0][0] = 'r'; // Black Rook 
    board[0][1] = 'n'; // Black Knight
    board[0][2] = 'b'; // Black Bishop
    board[0][3] = 'q'; // Black Queen
    board[0][4] = 'k'; // Black King
    board[0][5] = 'b';
    board[0][6] = 'n';
    board[0][7] = 'r';
    for (int c = 0; c < 8; c++) {
        board[1][c] = 'p'; // Black Pawns 
    }

    // Place white pieces(UPPERCASE)
    board[7][0] = 'R'; // White Rook 
    board[7][1] = 'N'; // White Knight
    board[7][2] = 'B'; // White Bishop
    board[7][3] = 'Q'; // White Queen
    board[7][4] = 'K'; // White King
    board[7][5] = 'B';
    board[7][6] = 'N';
    board[7][7] = 'R';
    for (int c = 0; c < 8; c++) {
        board[6][c] = 'P'; // White Pawns 
    }
}

// helper for scoring
int getPieceValue(char p) {
    char l = (p >= 'A' && p <= 'Z') ? p + 32 : p;
    if (l == 'p') { return 1; }
    if (l == 'n') { return 3; } // Knight = 3
    if (l == 'b') { return 3; } // Bishop = 3
    if (l == 'r') { return 5; }
    if (l == 'q') { return 9; }
    return 0;
}

// capture function
void Capture_func(char capturedPiece) {
    int val = getPieceValue(capturedPiece);
    if (whiteTurn) {
        whiteScore += val;
    }
    else {
        blackScore += val;
    }
    cout << "   CAPTURED! Took piece: " << getPieceName(capturedPiece) << " (+" << val << ")" << endl;
    cout << "   SCORE -> White: " << whiteScore << " | Black: " << blackScore << endl;
}

// pawn promotion with selection
void handlePromotion(int r, int c, sf::RenderWindow& window, const sf::Texture* textures, const float size) {
    bool isWhite = (r == 0);
    if (!((board[r][c] == 'P' && r == 0) || (board[r][c] == 'p' && r == 7))) {
        return; // no promotion needed
    }

    // pause game and show menu
    bool choosing = true;
    sf::RectangleShape menuBox(sf::Vector2f(size * 4, size));
    menuBox.setPosition(c * size - size * 1.5f, r * size); // center over pawn
    // keep inside screen
    if (menuBox.getPosition().x < 0) menuBox.setPosition(0, menuBox.getPosition().y);
    if (menuBox.getPosition().x + size * 4 > 800) menuBox.setPosition(800 - size * 4, menuBox.getPosition().y);

    menuBox.setFillColor(sf::Color(200, 200, 200));
    menuBox.setOutlineColor(sf::Color::Black);
    menuBox.setOutlineThickness(2);

    // options: Q, R, B, N
    sf::Sprite options[4];
    char pieces[4] = { 'Q', 'R', 'B', 'N' };
    if (!isWhite) {
        pieces[0] = 'q'; pieces[1] = 'r'; pieces[2] = 'b'; pieces[3] = 'n';
    }

    for (int i = 0; i < 4; i++) {
        int id = getTextureID(pieces[i]);
        options[i].setTexture(textures[id]);
        options[i].setScale(size / textures[id].getSize().x, size / textures[id].getSize().y);
        options[i].setPosition(menuBox.getPosition().x + i * size, menuBox.getPosition().y);
    }

    while (choosing && window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                window.close();
                return;
            }
            if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i pixel = { e.mouseButton.x, e.mouseButton.y };
                sf::Vector2f world = window.mapPixelToCoords(pixel);

                // check clicks
                for (int i = 0; i < 4; i++) {
                    if (options[i].getGlobalBounds().contains(world)) {
                        board[r][c] = pieces[i];
                        choosing = false;
                        cout << "Promoted to " << getPieceName(pieces[i]) << endl;
                    }
                }
            }
        }

        window.draw(menuBox);
        for (int i = 0; i < 4; i++) {
            window.draw(options[i]);
        }
        window.display();
    }
}

// adjust view on resize
void resizeView(const sf::Window& window, sf::View& view) {
    float aspectRatio = float(window.getSize().x) / float(window.getSize().y);
    float ratio = 1.0f;
    if (aspectRatio > ratio) { // horizontally centering the board and vertically full
        float scale = ratio / aspectRatio;
        view.setViewport(sf::FloatRect((1.0f - scale) / 2.0f, 0.0f, scale, 1.0f));
    }
    else {
        float scale = aspectRatio / ratio;
        view.setViewport(sf::FloatRect(0.0f, (1.0f - scale) / 2.0f, 1.0f, scale));
    }
}

int getTextureID(char p) {
    if (p == 'P') { return 1; } if (p == 'R') { return 2; } if (p == 'N') { return 3; }
    if (p == 'B') { return 4; } if (p == 'Q') { return 5; } if (p == 'K') { return 6; }
    if (p == 'p') { return 7; } if (p == 'r') { return 8; } if (p == 'n') { return 9; }
    if (p == 'b') { return 10; } if (p == 'q') { return 11; } if (p == 'k') { return 12; }
    return 0;
}

string getPieceName(char p) {
    if (p == 'P') { return "White Pawn"; }
    if (p == 'p') { return "Black Pawn"; }
    if (p == 'R') { return "White Rook"; }
    if (p == 'r') { return "Black Rook"; }
    if (p == 'N') { return "White Knight"; }
    if (p == 'n') { return "Black Knight"; }
    if (p == 'B') { return "White Bishop"; }
    if (p == 'b') { return "Black Bishop"; }
    if (p == 'Q') { return "White Queen"; }
    if (p == 'q') { return "Black Queen"; }
    if (p == 'K') { return "White King"; }
    if (p == 'k') { return "Black King"; }
    return "Empty";
}

int main()
{
    //  INITIALIZE BOARD 
    initializeBoard();

    // create game window
    sf::RenderWindow window(sf::VideoMode(800, 800), "Chess Phase 5");
    sf::View view(sf::FloatRect(0.0f, 0.0f, 800.0f, 800.0f));
    window.setView(view);

    const float size = 100.0f;
    sf::RectangleShape squares[8][8];

    // font setup for board text
    sf::Font font;
  
    bool hasFont = font.loadFromFile("arial.ttf");
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(20);
    text.setFillColor(sf::Color::Black);

    // score text setup
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::Red);
    scoreText.setStyle(sf::Text::Bold);
    scoreText.setOutlineColor(sf::Color::White);
    scoreText.setOutlineThickness(2);

    // set colors
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            squares[r][c].setSize(sf::Vector2f(size, size));
            squares[r][c].setPosition(c * size, r * size);
            if ((r + c) % 2 == 0) {
                squares[r][c].setFillColor(sf::Color(230, 235, 240)); // light blue
            }
            else {
                squares[r][c].setFillColor(sf::Color(60, 90, 140)); // dark blue
            }
        }
    }

    // load piece images
    sf::Texture tex[13];
    if (!tex[1].loadFromFile("wP.png")) { cout << "error loading wP.png" << endl; }
    tex[2].loadFromFile("wR.png"); tex[3].loadFromFile("wN.png");
    tex[4].loadFromFile("wB.png"); tex[5].loadFromFile("wQ.png"); tex[6].loadFromFile("wK.png");
    tex[7].loadFromFile("bP.png"); tex[8].loadFromFile("bR.png"); tex[9].loadFromFile("bN.png");
    tex[10].loadFromFile("bB.png"); tex[11].loadFromFile("bQ.png"); tex[12].loadFromFile("bK.png");

    sf::Sprite sprite[8][8];

    // drag state variables
    bool dragging = false;
    int dr = -1, dc = -1;
    sf::Sprite mover;

    // selection highlight box
    sf::RectangleShape selector(sf::Vector2f(size, size));
    selector.setFillColor(sf::Color(0, 255, 0, 100));
    bool selected = false;

    // valid move hints
    sf::RectangleShape hints[64];
    int hCount = 0;

    //  GAME LOOP 
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::Resized) {
                resizeView(window, view);
                window.setView(view);
            }

            if (gameOver) {
                // stop input
            }
            else {
                // handle mouse click
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
                {
                    sf::Vector2i pixel = { event.mouseButton.x, event.mouseButton.y };
                    sf::Vector2f world = window.mapPixelToCoords(pixel);
                    int c = world.x / size;
                    int r = world.y / size;

                    if (r >= 0 && r < 8 && c >= 0 && c < 8) {

                        char ac = 'a' + c;// convert column to letter ,algabric cordinates
                        int ar = 8 - r;// convert row to chess numbers
                        char p = board[r][c];
                        cout << "Clicked: " << ac << ar << " (Row " << r << ", Col " << c << ")";
                        if (p != ' ') {
                            cout << " -> " << getPieceName(p);
                        }
                        cout << endl;

                        selected = true;
                        selector.setPosition(c * size, r * size);

                        if (p != ' ') {
                            dragging = true;
                            dr = r; dc = c;
                            int id = getTextureID(p);
                            mover.setTexture(tex[id]);
                            mover.setScale(size / tex[id].getSize().x, size / tex[id].getSize().y);
                            mover.setPosition(world.x - size / 2, world.y - size / 2); // center of cursor

                            // show valid moves with red capture hint
                            hCount = 0;
                            for (int checkRow = 0; checkRow < 8; checkRow++) {
                                for (int checkCol = 0; checkCol < 8; checkCol++) {
                                    if (isValidMove(dr, dc, checkRow, checkCol)) {
                                        hints[hCount].setSize(sf::Vector2f(size, size));
                                        hints[hCount].setPosition(checkCol * size, checkRow * size);

                                        // red for capture
                                        // check valid capture target 
                                        char targetP = board[checkRow][checkCol];
                                        bool isCapture = false;
                                        if (targetP != ' ') {
                                            isCapture = true;
                                        }

                                        if (isCapture) {
                                            hints[hCount].setFillColor(sf::Color(255, 0, 0, 100));
                                        }
                                        else {
                                            hints[hCount].setFillColor(sf::Color(0, 255, 0, 100));
                                        }
                                        hCount++;
                                    }
                                }
                            }
                        }
                        else {
                            hCount = 0;
                        }
                    }
                }

                // update dragged piece
                if (event.type == sf::Event::MouseMoved && dragging) {
                    sf::Vector2i pixel = { event.mouseMove.x, event.mouseMove.y };
                    sf::Vector2f world = window.mapPixelToCoords(pixel);
                    mover.setPosition(world.x - size / 2, world.y - size / 2);
                }

                // drop the piece
                if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left && dragging)
                {
                    sf::Vector2i pixel = { event.mouseButton.x, event.mouseButton.y };
                    sf::Vector2f world = window.mapPixelToCoords(pixel);
                    int nc = world.x / size;
                    int nr = world.y / size;

                    if (nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
                        if (isValidMove(dr, dc, nr, nc)) {

                            char tempDest = board[nr][nc];
                            board[nr][nc] = board[dr][dc];
                            board[dr][dc] = ' ';

                            // check self-check
                            if (isCheck(whiteTurn)) {
                                // undo unsafe move
                                board[dr][dc] = board[nr][nc];
                                board[nr][nc] = tempDest;
                                cout << "invalid: king is in check" << endl;
                            }
                            else {
                                // valid move

                                // check capture
                                if (tempDest != ' ') {
                                    Capture_func(tempDest);
                                }

                                // pawn promotion check and menu
                                handlePromotion(nr, nc, window, tex, size);

                                whiteTurn = !whiteTurn;
                                cout << "Move Valid. " << (whiteTurn ? "White" : "Black") << "'s turn." << endl;

                                // check game over conditions
                                if (isCheckmate(whiteTurn)) {
                                    statusMsg = (whiteTurn ? "Black" : "White");
                                    statusMsg += " Wins!";
                                    if (whiteTurn) { // Black Won
                                        statusMsg += "\nBlack Score: " + to_string(blackScore);
                                    }
                                    else { // White Won
                                        statusMsg += "\nWhite Score: " + to_string(whiteScore);
                                    }
                                    cout << statusMsg << endl;
                                    gameOver = true;
                                }
                                else if (isStalemate(whiteTurn)) {
                                    statusMsg = "Draw!";
                                    statusMsg += "\nBlack Score: " + to_string(blackScore);
                                    cout << statusMsg << endl;
                                    gameOver = true;
                                }
                                else if (isCheck(whiteTurn)) {
                                    cout << "CHECK!" << endl;
                                    isKingInCheck = true; // FIXED: Enable check text
                                }
                                else {
                                    isKingInCheck = false; // FIXED: Disable check text if safe
                                }
                            }
                        }
                        else {
                            cout << "Invalid Move!" << endl;
                        }
                    }
                    dragging = false;
                    hCount = 0;
                }
            } // end of else block
        }

        window.clear();
        window.setView(view);

        // Draw Board
        for (int r = 0; r < 8; ++r) {
            for (int c = 0; c < 8; ++c) {
                window.draw(squares[r][c]);

                // draw coordinates
                if (hasFont) {
                    // 1-8 left
                    if (c == 0) {
                        text.setString(to_string(8 - r));
                        // center in vertical space of tile
                        text.setPosition(5, r * size + size / 2 - 10);
                        window.draw(text);
                    }
                    // a-h bottom (last row)
                    if (r == 7) {
                        string s = "";
                        s += (char)('a' + c);
                        text.setString(s);
                        // bottom left corner of the box
                        text.setPosition(c * size + 5, (r + 1) * size - 25);
                        window.draw(text);
                    }
                }
            }
        }

        // Draw Highlights
        if (selected) {
            window.draw(selector);
        }
        for (int i = 0; i < hCount; i++) {
            window.draw(hints[i]);
        }

        // Draw Chess Pieces (Sprites)
        for (int r = 0; r < 8; r++) {
            for (int c = 0; c < 8; c++) {
                if (!(dragging && r == dr && c == dc)) {
                    char p = board[r][c];
                    int id = getTextureID(p);
                    if (id != 0) {
                        sprite[r][c].setTexture(tex[id]);
                        sprite[r][c].setScale(size / tex[id].getSize().x, size / tex[id].getSize().y);
                        sprite[r][c].setPosition(c * size, r * size);
                        window.draw(sprite[r][c]);
                    }
                }
            }
        }

        if (hasFont) {
            scoreText.setCharacterSize(24);
            scoreText.setOrigin(0, 0);

            // set score text color
            scoreText.setFillColor(sf::Color::Red);

            scoreText.setString("W: " + to_string(whiteScore));
            scoreText.setPosition(10, 10);
            window.draw(scoreText);

            scoreText.setString("B: " + to_string(blackScore));
            scoreText.setPosition(700, 10);
            window.draw(scoreText);

            //  Draw Check Notification
            if (isKingInCheck && !gameOver) {
                scoreText.setString("CHECK!");
                sf::FloatRect checkRect = scoreText.getLocalBounds();
                scoreText.setOrigin(checkRect.left + checkRect.width / 2.0f, checkRect.top + checkRect.height / 2.0f);
                scoreText.setPosition(265, 400); // Center of board
                scoreText.setCharacterSize(80);
                scoreText.setFillColor(sf::Color::Red);
                scoreText.setOutlineColor(sf::Color::White);
                scoreText.setOutlineThickness(3);
                window.draw(scoreText);
            }

            if (gameOver) {
                // big game over text
                scoreText.setString(statusMsg);
                sf::FloatRect textRect = scoreText.getLocalBounds();
                scoreText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
                scoreText.setPosition(265, 400); // center of 800x800
                scoreText.setCharacterSize(60);
                scoreText.setOutlineThickness(4);
                scoreText.setOutlineColor(sf::Color::Black);
                scoreText.setFillColor(sf::Color::Green);

            

                window.draw(scoreText);
            }
        }

        // Draw Dragged Piece (Always on top)
        if (dragging) {
            window.draw(mover);
        }

        window.display();
    }
    return 0;
}