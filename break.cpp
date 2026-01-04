#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>
using namespace sf;

struct Brick { RectangleShape shape; bool alive; };

std::vector<Brick> createLevel(int level, int windowWidth) {
    std::vector<Brick> bricks;
    int rows = 3 + level;
    int cols = 10;
    float brickWidth = windowWidth / cols;
    float brickHeight = 20.f;
    float offsetY = 50.f;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            RectangleShape rect(Vector2f(brickWidth, brickHeight));
            int red = 180 + (rand() % 50);
            int green = 100 + (rand() % 50);
            int blue = 180 + (rand() % 50);
            rect.setFillColor(Color(red, green, blue));
            rect.setPosition(j * brickWidth, offsetY + i * (brickHeight + 5.f));
            bricks.push_back({rect, true});
        }
    }
    return bricks;
}

void drawSmoothGradient(RenderWindow &window) {
    VertexArray gradient(Quads, 4);
    gradient[0].position = Vector2f(0.f, 0.f);
    gradient[1].position = Vector2f(window.getSize().x, 0.f);
    gradient[2].position = Vector2f(window.getSize().x, window.getSize().y);
    gradient[3].position = Vector2f(0.f, window.getSize().y);

    gradient[0].color = Color(255, 230, 240);
    gradient[1].color = Color(255, 230, 240);
    gradient[2].color = Color(255, 180, 210);
    gradient[3].color = Color(255, 180, 210);

    window.draw(gradient);
}

int main() {
    srand(static_cast<unsigned int>(time(0)));
    const int windowWidth = 800;
    const int windowHeight = 600;
    RenderWindow window(VideoMode(windowWidth, windowHeight), "Breakout SFML 2 - Levels");
    window.setFramerateLimit(60);

    Font font;
    if (!font.loadFromFile("/System/Library/Fonts/Supplemental/Arial.ttf")) return 1;

    Text title("BREAKOUT GAME", font, 60);
    title.setFillColor(Color::White);
    title.setStyle(Text::Bold);
    title.setPosition(120, 100);

    RectangleShape startButton(Vector2f(250, 70)); startButton.setFillColor(Color(255, 182, 193)); startButton.setPosition(275, 250);
    Text startText("START", font, 32); startText.setFillColor(Color::White); startText.setStyle(Text::Bold); startText.setPosition(350, 265);

    RectangleShape levelsButton(Vector2f(250, 70)); levelsButton.setFillColor(Color(255, 182, 255)); levelsButton.setPosition(275, 350);
    Text levelsText("LEVELS", font, 32); levelsText.setFillColor(Color::White); levelsText.setStyle(Text::Bold); levelsText.setPosition(330, 365);

    RectangleShape exitButton(Vector2f(250, 70)); exitButton.setFillColor(Color(255, 105, 180)); exitButton.setPosition(275, 450);
    Text exitText("EXIT", font, 32); exitText.setFillColor(Color::White); exitText.setStyle(Text::Bold); exitText.setPosition(365, 465);

    bool inMenu = true;
    bool inLevelsMenu = false;
    int startLevel = 1;

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) window.close();

            if (inMenu && event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
                if (startButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    startLevel = 1;
                    inMenu = false;
                }
                if (levelsButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    inLevelsMenu = true;
                    inMenu = false;
                }
                if (exitButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    window.close();
                }
            }

            if (inLevelsMenu && event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
                for (int i = 0; i < 5; i++) {
                    RectangleShape levelBtn(Vector2f(120, 60));
                    levelBtn.setPosition(180 + (i%3)*180, 150 + (i/3)*100);
                    if (levelBtn.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                        startLevel = i+1;
                        inLevelsMenu = false;
                    }
                }
            }
        }

        if (inMenu) {
            window.clear();
            drawSmoothGradient(window);
            window.draw(title);
            window.draw(startButton); window.draw(startText);
            window.draw(levelsButton); window.draw(levelsText);
            window.draw(exitButton); window.draw(exitText);
            window.display();
        } else if (inLevelsMenu) {
            window.clear();
            drawSmoothGradient(window);
            Text selectTitle("Select Level", font, 50);
            selectTitle.setFillColor(Color::White); selectTitle.setPosition(240,50);
            window.draw(selectTitle);

            Color levelButtonColor(255, 105, 180);
            for (int i = 0; i < 5; i++) {
                RectangleShape levelBtn(Vector2f(120,60));
                levelBtn.setPosition(180 + (i%3)*180, 150 + (i/3)*100);
                levelBtn.setFillColor(levelButtonColor);

                Text levelText("Level " + std::to_string(i+1), font, 28);
                levelText.setFillColor(Color::White);
                levelText.setPosition(180 + (i%3)*180 + 20, 150 + (i/3)*100 + 15);

                window.draw(levelBtn);
                window.draw(levelText);
            }
            window.display();
        } else {
            RectangleShape paddle(Vector2f(120.f, 20.f)); paddle.setFillColor(Color::Cyan); paddle.setPosition(windowWidth/2.f-60.f, windowHeight-50.f);
            float paddleSpeed = 400.f;

            CircleShape ball(10.f); ball.setFillColor(Color::White); ball.setPosition(windowWidth/2.f, windowHeight/2.f);
            float baseSpeed = 300.f;
            int level = startLevel;
            float speedFactor = 1.f - (level-1)*0.05f;
            float ballSpeedX = baseSpeed * speedFactor;
            float ballSpeedY = -baseSpeed * speedFactor;

            bool gameRunning = true;
            std::vector<Brick> bricks = createLevel(level, windowWidth);
            Clock clock;

            while (window.isOpen() && gameRunning) {
                float dt = clock.restart().asSeconds();
                Event e; while(window.pollEvent(e)) if (e.type==Event::Closed) window.close();

                if ((Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Left)) && paddle.getPosition().x > 0)
                    paddle.move(-paddleSpeed*dt,0.f);
                if ((Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Right)) && paddle.getPosition().x + paddle.getSize().x < windowWidth)
                    paddle.move(paddleSpeed*dt,0.f);

                ball.move(ballSpeedX*dt, ballSpeedY*dt);
                FloatRect ballBounds = ball.getGlobalBounds();

                if (ballBounds.left <=0 || ballBounds.left+ballBounds.width>=windowWidth) ballSpeedX=-ballSpeedX;
                if (ballBounds.top<=0) ballSpeedY=-ballSpeedY;
                if (ball.getGlobalBounds().intersects(paddle.getGlobalBounds())) ballSpeedY=-ballSpeedY;

                for (auto &b : bricks) {
                    if (b.alive && ball.getGlobalBounds().intersects(b.shape.getGlobalBounds())) { b.alive=false; ballSpeedY=-ballSpeedY; break; }
                }

                if (ball.getPosition().y + ball.getRadius()*2 >= windowHeight) {
                    std::cout << "Game Over!" << std::endl; gameRunning=false;
                }

                bool allDead = true;
                for (auto &b : bricks) if (b.alive) allDead=false;

                if (allDead) {
                    level++;
                    if (level>5) { std::cout<<"You Win!"<<std::endl; gameRunning=false; break; }
                    ball.setPosition(windowWidth/2.f, windowHeight/2.f);
                    speedFactor = 1.f - (level-1)*0.05f;
                    ballSpeedX = baseSpeed * speedFactor;
                    ballSpeedY = -baseSpeed * speedFactor;
                    bricks = createLevel(level, windowWidth);
                }

               
                window.clear(Color::Black);
                window.draw(paddle); window.draw(ball);
                for(auto &b:bricks) if(b.alive) window.draw(b.shape);

                Text levelText("Level: "+std::to_string(level), font,20);
                levelText.setFillColor(Color::White); levelText.setPosition(10,10);
                window.draw(levelText);

                window.display();
            }

            inMenu=true;
            startLevel = 1;
        }
    }

    return 0;
}
