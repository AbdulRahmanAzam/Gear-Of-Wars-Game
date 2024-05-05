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

enum button_states {BTN_IDLE = 0, BTN_HOVER, BTN_ACTIVE};
enum TileType {NORMAL = 0, DAMAGING};
#define GRID_SIZE 50
#define CHECK_GRID_SIZE 70

class Tile{
    private:

    protected:
    // here we have to use sprite but due to set size we are using rectangle shape,
    //  as it makes easy in putting size as gridSize
    RectangleShape shape;
    int type;
    bool collision;

    public:
    Tile(){
        this->collision = false;
        this->type = 0;
    }
    Tile(const unsigned x, const unsigned y, const float gridSzieF, const Texture& texture, const IntRect& textureRect, bool collision = false, int type = TileType::NORMAL){
        this->shape.setSize(Vector2f(gridSzieF, gridSzieF));
        this->shape.setPosition(x * gridSzieF, y * gridSzieF);
        this->shape.setTexture(&texture);
        // IntRect currentFrame = IntRect(0, 0, 48, 30);
        this->shape.setTextureRect(textureRect);

        this->collision = collision;
        this->type = type;
    }
    virtual ~Tile(){}

    const string getString() const {
        stringstream ss;
        ss << this->shape.getTextureRect().left << " " << this->shape.getTextureRect().top << " " << this->collision << " " <<  this->type;
        return ss.str();
    }
    const Vector2f& getPosition() const{
        return this->shape.getPosition();
    }
    const bool& getCollision() const {
        return this->collision;
    }

    void update(){

    }

    void draw(RenderTarget& target){
        target.draw(this->shape);
    }
};

class TileMap{  
    private:
    float gridSizeF;
    unsigned gridSizeU;
    Vector2u maxSize;
    unsigned layers;
    vector<vector<vector<Tile* > > > mp;
    Texture tileTextureSheet;
    string textureFile;
    
    RectangleShape collisionBox;

    public:
    TileMap(float gridSize, unsigned width, unsigned height, const string textureFile) {
        this->gridSizeF = GRID_SIZE;
        this->gridSizeU = static_cast<unsigned>(this->gridSizeF);
        this->maxSize.x = width + 10; //// this was only width by default
        this->maxSize.y = height + 10;  //// this was only height by default
        this->layers = 1;
        this->textureFile = textureFile;

        // this->mp.resize(this->maxSize.x);
        // for(int i=0;i < maxSize.x; i++){
        //     this->mp.push_back(vector<vector<Tile*>>());

        //     for(int j=0;j < maxSize.y ; j++){
        //         this->mp[i].resize(this->maxSize.y);
        //         this->mp[j].push_back(vector<Tile*>());

        //         for(int k = 0;k < layers; k++){
        //             this->mp[i][j].resize(this->layers);
        //             this->mp[i][j].push_back(NULL);
        //         }
        //     }
        // }
        this->mp.resize(this->maxSize.x);
        for (int i = 0; i < this->maxSize.x; i++) {
            this->mp[i].resize(this->maxSize.y);
            for (int j = 0; j < this->maxSize.y; j++) {
                this->mp[i][j].resize(this->layers, nullptr);  // Initialize each element to nullptr
            }
        }
        this->tileTextureSheet.loadFromFile(this->textureFile);

        this->collisionBox.setSize(Vector2f(GRID_SIZE, GRID_SIZE));
        this->collisionBox.setFillColor(Color(255, 0, 0, 50));
        this->collisionBox.setOutlineColor(Color::Red);
        this->collisionBox.setOutlineThickness(1.f);
    }
    virtual ~TileMap(){
        this->clear();
    }

    bool isValidPos(const unsigned x, const unsigned y, const unsigned z){
        return (x < this->maxSize.x && x >= 0 &&  y < this->maxSize.y && y >= 0 && z < this->layers && z >= 0);
    }
    const Texture* getTileSheet() const {
        return &this->tileTextureSheet;  
    }

    void clear(){
        for(auto &x : this->mp){
            for(auto &y : x){
                for(auto &z : y){
                    delete z;
                }
                y.clear();
            }
            x.clear();
        }
        mp.clear();
    }
    void saveToFile(const string fileName){
        ofstream out_file;
        out_file.open(fileName);

        if(out_file.is_open()){
            out_file << this->maxSize.x << " " << this->maxSize.y 
            << "\n" << this->gridSizeU 
            << "\n" << this->layers
            << "\n" << this->textureFile
            << "\n";
              
            for(int i=0;i < maxSize.x; i++){
                for(int j=0;j < maxSize.y ; j++){
                    for(int k = 0;k < layers; k++){
                        if(this->mp[i][j][k])
                            out_file << i << " " << j << " " << k << " " << this->mp[i][j][k]->getString() << " ";                    
                    }
                }
            }
        }
        out_file.close();
    }
    void loadFromFile(const string fileName){
        ifstream in_file;
        in_file.open(fileName);

        if(in_file.is_open()){

            Vector2u size;
            unsigned gridSize = 0;
            unsigned layers = 0;
            string texture_file = "";
            unsigned x = 0;
            unsigned y = 0;
            unsigned z = 0;
            unsigned trX = 0;
            unsigned trY = 0;
            bool collision = false;
            int type = 0;


            in_file >> size.x >> size.y >> gridSize >> layers >> texture_file;

            this->gridSizeF = (float)gridSize;
            this->gridSizeU = gridSize;
            this->maxSize.x = size.x;
            this->maxSize.y = size.y;
            this->layers = layers;
            this->textureFile = texture_file;

            this->clear();
            

            // this->mp.resize(this->maxSize.x);
            // for (int i = 0; i < this->maxSize.x; i++) {
            //     this->mp[i].resize(this->maxSize.y);
            //     for (int j = 0; j < this->maxSize.y; j++) {
            //         this->mp[i][j].resize(this->layers, nullptr);  // Initialize each element to nullptr
            //     }
            // }

            this->mp.resize(this->maxSize.x, vector<vector<Tile*>>());
            for (size_t x = 0; x < maxSize.x; x++)
            {
                for (size_t y = 0; y < maxSize.y; y++)
                {
                    this->mp[x].resize(this->maxSize.y, vector<Tile*>());
                    for (size_t z = 0; z < layers; z++)
                    {
                        this->mp[x][y].resize(this->layers, nullptr);
                    }
                    
                }
                
            }
            


            while(in_file >> x >> y >> z >> trX >> trY >> collision >> type){
                this->mp[x][y][z] = new Tile(x, y, gridSizeF, this->tileTextureSheet, IntRect(trX, trY, this->gridSizeU, this->gridSizeU), collision, type);
            }
        }
        in_file.close();
    }

    void addTile(const unsigned x, const unsigned y, const unsigned z, const IntRect& textureRect, const bool& collision, const int& type){
        if(isValidPos(x,y,z) && this->mp[x][y][z] == nullptr){
            this->mp[x][y][z] = new Tile(x, y, gridSizeF, this->tileTextureSheet, textureRect);
        }
    }
    void removeTile(const unsigned x, const unsigned y, const unsigned z){
        if(isValidPos(x,y,z) && this->mp[x][y][z] != nullptr){
            delete this->mp[x][y][z];
            this->mp[x][y][z] = nullptr;
        }
    }
    

    void update(){

    }

    void draw(RenderTarget& target){
        for(auto &i : this->mp){
            for(auto &j : i){
                for(auto *k : j){
                    if(k != nullptr){
                        k->draw(target);
                        if(k->getCollision())
                            this->collisionBox.setPosition(k->getPosition());
                            target.draw(this->collisionBox);
                    }
                }
            }
        }
    }

};

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

    virtual const Vector2f& getPosition() const{
        return this->sprite->getPosition();
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

    void update(const Vector2i& mousePosWindow){
        // IDLE
        this->buttonState = BTN_IDLE;

        // HOVER
        if(this->button.getGlobalBounds().contains(static_cast<Vector2f>(mousePosWindow))){
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
    float gridSize;
    bool quit;

    float keyTime;
    float keyTimeMax;

    Vector2i mousePosScreen;
    Vector2i mousePosWindow;
    Vector2f mousePosView;
    Vector2u mousePosGrid;

    // resources
    map<string, Texture> textures;

    virtual void setKeyBinds() = 0;
    public:
    // constructors / destructors
    State(RenderWindow* window, map<string, int>* Keys, stack<State*>* states) : 
    window(window), Keys(Keys), states(states), quit(false) {
        this->gridSize = GRID_SIZE;
        keyTime = 0;
        keyTimeMax = 1.f;
    }
    virtual ~State(){

    }
    // functions
    const bool getQuit() const{
        // using in Game
        return this->quit;
    }
    bool getKeyTime(){
        if(keyTime < keyTimeMax)
            return false;

        keyTime = 0;
        return true;
    }

    void updateKeyTime(const float& dt){
        if(keyTime < keyTimeMax)
            this->keyTime += 10.f * dt;
    }
    virtual void updateMousePosition(View* view = nullptr){
        this->mousePosScreen = Mouse::getPosition();
        this->mousePosWindow = Mouse::getPosition(*this->window);
        if(view)
            this->window->setView(*view);

        this->mousePosView = this->window->mapPixelToCoords(Mouse::getPosition(*this->window));
        this->mousePosGrid = Vector2u(mousePosView.x / GRID_SIZE, mousePosView.y / GRID_SIZE);

        this->window->setView(this->window->getDefaultView());
    }
    // virtual void updateInput(const float& dt) = 0;
        virtual void update(const float& dt) = 0;
        virtual void draw(RenderTarget* target = nullptr) = 0;
};

class TextureSelector{
    private:
    RectangleShape bounds;
    Sprite sheet;
    RectangleShape selector;
    Vector2u mousePosGridSelector;
    IntRect textureRect;
    bool hidden;
    Button* hideButton;
    bool active;
    // Button* quitButton;  // made by me

    float keyTime;
    float keyTimeMax;

    public:
    TextureSelector(const float x, const float y, const float width, const float height, const Texture* textureSheet, Font& font, string text){
        float offset  = 100.f;

        bounds.setSize(Vector2f(width, height));
        bounds.setPosition(x + offset, y);
        bounds.setFillColor(Color(50, 50, 50, 100));
        bounds.setOutlineColor(Color(255, 255, 255, 200));
        bounds.setOutlineThickness(1.f);
        
        sheet.setTexture(*textureSheet);
        sheet.setPosition(x + offset, y);

        if(sheet.getGlobalBounds().width > bounds.getGlobalBounds().width || sheet.getGlobalBounds().height > bounds.getGlobalBounds().height){
            this->sheet.setTextureRect(IntRect(0, 0, bounds.getGlobalBounds().width, bounds.getGlobalBounds().height));
        }
        

        this->active = false;
        this->hidden = false;
        this->keyTime = 0.f;
        this->keyTimeMax = 1.f;
        
        this->selector.setPosition(x +  offset, y);
        this->selector.setSize(Vector2f(GRID_SIZE, GRID_SIZE));
        this->selector.setFillColor(Color::Transparent);
        this->selector.setOutlineColor(Color::Red);
        this->selector.setOutlineThickness(1.f);

        this->textureRect.width = GRID_SIZE;
        this->textureRect.height = GRID_SIZE;

        this->hideButton = new Button(x, y, 50, 50, font, text, Color(200, 200, 200), Color(150, 150, 150), Color(0, 0, 255));
        // this->quitButton = new Button(x, y + 200, 50, 50, font, "QUIT", Color(200, 200, 200), Color(150, 150, 150), Color(0, 0, 255));
    }
    ~TextureSelector(){
        delete this->hideButton;
        // delete this->quitButton; // made by me
    }

    const bool& getActive() const {
        return this->active;
    }
    const IntRect& getTextureRect() const {
        return this->textureRect;
    }
    const bool getKeyTime(){
        if(this->keyTime >= this->keyTimeMax){
            this->keyTime = 0.f;
            return true;
        }
        return false;
    }
    // const bool getQuit(){
    //     if(this->quitButton->isPressed()){
    //         return true;
    //     }
    //     return false;
    // }
    void updateKeyTime(const float& dt){
        if(this->keyTime < this->keyTimeMax){
            this->keyTime += 10.f * dt;
        }
    }
    void update(Vector2i& mousePosWindow, const float& dt){
        this->updateKeyTime(dt);
        this->hideButton->update(mousePosWindow);

        if(this->hideButton->isPressed() && this->getKeyTime()){
            if(this->hidden)
                this->hidden = false;
            else   
                this->hidden = true;
        }

        if(!hidden){
            // this->active = (this->bounds.getGlobalBounds().contains(Vector2f(mousePosWindow))) ? true : false;
            if(this->bounds.getGlobalBounds().contains(Vector2f(mousePosWindow))){
                this->active = true;
            }else{
                this->active = false;
            }
            
            if(this->active){
                this->mousePosGridSelector.x = (mousePosWindow.x - this->bounds.getPosition().x) / GRID_SIZE;
                this->mousePosGridSelector.y = (mousePosWindow.y - this->bounds.getPosition().y) / GRID_SIZE;
                
                this->selector.setPosition(bounds.getPosition().x + this->mousePosGridSelector.x * GRID_SIZE,  
                                        bounds.getPosition().y + this->mousePosGridSelector.y * GRID_SIZE);
                
                this->textureRect.left = this->selector.getPosition().x - this->bounds.getPosition().x;
                this->textureRect.top = this->selector.getPosition().y - this->bounds.getPosition().y;
                
            }

            // Update TExture Selector

            
        }
    }
    void draw(RenderTarget& target){

        if(!this->hidden){
            target.draw(this->bounds);
            target.draw(this->sheet);

            
    
        }
        if(this->active)
            target.draw(this->selector);
        this->hideButton->draw(&target);
        // this->quitButton->draw(&target);
    }
};

class EditorState : public State{ //====================================  EDITORSTATE  ============================================================
    private:
    View mainView;
    float cameraSpeed;

    Font font;
    Text cursorText;
    map<string, Button*> buttons;

    TileMap* mp;

    RectangleShape sideBar; 
    IntRect textureRect;
    RectangleShape selectorRect;
    TextureSelector* textureSelector;
    bool collision;
    int type;

    Button* quitButton;
    Button* saveButton;
    Button* loadButton;

    void setView(){
        // VideoMode desktopMode = VideoMode::getDesktopMode();
        this->mainView.setSize(Vector2f(1366, 768));
        this->mainView.setCenter(1366 / 2.0, 768 / 2.0);
        cameraSpeed = 100.f;
    }
    void setGUI(){
        this->sideBar.setSize(Vector2f(80.f ,window->getSize().y));
        this->sideBar.setFillColor(Color(50, 50, 50, 100));
        this->sideBar.setOutlineColor(Color(200, 200, 200, 150));
        this->sideBar.setOutlineThickness(2.f);

        selectorRect.setSize(Vector2f(GRID_SIZE, GRID_SIZE));
        selectorRect.setFillColor(Color(255, 255,255, 150));
        selectorRect.setOutlineColor(Color::Green);
        selectorRect.setOutlineThickness(1.f);

        selectorRect.setTexture(this->mp->getTileSheet());
        selectorRect.setTextureRect(this->textureRect);

        this->textureSelector = new TextureSelector(5.f, 5.f, 300.f, 300.f, this->mp->getTileSheet(), this->font, "EDIT");
    }
    void setText(){
        this->cursorText.setFont(this->font);
        this->cursorText.setCharacterSize(12);
    }
    void setVariables(){
        
    }
    void setBackground(){

    }
    void setFonts(){
        if(!this->font.loadFromFile("Fonts\\PublicPixel-E447g.ttf")){

        }
    }
    void setKeyBinds(){
        this->keyBinds["CLOSE"] = this->Keys->at("Escape");
        this->keyBinds["TOGGLE_COLLISION"] = Keyboard::Key::C;
        this->keyBinds["INC_TYPE"] = Keyboard::Key::Up;
        this->keyBinds["DEC_TYPE"] = Keyboard::Key::Down;
        this->keyBinds["CAMERA_UP"] = Keyboard::Key::W;
        this->keyBinds["CAMERA_DOWN"] = Keyboard::Key::S;
        this->keyBinds["CAMERA_LEFT"] = Keyboard::Key::A;
        this->keyBinds["CAMERA_RIGHT"] = Keyboard::Key::D;

    }
    void setButtons(){
        
        this->quitButton = new Button(5, 200, 50, 50, font, "QUIT", Color(200, 200, 200), Color(150, 150, 150), Color(0, 0, 255));
        this->loadButton = new Button(5, 300, 50, 50, font, "LOAD", Color(200, 200, 200), Color(150, 150, 150), Color(0, 0, 255));
        this->saveButton = new Button(5, 400, 50, 50, font, "SAVE", Color(200, 200, 200), Color(150, 150, 150), Color(0, 0, 255));
    }
    void setTileMap(){
        this->textureRect = IntRect(0, 0, 48, 30);
        this->mp = new TileMap(GRID_SIZE, 20, 20, "Textures\\TileSet_Gras_Works.png");

        this->collision = false;
        this->type = TileType::NORMAL;
    }

    public:
    EditorState(RenderWindow* window, map<string, int>* Keys, stack<State*>* states) : State(window, Keys, states) {
        this->setVariables();
        this->setView();
        this->setBackground();
        this->setFonts();
        this->setText();
        this->setKeyBinds();
        this->setButtons();
        this->setTileMap();
        this->setGUI();
    }
    virtual ~EditorState(){
        delete this->mp;
        delete this->textureSelector;
        delete this->quitButton;
        delete this->saveButton;
        delete this->loadButton;
        for(auto it = this->buttons.begin(); it != this->buttons.end(); ++it){
            delete it->second;
        }
    }

    void updateGUI(const float& dt){
        this->textureSelector->update(mousePosWindow, dt);
        
        if(!this->textureSelector->getActive()){
            selectorRect.setPosition(Vector2f(this->mousePosGrid.x * GRID_SIZE, this->mousePosGrid.y * GRID_SIZE));
            selectorRect.setTextureRect(this->textureRect);   /// ERROR check it out
        }
        stringstream ss;
        ss << this->mousePosView.x << "  " << this->mousePosView.y
        << "\n" << this->mousePosGrid.x << " " << this->mousePosGrid.y 
        << "\n" << this->textureRect.left << " " << this->textureRect.top
        << "\nCollision " << this->collision
        <<"\nType " << this->type;

        this->cursorText.setPosition(this->mousePosView.x + 70.f, this->mousePosView.y - 20);
        this->cursorText.setString(ss.str());

    }
    void updateInput(const float& dt){
        if(Keyboard::isKeyPressed(Keyboard::Key(this->keyBinds.at("CAMERA_UP")))){
            this->mainView.move(0, -this->cameraSpeed * dt);
        }else if(Keyboard::isKeyPressed(Keyboard::Key(this->keyBinds.at("CAMERA_DOWN")))){
            this->mainView.move(0, this->cameraSpeed * dt);
        }

        if(Keyboard::isKeyPressed(Keyboard::Key(this->keyBinds.at("CAMERA_LEFT")))){
            this->mainView.move(-this->cameraSpeed * dt, 0);
        }else if(Keyboard::isKeyPressed(Keyboard::Key(this->keyBinds.at("CAMERA_RIGHT")))){
            this->mainView.move(this->cameraSpeed * dt, 0);
        }

        if(Keyboard::isKeyPressed(Keyboard::Key(this->keyBinds.at("CLOSE"))) || quitButton->isPressed()){
            if(getKeyTime())
                this->quit = true;
            
        }
        if(saveButton->isPressed()){
            this->mp->saveToFile("text.slmp");
        }
        if(loadButton->isPressed()){
            this->mp->loadFromFile("text.slmp");
        }

        if(Mouse::isButtonPressed(Mouse::Left) && getKeyTime()){

            if(!this->sideBar.getGlobalBounds().contains(Vector2f(this->mousePosWindow))){
                if(!this->textureSelector->getActive()){
                    this->mp->addTile(this->mousePosGrid.x, this->mousePosGrid.y, 0, this->textureRect, this->collision, this->type);
                }else{
                    this->textureRect = this->textureSelector->getTextureRect();
                }
            }

        }else if(Mouse::isButtonPressed(Mouse::Right) && getKeyTime() && !this->sideBar.getGlobalBounds().contains(Vector2f(this->mousePosWindow))){
            if(!this->sideBar.getGlobalBounds().contains(Vector2f(this->mousePosWindow))){

                if(!this->textureSelector->getActive()){
                    this->mp->removeTile(this->mousePosGrid.x, this->mousePosGrid.y, 0);
                }
            }
        }

        // Toggle Collision
        if(Keyboard::isKeyPressed(Keyboard::Key(this->keyBinds.at("TOGGLE_COLLISION"))) && getKeyTime()){
            if(this->collision)
                this->collision = false;
            else    
                this->collision = true;
        }
        else if(Keyboard::isKeyPressed(Keyboard::Key(this->keyBinds.at("INC_TYPE"))) && getKeyTime()){
            ++this->type;

        }else if(Keyboard::isKeyPressed(Keyboard::Key(this->keyBinds.at("DEC_TYPE"))) && getKeyTime() && this->type > 0){
            --this->type;
        }
    }
    void updateButtons(const float& dt){
        for(auto &button : buttons){
            button.second->update(static_cast<Vector2i>(this->mousePosWindow));
        }
        quitButton->update(mousePosWindow);
        saveButton->update(mousePosWindow);
        loadButton->update(mousePosWindow);
    }
    void update(const float& dt){
        this->updateMousePosition(&this->mainView);
        this->updateInput(dt);
        // this->checkForQuit();
        this->updateKeyTime(dt);
        this->updateButtons(dt);
        this->updateGUI(dt);
    }

    void drawGUI(RenderTarget& target){
        if(!this->textureSelector->getActive()){
            target.setView(this->mainView);
            target.draw(this->selectorRect);
        }
        
        target.setView(this->window->getDefaultView());
        this->textureSelector->draw(target);
        target.draw(this->sideBar);

        target.setView(this->mainView);
        target.draw(this->cursorText);
    }
    void drawButtons(RenderTarget& target){
        for(auto &button : buttons){
            button.second->draw(&target);
        }
        
        quitButton->draw(&target);
        saveButton->draw(&target);
        loadButton->draw(&target);
    }
    void draw(RenderTarget* target = NULL){
        if(!target)
            target = this->window;

        target->setView(this->mainView);
        this->mp->draw(*target);

        target->setView(this->window->getDefaultView());
        this->drawButtons(*target);
        this->drawGUI(*target);
    }

};

class GameState : public State{  //====================================  GAMESTATE  ============================================================
    private:
    // Entity player;
    View mainView;
    RenderTexture renderTexture;
    Sprite renderSprite;

    Player* player;
    RectangleShape background;
    Texture bgtexture;

    TileMap* mp;

    void setView(){
        this->mainView.setSize(Vector2f(1366 , 768));
        this->mainView.setCenter(Vector2f(1366/2 , 768/2));
    }
    void setdefferedRender(){
        renderTexture.create(1366, 768);

        renderSprite.setTexture(this->renderTexture.getTexture());
        renderSprite.setTextureRect(IntRect(0, 0, 1366, 768));
    }
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
    void setTileMap(){
        this->mp = new TileMap(GRID_SIZE, 10, 10,  "Textures\\TileSet_Gras_Works.png");
        this->mp->loadFromFile("text.slmp");
    }
    public:
    // constructors / destructors
    GameState(RenderWindow* window, map<string, int>* Keys, stack<State*>* states) : State(window, Keys, states){
        this->setView();
        this->setdefferedRender();
        this->setbackground();
        this->setKeyBinds();
        this->setTileMap();
        this->setPlayer();
    }
    virtual ~GameState(){
        delete this->mp;
        delete this->player;
    }

    // functions
    void updateView(const float& dt){
        this->mainView.setCenter(this->player->getPosition());
    }
    void updateInput(const float &dt){
        // this->checkForQuit();
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
        this->updateMousePosition(&this->mainView);
        // this->updateKeyTime(dt);
        this->updateView(dt);
        this->updateInput(dt);
        this->player->update(dt);
    }
    
    void draw(RenderTarget* target = NULL){
        if(!target)
            target = this->window;

        this->renderTexture.clear();

        // target = &this->renderTexture;

        this->renderTexture.setView(this->mainView);
        // target->draw(background);
        this->renderTexture.draw(background);
        this->mp->draw(this->renderTexture);


        // this->mp->draw(*target);
        this->player->draw(this->renderTexture);

        // renderTexture.setView(this->renderTexture.getDefaultView());   // i dunno whether to put or not because he putted in the if paused

        // FINAL RENDER
        this->renderTexture.display();
        this->renderSprite.setTexture(this->renderTexture.getTexture());
        target->draw(this->renderSprite);
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
        this->buttons["EDITOR_STATE"] = new Button(100, 500, 150, 50, this->font, "EDITOR", Color(200, 200, 200), Color(150, 150, 150), Color(0, 0, 255));
        this->buttons["EXIT"] = new Button(100, 650, 150, 50, this->font, "EXIT", Color(200, 200, 200), Color(150, 150, 150), Color(0, 0, 255));
    }
    public:
    //constructor / destructor
    MainMenuState(RenderWindow* window, map<string, int>* Keys, stack<State*>* states) : State(window, Keys, states) {
        this->setMusic();
        this->setBackground();
        this->setFonts();
        this->setKeyBinds();
        this->setButtons();
    }
    virtual ~MainMenuState(){
        for(auto it = this->buttons.begin(); it != buttons.end(); ++it){
            delete it->second;
        }
    }

    void setMusic(){
        music.openFromFile("Music\\PUBG Theme Song (2Scratch Trap Remix).ogg");
        music.play();
    }
    // functions
    void updateButtons(const float& dt){   
        for(auto &button : buttons){
            button.second->update(this->mousePosWindow);
        }

        if(this->buttons["GAME_STATE"]->isPressed()){
            music.stop();
            this->states->push(new GameState(this->window, this->Keys, this->states));
        }

        if(this->buttons["EDITOR_STATE"]->isPressed()){
            music.stop();        
            this->states->push(new EditorState(this->window, this->Keys, this->states));
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
        
        // this->gridSize;
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
        window->clear(Color::Black);

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
