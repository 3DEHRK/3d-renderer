#include <SFML/Graphics.hpp>

struct vector3d{
    float x, y, z;
};

struct triangle{
    vector3d vertices[3];
};

struct mesh{
    std::vector<triangle> triangles;

    // Magic ChatGPT product, research in bjarnes book
    // Overloading the assignment operator for mesh
    mesh& operator=(const std::initializer_list<triangle>& init_list) {
        triangles = init_list; // Assign the initializer list to the triangles vector
        return *this;
    }
};

mesh cube = {
    //south
    {0.f,0.f,0.f, 1.f,0.f,0.f, 1.f,1.f,0.f},
    
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "3d renderer");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        //window.clear();
        //window.draw();

        window.display();
    }
    return 0;
}
