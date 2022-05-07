#include <iostream>
#include <memory>
#include <vector>
#include <cmath>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>


class AnimatedSprite : public sf::Sprite {
public:
    AnimatedSprite(const int fps, const std::string& path): fps_(fps)
    {
        if (!texture_.loadFromFile(path)) {
            std::cerr << "Could not load texture" << std::endl;
        }
        setTexture(texture_);
        setTextureRect(sf::IntRect(50, 0, 50, 37));
        setPosition(0,550);
    }

    void fall(const sf::Time &elapsed)
    {
        float dt = elapsed.asSeconds();
        t_ = t_ + dt;
        float d = (9.81/2.0)*pow(t_,1.5);
        move(0,d);
    }

    void setSpeed(const int& x_speed, const int& y_speed)
    {
        m_speed_x = x_speed;
        m_speed_y = y_speed;
    }

    void animate(const sf::Time &elapsed)
    {
        float dt = elapsed.asSeconds();
        t_ = t_ + dt;

        if(t_ > 1.0/fps_)
        {
            fragments_index++;
            t_ = 0.0;
            if(fragments_index == running_frames.size())
            {
                fragments_index = 0;
            }
        }
        setTextureRect(running_frames[fragments_index]);
        move(m_speed_x*dt,m_speed_y*dt);
    }

    void setBounds(const float& l_bound, const float& r_bound,const float& u_bound,const float& d_bound){
        l_bound_  = l_bound  ;
        r_bound_  = r_bound  ;
        u_bound_  = u_bound  ;
        d_bound_  = d_bound  ;
    }

    void add_animation_frame(const sf::IntRect& frame){
        //add animation frames to vecto
        running_frames.emplace_back(frame);
    }

    void moveInDirection(const sf::Time &elapsed,std::vector<bool> v)
    {
        float time = elapsed.asSeconds();
        sf::FloatRect hero = getGlobalBounds();
        auto hero_top = hero.top;
        auto hero_bottom = hero.top + hero.height;
        auto hero_left = hero.left;
        auto hero_right = hero.left+ hero.width;

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            if(hero_top - 1 >= u_bound_)
            {
                if(v[0] == true)
                {
                    move(0,m_speed_y*time * -1.0);
                }
            }
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        {
            if(hero_bottom + 1 <= d_bound_)
            {
                if(v[1] == true)
                {
                     move(0,m_speed_y*time);
                }
            }
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
            if(hero_left - 1 >= l_bound_)
            {
                if(v[2] == true)
                {
                     move(m_speed_x*time * -1.0,0);
                }
            }
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            if(hero_right + 1 <= r_bound_)
            {
                if(v[3] == true)
                {
                     move(m_speed_x*time,0);
                }
            }
        }
    }
private:
    sf::Texture texture_;
    int fps_;
    int m_speed_x = 0 ;
    int m_speed_y = 0 ;

    float l_bound_ = 0;
    float r_bound_ = 0;
    float u_bound_ = 0;
    float d_bound_ = 0;
    float t_ = 0.0;
    unsigned int fragments_index = 0;
    std::vector<sf::IntRect> running_frames;
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

std::vector<bool> Colisions(const AnimatedSprite &hero,std::vector<CustomWall> &walls)
{
    std::vector<bool> v(4, 1);

    sf::FloatRect hero_ = hero.getGlobalBounds();
    auto hero_top = hero_.top;
    auto hero_bottom = hero_.top + hero_.height;
    auto hero_left = hero_.left;
    auto hero_right = hero_.left+ hero_.width;

    for(auto &wall_: walls)
    {
        sf::FloatRect wall = wall_.getGlobalBounds();
        auto wall_bottom = wall.top + wall.height;
        auto wall_left = wall.left - hero_.width;
        auto wall_right = wall.left + wall.width + hero_.width;

        if(hero_top - 1 <= wall_bottom && hero_bottom + 1 >= wall_bottom && hero_left >= wall_left && hero_right <= wall_right)
        {
            v[0] = 0;
            break;
        }
    }
    for(auto &wall_: walls)
    {
        sf::FloatRect wall = wall_.getGlobalBounds();
        auto wall_top = wall.top;
        auto wall_left = wall.left - hero_.width;
        auto wall_right = wall.left + wall.width + hero_.width;

        if(hero_top - 1 <= wall_top && hero_bottom + 1 >= wall_top && hero_left >= wall_left && hero_right <= wall_right)
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

        if(hero_top <= wall_bottom && hero_bottom >= wall_top && hero_left - 1 <= wall_right && hero_right + 1 >= wall_right)
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

        if(hero_top <= wall_bottom && hero_bottom >= wall_top && hero_left - 1 <= wall_left && hero_right + 1 >= wall_left)
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

    sf::Texture texture_wall;
    if(!texture_wall.loadFromFile("wall.png")) { return 1; }

    std::vector<CustomWall> walls;

    CustomWall wall_1(&texture_wall, 100, 500, 250, 50);
    CustomWall wall_2(&texture_wall, 300, 350, 250, 50);
    CustomWall wall_3(&texture_wall, 50, 200, 200, 50);
    CustomWall wall_4(&texture_wall, 450, 120, 250, 50);

    walls.emplace_back(wall_1);
    walls.emplace_back(wall_2);
    walls.emplace_back(wall_3);
    walls.emplace_back(wall_4);

    AnimatedSprite hero(10, "Character\\character.png");
    hero.setBounds(0, window.getSize().x, 0, window.getSize().y);
    hero.setSpeed(200,200);

    //hero.add_animation_frame(sf::IntRect(0, 0, 50, 37)); // hero standing frame 1
    //hero.add_animation_frame(sf::IntRect(50, 0, 50, 37)); // hero standing frame 2
    //hero.add_animation_frame(sf::IntRect(100, 0, 50, 37)); // hero standing frame 3
    hero.add_animation_frame(sf::IntRect(150, 0, 50, 37)); // hero running frame 1
    hero.add_animation_frame(sf::IntRect(200, 0, 50, 37)); // hero running frame 1
    hero.add_animation_frame(sf::IntRect(250, 0, 50, 37)); // hero running frame 1
    hero.add_animation_frame(sf::IntRect(300, 0, 50, 37)); // hero running frame 1
    hero.add_animation_frame(sf::IntRect(350, 0, 50, 37)); // hero running frame 1
    hero.add_animation_frame(sf::IntRect(400, 0, 50, 37)); // hero running frame 1

    sf::Clock clock;

    // run the program as long as the window is open
    while (window.isOpen()) {
    hero.setTextureRect(sf::IntRect(50, 0, 50, 37));
    hero.setScale(1.5,1.5);

        sf::Time elapsed = clock.restart();

        sf::Event event;
        while (window.pollEvent(event)) {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Black);

        sf::Texture texture;
        if (!texture.loadFromFile("grass.png")) {
            std::cerr << "Could not load texture" << std::endl;
            return 1;
        }

        texture.setRepeated(true);
        sf::Sprite sprite;
        sprite.setTexture(texture);
        sprite.setTextureRect(sf::IntRect(0, 0, 800, 600));

        hero.moveInDirection(elapsed,Colisions(hero,walls));

        // draw everything here...
        window.draw(sprite);
        window.draw(hero);
        for(auto &wall: walls)
            window.draw(wall);



        // end the current frame
        window.display();
    }

    return 0;
}
