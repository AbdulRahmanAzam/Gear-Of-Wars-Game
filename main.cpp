#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <math.h>
#include <sstream>
#include <cmath>
#include <string>

// LENGTH OF VECTOR: |V| = sqrt(V.x^2 + V.y^2)
// NORMALIZE VECTOR: U = V / |V|
using namespace std;
using namespace sf;

class Game{
    private:
    RenderWindow window;
    Event event;
    // Player* player;

    void setVariables(){

    }
    void pollevent(){
        while (this->window.pollEvent(this->event)){
            if (event.type == Event::Closed)
                window.close();

            if (event.type == Keyboard::isKeyPressed(Keyboard::Escape))
                window.close();
        }
    }
    void setWindow(){
        this->window.create(VideoMode(1366, 768), "Gear of War", Style::Default);
        this->window.setFramerateLimit(60);
    }
    void setPlayer(){
        this->player = new Player();
    }

    public:
    // Constructor/ Destructor
    Game(){
        this->setVariables();
        this->setWindow();
    }
    ~Game(){}

    // Accessors
    const bool isWindowOpen() const {
        return this->window.isOpen();
    }

    // Functions
    void updatePlayer(){
        this->player->update;
    }
    void update(){
        this->pollevent();
        this->updatePlayer();
    }

    void drawPlayer(){
        this->player->draw(this->window);
    }
    void draw(){
        window.clear(Color::White);

        this->player->draw(this->window);

        // drawing finish
        window.display();
    }

};

int main()
{
    Game game;

    while (game.isWindowOpen())
    {
        // updating
        game.update();

        // drawing
        game.draw();
    }

    return 0;
}