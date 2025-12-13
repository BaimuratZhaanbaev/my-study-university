#include <iostream>
#include <cmath>
#include <cstdlib>

class Piece {
protected:
    int row, col;
    bool isWhite;

    bool isPathClear(int toRow, int toCol, Piece* const board[8][8]) const {
        int dr = toRow - row;
        int dc = toCol - col;
        int steps = std::max(std::abs(dr), std::abs(dc));
        if (steps <= 1) return true;

        int stepRow = (dr > 0) ? 1 : (dr < 0) ? -1 : 0;
        int stepCol = (dc > 0) ? 1 : (dc < 0) ? -1 : 0;

        for (int i = 1; i < steps; ++i) {
            if (board[row + i * stepRow][col + i * stepCol] != nullptr)
                return false;
        }
        return true;
    }

public:
    Piece(int r, int c, bool white) : row(r), col(c), isWhite(white) {}
    virtual ~Piece() = default;

    virtual bool isValidMove(int toRow, int toCol, Piece* const board[8][8]) const = 0;

    virtual char getSymbol() const = 0;

    int getRow() const { return row; }
    int getCol() const { return col; }
    bool getColor() const { return isWhite; }
    void setPosition(int r, int c) { row = r; col = c; }
};

class Pawn : public Piece {
public:
    Pawn(int r, int c, bool white) : Piece(r, c, white) {}

    bool isValidMove(int toRow, int toCol, Piece* const board[8][8]) const override {
        int dr = toRow - row;
        int dc = toCol - col;
        int direction = isWhite ? 1 : -1;

        if (dc == 0 && dr == direction && board[toRow][toCol] == nullptr)
            return true;

        if (dc == 0 && dr == 2 * direction &&
            ((isWhite && row == 1) || (!isWhite && row == 6)) &&
            board[toRow][toCol] == nullptr &&
            board[row + direction][col] == nullptr)
            return true;

        if (std::abs(dc) == 1 && dr == direction && board[toRow][toCol] != nullptr)
            return true;

        return false;
    }

    char getSymbol() const override { return isWhite ? 'P' : 'p'; }
};

class Rook : public Piece {
public:
    Rook(int r, int c, bool white) : Piece(r, c, white) {}

    bool isValidMove(int toRow, int toCol, Piece* const board[8][8]) const override {
        int dr = toRow - row;
        int dc = toCol - col;

        if (dr != 0 && dc != 0) return false;

        return isPathClear(toRow, toCol, board);
    }

    char getSymbol() const override { return isWhite ? 'R' : 'r'; }
};

int main() {
    std::setlocale(LC_ALL, "Russian");

    Piece* board[8][8] = { nullptr };

    board[1][4] = new Pawn(1, 4, true);
    board[0][0] = new Rook(0, 0, true);

    std::cout << "Пешка e2→e4: "
        << (board[1][4]->isValidMove(3, 4, board) ? "Да" : "Нет") << "\n";

    std::cout << "Ладья a1→a8: "
        << (board[0][0]->isValidMove(7, 0, board) ? "Да" : "Нет") << "\n";

    // Освобождение памяти
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
            delete board[i][j];

    return 0;
}
