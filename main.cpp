#include <SFML/Graphics.hpp>
#include <string.h>
#include <iostream>
#include <time.h>
#include <random>
#include <chrono>

sf::Color GREY(169, 169, 169);
sf::Color DARK_GREY(20, 20, 20);

class Block {
private:
    sf::RectangleShape block;
    sf::Text DisplayValue;
    int number;
    sf::Font font;
    bool isStop = false;
public:
    Block(int x, int y, int size) {
        init(x, y, size);
    }
    Block() { init(0, 0, 0); }
    Block(const Block& copy) {
        this->block = copy.block;
        this->DisplayValue = copy.DisplayValue;
        this->number = copy.number;
        this->font = copy.font;
    }
    int getNumber() { return number; }
    void init(int x, int y, int size) {
        block.setSize(sf::Vector2f(size, size));
        block.setPosition(x, y);
        block.setFillColor(GREY);
        number = 0;
        font.loadFromFile("assets/Questrian.otf");
        DisplayValue.setFont(font);
        DisplayValue.setString("0");
        DisplayValue.setPosition(sf::Vector2f(block.getPosition().x+30, block.getPosition().y + 25));
    }
    sf::RectangleShape& getBlock() { return block; }
    int ProcessInput() {
        if (!isStop) {
            if (number + 1 > 9) number = 0;
            else number++;
            DisplayValue.setString(std::to_string(number));
            return number;
        }
    }
    void draw(sf::RenderWindow& window) {
        window.draw(block);
        if(number != 0)
        window.draw(DisplayValue);
    }
    bool getStop() { return isStop; }
    void makeStop() {
        block.setFillColor(DARK_GREY);
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::minstd_rand0 generator(seed);
        number = 1+generator() % 9;
        DisplayValue.setString(std::to_string(number));
        isStop = true;
    }
    void setNumber(int n) { 
        this->number = n; 
        DisplayValue.setString(std::to_string(number));
    }
};
class Grid {
    Block** blocks;
    int** values;
public:
    Grid(int x, int y, int BlockSize) {
        blocks = new Block* [3];
        values = new int* [3];
        float xCord = x, yCord = y;
        for (int i = 0; i < 3; i++) {
            xCord = x;
            values[i] = new int[3];
            blocks[i] = new Block[3];
            for (int j = 0; j < 3; j++) {
                values[i][j] = 0;
                blocks[i][j].init(xCord + 5., yCord + 5., BlockSize);
                xCord += BlockSize + 5.;
            }
            yCord += BlockSize + 5.;
        }
    }
    ~Grid() {
        for (int i = 0; i < 3; i++) {
            delete[] blocks[i];
            delete[] values[i];
        }
        delete[] blocks;
        delete[] values;
    }
    bool check() {
        bool result = true;
        int map[10] = { 0 };
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if(values[i][j] != 0) map[values[i][j]]++;
            }
        }
        for (int i = 0; i < 10; i++) {
            if (map[i] > 1) {
                for (int k = 0; k < 3; k++) {
                    for (int j = 0; j < 3; j++) {
                        if (values[k][j] == i) {
                            result = false;
                            blocks[k][j].getBlock().setFillColor(sf::Color::Red);
                        }
                    }
                }
            }
        }
        return result;
    }
    int** getValues() { return values; }
    int**& getRefValues() { 
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                values[i][j] = blocks[i][j].getNumber();
            }
        }
        return values; 
    }
    Block** getBlocks() { return blocks; }
    void draw(sf::RenderWindow& window) {
        for (int i = 0; i < 3; i++) {
            for(int j=0; j<3; j++)
            blocks[i][j].draw(window);
        }
    }
    void init() {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if(!blocks[i][j].getStop())
                blocks[i][j].getBlock().setFillColor(GREY);
            }
        }
    }
    void HandleInput(sf::RenderWindow& window) {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                sf::Vector2f mouse;
                mouse.x = sf::Mouse::getPosition(window).x;
                mouse.y = sf::Mouse::getPosition(window).y;
                if (blocks[i][j].getBlock().getGlobalBounds().contains(mouse)) {
                    values[i][j] = blocks[i][j].ProcessInput();
                }
            }
        }
    }
};
class GameState {
public:
    virtual int run(sf::RenderWindow& window) = 0;
    virtual int identity() = 0;
};
class PlayState : public GameState {
    Grid*** board;
    sf::RectangleShape* DividingLines;
public:
    PlayState() {
        float xCord = 0, yCord = 0;
        board = new Grid ** [3];
        for (int i = 0; i < 3; i++) {
            xCord = 0;
            board[i] = new Grid*[3];
            for (int j = 0; j < 3; j++) {
                board[i][j] = new Grid(xCord, yCord, 80);
                xCord += 255;
            }
            yCord += 255;
        }
        DividingLines = new sf::RectangleShape[4];

        DividingLines[0].setPosition(255, 5.);
        DividingLines[0].setSize(sf::Vector2f(5., 760.));
        DividingLines[0].setFillColor(sf::Color::Black);

        DividingLines[1].setPosition(510, 5.);
        DividingLines[1].setSize(sf::Vector2f(5., 760.));
        DividingLines[1].setFillColor(sf::Color::Black);

        DividingLines[2].setPosition(5., 255.);
        DividingLines[2].setSize(sf::Vector2f(760, 5));
        DividingLines[2].setFillColor(sf::Color::Black);

        DividingLines[3].setPosition(5., 510);
        DividingLines[3].setSize(sf::Vector2f(760., 5));
        DividingLines[3].setFillColor(sf::Color::Black);

        Block*** blockValues = new Block * *[9];
        for (int i = 0; i < 9; i++) blockValues[i] = new Block * [9];
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                Block** tempBlock = board[i][j]->getBlocks();
                for (int a = i * 3, c = 0; a < i * 3 + 3; a++, c++) {
                    for (int b = j * 3, d = 0; b < j * 3 + 3; b++, d++) {
                        blockValues[a][b] = &tempBlock[c][d];
                    }
                }
            }
        }
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::minstd_rand0 generator(seed);
        int RANDOM_NBR_OF_STOPS = 3 + generator() % 6;
        for (int i = 0; i < RANDOM_NBR_OF_STOPS; i++) {
            int RANDOM_ROW_INDEX = generator() % 9;
            int RANDOM_COL_INDEX = generator() % 9;
            blockValues[RANDOM_ROW_INDEX][RANDOM_COL_INDEX]->makeStop();
        }
    }
    ~PlayState() {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                delete board[i][j];
            }
        }
        delete[] board;
    }
    int identity() override { return 0; }
    void check() {
        bool duplicate = false;
        Block ***blockValues = new Block**[9];
        for (int i = 0; i < 9; i++) blockValues[i] = new Block * [9];
        int boardValues[9][9] = { 0 }, checkValues[9][9] = { 0 };
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                board[i][j]->check();
                int** temp = board[i][j]->getValues();
                Block** tempBlock = board[i][j]->getBlocks();
                for (int a = i * 3, c=0; a < i * 3 + 3; a++, c++) {
                    for (int b = j * 3, d=0; b < j * 3 + 3; b++, d++) {
                        boardValues[a][b] = temp[c][d];
                        blockValues[a][b] = &tempBlock[c][d];
                    }
                }
            }
        }
        for (int i = 0; i < 9; i++) {
            for (int j = 1; j < 9; j++) {
                for (int k = j - 1; k >= 0; k--) {
                    if (boardValues[i][j] != 0 && boardValues[i][j] == boardValues[i][k]) {
                        checkValues[i][j]++;
                        duplicate = true;
                    }
                }
            }
        }
        for (int i = 0; i < 9; i++) {
            for (int j = 1; j < 9; j++) {
                for (int k = j - 1; k >= 0; k--) {
                    if (boardValues[j][i] != 0 && boardValues[j][i] == boardValues[k][i]){
                        checkValues[j][i]++;
                        duplicate = true;
                    }
                }
            }
        }
        for (int i = 0; i < 9; i++) {
            for (int j = 1; j < 9; j++) {
                if (checkValues[i][j] >= 1) blockValues[i][j]->getBlock().setFillColor(sf::Color::Red);
            }
        }
    }
    bool CheckWin() {
        bool duplicate = false;
        int** boardValues = getNumberGrid();
        int checkValues[9][9] = { 0 };
        for (int i = 0; i < 9; i++) {
            for (int j = 1; j < 9; j++) {
                for (int k = j - 1; k >= 0; k--) {
                    if (boardValues[i][j] != 0 && boardValues[i][j] == boardValues[i][k]) {
                        checkValues[i][j]++;
                        duplicate = true;
                    }
                }
            }
        }
        for (int i = 0; i < 9; i++) {
            for (int j = 1; j < 9; j++) {
                for (int k = j - 1; k >= 0; k--) {
                    if (boardValues[j][i] != 0 && boardValues[j][i] == boardValues[k][i]) {
                        checkValues[j][i]++;
                        duplicate = true;
                    }
                }
            }
        }
        if (!duplicate) {
            for (int i = 0; i < 9; i++) {
                int sum = 0;
                for (int j = 0; j < 9; j++)
                    sum += boardValues[i][j];
                if (sum != 45) return false;
            }
            for (int i = 0; i < 9; i++) {
                int sum = 0;
                for (int j = 0; j < 9; j++)
                    sum += boardValues[j][i];
                if (sum != 45) return false;
            }
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    if (!board[i][j]->check()) return false;
                }
            }
            return true;
        }
        return false;
    }
    bool CheckWin(int** boardValues) {
        bool duplicate = false;
        int checkValues[9][9] = { 0 };
        for (int i = 0; i < 9; i++) {
            for (int j = 1; j < 9; j++) {
                for (int k = j - 1; k >= 0; k--) {
                    if (boardValues[i][j] != 0 && boardValues[i][j] == boardValues[i][k]) {
                        checkValues[i][j]++;
                        duplicate = true;
                    }
                }
            }
        }
        for (int i = 0; i < 9; i++) {
            for (int j = 1; j < 9; j++) {
                for (int k = j - 1; k >= 0; k--) {
                    if (boardValues[j][i] != 0 && boardValues[j][i] == boardValues[k][i]) {
                        checkValues[j][i]++;
                        duplicate = true;
                    }
                }
            }
        }
        if (!duplicate) {
            for (int i = 0; i < 9; i++) {
                int sum = 0;
                for (int j = 0; j < 9; j++)
                    sum += boardValues[i][j];
                if (sum != 45) return false;
            }
            for (int i = 0; i < 9; i++) {
                int sum = 0;
                for (int j = 0; j < 9; j++)
                    sum += boardValues[j][i];
                if (sum != 45) return false;
            }
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    if (!board[i][j]->check()) return false;
                }
            }
            return true;
        }
        return false;
    }
    Block*** getBlockGrid() {
        Block*** blockValues = new Block * *[9];
        for (int i = 0; i < 9; i++) blockValues[i] = new Block * [9];
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                Block** tempBlock = board[i][j]->getBlocks();
                for (int a = i * 3, c = 0; a < i * 3 + 3; a++, c++) {
                    for (int b = j * 3, d = 0; b < j * 3 + 3; b++, d++) {
                        blockValues[a][b] = &tempBlock[c][d];
                    }
                }
            }
        }
        return blockValues;
    }
    int** getNumberGrid() {
        int** boardValues = new int*[9];
        for (int i = 0; i < 9; i++) boardValues[i] = new int [9];
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                int** temp = board[i][j]->getRefValues();
                for (int a = i * 3, c = 0; a < i * 3 + 3; a++, c++) {
                    for (int b = j * 3, d = 0; b < j * 3 + 3; b++, d++) {
                        boardValues[a][b] = temp[c][d];
                    }
                }
            }
        }
        return boardValues;
    }
    bool isPossible(int row, int col, int n, int** numberGrid) {
        for (int i = 0; i < 9; i++) {
            if (numberGrid[row][i] == n) 
                return false;
            if (numberGrid[i][col] == n) 
                return false;
        }
        int BoxX = round(row / 3) * 3;
        int BoxY = round(col / 3) * 3;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (numberGrid[i + BoxX][j + BoxY] == n) 
                    return false;
            }
        }
        return true;
    }
    int solve(int** numberGrid) {
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                if (numberGrid[i][j] == 0) {
                    for (int n = 1; n < 10; n++) {
                        if (isPossible(i, j, n, numberGrid)) {
                            numberGrid[i][j] = n;
                            if (solve(numberGrid) == -1) {
                                return -1;
                            } else
                            numberGrid[i][j] = 0;
                        }
                    }
                    return 0;
                }
            }
        }
        if (CheckWin(numberGrid)) {
            Block*** gridBlocks = getBlockGrid();
            for (int i = 0; i < 9; i++) {
                for (int j = 0; j < 9; j++) {
                    if(!gridBlocks[i][j]->getStop())
                    gridBlocks[i][j]->setNumber(numberGrid[i][j]);
                }
            }
            return -1;
        }
    }
    void showWinCard() {
        sf::RectangleShape WinCard;
        WinCard.setSize(sf::Vector2f(770, 770));
        WinCard.setPosition(0, 0);
        sf::Texture WinCardTexture;
        
    }
    int run(sf::RenderWindow& window) override {
        while (window.isOpen()) {
            if (CheckWin()) {
                showWinCard();
            }
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) this->check();
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                    int** numberGrid = getNumberGrid();
                    Block*** gBoard = getBlockGrid();
                    for (int i = 0; i < 9; i++) {
                        for (int j = 0; j < 9; j++) {
                            if (!gBoard[i][j]->getStop()) {
                                numberGrid[i][j] = 0;
                            }
                        }
                    }
                    this->solve(numberGrid);
                }
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    for (int i = 0; i < 3; i++) {
                        for (int j = 0; j < 3; j++) {
                            board[i][j]->HandleInput(window);
                        }
                    }
                    for (int i = 0; i < 3; i++) {
                        for (int j = 0; j < 3; j++) {
                            board[i][j]->init();
                        }
                    }
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) return 0;
                
            }
            window.clear(sf::Color::White);
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    board[i][j]->draw(window);
                }
            }
            for (int i = 0; i < 4; i++) {
                window.draw(DividingLines[i]);
            }
            window.display();
        }
    }
};
class PauseState : public GameState {
    sf::RectangleShape PauseMenu;
    sf::Texture PauseMenuTexture;
public:
    PauseState() {
        PauseMenu.setPosition(40, 50);
        PauseMenu.setSize(sf::Vector2f(670, 670));
        PauseMenuTexture.loadFromFile("assets/PauseMenu.png");
        PauseMenu.setTexture(&PauseMenuTexture);
    }
    int identity() override {
        return 1;
    }
    int run(sf::RenderWindow& window) override {
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) return 0;
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    sf::Vector2i mouse = sf::Mouse::getPosition(window);
                    if (mouse.x >= 173 && mouse.x <= 594 &&
                        mouse.y >= 258 && mouse.y <= 356) {
                        return 0;
                    }
                    if (mouse.x >= 230 && mouse.x <= 530 &&
                        mouse.y >= 420 && mouse.y <= 520) {
                        window.close();
                    }
                }
            }
            // window.clear(sf::Color::White);
            window.draw(PauseMenu);
            window.display();
        }
    }
};
class Game {
    GameState** states;
    sf::RenderWindow* window;
    GameState* ActiveState;
public:
    Game() {
        window = new sf::RenderWindow(sf::VideoMode(770, 770), "Sudoku");
        states = new GameState * [2];
        states[0] = new PlayState;
        states[1] = new PauseState;
        ActiveState = states[0];
    }
    sf::RenderWindow* getWindow() { return window; }
    void Play() {
        while (window->isOpen()) {
            ActiveState->run(*window);
            sf::Event event;
            while (window->pollEvent(event)) {
                if (ActiveState->identity() == 0) 
                    ActiveState = states[1];
                else if (ActiveState->identity() == 1)
                    ActiveState = states[0];

                break;
            }
        }
    }
};

int main() {
    Game g;
    g.Play();
    return 0;
}