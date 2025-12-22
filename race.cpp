#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>
#include <algorithm>

struct Obstacle {
    sf::RectangleShape shape;
};

int main() {
    srand(static_cast<unsigned int>(time(0)));

    sf::RenderWindow window(sf::VideoMode(800, 600), "2D Racing");
    window.setFramerateLimit(60);

    float speed = 0.f;
    float maxSpeed = 5.f;

  
    sf::RectangleShape leftLine(sf::Vector2f(10.f, 600.f));
    leftLine.setFillColor(sf::Color::White);
    leftLine.setPosition(200.f, 0.f);

    sf::RectangleShape rightLine(sf::Vector2f(10.f, 600.f));
    rightLine.setFillColor(sf::Color::White);
    rightLine.setPosition(600.f, 0.f);

    
    std::vector<Obstacle> obstacles;
    float obstacleTimer = 0.f;
    float obstacleInterval = 120.f;

    
    sf::Vector2f carPos(400.f, 500.f);
    float carRotation = 0.f;

    sf::RectangleShape carBody(sf::Vector2f(50.f, 100.f));
    carBody.setFillColor(sf::Color(200,0,0));
    carBody.setOrigin(25.f, 50.f);

    sf::RectangleShape carRoof(sf::Vector2f(30.f, 40.f));
    carRoof.setFillColor(sf::Color(150,0,0));
    carRoof.setOrigin(15.f, 20.f);

    sf::CircleShape wheelFL(10.f);
    wheelFL.setFillColor(sf::Color::Black);
    wheelFL.setOrigin(10.f, 10.f);
    sf::CircleShape wheelFR(10.f);
    wheelFR.setFillColor(sf::Color::Black);
    wheelFR.setOrigin(10.f, 10.f);
    sf::CircleShape wheelBL(10.f);
    wheelBL.setFillColor(sf::Color::Black);
    wheelBL.setOrigin(10.f, 10.f);
    sf::CircleShape wheelBR(10.f);
    wheelBR.setFillColor(sf::Color::Black);
    wheelBR.setOrigin(10.f, 10.f);

    sf::CircleShape headlightL(5.f);
    headlightL.setFillColor(sf::Color::Yellow);
    headlightL.setOrigin(5.f,5.f);
    sf::CircleShape headlightR(5.f);
    headlightR.setFillColor(sf::Color::Yellow);
    headlightR.setOrigin(5.f,5.f);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

       
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) speed += 0.2f;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) speed -= 0.2f;
        else speed *= 0.98f;

        if (speed > maxSpeed) speed = maxSpeed;
        if (speed < -maxSpeed) speed = -maxSpeed;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) carRotation -= 2.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) carRotation += 2.f;

        
        float angleRad = carRotation * 3.14159f / 180.f;
        carPos.x += speed * std::sin(angleRad);
        carPos.y += -speed * std::cos(angleRad);

        
        if (carPos.x - 25.f < leftLine.getPosition().x || carPos.x + 25.f > rightLine.getPosition().x) {
            std::cout << "Game Over! You went off the track.\n";
            window.close();
        }

        
        obstacleTimer += std::abs(speed);
        if (obstacleTimer >= obstacleInterval) {
            obstacleTimer = 0.f;
            Obstacle obs;
            obs.shape.setSize(sf::Vector2f(60.f, 60.f));
            obs.shape.setFillColor(sf::Color::Blue);
            obs.shape.setPosition(300.f + rand() % 200, -60.f);
            obstacles.push_back(obs);
        }

       
        for (auto& obs : obstacles) {
            obs.shape.move(0.f, 3.f + speed / 2.f);
        }

        
        obstacles.erase(
            std::remove_if(obstacles.begin(), obstacles.end(),
                [](const Obstacle& o){ return o.shape.getPosition().y > 600.f; }),
            obstacles.end()
        );

        
        for (auto& obs : obstacles) {
            sf::FloatRect carBounds(carPos.x - 25.f, carPos.y - 50.f, 50.f, 100.f);
            if (carBounds.intersects(obs.shape.getGlobalBounds())) {
                std::cout << "Game Over! Collision!\n";
                window.close();
            }
        }

        
        auto rotateOffset = [&](float ox, float oy) -> sf::Vector2f {
            return sf::Vector2f(
                ox * std::cos(angleRad) - oy * std::sin(angleRad),
                ox * std::sin(angleRad) + oy * std::cos(angleRad)
            );
        };

        carBody.setPosition(carPos);
        carBody.setRotation(carRotation);

        carRoof.setPosition(carPos + rotateOffset(0.f,-20.f));
        carRoof.setRotation(carRotation);

        wheelFL.setPosition(carPos + rotateOffset(-20.f,-30.f));
        wheelFL.setRotation(carRotation);
        wheelFR.setPosition(carPos + rotateOffset(20.f,-30.f));
        wheelFR.setRotation(carRotation);
        wheelBL.setPosition(carPos + rotateOffset(-20.f,30.f));
        wheelBL.setRotation(carRotation);
        wheelBR.setPosition(carPos + rotateOffset(20.f,30.f));
        wheelBR.setRotation(carRotation);

        headlightL.setPosition(carPos + rotateOffset(-15.f,-50.f));
        headlightR.setPosition(carPos + rotateOffset(15.f,-50.f));

        
        window.clear(sf::Color(50, 150, 50));
        window.draw(leftLine);
        window.draw(rightLine);
        window.draw(carBody);
        window.draw(carRoof);
        window.draw(wheelFL);
        window.draw(wheelFR);
        window.draw(wheelBL);
        window.draw(wheelBR);
        window.draw(headlightL);
        window.draw(headlightR);
        for (auto& obs : obstacles)
            window.draw(obs.shape);

        window.display();
    }

    return 0;
}
