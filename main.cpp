#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>
using namespace sf;

int main() {
    srand(static_cast<unsigned int>(time(0)));

    const int windowWidth = 800;
    const int windowHeight = 600;
    RenderWindow window(VideoMode(windowWidth, windowHeight), "Flying cube");
    window.setFramerateLimit(60);

   
    RectangleShape player(Vector2f(50.f, 50.f));
    player.setFillColor(Color::Red);
    player.setPosition(100.f, windowHeight / 2.f - 25.f);

    float playerSpeed = 350.f;     
    float obstacleSpeed = 400.f;  
    float gapHeight = 250.f;      

    
    struct Obstacle {
        RectangleShape top;
        RectangleShape bottom;
        bool passed = false; 
    };

    std::vector<Obstacle> obstacles;

    for (int i = 0; i < 3; i++) {
        float x = windowWidth + 300 + i * 400;
        float topHeight = rand() % 200 + 100;

        RectangleShape top(Vector2f(80.f, topHeight));
        top.setFillColor(Color::Blue);
        top.setPosition(x, 0);

        RectangleShape bottom(Vector2f(80.f, windowHeight - topHeight - gapHeight));
        bottom.setFillColor(Color::Blue);
        bottom.setPosition(x, topHeight + gapHeight);

        obstacles.push_back({top, bottom, false});
    }

    int score = 0;
    Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
        }

        
        if (Keyboard::isKeyPressed(Keyboard::Up))
            player.move(0.f, -playerSpeed * dt);
        if (Keyboard::isKeyPressed(Keyboard::Down))
            player.move(0.f, playerSpeed * dt);

      
        if (player.getPosition().y < 0) player.setPosition(100.f, 0);
        if (player.getPosition().y + 50 > windowHeight) player.setPosition(100.f, windowHeight - 50);

       
        for (auto &ob : obstacles) {
            ob.top.move(-obstacleSpeed * dt, 0.f);
            ob.bottom.move(-obstacleSpeed * dt, 0.f);

            
            if (ob.top.getPosition().x + 80.f < 0) {
                float x = windowWidth + 400;
                float topHeight = rand() % 200 + 100;

                ob.top.setSize(Vector2f(80.f, topHeight));
                ob.top.setPosition(x, 0);

                ob.bottom.setSize(Vector2f(80.f, windowHeight - topHeight - gapHeight));
                ob.bottom.setPosition(x, topHeight + gapHeight);

                ob.passed = false; 
            }

        
            if (!ob.passed && ob.top.getPosition().x + 80.f < player.getPosition().x) {
                score++;
                ob.passed = true;
                std::cout << "Score: " << score << std::endl;
            }
        }

        
        bool collision = false;
        for (auto &ob : obstacles) {
            if (player.getGlobalBounds().intersects(ob.top.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(ob.bottom.getGlobalBounds())) {
                collision = true;
                break;
            }
        }

        if (collision) {
            std::cout << "Game Over! Final score: " << score << std::endl;
            sf::sleep(sf::seconds(3));
            break;
        }

       
        window.clear(Color::Cyan);
        window.draw(player);
        for (auto &ob : obstacles) {
            window.draw(ob.top);
            window.draw(ob.bottom);
        }
        window.display();
    }

    return 0;
}
