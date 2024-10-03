#include <SFML/Graphics.hpp>
#include <iostream>
#include <time.h>

using namespace sf;

#define ROWS      18
#define COLUMNS   12
#define BLOCKSIZE 40

int tetrominoes[7][4][2] =
{
    {{0, 0}, {0, 1}, {0, 2}, {0, 3}}, // I
    {{0, 0}, {1, 0}, {1, 1}, {1, 2}}, // L
    {{0, 0}, {0, 1}, {0, 2}, {1, 0}}, // J
    {{0, 0}, {0, 1}, {1, 0}, {1, 1}}, // O
    {{0, 0}, {0, 1}, {1, 1}, {1, 2}}, // S
    {{0, 0}, {0, 1}, {0, 2}, {1, 1}}, // T
    {{1, 0}, {0, 1}, {1, 1}, {0, 2}}  // Z
};


Color tetrominoColors[7] =
{
    Color::Cyan,
    Color::Blue,
    Color::Yellow,
    Color::Green,
    Color::Magenta,
    Color::Red,
    Color::Green
};


void drawMap(int map[ROWS][COLUMNS], RenderWindow& window)
{
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLUMNS; j++)
        {
            if (map[i][j] > 0)
            {
                RectangleShape mapRect;
                mapRect.setFillColor(tetrominoColors[map[i][j] - 1]);
                mapRect.setSize(Vector2f(BLOCKSIZE, BLOCKSIZE));
                mapRect.setPosition(Vector2f((BLOCKSIZE + 1) * j, (BLOCKSIZE + 1) * i));
                window.draw(mapRect);
            }
        }
    }
}


void drawTetromino(int tetramino[4][2], RenderWindow& window, int offsetX, int offsetY, int randomColor)
{
    for (int i = 0; i < 4; i++)
    {
        int x = tetramino[i][0] + offsetX;
        int y = tetramino[i][1] + offsetY;

        RectangleShape block;
        block.setFillColor(tetrominoColors[randomColor]);
        block.setSize(Vector2f(BLOCKSIZE, BLOCKSIZE));
        block.setPosition(Vector2f((BLOCKSIZE + 1) * x, (BLOCKSIZE + 1) * y));
        window.draw(block);
    }
}


bool canMove(int tetramino[4][2], int offsetX, int offsetY, int field[ROWS][COLUMNS])
{
    for (int i = 0; i < 4; i++)
    {
        int x = tetramino[i][0] + offsetX;
        int y = tetramino[i][1] + offsetY + 1;
        // проверяем границы и занятые клетки
        if (y < 0 || y >= ROWS || x < 0 || x >= COLUMNS || field[y][x] > 0)
        {
            return false;
        }
    }
    return true;
}


void rotateTetromino(int tetromino[4][2], int tetrominoX, int tetrominoY, int field[ROWS][COLUMNS]) {
    int temp[4][2];
    for (int i = 0; i < 4; i++) {
        temp[i][0] = -tetromino[i][1];  // новая координата x после поворота
        temp[i][1] = tetromino[i][0];   // новая координата y после поворота
    }

    // проверка, можем ли повернуть фигуру
    if (canMove(temp, tetrominoX, tetrominoY, field)) {
        for (int i = 0; i < 4; i++) {
            tetromino[i][0] = temp[i][0]; // обновляем координаты
            tetromino[i][1] = temp[i][1]; // обновляем координаты
        }
    }
}


void fixTetramino(int tetramino[4][2], int offsetX, int offsetY, int field[ROWS][COLUMNS], int index)
{
    for (int i = 0; i < 4; i++)
    {
        int x = tetramino[i][0] + offsetX;
        int y = tetramino[i][1] + offsetY;

        field[y][x] = index + 1; // сохраняем индекс фигуры на поле
    }
}


void deleteCompleteLines(int field[ROWS][COLUMNS], int& score)
{
    for (int i = ROWS - 1; i >= 0; i--)
    {
        bool lineComplete = true;
        for (int j = 0; j < COLUMNS; j++)
        {
            if (field[i][j] == 0)
            {
                lineComplete = false;
                break;
            }
        }

        if (lineComplete)
        {
            // удаление заполненной строки и сдвиг остальных строк вниз
            for (int k = i; k > 0; k--)
            {
                for (int j = 0; j < COLUMNS; j++)
                {
                    field[k][j] = field[k - 1][j]; // сдвигаем строку вниз
                }
            }
            
            // очистка
            for (int j = 0; j < COLUMNS; j++)
            {
                field[0][j] = 0;
            }
            i++;
            score++;
        }
    }
}


int main()
{
    int field[ROWS][COLUMNS]{};

    bool leftPressed = false,
        rightPressed = false,
        upPressed = false,
        downPressed = false;

    srand(time(0));

    int winWidth = COLUMNS * (BLOCKSIZE + 1) - 1,
        winHeight = ROWS * (BLOCKSIZE + 1) - 1;

    //  загрузка шрифтов
    int score = 0;
    Font font;
	if (!font.loadFromFile("src/Arial.ttf")) 
	{
        return -1;
    }

	Text text;
	text.setFont(font);
	text.setString(std::to_string(score));
	text.setCharacterSize(40);
	text.setFillColor(Color::Black);
	text.setPosition(Vector2f(winWidth / 2, winHeight / 4));

    RenderWindow window(VideoMode(winWidth, winHeight), "Tetris++");
    
    float timer = 0,
        delay = 0.3;

    Clock clock;

    int randomTetrominoIndex = rand() % 7;
    int tetromino[4][2];

    for (int i = 0; i < 4; i++)
    {
        tetromino[i][0] = tetrominoes[randomTetrominoIndex][i][0];
        tetromino[i][1] = tetrominoes[randomTetrominoIndex][i][1];
    }

    int tetrominoX = COLUMNS / 2 - 1;
    int tetrominoY = 0;

    while (window.isOpen())
    {
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer += time;
        
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
        }

        // влево
        if (leftPressed == false)
        {
            if (Keyboard::isKeyPressed(Keyboard::Left))
            {
                leftPressed = true;
                if (canMove(tetromino, tetrominoX - 1, tetrominoY, field)) tetrominoX--;
            }
        }
        else if ((event.type == Event::KeyReleased) && (event.key.code == Keyboard::Left)) leftPressed = false;


        // вправо
        if (rightPressed == false)
        {
            if (Keyboard::isKeyPressed(Keyboard::Right))
            {
                rightPressed = true;
                if (canMove(tetromino, tetrominoX + 1, tetrominoY, field)) tetrominoX++;
            }
        }
        else if ((event.type == Event::KeyReleased) && (event.key.code == Keyboard::Right)) rightPressed = false;


        // вниз
        if (downPressed == false)
        {
            if (Keyboard::isKeyPressed(Keyboard::Down))
            {
                downPressed = true;
                if (canMove(tetromino, tetrominoX, tetrominoY + 1, field)) tetrominoY++;
            }
        }
        else if ((event.type == Event::KeyReleased) && (event.key.code == Keyboard::Down)) downPressed = false;


        // поворот фигуры
        if (upPressed == false)
        {
            if (Keyboard::isKeyPressed(Keyboard::Up))
            {
                upPressed = true;
                rotateTetromino(tetromino, tetrominoX, tetrominoY, field);
            }
        }
        else if ((event.type == Event::KeyReleased) && (event.key.code == Keyboard::Up)) upPressed = false;


        if (timer >= delay)
        {
            if (canMove(tetromino, tetrominoX, tetrominoY, field)) {
                tetrominoY++;
            }
            else
            {
                fixTetramino(tetromino, tetrominoX, tetrominoY, field, randomTetrominoIndex);
                deleteCompleteLines(field, score);
                randomTetrominoIndex = rand() % 7;
                for (int i = 0; i < 4; i++)
                {
                    tetromino[i][0] = tetrominoes[randomTetrominoIndex][i][0];
                    tetromino[i][1] = tetrominoes[randomTetrominoIndex][i][1];
                }
                tetrominoX = COLUMNS / 2 - 1;
                tetrominoY = 0;
            }
            timer = 0;
        }

        text.setString(std::to_string(score));
        window.clear(Color::White);
        drawMap(field, window);
        drawTetromino(tetromino, window, tetrominoX, tetrominoY, randomTetrominoIndex);
        window.draw(text);
        window.display();
        if (tetrominoY == 0 && !canMove(tetromino, tetrominoX, tetrominoY, field))
        {
            for (int i = 0; i < ROWS; i++)
            {
                for (int j = 0; j < COLUMNS; j++)
                {
                    field[i][j] = 0;
                    score = 0;
                }
            }
        }
    }
    return 0;
}