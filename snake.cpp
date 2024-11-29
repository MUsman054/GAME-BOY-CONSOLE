#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include"GameSpirit.h"
using namespace sf;
using namespace std;

class Grid {
private:
    RectangleShape** grid;
    int rows, cols;

public:
    Grid(int width = 800, int height = 600, int cellSize = 20) {
        rows = height / cellSize;
        cols = width / cellSize;

        grid = new RectangleShape * [rows];
        for (int i = 0; i < rows; ++i) {
            grid[i] = new RectangleShape[cols];
            for (int j = 0; j < cols; ++j) {
                grid[i][j].setSize(Vector2f(cellSize - 1, cellSize - 1));
                grid[i][j].setFillColor(Color::White);
                grid[i][j].setPosition(Vector2f(j * cellSize, i * cellSize));
            }
        }
    }

    ~Grid() {
        for (int i = 0; i < rows; ++i) {
            delete[] grid[i];
        }
        delete[] grid;
    }

    void draw(RenderWindow& window) {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                window.draw(grid[i][j]);
            }
        }
    }
};

class Food {
protected:
    RectangleShape food;
    static int food_count;
    int squareSize;

public:
    Food(int cellSize = 20)
        : squareSize(cellSize) {
        food.setSize(Vector2f(squareSize, squareSize));
        food.setFillColor(Color::Red);
        setFoodPosition();
    }

    void draw(RenderWindow& window) {
        window.draw(food);
    }

    void setFoodPosition() {
        int f_x = (rand() % (800 / squareSize)) * squareSize;
        int f_y = (rand() % (600 / squareSize)) * squareSize;
        food.setPosition(Vector2f(f_x, f_y));
    }

    Vector2f get_food_position() const {
        return food.getPosition();
    }
    static int get_food_count() { return food_count; }
    static void increase_food_count() { ++food_count; }
};
int Food::food_count = 0;

class SuperFood : public Food {
public:
    SuperFood(int cellSize = 20)
        : Food(cellSize) {
        food.setFillColor(Color::Yellow);
        setFoodPosition();
    }
};

class Snake :public GameSpirit {
 private:
    RectangleShape* body;
    int length;
    int squareSize;
    Grid grid;
    Food food;
    SuperFood superfood;
    int score;
    bool chk_game_over;
    bool is_superfood_active;
    Clock superfood_timer;

public:
    Snake(int gridWidth = 800, int gridHeight = 600, int cellSize = 20)
        : grid(gridWidth, gridHeight, cellSize), food(cellSize), superfood(cellSize), squareSize(cellSize),
          length(4), chk_game_over(false), is_superfood_active(false), score(0) {
        body = new RectangleShape[length];
        for (int i = 0; i < length; ++i) 
        {
            body[i].setSize(Vector2f(squareSize, squareSize));
            body[i].setFillColor(Color::Green);
            body[i].setPosition(Vector2f(400 - i * squareSize, 300));
        }
    }

    ~Snake() { delete[] body; }
    void setHeadTexture(Texture& texture) {
        body[0].setTexture(&texture);
    }

    void draw(RenderWindow& window) {
        grid.draw(window);

        for (int i = 0; i < length; ++i) {
            window.draw(body[i]);
        }

        food.draw(window);

        if (is_superfood_active) {
            superfood.draw(window);
        }
    }

    void move(int dx, int dy) {
        if (chk_game_over)
            return;

        for (int i = length - 1; i > 0; --i) {
            body[i].setPosition(body[i - 1].getPosition());
        }
        body[0].move(Vector2f(dx, dy));
    }

    void checkBoundary() {
        if (body[0].getPosition().x < 0)
            body[0].setPosition(Vector2f(800, body[0].getPosition().y));
        else if (body[0].getPosition().x >= 800)
            body[0].setPosition(Vector2f(0, body[0].getPosition().y));
        if (body[0].getPosition().y < 0)
            body[0].setPosition(Vector2f(body[0].getPosition().x, 600));
        else if (body[0].getPosition().y >= 600)
            body[0].setPosition(Vector2f(body[0].getPosition().x, 0));
    }

    void grow() {
        RectangleShape* newBody = new RectangleShape[length + 1];
        for (int i = 0; i < length; ++i) {
            newBody[i] = body[i];
        }

        newBody[length].setSize(Vector2f(squareSize, squareSize));
        newBody[length].setFillColor(Color::Green);
        newBody[length].setPosition(body[length - 1].getPosition());

        delete[] body;
        body = newBody;
        ++length;
    }

    void itselfCollision() {
        for (int i = 1; i < length; ++i) {
            if (body[0].getPosition() == body[i].getPosition()) {
                chk_game_over = true;
                break;
            }
        }
    }

    void food_eaten() {
        Vector2f foodPosition = food.get_food_position();
        Vector2f headPosition = body[0].getPosition();

        if (headPosition.x == foodPosition.x && headPosition.y == foodPosition.y) {
            food.increase_food_count();
            score += 2;
            grow();
            food.setFoodPosition();

            if (food.get_food_count() % 5 == 0) {
                is_superfood_active = true;
                superfood.setFoodPosition();
                superfood_timer.restart(); 
            }
        }

        if (is_superfood_active) {
            Vector2f superFoodPosition = superfood.get_food_position();
            if (headPosition.x == superFoodPosition.x && headPosition.y == superFoodPosition.y) {
                score += 5;
                grow();
                is_superfood_active = false;
            }

            if (superfood_timer.getElapsedTime().asSeconds() > 5) {
                is_superfood_active = false;
            }
        }
    }

    bool getchk_game_over() { return chk_game_over; }
    int getscore() { return score; }
};


int main() 
{
    RenderWindow window(VideoMode(800, 600), "Snake Game");
    Snake snake;
    sf::Music music;
    if (!music.openFromFile("snakeSound.ogg")) 
    {
        cerr << "Error loading music!" << endl;
        return -1;  
    }
    music.play();
     sf::Texture snaki;
    if (!snaki.loadFromFile("images2.png")) {
        cerr << "Error loading texture!" << endl;
        return -1;
    }
    
    snake.setHeadTexture(snaki);
    

    Font font;
    if (!font.loadFromFile("/home/muhammadusman/Downloads/arial-font/arial.ttf")) {
        cerr << "Error loading font!" << endl;
        return -1;
    }

    Text game_over;
    game_over.setFont(font);
    game_over.setCharacterSize(50);
    game_over.setFillColor(Color::Red);
    game_over.setPosition(250, 250);

    Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(30);
    scoreText.setFillColor(Color::Blue);
    scoreText.setPosition(0, 0);

    window.setFramerateLimit(15);

    int dx = 20, dy = 0;
    bool pause_game = false;
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) 
        {
            if (event.type == Event::Closed)
                window.close();
        }

        if (!snake.getchk_game_over()) {
            if (Keyboard::isKeyPressed(Keyboard::Up) && dy == 0) {
                dx = 0;
                dy = -20;
            }
            if (Keyboard::isKeyPressed(Keyboard::Down) && dy == 0) {
                dx = 0;
                dy = 20;
            }
            if (Keyboard::isKeyPressed(Keyboard::Left) && dx == 0) {
                dx = -20;
                dy = 0;
            }
            if (Keyboard::isKeyPressed(Keyboard::Right) && dx == 0) {
                dx = 20;
                dy = 0;
            }
            if(Keyboard::isKeyPressed(Keyboard::Escape))
            {
                pause_game = true;
            }
            if(Keyboard::isKeyPressed(Keyboard::P) && pause_game)
            {
                pause_game = false;
            }
            if(!pause_game)
            {
                snake.move(dx, dy);
            snake.checkBoundary();
            snake.itselfCollision();
            snake.food_eaten();}
        }

        scoreText.setString("Score: " + to_string(snake.getscore()));

        window.clear();
        snake.draw(window);
        window.draw(scoreText);

        if (snake.getchk_game_over()) 
        {
            game_over.setString("Game Over\nYour Score: " + to_string(snake.getscore()));
            window.draw(game_over);
        }

        window.display();
    }

    return 0;
}
