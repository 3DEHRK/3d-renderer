#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
class Line3D{
    public:
    Line3D(float xs, float ys, float zs, float xe, float ye, float ze) : xs(xs), ys(ys), zs(zs), xe(xe), ye(ye), ze(ze) {}

    float xs;
    float ys;
    float zs;
    float xe;
    float ye;
    float ze;
};

float focal = 300.0f;

class Line{
    public:
    Line(float xs, float ys, float xe, float ye) : xs(xs), ys(ys), xe(xe), ye(ye) {}
    Line(Line3D l3d) {
        xs = (l3d.xs * focal / (focal - l3d.zs));
        ys = (l3d.ys * focal / (focal - l3d.zs));
        xe = (l3d.xe * focal / (focal - l3d.ze));
        ye = (l3d.ye * focal / (focal - l3d.ze));
    }
    float xs;
    float ys;
    float xe;
    float ye;
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "3d");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.clear(sf::Color::Black);

        std::vector<Line> linesVec;
        linesVec.push_back(Line(Line3D(0.f, 0.f, 0.f, 50.f, 0.f, 0.f)));
        linesVec.push_back(Line(Line3D(50.f, 0.f, 0.f, 50.f, 50.f, 0.f)));
        linesVec.push_back(Line(Line3D(50.f, 50.f, 0.f, 0.f, 50.f, 0.f)));
        linesVec.push_back(Line(Line3D(0.f, 50.f, 0.f, 0.f, 0.f, 0.f)));

        linesVec.push_back(Line(Line3D(0.f, 0.f, 50.f, 50.f, 0.f, 50.f)));
        linesVec.push_back(Line(Line3D(50.f, 0.f, 50.f, 50.f, 50.f, 50.f)));
        linesVec.push_back(Line(Line3D(50.f, 50.f, 50.f, 0.f, 50.f, 50.f)));
        linesVec.push_back(Line(Line3D(0.f, 50.f, 50.f, 0.f, 0.f, 50.f)));

        linesVec.push_back(Line(Line3D(0.f, 0.f, 0.f, 0.f, 0.f, 50.f)));
        linesVec.push_back(Line(Line3D(50.f, 0.f, 0.f, 50.f, 0.f, 50.f)));
        linesVec.push_back(Line(Line3D(50.f, 50.f, 0.f, 50.f, 50.f, 50.f)));
        linesVec.push_back(Line(Line3D(0.f, 50.f, 0.f, 0.f, 50.f, 50.f)));

        for (auto line : linesVec){
        sf::Vertex v_line[] =
        {
            sf::Vertex(sf::Vector2f(line.xs, line.ys)),
            sf::Vertex(sf::Vector2f(line.xe, line.ye))
        };
        window.draw(v_line, 2, sf::Lines);
        }

        window.display();
    }
    return 0;
}
