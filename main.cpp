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
enum PLAYER_STATE
{
    IDLE = 0,
    LEFT,
    RIGHT,
    JUMPING
};
#define GRID_SIZE 50
#define CHECK_GRID_SIZE 70
#define ENEMY_SPEED 70.0f
#define BULLET_SPEED 15.f

#define WINDOWS_WIDTH 1366
#define WINDOWS_HEIGHT 768

class Bullet2 {
public:
    Sprite shape;

    Bullet2(Texture* texture, float posx, float posy) {
        this->shape.setTexture(*texture);
        this->shape.setScale(0.07f, 0.07f);
        this->shape.setRotation(45);
        this->shape.setPosition(posx, posy + 37.f);
    }
};

class Enemy2 {
public:
    Sprite shape;
    int HP;
    int HPMAX;

    Enemy2(Texture* texture, Vector2u windowSize) {
        this->HPMAX = rand() % 3 + 1;
        this->HP = this->HPMAX;
        this->shape.setTexture(*texture);

        this->shape.setScale(0.2f, 0.2f);
        this->shape.setPosition(windowSize.x - this->shape.getGlobalBounds().width, rand() % int(windowSize.y - this->shape.getGlobalBounds().height));
    }

    void update(float dt) {
        this->shape.move(-7.f * dt, 0.f);
    }
};

class Player2 {
public:
    Sprite shape;
    Texture* texture;
    int HP;
    int HPMAX;

    std::vector<Bullet2> bullets;

    Player2(Texture* texture) {
        this->HPMAX = 10;
        this->HP = this->HPMAX;

        this->texture = texture;
        this->shape.setTexture(*texture);

        this->shape.setScale(0.07f, 0.07f);
        this->shape.setRotation(90);
        this->shape.setPosition(400, 300);
    }

    void shoot(Texture* bulletTexture, float dt) {
        bullets.push_back(Bullet2(bulletTexture, shape.getPosition().x, shape.getPosition().y));
    }

    void updateBullets(float dt, const Vector2u& windowSize, std::vector<Enemy2>& enemies, int& score) {
        for (size_t i = 0; i < bullets.size(); i++) {
            // Move
            bullets[i].shape.move(10.f * dt, 0.f);

            // Out of window bounds
            if (bullets[i].shape.getPosition().x > windowSize.x) {
                bullets.erase(bullets.begin() + i);
                break;
            }

            // Collision
            for (size_t k = 0; k < enemies.size(); k++) {
                if (bullets[i].shape.getGlobalBounds().intersects(enemies[k].shape.getGlobalBounds())) {
                    if (enemies[k].HP <= 1) {
                        score += enemies[k].HPMAX;
                        enemies.erase(enemies.begin() + k);
                    } else {
                        enemies[k].HP--;
                    }
                    bullets.erase(bullets.begin() + i);
                    break;
                }
            }
        }
    }
};

class Game2 {
private:
    RenderWindow window;
    Texture playerTex, enemyTex, bulletTex;
    Font font;
    Player2* player;
    std::vector<Enemy2> enemies;
    Text scoreT, hpT, endT;
    int score;
    float shootTimer, enemyTimer, dtMultiplier;

    void initWindow() {
        this->window.create(VideoMode(1366, 768), "Shooter 360!", Style::Default);
        this->window.setFramerateLimit(60);
    }

    void initTextures() {
        if (!this->font.loadFromFile("..\\Font and Texture\\Balleny.ttf"))
            throw "Could not load font";

        this->playerTex.loadFromFile("..\\Font and Texture\\ship.png");
        this->enemyTex.loadFromFile("..\\Font and Texture\\enemy.png");
        this->bulletTex.loadFromFile("..\\Font and Texture\\missile.png");
    }

    void initPlayer() {
        this->player = new Player2(&playerTex);
    }

    void initText() {
        this->scoreT.setFont(font);
        this->scoreT.setCharacterSize(40);
        this->scoreT.setFillColor(Color::White);
        this->scoreT.setPosition(window.getSize().x - 110.f, 10.f);

        this->hpT.setFont(font);
        this->hpT.setCharacterSize(20);
        this->hpT.setFillColor(Color::White);

        this->endT.setFont(font);
        this->endT.setCharacterSize(100);
        this->endT.setFillColor(Color::Red);
        this->endT.setPosition(window.getSize().x / 2 - 200, window.getSize().y / 2 - 100);
        this->endT.setString("GAME OVER!");
    }

public:
    Game2() {
        this->initWindow();
        this->initTextures();
        this->initPlayer();
        this->initText();

        this->score = 0;
        this->shootTimer = 20.f;
        this->enemyTimer = 20.f;
        this->dtMultiplier = 62.5f;
    }

    ~Game2() {
        delete this->player;
    }

    void run() {
        Clock clock;
        float dt;

        while (this->window.isOpen()) {
            dt = clock.restart().asSeconds() * this->dtMultiplier;
            this->update(dt);
            this->render();
        }
    }

    void update(float dt) {
        Event event;
        while (this->window.pollEvent(event)) {
            if (event.type == Event::Closed)
                this->window.close();

            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
                this->window.close();
        }

        if (this->player->HP > 0) {
            this->updateInput(dt);
            this->player->updateBullets(dt, this->window.getSize(), this->enemies, this->score);
            this->updateEnemies(dt);
        }

        this->updateText();
    }

    void updateInput(float dt) {
        if (Keyboard::isKeyPressed(Keyboard::W))
            this->player->shape.move(0.f, -10.f * dt);
        if (Keyboard::isKeyPressed(Keyboard::S))
            this->player->shape.move(0.f, 10.f * dt);
        if (Keyboard::isKeyPressed(Keyboard::D))
            this->player->shape.move(10.f * dt, 0.f);
        if (Keyboard::isKeyPressed(Keyboard::A))
            this->player->shape.move(-10.f * dt, 0.f);

        // Collision with window
        if (this->player->shape.getPosition().x <= this->player->shape.getGlobalBounds().width)
            this->player->shape.setPosition(this->player->shape.getGlobalBounds().width, this->player->shape.getPosition().y);

        if (this->player->shape.getPosition().x >= this->window.getSize().x)
            this->player->shape.setPosition(this->window.getSize().x, this->player->shape.getPosition().y);

        if (this->player->shape.getPosition().y <= 0)
            this->player->shape.setPosition(this->player->shape.getPosition().x, 0);

        if (this->player->shape.getPosition().y >= this->window.getSize().y - this->player->shape.getGlobalBounds().height)
            this->player->shape.setPosition(this->player->shape.getPosition().x, this->window.getSize().y - this->player->shape.getGlobalBounds().height);

        if (this->shootTimer < 7)
            this->shootTimer += 1.f * dt;
        else if (Mouse::isButtonPressed(Mouse::Left) && this->shootTimer >= 7) {
            this->shootTimer = 0;
            this->player->shoot(&this->bulletTex, dt);
        }
    }

    void updateEnemies(float dt) {
        if (this->enemyTimer >= 20) {
            this->enemyTimer = 0;
            this->enemies.push_back(Enemy2(&this->enemyTex, this->window.getSize()));
        } else {
            this->enemyTimer += 1.f * dt;
        }

        for (size_t i = 0; i < this->enemies.size(); i++) {
            this->enemies[i].update(dt);

            if (this->enemies[i].shape.getPosition().x <= 0) {
                this->enemies.erase(this->enemies.begin() + i);
                break;
            }

            if (this->enemies[i].shape.getGlobalBounds().intersects(this->player->shape.getGlobalBounds())) {
                this->enemies.erase(this->enemies.begin() + i);
                this->player->HP--;
                break;
            }
        }
    }

    void updateText() {
        this->hpT.setPosition(this->player->shape.getPosition().x - this->hpT.getGlobalBounds().width, this->player->shape.getPosition().y);
        this->hpT.setString(std::to_string(this->player->HP) + "/" + std::to_string(this->player->HPMAX));
        this->scoreT.setString("Score: " + std::to_string(this->score));
    }

    void render() {
        this->window.clear();

        this->window.draw(this->player->shape);

        for (auto& bullet : this->player->bullets) {
            this->window.draw(bullet.shape);
        }

        for (auto& enemy : this->enemies) {
            Text ehp;
            ehp.setFont(this->font);
            ehp.setCharacterSize(20);
            ehp.setFillColor(Color::White);
            ehp.setString(std::to_string(enemy.HP) + "/" + std::to_string(enemy.HPMAX));
            ehp.setPosition(enemy.shape.getPosition().x, enemy.shape.getPosition().y - ehp.getGlobalBounds().height);
            this->window.draw(ehp);
            this->window.draw(enemy.shape);
        }

        this->window.draw(this->scoreT);
        this->window.draw(this->hpT);

        if (this->player->HP <= 0) {
            this->window.draw(this->endT);
        }

        this->window.display();
    }
};




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
        // this->shape.scale(Vector2f(3.5, 3.5));
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
    const FloatRect getGlobalBounds() const {
        return this->shape.getGlobalBounds();
    }
    const bool intersects(const FloatRect bounds) const{
        return this->shape.getGlobalBounds().intersects(bounds);
    }
    const int& getType(){
        return this->type;
    }

    void update(){

    }

    void draw(RenderTarget& target){
        target.draw(this->shape);
    }
};

class Button{ //====================================================  BUTTON  ===========================================================
private:
    int buttonState;

    RectangleShape button;
    Font font;
    Text text;

    Color textIdle;
    Color texthover;
    Color textactive;
    Color idleColor;
    Color hoverColor;
    Color activeColor;

public:
    Button(float x, float y, float width, float height, Font &font, string text, unsigned character_Size,
           Color text_idle_color, Color text_hover_color, Color text_active_color, Color idleColor,
           Color hoverColor, Color activeColor)
        : idleColor(idleColor), hoverColor(hoverColor), activeColor(activeColor), font(font), textIdle(text_idle_color), texthover(text_hover_color), textactive(text_active_color)
    {
        button.setPosition(Vector2f(x, y));
        button.setSize(Vector2f(width, height));
        button.setFillColor(idleColor);

        this->text.setFont(font);
        this->text.setString(text);
        this->text.setFillColor(text_idle_color);
        this->text.setCharacterSize(character_Size);
        this->text.setPosition(
            button.getPosition().x + (button.getGlobalBounds().width / 2.f) - this->text.getGlobalBounds().width / 2.f,
            button.getPosition().y + (button.getGlobalBounds().height / 2.f) - this->text.getGlobalBounds().height / 2.f);

        buttonState = BTN_IDLE;
    }
    virtual ~Button()
    {
    }

    const bool isPressed() const
    {
        return buttonState == BTN_ACTIVE;
    }

    void update(const Vector2i& mousePosWindow)
    {
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

        if (this->buttonState == BTN_HOVER)
        {
            this->button.setFillColor(hoverColor);
            this->text.setFillColor(this->texthover);
        }
        else if (this->buttonState == BTN_ACTIVE)
        {
            this->button.setFillColor(activeColor);
            this->text.setFillColor(this->textactive);
        }
        else if (this->buttonState == BTN_IDLE)
        {
            this->button.setFillColor(idleColor);
            this->text.setFillColor(this->textIdle);
        }
        else
        {
            this->button.setFillColor(Color::Red);
            this->text.setFillColor(Color::Blue);
        }
    }

    void draw(RenderTarget &target)
    {
        target.draw(this->button);
        target.draw(this->text);
    }
};

class Entity{  // ===================================================  ENTITY  ============================================================
    protected:
    Sprite sprite;
    float movementSpeed;

    Vector2f velocity;

    // AnimationComponent *animationComponent;

    public:
    // constructor / destructor
    Entity(){
        this->movementSpeed = 100.f;
        // this->animationComponent = nullptr;
        // this->sprite.setScale(Vector2f(2, 2));
    }
    virtual ~Entity(){
        // delete this->animationComponent;
    }
    //  void setAnimationComponent(Texture &textureSheet)
    // {
    //     this->animationComponent = new AnimationComponent(this->sprite, textureSheet);
    // }
    void setTexture(Texture &texture)
    {
        this->sprite.setTexture(texture);
        // this->sprite.setScale(Vector2f(1.f, 1.f));
    }

    // new fucntions
    const Vector2f& getPosition(){
        return this->sprite.getPosition();
    }
    const FloatRect getGlobalBounds(){
        return this->sprite.getGlobalBounds();
    }
    virtual void setPosition(const float x, const float y){
            this->sprite.setPosition(x,y);
    }
    // void updateAnimation(const float &dt)
    // {   
    //     if(this->velocity.x > 0){
    //         this->animationComponent->play("WALK", dt,true);  
    //         this->sprite.setOrigin(0.f, 0.f);
    //     }else if(this->velocity.x < 0){

    //         sprite.setScale(-(sprite.getScale().x), sprite.getScale().y);
    //         this->sprite.setOrigin(this->sprite.getGlobalBounds().width / 3.f , 0.f);
    //         this->animationComponent->play("WALK",dt, true);
    //     }
    //     else if(this->velocity.x == 0){
    //         this->animationComponent->play("IDLE", dt,true);
    //     }
    
    // }
    Vector2f& getVelocity(){
        return this->velocity;
    }

    virtual void move(const float& dt, const float x, const float y){
        this->velocity = Vector2f(x * this->movementSpeed * dt,  y * this->movementSpeed * dt);
        this->sprite.move(this->velocity);

        // this->updateAnimation(dt);
    }

    virtual void update(const float& dt){
        
    };
    virtual void draw(RenderTarget* target){
            target->draw(this->sprite);
    };
};

class TileMap{  //===================================================  TILEMAP  ============================================================ 
    private:
    float gridSizeF;
    unsigned gridSizeU;
    Vector2u maxSizeWorldGrid;
    Vector2f maxSizeWorldF;
    unsigned layers;
    vector<vector<vector<Tile* > > > mp;
    Texture tileTextureSheet;
    string textureFile;
    
    RectangleShape collisionBox;

    //Culling
    int fromX;
    int toX;
    int fromY;
    int toY;
    int layer;

    public:
    TileMap(float gridSize, unsigned width, unsigned height, const string textureFile) {
        this->gridSizeF = GRID_SIZE;
        this->gridSizeU = static_cast<unsigned>(this->gridSizeF);
        this->maxSizeWorldGrid.x = width + 10; //// this was only width by default
        this->maxSizeWorldGrid.y = height + 10;  //// this was only height by default
        this->maxSizeWorldF.x = width * GRID_SIZE;
        this->maxSizeWorldF.y = height * GRID_SIZE;
        this->layers = 1;
        this->textureFile = textureFile;

        this->fromX = 0;
        this-> toX = 0;
        this->fromY = 0;
        this->toY = 0;
        this->layer = 0;

        this->mp.resize(this->maxSizeWorldGrid.x);
        for (int i = 0; i < this->maxSizeWorldGrid.x; i++) {
            this->mp[i].resize(this->maxSizeWorldGrid.y);
            for (int j = 0; j < this->maxSizeWorldGrid.y; j++) {
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
        return (x < this->maxSizeWorldGrid.x && x >= 0 &&  y < this->maxSizeWorldGrid.y && y >= 0 && z < this->layers && z >= 0);
    }
    const Texture* getTileSheet() const {
        return &this->tileTextureSheet;  
    }
    // void checkCollision(Entity* entity, const float& dt){
    // }
    void checkCollision(Entity* entity, const float& dt){
        if(entity->getPosition().y < 0.f){
            entity->setPosition(entity->getPosition().x, 0.f); // Collision on the bottom.

        }else if(entity->getPosition().y + entity->getGlobalBounds().height> this->maxSizeWorldF.y){   // Collision on the top.
            
            entity->setPosition(entity->getPosition().x, this->maxSizeWorldF.y - entity->getGlobalBounds().height);
        }
        //TILES

        // {
        //     this->layer = 0;
        //     this->fromX = entity->getGridPosition(this->gridSizeU).x - 2;
        //     if (this->fromX < 0)
        //     {
        //         this->fromX = 0;
        //     }
        //     else if(this-> fromX > this->maxSizeWorldGrid.x){
        //         this->fromX = maxSizeWorldGrid.x;
        //     }
            
        //     this->toX = entity->getGridPosition(this->gridSizeU).x + 1;
        //     if (this->toX < 0)
        //     {
        //         this->toX = 0;
        //     }
        //     else if(this-> toX > this->maxSizeWorldGrid.x){
        //         this->toX = maxSizeWorldGrid.x;
        //     }

        //     this->fromY = entity->getGridPosition(this->gridSizeU).y - 2;
        //     if (this->fromY < 0)
        //     {
        //         this->fromY = 0;
        //     }
        //     else if(this-> fromY > this->maxSizeWorldGrid.y){
        //         this->fromY = maxSizeWorldGrid.y;
        //     }

        //     this->toY = entity->getGridPosition(this->gridSizeU).y + 1;
        //     if (this->toY < 0)
        //     {
        //         this->toY = 0;
        //     }
        //     else if(this-> toY > this->maxSizeWorldGrid.y){
        //         this->toY = maxSizeWorldGrid.y;

        //     }
        // }
        
        // for(size_t x = this->fromX; x < this->toX; x++){
        //     for(size_t y = this->fromY; y < this->toY; y++){
        //         FloatRect playerBounds = entity->getGlobalBounds();
        //         FloatRect wallBounds = this->mp[x][y][this->layer]->getGlobalBounds();
        //         FloatRect nextPositionBounds = entity->getNextPositionBounds(dt);

        //         if(this->mp[x][y][this->layer]->getCollision() && this->mp[x][y][this->layer]->intersects(nextPositionBounds));{
        //             //Bottom collision
        //             if(playerBounds.top < wallBounds.top
        //                 && playerBounds.top + playerBounds.height < wallBounds.top + wallBounds.height
        //                 && playerBounds.left < wallBounds.left + wallBounds.width
        //                 && playerBounds.left + playerBounds.width > wallBounds.left){

        //                 entity->stopVelocityY();
        //                 entity ->setPosition(playerBounds.left, wallBounds.top - playerBounds.height);

        //             // Top COllision
        //             }else if(playerBounds.top > wallBounds.top
        //                 && playerBounds.top + playerBounds.height < wallBounds.top + wallBounds.height
        //                 && playerBounds.left < wallBounds.left + wallBounds.width
        //                 && playerBounds.left + playerBounds.width > wallBounds.left){

        //                 entity->stopVelocityY();
        //                 entity ->setPosition(playerBounds.left, wallBounds.top + playerBounds.height);
        //             }
        //         }
        //     }
        // }
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
            out_file << this->maxSizeWorldGrid.x << " " << this->maxSizeWorldGrid.y 
            << "\n" << this->gridSizeU 
            << "\n" << this->layers
            << "\n" << this->textureFile
            << "\n";
              
            for(int i=0;i < maxSizeWorldGrid.x; i++){
                for(int j=0;j < maxSizeWorldGrid.y ; j++){
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
            this->maxSizeWorldGrid.x = size.x;
            this->maxSizeWorldGrid.y = size.y;
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

            this->mp.resize(this->maxSizeWorldGrid.x, vector<vector<Tile*>>());
            for (size_t x = 0; x < maxSizeWorldGrid.x; x++)
            {
                for (size_t y = 0; y < maxSizeWorldGrid.y; y++)
                {
                    this->mp[x].resize(this->maxSizeWorldGrid.y, vector<Tile*>());
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

    void draw(RenderTarget& target, Entity* entity = nullptr){
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

class Bullet{
    RectangleShape shape;
    Vector2f position;
    Vector2f velocity;
    Texture texture;

    Vector2f normalize(Vector2f direction){
        float length = sqrt(direction.x * direction.x + direction.y * direction.y);
        return Vector2f(direction.x / length, direction.y / length);
    }
    
    public:
    Bullet(Texture& bulletTexture ,Vector2f startPos, Vector2f targetPosition) : position(startPos) {
        this->shape.setSize(Vector2f(10.f, 10.f));
        // this->shape.setFillColor(Color::Red);
        this->shape.setPosition(startPos);
        this->shape.setOrigin(5.0f, 5.0f);
        this->shape.setTexture(&bulletTexture);

        
        this->velocity = targetPosition * BULLET_SPEED;
    }
    RectangleShape getShape(){
        return this->shape;
    }
    void setScale(Vector2f scale){
        this->shape.setScale(scale);
    }
    void update(const float& dt){
        position += Vector2f(velocity * dt * BULLET_SPEED);
        shape.setPosition(position);
    }

    void draw(RenderTarget& target){
        target.draw(this->shape);
    }
};

class Player : public Entity{  //======================================  PLAYER  ============================================================
    private:
    bool attacking;
    float gravity;
    bool jump;
    vector<Bullet> bullets;
    Clock shootCoolDown;
    Texture bulletTexture;


    // Health bar
    float TotalHealth;
    float health;
    RectangleShape bar;
    RectangleShape outline;
    Vector2f position;
    float maxWidth;

    // ANIMATION
    int animationState;
    IntRect currentFrame;
    bool animSwitch;
    Clock animationTimer;
    Vector2f scale;
    float velocityMin;
    float velocityMax;
    float drag;
    float acceleration;

    void setVariable(){
        this->attacking = false;
        this->gravity = 3.f;
        this->jump = false;

        this->currentFrame = IntRect(0, 0, 40, 52);
        this->animationTimer.restart();
        this->animationState = IDLE;
        this->animSwitch = true;
        this->scale = Vector2f(2, 2);
        this->velocityMin = 2.f;
        this->velocityMax = 22.f;
        this->acceleration = 3.f;
        this->drag = 0.8;

        this->sprite.setScale(this->scale);
        this->sprite.setTextureRect(this->currentFrame);

        this->bulletTexture.loadFromFile("Textures\\bullet.png");
    }   
    void setComponents(){

    }
    void setHealthBar(){
        this->TotalHealth = 200;
        this->health = 200;
        this->bar.setSize(Vector2f(this->TotalHealth, 30));
        this->bar.setPosition(Vector2f(1366/2 - 50, 20));
        this->bar.setFillColor(Color::Red);

        outline.setSize(Vector2f(this->TotalHealth, 30));
        outline.setPosition(Vector2f(1366/2 - 50, 20));
        outline.setFillColor(Color::Transparent);
        outline.setOutlineThickness(1); // Adjust the thickness as needed
        outline.setOutlineColor(Color::Red);
    }
    public:
    // constructors/ destructors
    Player(float x, float y, Texture& texture){
        this->setVariable();
        this->setComponents();
        this->setPosition(x, y);
        this->setTexture(texture);
        // this->setAnimationComponent(texture);
        this->setHealthBar();

        // this->animationComponent->addAnimation("IDLE",10.f, 0, 0, 4, 0, 40, 52);// it si the size of my texture sheert
        // this->animationComponent->addAnimation("WALK",10.f, 0, 1, 9, 1, 40, 52);// it si the size of my texture sheert
        // this->animationComponent->addAnimation("RIGHT",7.f,0, 3, 7, 3, 40, 52);// it si the size of my texture sheert
    }
    virtual ~Player(){

    }

    // Accessors
    const Vector2f getPosition(){
        return this->sprite.getPosition();
    }
    bool canJump(){
        return this->jump;
    }
    void setcanJump(bool jump){
        this->jump = jump;
    }
    void Jump(const float& dt){
        // if (this->velocity.y >= 0){
        this->velocity.y = -60.f;
        this->jump = false;
    
        // this->sprite.move(velocity.x, velocity.y);
    }
    
    // Functions
    void move(const float dir_x, const float dir_y){
        // accelarion
        this->velocity.x += dir_x * this->acceleration;

        // limit velicty
        if(abs(this->velocity.x) > this->velocityMax){
            this->velocity.x = this->velocityMax * (this->velocity.x < 0) ? -1.f : 1.f; 
        }
    }
    vector<Bullet>& getbullets(){
        return bullets;
    }
    int score = 0;
    
    void updateHealthBar(){
        health -= 0.05;
        float ratio = this->health/this->TotalHealth;
        this->bar.setSize(Vector2f(ratio * this->TotalHealth, this->bar.getSize().y));
    }
    void updatePhysics(){
        // gravity
        this->velocity.y += 1.0f * this->gravity;
        
        // deceleration
        this->velocity *= this->drag;

        // limiting deceleration
        if(abs(this->velocity.x) < this->velocityMin){
            velocity.x = 0;
        }
        if(abs(this->velocity.y) < this->velocityMin){
            velocity.y = 0;
        }

        if(abs(this->velocity.x) <= 1.f)
            this->velocity.x = 0.f;

        this->sprite.move(this->velocity);
    }
    void updateAttack(){
        if (Mouse::isButtonPressed(Mouse::Left)){
            this->attacking = true;
        }else{
            this->attacking = false;
        }
    }
    RectangleShape gethealthBar(){
        return this->bar;
    }
    RectangleShape getBarOutline(){
        return this->outline;
    }
    
    bool getAnimSwitch() {
        bool temp = this->animSwitch;
        this->animSwitch = false;
        return temp;
    }
    void updateAnimation(){ // 123
        float speedPercent = (abs(this->velocity.x) / this->velocityMax);

        if(this->animationState == IDLE){

            if(this->animationTimer.getElapsedTime().asMilliseconds() >= 200.f || this->getAnimSwitch()){ // IDLE Animation
                this->currentFrame.top = 0.f;
                this->currentFrame.left += 40.f;

                if(this->currentFrame.left >= 160.f){
                    this->currentFrame.left = 0;
                }
                
                this->animationTimer.restart();
                this->sprite.setTextureRect(this->currentFrame);
            }

        }else if(this->animationState == RIGHT){
            if(this->animationTimer.getElapsedTime().asMilliseconds() >= 40.f / speedPercent  || this->getAnimSwitch()){ // MOVING RIGHT Animation
                this->currentFrame.top = 50.f;
                this->currentFrame.left += 40.f;

                if(this->currentFrame.left >= 360.f){
                    this->currentFrame.left = 0;
                }
                
                this->animationTimer.restart();
                this->sprite.setTextureRect(this->currentFrame);
            }
                this->sprite.setScale(this->scale);
                this->sprite.setOrigin(0.f, 0.f);

        }else if(this->animationState == LEFT){
            if(this->animationTimer.getElapsedTime().asMilliseconds() >= 40.f / speedPercent || this->getAnimSwitch()){ // MOVING LEFT Animation
                this->currentFrame.top = 50.f;
                this->currentFrame.left += 40.f;

                if(this->currentFrame.left >= 160.f){
                    this->currentFrame.left = 0;
                }
                
                this->animationTimer.restart();
                this->sprite.setTextureRect(this->currentFrame);
            }
                this->sprite.setScale(-(this->scale.x), this->scale.y);
                this->sprite.setOrigin(this->sprite.getGlobalBounds().width / 3.f , 0.f);
        }else{
            this->animationTimer.restart();
        }
    }
    void updateMovement(){  // 123

        if(this->velocity.x > 0.f){
             this->animationState = RIGHT;
        }else if(this->velocity.x < 0.f){
             this->animationState = LEFT;
        }else{
             this->animationState = IDLE;
        }
    }
    Vector2f normalize(const Vector2f& position){
        float length = sqrt(position.x * position.x + position.y * position.y);
        return position / length;
    }

   bool canAttack(const RenderWindow &window, const View& view){
        Vector2f position = this->sprite.getPosition(); // Use sprite's position directly
        Vector2f MousePosition = window.mapPixelToCoords(Mouse::getPosition(window), view); // Convert mouse position to world coordinates
        Vector2f aimDirection = MousePosition - position;

        aimDirection = normalize(aimDirection);

        if (shootCoolDown.getElapsedTime().asMilliseconds() >= 200){
            this->bullets.push_back(Bullet(bulletTexture, this->sprite.getPosition(), aimDirection));
            
            shootCoolDown.restart();
            return true;
        }
        return false;
    }
    FloatRect getGlobalBounds(){
        return this->sprite.getGlobalBounds();
    }
    void updateCollision(RenderTarget& target){
        if(this->sprite.getPosition().y + this->sprite.getGlobalBounds().height >= 768/2){
            this->jump = true;
            velocity.y = 0;

            this->sprite.setPosition(  this->sprite.getPosition().x,
                    768/2 - this->sprite.getGlobalBounds().height);
        }

        // if(this->position.y + 100 >= target.getSize().y){
        //     this->jump = true;
        // }
        CircleShape circle;
        circle.setFillColor(Color::Red);
        circle.setRadius(10.f);
        circle.setPosition(Vector2f(50, 768/2));

        target.draw(circle);
    }

    void update(const float& dt){
        this->updateAttack();
        this->updateHealthBar();
        this->updateMovement();
        this->updateAnimation();
        this->updatePhysics();
        
        for(auto& bullet : bullets){
            bullet.update(dt);
        }
    }
    
    void drawButtons(RenderTarget& target){
        for(int i = 0;i < bullets.size(); i++){
            
            if(bullets[i].getShape().getPosition().x < - 1366/2){
                bullets.erase(bullets.begin() + i, bullets.begin() + i + 1);
            }
            // this->bullets[0].setScale(Vector2f(5, 5));
            target.draw(bullets[i].getShape());
        }
    }
    void draw(RenderTarget& target){
        target.draw(this->sprite);
        this->updateCollision(target);
        this->drawButtons(target);
    }
};

class Enemy{
    private:
    // bool attacking;
    // bool dead;

    Texture* texture;
    Texture bulletTexture;
    RectangleShape shape;
    Player* player;
    Vector2f position;

    vector<Bullet> bullets;
    Clock shootCoolDown;

    void setbullets(){

    }
    void setVariable(){
        // this->attacking = false;
    }
    void setTexture(){
        this->texture = new Texture();
        int random = rand() % 3 + 1;
        if(random == 1){
            this->texture->loadFromFile("Textures\\PNGs\\Nairan - Battlecruiser - Base.png");
            this->bulletTexture.loadFromFile("Textures\\bullets\\battlecruiser.png");
            shape.setScale(Vector2f(2, 2));
        }else if(random == 2){
            this->texture->loadFromFile("Textures\\PNGs\\Nairan - Torpedo Ship - Base.png");
            this->bulletTexture.loadFromFile("Textures\\bullets\\torpedo.png");
            shape.setScale(Vector2f(3, 3));
        }else{
            this->texture->loadFromFile("Textures\\PNGs\\Nairan - Dreadnought - Base.png");
            this->bulletTexture.loadFromFile("Textures\\bullets\\7_2.png");
            shape.setScale(Vector2f(2, 2));
        }

    }
    
    Vector2f normalize(const Vector2f& position){
        float length = sqrt(position.x * position.x + position.y * position.y);
        return Vector2f(position.x / length, position.y / length);
    }

    float calculateAngle(){
        float dx = player->getPosition().x - this->position.x;
        float dy = player->getPosition().y - this->position.y;

        return atan2(dy, dx) * 180 / 3.14159265;
    }

    public:
    Enemy(Vector2f StartPosition, Player* player) : player(player), position(StartPosition) {
        this->setVariable();
        this->setTexture();

        // shape.setFillColor(Color::Red);
        shape.setSize(Vector2f(30.f, 30.f));
        shape.setOrigin(15.0f, 15.0f);
        shape.setPosition(this->position);
        shape.setTexture(this->texture);
    }
    virtual ~Enemy(){
    }
    FloatRect getGlobalBounds(){
        return this->shape.getGlobalBounds();
    }
    RectangleShape& getShape(){
        return this->shape;
    }

    void updatebullets(const float& dt){
        if(shootCoolDown.getElapsedTime().asMilliseconds() >= 400){
            this->bullets.push_back(Bullet(bulletTexture, this->position, normalize(this->player->getPosition() - this->position)));
            shootCoolDown.restart();
        }

        for(int i=0;i < bullets.size(); i++){
            bullets[i].update(dt);

            if(bullets[i].getShape().getGlobalBounds().intersects(player->getGlobalBounds())){
                bullets.erase(bullets.begin() + i, bullets.begin() + i + 1);
            }
        }   
    }
    void update(const float& dt){
        Vector2f direction = this->player->getPosition() - this->position;
        direction = normalize(direction);
        this->position += static_cast<Vector2f>(direction * ENEMY_SPEED * dt);
        shape.setPosition(this->position);

        this->shape.setRotation(calculateAngle() + 90.0);

        this->updatebullets(dt);
    }

    void draw(RenderTarget& target){

        for(int i = 0;i < bullets.size(); i++){  
            // bullets[i].getVelocity() = (this->shape.getPosition() - this->player->getPosition()) /  sqrt(position.x * position.x + position.y * position.y);
            target.draw(bullets[i].getShape());

            if(bullets[i].getShape().getPosition().x < -1366/2){
                bullets.erase(bullets.begin() + i);
            }
        }
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

class TextureSelector{   //==========================================  TEXTURE SELECTOR  =======================================================
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

        this->hideButton = new Button(x, y, 50, 50, font, text, 12, Color::Magenta, Color(0, 0, 0), Color::Magenta, Color(200, 200, 200), Color(150, 150, 150), Color(0, 0, 255));
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
        this->hideButton->draw(target);
        // this->quitButton->draw(&target);
    }
};

class EditorState : public State{ //=================================  EDITORSTATE  ============================================================
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
        // this->mainView.setSize(Vector2f(window->getSize()));
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
        
        this->quitButton = new Button(5, 200, 50, 50, font, "QUIT", 12, Color::Magenta, Color(0, 0, 0), Color::Magenta, Color(200, 200, 200), Color(150, 150, 150), Color(0, 0, 255));
        this->loadButton = new Button(5, 300, 50, 50, font, "LOAD", 12, Color::Magenta, Color(0, 0, 0), Color::Magenta, Color(200, 200, 200), Color(150, 150, 150), Color(0, 0, 255));
        this->saveButton = new Button(5, 400, 50, 50, font, "SAVE", 12, Color::Magenta, Color(0, 0, 0), Color::Magenta, Color(200, 200, 200), Color(150, 150, 150), Color(0, 0, 255));
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
            this->mainView.move(0, -floor(this->cameraSpeed * dt));
        }else if(Keyboard::isKeyPressed(Keyboard::Key(this->keyBinds.at("CAMERA_DOWN")))){
            this->mainView.move(0, floor(this->cameraSpeed * dt));
        }

        if(Keyboard::isKeyPressed(Keyboard::Key(this->keyBinds.at("CAMERA_LEFT")))){
            this->mainView.move(-floor(this->cameraSpeed * dt), 0);
        }else if(Keyboard::isKeyPressed(Keyboard::Key(this->keyBinds.at("CAMERA_RIGHT")))){
            this->mainView.move(floor(this->cameraSpeed * dt), 0);
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
            button.second->draw(target);
        }
        
        quitButton->draw(target);
        saveButton->draw(target);
        loadButton->draw(target);
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

class GameState : public State{  //==================================  GAMESTATE  ============================================================
    private:
    // Entity player;
    View mainView;
    RenderTexture renderTexture;
    Sprite renderSprite;

    Player* player;
    RectangleShape background;
    Texture bgtexture;

    TileMap* mp;
    map<string, Button*> buttons;
    Font font;

    int maxEnemies;  //123
    vector<Enemy> enemies; //123
    Vector2f enemyPosition;

    void setEnemeis(){ //123
        this->maxEnemies = 10;
        while(enemies.size() < maxEnemies){
            this->enemyPosition = Vector2f(rand() % window->getSize().x + 1.f, 0);
            enemies.push_back(Enemy(enemyPosition, player));  // putting randon position
        }
    }
    void setFonts(){
        if(!this->font.loadFromFile("Fonts\\PublicPixel-E447g.ttf")){
            
        }
    }
    void setButtons(){
        this->buttons["EXIT"] = new Button(10.f, 10.f, 50.f, 50.f, this->font, "EXIT", 12,
                Color::Magenta, Color(0, 0, 0), Color::Magenta,
                Color(200, 200, 200), Color(150, 150, 150), Color(0, 0, 255));
    }
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
            this->keyBinds["JUMP"] = this->Keys->at("W");

    }
    void setPlayer(){
        // this->textures["PLAYER_IDLE"].loadFromFile("Textures\\TextureSheet.png");
        this->textures["PLAYER_IDLE"].loadFromFile("Textures\\player_sheet.png");
        this->player = new Player(0, window->getSize().y, this->textures["PLAYER_IDLE"]);
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
        this->setFonts(); // 
        this->setButtons(); //
        this->setTileMap();
        this->setPlayer();
        this->setEnemeis();  // 123
    }

    virtual ~GameState(){
        delete this->mp;
        delete this->player;
    }

    // functions
    void updateView(const float& dt){
        this->mainView.setCenter(floor(this->player->getPosition().x), floor(this->player->getPosition().y)); // can delete floor later if nothing changes
    }
    void updateTileMap(const float& dt){
        this->mp->update();
        this->mp->checkCollision(this->player, dt);
    }
    void updateEnemies(const float& dt){ // 123
        while(enemies.size() < maxEnemies){
            this->enemyPosition = Vector2f(rand() % window->getSize().x + 1.f, 0);
            enemies.push_back(Enemy(enemyPosition, player));  // putting randon position
        }

        for(auto& enemy : enemies){
            enemy.update(dt);
        }
    }
    void updateInput(const float &dt){
        if(Keyboard::isKeyPressed(Keyboard::Key(this->keyBinds.at("MOVE_LEFT")))){
            this->player->move(-1, 0);
        }  
        if(Keyboard::isKeyPressed(Keyboard::Key(this->keyBinds.at("MOVE_RIGHT")))){
            this->player->move(1, 0); 
        }   
        if(Keyboard::isKeyPressed(Keyboard::Key(this->keyBinds.at("JUMP"))) && this->player->canJump()){
            this->player->Jump(dt);
        }

        // Mouse::getPosition(Vector2f(window->getSize()))
        if(Mouse::isButtonPressed(Mouse::Left) && this->player->canAttack(*window, mainView)){
            // this->player->Attack(dt, mousePosView, enemies);
            for(int i=0;i < this->player->getbullets().size(); i++){
                this->player->getbullets()[i].update(dt);

                for(int j=0; j < this->enemies.size(); j++){
                    if(this->player->getbullets()[i].getShape().getGlobalBounds().intersects(enemies[j].getGlobalBounds())){
                        this->player->getbullets().erase(this->player->getbullets().begin() + i, this->player->getbullets().begin() + i + 1);
                        this->enemies.erase(this->enemies.begin() + j);
                        this->player->score++;
                    }
                }
            }
        }
        

        if(Keyboard::isKeyPressed(Keyboard::Key(this->keyBinds.at("CLOSE")))){
            this->quit = true;
        } 

        if(buttons["EXIT"]->isPressed()){
            this->quit = true;
        }
    }
    void updateButtons(const float& dt){
        for(auto& button : buttons){
            button.second->update(this->mousePosWindow);
        }
    }
    void update(const float& dt){
        this->updateMousePosition(&this->mainView);
        // this->updateKeyTime(dt);
        this->updateView(dt);
        this->updateInput(dt);
        this->player->update(dt);
        this->updateEnemies(dt);
        this->updateButtons(dt);
        this->updateTileMap(dt);
    }
    
    void draw(RenderTarget* target = NULL){
        if(!target)
            target = this->window;


        // this->renderTexture.draw(background);

        this->renderTexture.clear();

        // target = &this->renderTexture;

        renderTexture.setView(this->renderTexture.getDefaultView());   // i dunno whether to put or not because he putted in the if paused
        renderTexture.draw(background);

        this->renderTexture.setView(this->mainView);
        // target->draw(background);
        this->mp->draw(this->renderTexture);

        // this->mp->draw(*target);
        this->player->draw(this->renderTexture);
        for(auto& enemy : enemies){
            renderTexture.draw(enemy.getShape());
            enemy.draw(renderTexture);
        }


        // FINAL RENDER
        this->renderTexture.display();
        this->renderSprite.setTexture(this->renderTexture.getTexture());
        target->draw(this->renderSprite);
        for(auto &button : buttons){
            button.second->draw(*target);
        }
        target->draw(player->gethealthBar());
        target->draw(player->getBarOutline());

    }
};

class MainMenuState : public State { //==============================  MAINMENUSTATE  ============================================================
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
      this->buttons["GAME_STATE"] = new Button(100.f, 200.f, 150.f, 50.f, this->font, "NEW GAME", 12, Color::Magenta, Color(0, 0, 0), Color::Magenta, Color(200, 200, 200), Color(150, 150, 150), Color(0, 0, 255));

        this->buttons["SPACE_SHOOTER"] = new Button(100.f, 300.f, 150.f, 50.f, this->font, "SPACE SHOOTER", 12,
                Color::Magenta, Color(0, 0, 0), Color::Magenta,
                Color(200, 200, 200), Color(150, 150, 150), Color(0, 0, 255));
                
        this->buttons["EDITOR_STATE"] = new Button(100.f, 400.f, 150.f, 50.f, this->font, "EDITOR", 12,
                Color::Magenta, Color(0, 0, 0), Color::Magenta,
                Color(200, 200, 200), Color(150, 150, 150), Color(0, 0, 255));
                
        this->buttons["EXIT"] = new Button(100.f, 500.f, 150.f, 50.f, this->font, "EXIT", 12, 
                Color::Magenta, Color(0, 0, 0), Color::Magenta, 
                Color(200, 200, 200), Color(150, 150, 150), Color(0, 0, 255));


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
            // music.stop();
            this->states->push(new GameState(this->window, this->Keys, this->states));
            // music.play();
        }

        if (this->buttons["SPACE_SHOOTER"]->isPressed()){
            Game2 game;
            game.run();
        }
        

        if(this->buttons["EDITOR_STATE"]->isPressed()){
            music.stop();        
            this->states->push(new EditorState(this->window, this->Keys, this->states));
            // music.play();
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
        // this->checkForQuit();
        this->updateButtons(dt);
    }

    void drawButtons(RenderTarget& target){
        for(auto &button : buttons){
            button.second->draw(target);
        }
    }
    void draw(RenderTarget* target){
        if(!target)
            target = this->window;

        target->draw(this->background);
        this->drawButtons(*target);

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
        this->window = new RenderWindow(VideoMode(1366, 768), "Gears of War", Style::Default);
        // this->window = new RenderWindow(VideoMode::getDesktopMode(), "Gears of War", Style::Default);
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
        if(!this->states.empty() && this->window->hasFocus()){
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
    srand(time(NULL));
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
