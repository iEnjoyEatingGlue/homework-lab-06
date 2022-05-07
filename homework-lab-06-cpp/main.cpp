#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

class CustomGuy: public sf::Sprite
{
public:
    CustomGuy(sf::Texture *texture)
    {
        setTexture(*texture);
    }
    void setBounds(const float &top_,const float &bot_,const float &left_,const float &right_)
    {
        bound_top = top_;
        bound_bottom = bot_;
        bound_left = left_;
        bound_right = right_;
    }
    void moveInDirection(const sf::Time &elapsed,std::vector<bool> v)
    {
        float time = elapsed.asSeconds();
        sf::FloatRect guy = getGlobalBounds();
        auto guy_top = guy.top;
        auto guy_bottom = guy.top + guy.height;
        auto guy_left = guy.left;
        auto guy_right = guy.left+ guy.width;

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            if(guy_top - 1 >= bound_top)
            {
                if(v[0] == true)
                {
                    move(0,m_speed_y*time * -1.0);
                }
            }
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        {
            if(guy_bottom + 1 <= bound_bottom)
            {
                if(v[1] == true)
                {
                     move(0,m_speed_y*time);
                }
            }
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
            if(guy_left - 1 >= bound_left)
            {
                if(v[2] == true)
                {
                     move(m_speed_x*time * -1.0,0);
                }
            }
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            if(guy_right + 1 <= bound_right)
            {
                if(v[3] == true)
                {
                     move(m_speed_x*time,0);
                }
            }
        }
    }
    void setSpeed(const int &x,const int &y)
    {
        m_speed_x = x;
        m_speed_y = y;
    }

private:
    int m_speed_x = 0;
    int m_speed_y = 0;

    int bound_top = 0;
    int bound_bottom = 0;
    int bound_left = 0;
    int bound_right = 0;
};

class CustomGrass: public sf::Sprite
{
public:
    CustomGrass(sf::Texture* texture)
    {
        sf::RenderWindow window(sf::VideoMode(800, 600), "My window");
        setTexture(*texture);
        texture->setRepeated(true);
        setTextureRect(sf::IntRect(0, 0, window.getSize().x, window.getSize().y));
    }
};

class CustomWall: public sf::Sprite
{
public:
    CustomWall(sf::Texture *texture,int left, int top, int width, int height)
        : Left(left), Top(top), Width(width), Height(height)
    {
        sf::RenderWindow window(sf::VideoMode(800, 600), "My window");
        setTexture(*texture);
        texture->setRepeated(true);
        setTextureRect(sf::IntRect(0,0,Width,Height));
        setPosition(Left,Top);
    }
private:
    int Left;
    int Top;
    int Width;
    int Height;
};

std::vector<bool> Colisions(const CustomGuy &guy,std::vector<CustomWall> &walls)
{
    std::vector<bool> v(4, 1);

    sf::FloatRect guy_ = guy.getGlobalBounds();
    auto guy_top = guy_.top;
    auto guy_bottom = guy_.top + guy_.height;
    auto guy_left = guy_.left;
    auto guy_right = guy_.left+ guy_.width;

    for(auto &wall_: walls)
    {
        sf::FloatRect wall = wall_.getGlobalBounds();
        auto wall_bottom = wall.top + wall.height;
        auto wall_left = wall.left - guy_.width;
        auto wall_right = wall.left + wall.width + guy_.width;

        if(guy_top - 1 <= wall_bottom && guy_bottom + 1 >= wall_bottom && guy_left >= wall_left && guy_right <= wall_right)
        {
            v[0] = 0;
            break;
        }
    }
    for(auto &wall_: walls)
    {
        sf::FloatRect wall = wall_.getGlobalBounds();
        auto wall_top = wall.top;
        auto wall_left = wall.left - guy_.width;
        auto wall_right = wall.left + wall.width;

        if(guy_top - 1 <= wall_top && guy_bottom + 1 >= wall_top && guy_left >= wall_left && guy_right <= wall_right)
        {
            v[1] = 0;
            break;
        }
    }
    for(auto &wall_: walls)
    {
        sf::FloatRect wall = wall_.getGlobalBounds();
        auto wall_top = wall.top;
        auto wall_bottom = wall.top + wall.height;
        auto wall_right = wall.left + wall.width;

        if(guy_top <= wall_bottom && guy_bottom >= wall_top && guy_left - 1 <= wall_right&& guy_right + 1 >= wall_right)
        {
            v[2] = 0;
            break;
        }
    }
    for(auto &wall_: walls)
    {
        sf::FloatRect wall = wall_.getGlobalBounds();
        auto wall_top = wall.top;
        auto wall_bottom = wall.top + wall.height;
        auto wall_left = wall.left;

        if(guy_top <= wall_bottom && guy_bottom >= wall_top && guy_left - 1 <= wall_left && guy_right + 1 >= wall_left)
        {
            v[3] = 0;
            break;
        }
    }
    return(v);
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "My window");

    sf::Clock clock;
    float time_passed = 0.0;

    sf::Texture texture_guy;
    sf::Texture texture_grass;
    sf::Texture texture_wall;

    if(!texture_guy.loadFromFile("guy.png")) { return 1; }
    if(!texture_grass.loadFromFile("grass.png")) { return 1; }
    if(!texture_wall.loadFromFile("wall.png")) { return 1; }

    CustomGuy guy(&texture_guy);
    CustomGrass grass(&texture_grass);
    std::vector<CustomWall> walls;

    CustomWall wall_1(&texture_wall, 100, 100, 250, 50);
    CustomWall wall_2(&texture_wall, 250, 150, 100, 400);
    CustomWall wall_3(&texture_wall, 250, 550, 250, 50);
    CustomWall wall_4(&texture_wall, 450, 100, 50, 450);

    walls.emplace_back(wall_1);
    walls.emplace_back(wall_2);
    walls.emplace_back(wall_3);
    walls.emplace_back(wall_4);

    guy.setSpeed(200,200);
    guy.setBounds(0, window.getSize().y, 0, window.getSize().x);

    while (window.isOpen())
    {
        sf::Time elapsed = clock.restart();
        time_passed = time_passed + elapsed.asSeconds();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        guy.moveInDirection(elapsed,Colisions(guy,walls));

        window.draw(grass);
        window.draw(guy);
        for(auto &wall: walls)
            window.draw(wall);

        window.display();
    }
    return 0;
}
