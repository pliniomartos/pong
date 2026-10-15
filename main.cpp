#include <SFML/Graphics.hpp>

const sf::Keyboard::Key controls[4] = {
    sf::Keyboard::Key::A,    // Player1 UP
    sf::Keyboard::Key::Z,    // Player1 Down
    sf::Keyboard::Key::Up,   // Player2 UP
    sf::Keyboard::Key::Down, // Player2 Down
};

sf::Vector2f ball_velocity;
bool is_player_serving = true;
const float initial_velocity_x = 100.f;
const float initial_velocity_y = 60.f;
const float velocity_multiplier = 1.1f;
bool ai_mode = true;

//Parameters
const sf::Vector2f paddleSize(25.f, 100.f);
const float ballRadius =10.f;
const int gameWidth = 800;
const int gameHeight = 600;
const float paddleSpeed = 400.f;
const float paddleOffsetWall = 10.f;
const float time_step = 0.017f; //60fps

//Objects of the game
sf::CircleShape ball;
sf::RectangleShape paddles[2];
sf::Font font;
sf::Text text;

//score
int player1_score = 0;
int player2_score = 0;

void reset() {
    //reset paddle position
    paddles[0].setPosition(paddleOffsetWall + paddleSize.x /2.f, gameHeight / 2.f);
    paddles[1].setPosition(gameWidth - paddleOffsetWall - paddleSize.x / 2.f, gameHeight / 2.f);
    //reset Ball Position
    ball.setPosition(gameWidth / 2.f, gameHeight / 2.f);
    //reset ball velocity
    ball_velocity = sf::Vector2f((is_player_serving ? initial_velocity_x : -initial_velocity_x), initial_velocity_y);

    // Update Score Text
    text.setString(std::to_string(player1_score) + " : " + std::to_string(player2_score));
    // Keep Score Text Centered
    text.setPosition((gameWidth * .5f) - (text.getGlobalBounds().width * .5f), 10.f);

}
void init(){
    //Set size and origin of paddles
    for (sf::RectangleShape &p : paddles) {
        p.setSize(paddleSize);
        p.setOrigin(paddleSize / 2.f);
    }
    //set size and origin of ball
    ball.setRadius(ballRadius);
    ball.setOrigin(ballRadius, ballRadius); // Should be half ball width and height

    //Load font-face form res dir
    font.loadFromFile("resources/fonts/RobotoMono-Regular.ttf");
    //ste text element to use font
    text.setFont(font);
    //set the character size to 24 pixels
    text.setCharacterSize(24);
    //set text color
    text.setFillColor(sf::Color::White);

    reset();
}

void update(float dt) {
    // Update Everything
    // handle paddle movement
    float direction = 0.0f;
    if (sf::Keyboard::isKeyPressed(controls[0])) {
        direction--;
    }
    if (sf::Keyboard::isKeyPressed(controls[1])) {
        direction++;
    }
    paddles[0].move(sf::Vector2f(0.f, direction * paddleSpeed * dt));

    // handle paddle 2 movement (AI or Player 2)
    if (ai_mode) {
        // AI: move paddle toward ball's Y position
        float ai_direction = 0.0f;
        float paddle_y = paddles[1].getPosition().y;
        float ball_y = ball.getPosition().y;

        // Add a small threshold to prevent jittering
        const float threshold = 5.0f;

        if (ball_y < paddle_y - threshold) {
            ai_direction = -1.0f; // Move up
        } else if (ball_y > paddle_y + threshold) {
            ai_direction = 1.0f; // Move down
        }

        paddles[1].move(sf::Vector2f(0.f, ai_direction * paddleSpeed * dt));
    } else {
        // Player 2 controls
        float direction2 = 0.0f;
        if (sf::Keyboard::isKeyPressed(controls[2])) {
            direction2--;
        }
        if (sf::Keyboard::isKeyPressed(controls[3])) {
            direction2++;
        }
        paddles[1].move(sf::Vector2f(0.f, direction2 * paddleSpeed * dt));
    }

    //move ball
    ball.move(ball_velocity * dt);

    //check ball collision
    const float bx = ball.getPosition().x;
    const float by = ball.getPosition().y;
    if (by > gameHeight) {
        ball_velocity.y *= -1.f;
        ball.move(sf::Vector2f(0.f, -10.f));
    }else if (by < 0) {
        ball_velocity.y *= -1.f;
        ball.move(sf::Vector2f(0.f, 10.f));
    }else if (bx > gameWidth) {
        //right wall;
        player1_score++;
        reset();
    }else if (bx < 0) {
        //left wall
        player2_score++;
        reset();
    }else if (
        //ball is inline or behind paddle AND
        bx < paddleSize.x + paddleOffsetWall &&
        //ball is below top edge of paddle AND
        by > paddles[0].getPosition().y - (paddleSize.y * 0.5) &&
        //ball is above bottom edge of paddle
        by < paddles[0].getPosition().y + (paddleSize.y * 0.5)) {
        //bounce off left paddle
        ball_velocity.x *= -1.f;
        ball_velocity.y *= 1.f;
        ball.move(sf::Vector2f(10.f, 0.f));
        } else if (
            //ball is inline or past right paddle AND
            bx > gameWidth - paddleSize.x - paddleOffsetWall &&
            //ball is below top edge of paddle AND
            by > paddles[1].getPosition().y - (paddleSize.y * 0.5) &&
            //ball is above bottom edge of paddle
            by < paddles[1].getPosition().y + (paddleSize.y * 0.5))
        {
            // bounce off right paddle
            ball_velocity.x *= -1.f;
            ball_velocity.y *= 1.f;
            ball.move(sf::Vector2f(-10.f, 0.f));
        }
}

void render(sf::RenderWindow &window) {
    // Draw Everything
    window.draw(paddles[0]);
    window.draw(paddles[1]);
    window.draw(ball);
    window.draw(text);
}

void clean(){
    //free up the memory if necessary.
}

int main () {
    //create the window
    sf::RenderWindow window(sf::VideoMode({gameWidth, gameHeight}), "PONG");
    //initialise and load
    init();

    sf::Clock clock;

    while(window.isOpen()){
        //Calculate dt
        sf::Time elapsed = clock.restart();
        float dt = elapsed.asSeconds();

        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.clear(sf::Color::Black);
        update(dt);
        render(window);
        //wait for the time_step to finish before displaying the next frame.
        sf::sleep(sf::seconds(time_step));
        //Wait for Vsync
        window.display();
    }
    //Unload and shutdown
    clean();

    return 0;
}