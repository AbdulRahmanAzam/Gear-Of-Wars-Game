#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <vector>
#include <math.h>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <cmath>
#include <string>
#include <map>
#include <stack>

// LENGTH OF VECTOR: |V| = sqrt(V.x^2 + V.y^2)
// NORMALIZE VECTOR: U = V / |V|
using namespace std;
using namespace sf;

enum button_states{BTN_IDLE = 0, BTN_HOVER, BTN_ACTIVE};

class Entity{  // ===================================================  ENTITY  ============================================================

    protected:
    Texture* texture;
    Sprite* sprite;

    float movementSpeed;

    public:
    // constructor / destructor
    Entity(){
        this->texture = NULL;
        this->sprite = NULL;
        this->movementSpeed = 100.f;
    }
    virtual ~Entity(){
        delete this->sprite;
    }

    void setSprite(Texture* texture){
        this->texture = texture;
        this->sprite = new Sprite(*this->texture);
        this->sprite->setScale(Vector2f(0.2f, 0.2f));
    }
    virtual void setPosition(const float x, const float y){
        if(this->sprite)
            this->sprite->setPosition(x,y);
    }
    virtual void move(const float& dt, const float x, const float y){
        if(this->sprite)
            this->sprite->move(x * this->movementSpeed * dt, y * this->movementSpeed * dt);
    }

    virtual void update(const float& dt){
        
    };
    virtual void draw(RenderTarget* target){
        if(this->sprite)
            target->draw(*this->sprite);
    };
};

class Player : public Entity{  //===================================================  PLAYER  ============================================================
    private:
    // Sprite sprite;

    void setVariable(){

    }
    void setComponents(){

    }
    public:
    // constructors/ destructors
    Player(float x, float y, Texture* texture){
        this->setVariable();
        this->setComponents();
        this->setSprite(texture);
        this->setPosition(x, y);
    }
    Player(){}
    virtual ~Player(){

    }

    // Accessors

    // Functions
    void update(const float& dt){

    }

    void draw(RenderTarget& target){
        target.draw(*this->sprite);
    }
};

class Button{ //===================================================  BUTTON  ============================================================
    private:
    int buttonState;

    RectangleShape button;
    Font font;
    Text text;

    Color idleColor;
    Color hoverColor;
    Color activeColor;

    public:
    Button(float x, float y, float width, float height, Font& font, string text, 
        Color idleColor, Color hoverColor, Color activeColor) : idleColor(idleColor), 
        hoverColor(hoverColor), activeColor(activeColor), font(font) {
        button.setPosition(Vector2f(x, y));
        button.setSize(Vector2f(width, height));
        
        this->text.setFont(font);
        this->text.setString(text);
        this->text.setFillColor(Color::Magenta);
        this->text.setCharacterSize(12);
        this->text.setPosition(
            button.getPosition().x + (button.getGlobalBounds().width / 2.f) - this->text.getGlobalBounds().width / 2.f,
            button.getPosition().y + (button.getGlobalBounds().height / 2.f) - this->text.getGlobalBounds().height / 2.f
        );

        button.setFillColor(idleColor);
        buttonState = BTN_IDLE;
    }
    virtual ~Button(){

    }

    const bool isPressed() const {
        return buttonState == BTN_ACTIVE;
    }

    void update(const Vector2f mousePos){
        // IDLE
        this->buttonState = BTN_IDLE;

        // HOVER
        if(this->button.getGlobalBounds().contains(mousePos)){
            this->buttonState = BTN_HOVER;

            // PRESSED
            if(Mouse::isButtonPressed(Mouse::Left)){
                this->buttonState = BTN_ACTIVE;
            }
        }

        if(this->buttonState == BTN_HOVER){
            this->button.setFillColor(hoverColor);
        }else if(this->buttonState == BTN_ACTIVE){
            this->button.setFillColor(activeColor);
        }else{
            this->button.setFillColor(idleColor);
        }
    }

    void draw(RenderTarget* target){
        target->draw(this->button);
        target->draw(this->text);
    }
};

class State{ // ABSTRACT CLASSES ======================================  STATE  ============================================================
    private:

    protected:
    stack<State*>* states;
    RenderWindow* window;
    map<string, int>* Keys;
    map<string, int> keyBinds;
    bool quit;

    Vector2i mousePosScreen;
    Vector2i mousePosWindow;
    Vector2f mousePosView;
    Vector2i mousePosGrid;

    // resources
    map<string, Texture> textures;

    virtual void setKeyBinds() = 0;

    public:
    // constructors / destructors
    State(RenderWindow* window, map<string, int>* Keys, stack<State*>* states) : 
    window(window), Keys(Keys), states(states), quit(false) {
        
    }
    virtual ~State(){

    }
    
    // functions
    const bool getQuit() const{
        // using in Game
        return this->quit;
    }
    // virtual void checkForQuit(){
    //     if(Keyboard::isKeyPressed(Keyboard::Key(this->keyBinds.at("CLOSE")))){
    //         this->quit = true;
    //     }
    // }

    virtual void updateMousePosition(){
        this->mousePosScreen = Mouse::getPosition();
        this->mousePosWindow = Mouse::getPosition(*this->window);
        this->mousePosView = this->window->mapPixelToCoords(Mouse::getPosition(*this->window));
        // this->mousePosGrid =;
    }
    // virtual void updateInput(const float& dt) = 0;
    virtual void update(const float& dt) = 0;
    virtual void draw(RenderTarget* target = nullptr) = 0;
};

class GameState : public State{  //====================================  GAMESTATE  ============================================================
    private:
    // Entity player;
    Player* player;
    RectangleShape background;
    Texture bgtexture;

    void setbackground(){
        background.setSize(Vector2f(window->getSize()));
        bgtexture.loadFromFile("Textures\\background.png");
        background.setTexture(&this->bgtexture);
    }
    void setKeyBinds(){
            this->keyBinds["CLOSE"] = this->Keys->at("Escape");

            this->keyBinds["MOVE_LEFT"] = this->Keys->at("A");
            this->keyBinds["MOVE_RIGHT"] = this->Keys->at("D");
            this->keyBinds["MOVE_UP"] = this->Keys->at("W");
            this->keyBinds["MOVE_DOWN"] = this->Keys->at("S");

    }
    void setPlayer(){
        this->textures["PLAYER_IDLE"].loadFromFile("Textures\\Player.png");
        this->player = new Player(0, 0, &this->textures["PLAYER_IDLE"]);
    }
    public:
    // constructors / destructors
    GameState(RenderWindow* window, map<string, int>* Keys, stack<State*>* states) : State(window, Keys, states){
        this->setbackground();
        this->setKeyBinds();
        this->setPlayer();
    }
    virtual ~GameState(){
        delete this->player;
    }

    // functions
    void updateInput(const float &dt){
        // this->checkForQuit();

        // check it later whether it work correct or not (now doing same as he is teaching)
        // if later it works then remove setkeybinds and remove all the below stuff
        // if(Keyboard::isKeyPressed(Keyboard::A)){
        //     this->player.move(dt, -10.f, 0.f);
        // }   
        // if(Keyboard::isKeyPressed(Keyboard::D)){
        //     this->player.move(dt, 10.f, 0.f);
        // }   
        // if(Keyboard::isKeyPressed(Keyboard::W)){
        //     this->player.move(dt, 0.f, -10.f);
        // }
        // if(Keyboard::isKeyPressed(Keyboard::S)){
        //     this->player.move(dt, 0.f, 10.f);
        // }


        if(Keyboard::isKeyPressed(Keyboard::Key(this->keyBinds.at("MOVE_LEFT")))){
            this->player->move(dt, -10.f, 0.f);
        }   
        if(Keyboard::isKeyPressed(Keyboard::Key(this->keyBinds.at("MOVE_RIGHT")))){
            this->player->move(dt, 10.f, 0.f);
        }   
        if(Keyboard::isKeyPressed(Keyboard::Key(this->keyBinds.at("MOVE_UP")))){
            this->player->move(dt, 0.f, -10.f);
        }
        if(Keyboard::isKeyPressed(Keyboard::Key(this->keyBinds.at("MOVE_DOWN")))){
            this->player->move(dt, 0.f, 10.f);
        }

        if(Keyboard::isKeyPressed(Keyboard::Key(this->keyBinds.at("CLOSE")))){
            this->quit = true;
        }
    }
    void update(const float& dt){

        this->updateMousePosition();
        this->updateInput(dt);
        // if (this->player) {
            this->player->update(dt);
        // } 
    }
    
    void draw(RenderTarget* target = NULL){
        if(!target)
            target = this->window;

        target->draw(background);
        // target->draw(&player);
        this->player->draw(*target);
        // if(this->player)
    }
};

class MainMenuState : public State { //===============================  MAINMENUSTATE  ============================================================
    private:
    RectangleShape background;
    Texture bgTexture;
    Font font;
    Text text;

    Music music;

    map<string, Button*> buttons;

    void setBackground(){
        this->background.setSize(Vector2f(this->window->getSize()));
        // this->bgTexture.loadFromFile("C:\\Users\\azama\\SFML CODE\\RPG game\\Background.png");
        this->bgTexture.loadFromFile("Textures\\mainmenu.jpg");
        this->background.setTexture(&this->bgTexture);
    }
    void setKeyBinds(){
        this->keyBinds["CLOSE"] = this->Keys->at("Escape");

        // this->keyBinds["MOVE_LEFT"] = this->Keys->at("A");
        // this->keyBinds["MOVE_RIGHT"] = this->Keys->at("D");
        // this->keyBinds["MOVE_UP"] = this->Keys->at("W");
        // this->keyBinds["MOVE_DOWN"] = this->Keys->at("S");
    }
    void setFonts(){
        if(!this->font.loadFromFile("Fonts\\PublicPixel-E447g.ttf")){
            
        }
    }
    void setButtons(){
        this->buttons["GAME_STATE"] = new Button(100, 200, 150, 50, this->font, "NEW GAME", Color(200, 200, 200), Color(150, 150, 150), Color(0, 0, 255));
        this->buttons["SETTINGS"] = new Button(100, 350, 150, 50, this->font, "SETTINGS", Color(200, 200, 200), Color(150, 150, 150), Color(0, 0, 255));
        this->buttons["EXIT"] = new Button(100, 500, 150, 50, this->font, "QUIT", Color(200, 200, 200), Color(150, 150, 150), Color(0, 0, 255));
    }
    void setMusic(){
        music.openFromFile("Music\\PUBG Theme Song (2Scratch Trap Remix).ogg");
        music.play();
    }
    public:
    //constructor / destructor
    MainMenuState(RenderWindow* window, map<string, int>* Keys, stack<State*>* states) : State(window, Keys, states) {
        this->setMusic();
        this->setBackground();
        // this->background.setFillColor(Color::White);
        this->setFonts();
        this->setKeyBinds();
        
        this->setButtons();
    }
    virtual ~MainMenuState(){
        for(auto it = this->buttons.begin(); it != buttons.end(); ++it){
            delete it->second;
        }
    }

    // functions
    void updateButtons(const float& dt){   
        for(auto &button : buttons){
            button.second->update(this->mousePosView);
        }

        if(this->buttons["GAME_STATE"]->isPressed()){
            music.stop();
            this->states->push(new GameState(this->window, this->Keys, this->states));
        }

        if(this->buttons["EXIT"]->isPressed()){
            this->quit = true;
        }
    }   
    // void updateInput(const float& dt){
    //     this->checkForQuit();
    // }
    void checkForQuit(){
        if(Keyboard::isKeyPressed(Keyboard::Key(this->keyBinds.at("CLOSE")))){
            this->quit = true;
        }
    }
    void update(const float& dt){
        this->updateMousePosition();
        // this->updateInput(dt);
        this->checkForQuit();
        this->updateButtons(dt);
    }

    void drawButtons(RenderTarget* target){
        for(auto &button : buttons){
            button.second->draw(target);
        }
    }
    void draw(RenderTarget* target){
        if(!target)
            target = this->window;

        target->draw(this->background);
        this->drawButtons(target);

        // REMOVE LATER !!!
        Text mouseText;
        mouseText.setPosition(this->mousePosView.x, this->mousePosView.y);
        mouseText.setFont(this->font);
        mouseText.setCharacterSize(12);
        stringstream ss;
        ss << this->mousePosView.x << "  " << this->mousePosView.y;
        mouseText.setString(ss.str());
        target->draw(mouseText);
    }
};

class Game{  // ===================================================  GAME  ============================================================
    private:
    RenderWindow* window;
    Event event;

    Image icon;

    Clock clock;
    float dt;

    stack<State*> states;
    map<string, int> Keys;

    void setVariables(){

    }
    void pollevent(){
        while (this->window->pollEvent(this->event)){
            if (event.type == Event::Closed)
                window->close();

            if (event.type == Keyboard::isKeyPressed(Keyboard::Escape))
                window->close();
        }
    }
    void setWindow(){
        this->window = new RenderWindow(VideoMode(1366, 768), "Gear of War", Style::Default);
        this->window->setFramerateLimit(60);
    }
    
    void setStates(){
        this->states.push(new MainMenuState(this->window, &this->Keys, &this->states));
    }
    void setIcon(){
        if(!this->icon.loadFromFile("Textures\\gaming-icon-2.jpg")){

        }
        unsigned int width = icon.getSize().x;
        unsigned int height = icon.getSize().y;
        const sf::Uint8* pixelData = icon.getPixelsPtr();
        window->setIcon(width, height, pixelData);
    }
    void setKeys(){
        // using emplace instead using (pair and insert)
        this->Keys["Escape"] = Keyboard::Key::Escape;
        this->Keys["A"] = Keyboard::Key::A ;
        this->Keys["S"] = Keyboard::Key::S ;
        this->Keys["D"] = Keyboard::Key::D ;
        this->Keys["W"] = Keyboard::Key::W ;
    }
    public:
    // Constructor / Destructor
    Game(){
        this->setWindow();
        this->setVariables();
        this->setKeys();
        this->setStates();
        this->setIcon();
    }
    virtual ~Game(){
        delete this->window;
        while(!states.empty()){
            delete this->states.top();
            this->states.pop();
        }
    }
    // Accessors
    const bool isWindowOpen() const {
        return this->window->isOpen();
    }

    // Functions
    // update
    void updateDt(){
        this->dt = this->clock.restart().asSeconds();
    }
    void updateState(){
        if(!this->states.empty()){
            this->states.top()->update(this->dt);
            if(this->states.top()->getQuit()){

                // this->states.top()->endState();
                delete this->states.top();
                this->states.pop();
            }
        }else{
            this->window->close();
        }
    }
    void update(){
        this->pollevent();
        this->updateDt();
        this->updateState();
    }

    // draw
    void drawState(){
        if(!this->states.empty())
            this->states.top()->draw();
    }
    void draw(){
        window->clear(Color::White);

        this->drawState();

        // drawing finish
        window->display();
    }
};

int main(){  //===================================================  MAIN  ============================================================
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

