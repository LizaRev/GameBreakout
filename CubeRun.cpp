#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace sf;

struct Platform {
    RectangleShape top;
    RectangleShape side;
};

struct Player {
    RectangleShape body;
    CircleShape leftEye, rightEye;
    CircleShape leftPupil, rightPupil;
    RectangleShape mouth;
    Vector2f velocity;
    bool onGround = false;
    float lastPlatformX = 0.f; 
    float lastPlatformY = 0.f; 
};

Platform createPlatform(float lastX, float lastY, bool initial = false) {
    Platform p;
    float width = initial ? 400.f : 100 + rand() % 60; 
    float height = 30.f;

    float gapX = initial ? 0.f : 120 + rand() % 100;
    float offsetX = initial ? lastX : lastX + gapX;

    float offsetY = initial ? lastY : lastY + (rand() % 160 - 80);
    if (offsetY < 150) offsetY = 150;
    if (offsetY > 500) offsetY = 500;

    p.top = RectangleShape(Vector2f(width, height));
    p.top.setFillColor(Color(139, 69, 19));
    p.top.setPosition(offsetX, offsetY);

    p.side = RectangleShape(Vector2f(width, 15.f));
    p.side.setFillColor(Color(90, 40, 10));
    p.side.setPosition(offsetX, offsetY + height);

    return p;
}

int main() {
    srand(static_cast<unsigned int>(time(0)));
    RenderWindow window(VideoMode(800, 600), "Game");
    window.setFramerateLimit(60);

    const float gravity = 0.6f;
    const float moveSpeed = 6.f;
    const float jumpSpeed = -13.f;
    float platformSpeed = 3.5f;

    Player player;
    player.body = RectangleShape(Vector2f(35.f, 35.f));
    player.body.setFillColor(Color::Cyan);

    std::vector<Platform> platforms;

    // Перша довга платформа
    Platform firstPlatform = createPlatform(50.f, 500.f, true);
    platforms.push_back(firstPlatform);

    // Кубик ставимо на першу платформу
    player.body.setPosition(firstPlatform.top.getPosition().x + 50.f,
                            firstPlatform.top.getPosition().y - player.body.getSize().y);
    player.lastPlatformX = player.body.getPosition().x;
    player.lastPlatformY = firstPlatform.top.getPosition().y;

    // Очі та рот
    player.leftEye = CircleShape(5.f); player.leftEye.setFillColor(Color::Black);
    player.rightEye = CircleShape(5.f); player.rightEye.setFillColor(Color::Black);
    player.leftPupil = CircleShape(2.f); player.leftPupil.setFillColor(Color::White);
    player.rightPupil = CircleShape(2.f); player.rightPupil.setFillColor(Color::White);
    player.mouth = RectangleShape(Vector2f(20.f, 5.f)); player.mouth.setFillColor(Color::Red);

    
    for (int i = 0; i < 8; ++i) {
        Platform last = platforms.back();
        platforms.push_back(createPlatform(last.top.getPosition().x + last.top.getSize().x,
                                           last.top.getPosition().y));
    }

   
    RectangleShape ground(Vector2f(1600.f, 50.f));
    ground.setFillColor(Color::Green);
    ground.setPosition(0.f, 550.f);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) window.close();
        }

        
        if (Keyboard::isKeyPressed(Keyboard::A)) player.body.move(-moveSpeed, 0.f);
        if (Keyboard::isKeyPressed(Keyboard::D)) player.body.move(moveSpeed, 0.f);
        if (Keyboard::isKeyPressed(Keyboard::Space) && player.onGround) {
            player.velocity.y = jumpSpeed;
            player.onGround = false;
        }

       
        player.velocity.y += gravity;
        player.body.move(0.f, player.velocity.y);

       
        player.onGround = false;
        bool onAnyPlatform = false;
        FloatRect pBounds = player.body.getGlobalBounds();

        for (auto &p : platforms) {
            FloatRect platBounds = p.top.getGlobalBounds();
            if (pBounds.intersects(platBounds)) {
                if (player.velocity.y > 0.f && pBounds.top + pBounds.height - player.velocity.y <= platBounds.top + 5.f) {
                    player.body.setPosition(pBounds.left, platBounds.top - player.body.getSize().y);
                    player.velocity.y = 0;
                    player.onGround = true;
                    player.lastPlatformY = platBounds.top;
                    player.lastPlatformX = p.top.getPosition().x + 50.f; 
                    onAnyPlatform = true;
                }
            }
        }

        
        for (size_t i = 0; i < platforms.size(); ++i) {
            platforms[i].top.move(-platformSpeed, 0);
            platforms[i].side.move(-platformSpeed, 0);

            if (platforms[i].top.getPosition().x + platforms[i].top.getSize().x < -50) {
                float maxX = 0;
                float lastY = 300;
                for (auto &pl : platforms) {
                    float rightEdge = pl.top.getPosition().x + pl.top.getSize().x;
                    if (rightEdge > maxX) {
                        maxX = rightEdge;
                        lastY = pl.top.getPosition().y;
                    }
                }
                platforms[i] = createPlatform(maxX, lastY);
            }
        }

        
        if (!onAnyPlatform && player.body.getPosition().y > 600) {
            player.body.setPosition(player.lastPlatformX,
                                    player.lastPlatformY - player.body.getSize().y);
            player.velocity.y = 0;
            player.onGround = true;
        }

        
        Vector2f pos = player.body.getPosition();
        player.leftEye.setPosition(pos.x + 8.f, pos.y + 10.f);
        player.rightEye.setPosition(pos.x + 25.f, pos.y + 10.f);
        player.leftPupil.setPosition(pos.x + 10.f, pos.y + 12.f);
        player.rightPupil.setPosition(pos.x + 27.f, pos.y + 12.f);
        player.mouth.setPosition(pos.x + 10.f, pos.y + 25.f);

        
        window.clear(Color(135, 206, 235)); 
        window.draw(ground);
        for (auto &p : platforms) {
            window.draw(p.side);
            window.draw(p.top);
        }
        window.draw(player.body);
        window.draw(player.leftEye);
        window.draw(player.rightEye);
        window.draw(player.leftPupil);
        window.draw(player.rightPupil);
        window.draw(player.mouth);

        window.display();
    }

    return 0;
}
