// juliasets.cpp 1006 TESTING
#include <iostream>
#include <SFML/Graphics.hpp>

static sf::Vector2f doubleEmulate(double val) {
    float fval = (float)val;
    return sf::Vector2f(fval, val - fval);
}

int main() {
    // Create a window (800x600 with title "Julia Set")
    sf::Vector2u windowSize(800, 600);
    sf::RenderWindow window(sf::VideoMode(windowSize), "Julia Set");
    sf::Shader shader;
    sf::RectangleShape shaderQuad = sf::RectangleShape(sf::Vector2f(windowSize));
    sf::Clock clock;
    sf::ContextSettings settings;

    float baseZoomFactor = 6.7; // perfect amount
    double zoomFactor = baseZoomFactor;
    float zoomSpeed = 2.0;

    double offsetX = 0.0;
    double offsetY = 0.0;

    int baseIters = 30;
    int iters = 30;
    float dt = 0.0f;
    float sumdt = 0;
    int k = 5;
    int i = 0;

    sf::Vector2f seed(0, 0);
    float seedChangeSpeed = 0.1;

    sf::Vector2f mouseStartDrag(0, 0);
    bool mouseHeld = false;
    bool mouseIsPressed = false;

    if (!shader.loadFromFile("shader.frag", sf::Shader::Type::Fragment)) {
        std::cout << "SHADER ERROR: loading shader failed\n";
        return -1;
    }

    // Main loop (runs until the window is closed)
    while (window.isOpen()) {
        //set uniform stuff
        shader.setUniform("u_resolution", sf::Vector2f(window.getSize()));
        shader.setUniform("numIterations", iters);
        shader.setUniform("u_seed", seed);
        shader.setUniform("e_zoomFactor", doubleEmulate(zoomFactor));
        shader.setUniform("u_time", clock.getElapsedTime().asSeconds());
        //update variables
        dt = clock.restart().asSeconds();
        iters = round(baseIters * k * log(baseZoomFactor / zoomFactor + 1));
        i++;
        sumdt += dt;

        //print stats
        if (sumdt > 1) {
            std::cout << "#################################" << "\n";
            std::cout << "i: " << i << "\n";
            std::cout << "fps: " << round(1 / dt) << "\n";
            std::cout << "iters: " << iters << "\n";
            std::cout << "zoom: " << zoomFactor << "\n";
            std::cout << "offset: " << offsetX << ", " << offsetY << "\n";
            std::cout << "mouse: " << sf::Mouse::getPosition(window).x << ", " << sf::Mouse::getPosition(window).y << "\n";
            std::cout << "screen res " << window.getSize().x << ", ";
            std::cout << window.getSize().y << "\n";
            std::cout << "#################################" << "\n";
            sumdt = 0;
            i = 0;
        }
        // handle mouse drag
        mouseIsPressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        if (mouseIsPressed && !mouseHeld) { // starter, starts drag action
            mouseStartDrag = sf::Vector2f(sf::Mouse::getPosition(window));
            mouseHeld = true;
            std::cout << "mouse start\n";
        }
        else if (mouseHeld && mouseIsPressed) { // during dragging this happens
            // shows changes without committing them until user stops dragging
            shader.setUniform("e_offsetX", doubleEmulate(offsetX + (sf::Mouse::getPosition(window).x - mouseStartDrag.x) * zoomFactor));
            shader.setUniform("e_offsetY", doubleEmulate(offsetY + (sf::Mouse::getPosition(window).y - mouseStartDrag.y) * zoomFactor));
        }
        else if (mouseHeld && !mouseIsPressed) { // when dragging stops
            mouseHeld = false;
            // offset should be the previous offset + current offset 
            offsetX += (sf::Mouse::getPosition(window).x - mouseStartDrag.x) * zoomFactor;
            offsetY += (sf::Mouse::getPosition(window).y - mouseStartDrag.y) * zoomFactor;
            // pass to gpu
            shader.setUniform("e_offsetX", doubleEmulate(offsetX));
            shader.setUniform("e_offsetY", doubleEmulate(offsetY));
            std::cout << "mouse end\n";
        }
        /* FOR DEBUGGING PURPOSES, COMMENTED OUT FOR NOW
        else if (!mouseHeld) {
            mouseStartDrag = sf::Vector2f(0, 0);
        }
        else {
            shader.setUniform("e_offsetX", doubleEmulate(offsetX));
            shader.setUniform("e_offsetY", doubleEmulate(offsetY));
        }*/

        // handle keyboard input
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
            zoomFactor *= pow(1 / zoomSpeed, dt);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
            zoomFactor *= pow(zoomSpeed, dt);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
            seed += sf::Vector2f(seedChangeSpeed * dt, 0);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
            seed += sf::Vector2f(-seedChangeSpeed * dt, 0);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) {
            seed += sf::Vector2f(0, -seedChangeSpeed * dt);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E)) {
            seed += sf::Vector2f(0, seedChangeSpeed * dt);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R)) {
            baseIters += 1;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F)) {
            baseIters -= 1;
        }
        //event loop
        while (const std::optional event = window.pollEvent())
        {
            // Close window: exit
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        // Clear the screen (black)
        window.clear(sf::Color::Black);
        window.draw(shaderQuad, &shader);
        window.display();
    }
    std::cout << "Exiting program normally.\n";
    return 0;
}