#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

// ������������
int WIDTH = 30;  // ������ ���
int HEIGHT = 30; // ������ ���
const int TILE_SIZE = 20; // ����� ������
const float MOVE_INTERVAL = 0.07f; // �������� �� ������ �����

class Snake {
public:
    std::vector<sf::RectangleShape> body; // ҳ�� �����
    sf::Vector2i direction; // �������� ����
    int length;

    Snake() {
        direction = sf::Vector2i(1, 0); // ���������� �������� ������
        length = 1; // ��������� �������

        // �������� ������� ����� � ���������� ����
        body.push_back(sf::RectangleShape(sf::Vector2f(TILE_SIZE, TILE_SIZE)));
        body[0].setPosition(1 * TILE_SIZE, 1 * TILE_SIZE); // ������������ �� 1,1
        body[0].setFillColor(sf::Color::Green); // ������� ����
    }

    void move() {
        // ������ ���� ������ �����
        sf::Vector2f headPos = body[0].getPosition();
        headPos.x += direction.x * TILE_SIZE;
        headPos.y += direction.y * TILE_SIZE;

        // �������� �� �������� � ������
        if (headPos.x < 0 || headPos.x >= WIDTH * TILE_SIZE || headPos.y < 0 || headPos.y >= HEIGHT * TILE_SIZE) {
            die();
            return; // ���������, ���� ��������
        }

        // ��������� ���
        body.insert(body.begin(), sf::RectangleShape(sf::Vector2f(TILE_SIZE, TILE_SIZE)));
        body[0].setPosition(headPos);
        body[0].setFillColor(sf::Color::Green); // ���� �����

        // ��������� ������� �������, ���� ������� ����� �� ��������
        if (body.size() > length) {
            body.pop_back();
        }

        // �������� �� �������� � ����� �����
        for (size_t i = 1; i < body.size(); i++) {
            if (body[i].getGlobalBounds().intersects(body[0].getGlobalBounds())) {
                die();
                return; // ���������, ���� ��������
            }
        }
    }

    void changeDirection(int dx, int dy) {
        // �������� �� ����������� ��������
        if ((dx == 1 && direction.x == -1) || (dx == -1 && direction.x == 1) ||
            (dy == 1 && direction.y == -1) || (dy == -1 && direction.y == 1))
            return;

        direction.x = dx;
        direction.y = dy;
    }

    sf::FloatRect getBounds() {
        return body[0].getGlobalBounds();
    }

    void grow() {
        length++; // ��������� ������� �����
    }

    void die() {
        std::cout << "Game Over! Final score: " << (length - 1) << std::endl;
        body.clear(); // �������� ���
        length = 0; // ������� �������
    }

    bool isAlive() {
        return length > 0; // ��������, �� ���� �����
    }

    int getLength() {
        return length; // ��������� ������� �����
    }
};

class Food {
public:
    sf::RectangleShape shape;

    Food() {
        shape.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
        shape.setFillColor(sf::Color::Red); // ������� ������
        spawn(WIDTH, HEIGHT); // ������ spawn() � ����������� ��������
    }

    void spawn(int maxWidth, int maxHeight) {
        // ��������� ��������� �� � ����� �����
        int x = rand() % maxWidth;
        int y = rand() % maxHeight;
        shape.setPosition(x * TILE_SIZE, y * TILE_SIZE);
    }

    sf::FloatRect getBounds() {
        return shape.getGlobalBounds();
    }
};

void updateGameArea(int& width, int& height) {
    // �������� ����� ����� ���� 5 �����
    static int applesEaten = 0;
    applesEaten++;

    if (applesEaten % 5 == 0) {
        // �������� ����� �� 1 ������
        if (width > 1) width--;
        if (height > 1) height--;
    }
}

void drawBorders(sf::RenderWindow& window) {
    // ��������� ������ ���
    sf::RectangleShape topBorder(sf::Vector2f(WIDTH * TILE_SIZE, TILE_SIZE));
    topBorder.setFillColor(sf::Color::Red);
    topBorder.setPosition(0, 0);

    sf::RectangleShape bottomBorder(sf::Vector2f(WIDTH * TILE_SIZE, TILE_SIZE));
    bottomBorder.setFillColor(sf::Color::Red);
    bottomBorder.setPosition(0, (HEIGHT - 1) * TILE_SIZE);

    sf::RectangleShape leftBorder(sf::Vector2f(TILE_SIZE, HEIGHT * TILE_SIZE));
    leftBorder.setFillColor(sf::Color::Red);
    leftBorder.setPosition(0, 0);

    sf::RectangleShape rightBorder(sf::Vector2f(TILE_SIZE, HEIGHT * TILE_SIZE));
    rightBorder.setFillColor(sf::Color::Red);
    rightBorder.setPosition((WIDTH - 1) * TILE_SIZE, 0);

    // �������� ���
    window.draw(topBorder);
    window.draw(bottomBorder);
    window.draw(leftBorder);
    window.draw(rightBorder);
}

bool checkCollisionWithBorders(Snake& snake) {
    // �������� �� �������� � ��������� ������
    sf::FloatRect headBounds = snake.getBounds();
    return headBounds.left < 0 || headBounds.top < 0 ||
        headBounds.left + headBounds.width > WIDTH * TILE_SIZE ||
        headBounds.top + headBounds.height > HEIGHT * TILE_SIZE;
}

int main() {
    srand(static_cast<unsigned>(time(0))); // ����������� ���������� ���������� �����
    sf::RenderWindow window(sf::VideoMode(WIDTH * TILE_SIZE, HEIGHT * TILE_SIZE), "Snake Game");
    window.setFramerateLimit(60); // �������� ��������� �����

    Snake snake;
    Food food;

    float moveTimer = 0.0f; // ������ ��� ���� �����
    sf::Clock clock; // ������� ��'��� ��� ���������� ����

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // ��������� ������
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && snake.direction.y == 0) snake.changeDirection(0, -1);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && snake.direction.y == 0) snake.changeDirection(0, 1);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && snake.direction.x == 0) snake.changeDirection(-1, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && snake.direction.x == 0) snake.changeDirection(1, 0);

        if (snake.isAlive()) {
            moveTimer += clock.restart().asSeconds(); // ������ ��� ����� �� �������

            // ��� �����
            if (moveTimer >= MOVE_INTERVAL) {
                snake.move();
                moveTimer = 0.0f; // ������� ������
            }

            // �������� �� �'������ ��
            if (snake.getBounds().intersects(food.getBounds())) {
                snake.grow();
                food.spawn(WIDTH, HEIGHT); // �'��� ���, �������� � ����� �����
                updateGameArea(WIDTH, HEIGHT); // ��������� ������ �����
                std::cout << "Apples Eaten: " << (snake.getLength() - 1) << std::endl; // �������� ������ ����� �'�����
            }

            // �������� ������
            window.clear();

            // ��������� �����
            for (auto& segment : snake.body) {
                window.draw(segment);
            }

            // ��������� ��
            window.draw(food.shape);

            // ��������� ���
            drawBorders(window);

            // �������� �� �������� � ��������� ������
            if (checkCollisionWithBorders(snake)) {
                snake.die();
            }

            // ³���������� ����
            window.display();
        }
        else {
            // ���� ����� ������, �������� �����������
            std::cout << "Game Over! Final score: " << (snake.getLength() - 1) << std::endl;
            window.close(); // ��������� ����
        }
    }

    return 0;
}
