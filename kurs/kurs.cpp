#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

// Налаштування
int WIDTH = 30;  // Ширина гри
int HEIGHT = 30; // Висота гри
const int TILE_SIZE = 20; // Розмір плитки
const float MOVE_INTERVAL = 0.07f; // Інтервал між рухами змійки

class Snake {
public:
    std::vector<sf::RectangleShape> body; // Тіло змійки
    sf::Vector2i direction; // Напрямок руху
    int length;

    Snake() {
        direction = sf::Vector2i(1, 0); // Початковий напрямок вправо
        length = 1; // Початкова довжина

        // Стартова позиція змійки у безпечному місці
        body.push_back(sf::RectangleShape(sf::Vector2f(TILE_SIZE, TILE_SIZE)));
        body[0].setPosition(1 * TILE_SIZE, 1 * TILE_SIZE); // Встановлюємо на 1,1
        body[0].setFillColor(sf::Color::Green); // Зелений колір
    }

    void move() {
        // Додаємо нову голову змійки
        sf::Vector2f headPos = body[0].getPosition();
        headPos.x += direction.x * TILE_SIZE;
        headPos.y += direction.y * TILE_SIZE;

        // Перевірка на зіткнення зі стінами
        if (headPos.x < 0 || headPos.x >= WIDTH * TILE_SIZE || headPos.y < 0 || headPos.y >= HEIGHT * TILE_SIZE) {
            die();
            return; // Завершуємо, якщо зіткнення
        }

        // Переміщуємо тіло
        body.insert(body.begin(), sf::RectangleShape(sf::Vector2f(TILE_SIZE, TILE_SIZE)));
        body[0].setPosition(headPos);
        body[0].setFillColor(sf::Color::Green); // Колір змійки

        // Видаляємо останній сегмент, якщо довжина змійки не збільшена
        if (body.size() > length) {
            body.pop_back();
        }

        // Перевірка на зіткнення з самим собою
        for (size_t i = 1; i < body.size(); i++) {
            if (body[i].getGlobalBounds().intersects(body[0].getGlobalBounds())) {
                die();
                return; // Завершуємо, якщо зіткнення
            }
        }
    }

    void changeDirection(int dx, int dy) {
        // Перевірка на протилежний напрямок
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
        length++; // Збільшення довжини змійки
    }

    void die() {
        std::cout << "Game Over! Final score: " << (length - 1) << std::endl;
        body.clear(); // Очистити тіло
        length = 0; // Скинути довжину
    }

    bool isAlive() {
        return length > 0; // Перевірка, чи жива змійка
    }

    int getLength() {
        return length; // Повертаємо довжину змійки
    }
};

class Food {
public:
    sf::RectangleShape shape;

    Food() {
        shape.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
        shape.setFillColor(sf::Color::Red); // Червоне яблуко
        spawn(WIDTH, HEIGHT); // Виклик spawn() з початковими розмірами
    }

    void spawn(int maxWidth, int maxHeight) {
        // Випадкове положення їжі в нових межах
        int x = rand() % maxWidth;
        int y = rand() % maxHeight;
        shape.setPosition(x * TILE_SIZE, y * TILE_SIZE);
    }

    sf::FloatRect getBounds() {
        return shape.getGlobalBounds();
    }
};

void updateGameArea(int& width, int& height) {
    // Зменшуємо розмір карти кожні 5 яблук
    static int applesEaten = 0;
    applesEaten++;

    if (applesEaten % 5 == 0) {
        // Зменшуємо розмір на 1 плитку
        if (width > 1) width--;
        if (height > 1) height--;
    }
}

void drawBorders(sf::RenderWindow& window) {
    // Створюємо червоні межі
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

    // Виводимо межі
    window.draw(topBorder);
    window.draw(bottomBorder);
    window.draw(leftBorder);
    window.draw(rightBorder);
}

bool checkCollisionWithBorders(Snake& snake) {
    // Перевірка на зіткнення з червоними межами
    sf::FloatRect headBounds = snake.getBounds();
    return headBounds.left < 0 || headBounds.top < 0 ||
        headBounds.left + headBounds.width > WIDTH * TILE_SIZE ||
        headBounds.top + headBounds.height > HEIGHT * TILE_SIZE;
}

int main() {
    srand(static_cast<unsigned>(time(0))); // Ініціалізація генератора випадкових чисел
    sf::RenderWindow window(sf::VideoMode(WIDTH * TILE_SIZE, HEIGHT * TILE_SIZE), "Snake Game");
    window.setFramerateLimit(60); // Збільшено обмеження кадрів

    Snake snake;
    Food food;

    float moveTimer = 0.0f; // Таймер для руху змійки
    sf::Clock clock; // Часовий об'єкт для відстеження часу

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Керування змійкою
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && snake.direction.y == 0) snake.changeDirection(0, -1);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && snake.direction.y == 0) snake.changeDirection(0, 1);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && snake.direction.x == 0) snake.changeDirection(-1, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && snake.direction.x == 0) snake.changeDirection(1, 0);

        if (snake.isAlive()) {
            moveTimer += clock.restart().asSeconds(); // Додаємо час кадру до таймера

            // Рух змійки
            if (moveTimer >= MOVE_INTERVAL) {
                snake.move();
                moveTimer = 0.0f; // Скидаємо таймер
            }

            // Перевірка на з'їдання їжі
            if (snake.getBounds().intersects(food.getBounds())) {
                snake.grow();
                food.spawn(WIDTH, HEIGHT); // З'їли їжу, спавнимо в нових межах
                updateGameArea(WIDTH, HEIGHT); // Оновлюємо ігрову площу
                std::cout << "Apples Eaten: " << (snake.getLength() - 1) << std::endl; // Виводимо скільки яблук з'їдено
            }

            // Очищення екрану
            window.clear();

            // Виведення змійки
            for (auto& segment : snake.body) {
                window.draw(segment);
            }

            // Виведення їжі
            window.draw(food.shape);

            // Виведення меж
            drawBorders(window);

            // Перевірка на зіткнення з червоними межами
            if (checkCollisionWithBorders(snake)) {
                snake.die();
            }

            // Відображення вікна
            window.display();
        }
        else {
            // Якщо змійка мертва, виводимо повідомлення
            std::cout << "Game Over! Final score: " << (snake.getLength() - 1) << std::endl;
            window.close(); // Закриваємо вікно
        }
    }

    return 0;
}
