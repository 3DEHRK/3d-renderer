#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>

class Line3D{
    public:
    Line3D(float xs, float ys, float zs, float xe, float ye, float ze) : xs(xs), ys(ys), zs(zs), xe(xe), ye(ye), ze(ze) {}
    float xs, ys, zs, xe, ye, ze;
};

class Line{
    //do the maths
    float focalLength = 1000.0f;

    public:
    Line(float xs, float ys, float xe, float ye) : xs(xs), ys(ys), xe(xe), ye(ye) {}
    Line(Line3D l3d) {
        xs = (l3d.xs * focalLength / (focalLength - l3d.zs));
        //xs = (l3d.xs / l3d.zs);

        ys = (l3d.ys * focalLength / (focalLength - l3d.zs));
        //ys = (l3d.ys / l3d.zs);

        xe = (l3d.xe * focalLength / (focalLength - l3d.ze));
        //xe = (l3d.xe / l3d.ze);

        ye = (l3d.ye * focalLength / (focalLength - l3d.ze));
        //ye = (l3d.ye / l3d.ze);
    }
    float xs, ys, xe, ye;
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "3d");

    float xyOffs = 0.f;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
        float xOffset = static_cast<float>(pixelPos.x);
        float yOffset = static_cast<float>(pixelPos.y);

        if(yOffset < 101.f && xOffset >= yOffset)
            xOffset = std::max(xOffset, 101.f);
        
        if(xOffset < 101.f && xOffset <= yOffset)
            yOffset = std::max(yOffset, 101.f);

        window.clear(sf::Color::Black);

        std::vector<Line> linesVec;

        linesVec.push_back(Line(Line3D(0 + xOffset, 0 + yOffset, 0, 0 + 100.f + xOffset, 0 + yOffset, 0)));
        linesVec.push_back(Line(Line3D(0 + 100.f + xOffset, 0 + yOffset, 0, 0 + 100.f + xOffset, 0 + 100.f + yOffset, 0)));
        linesVec.push_back(Line(Line3D(0 + 100.f + xOffset, 0 + 100.f + yOffset, 0, 0 + xOffset, 0 + 100.f + yOffset, 0)));
        linesVec.push_back(Line(Line3D(0 + xOffset, 0 + 100.f + yOffset, 0, 0 + xOffset, 0 + yOffset, 0)));

        linesVec.push_back(Line(Line3D(0 + xOffset, 0 + yOffset, 0 + 100.f, 0 + 100.f + xOffset, 0 + yOffset, 0 + 100.f)));
        linesVec.push_back(Line(Line3D(0 + 100.f + xOffset, 0 + yOffset, 0 + 100.f, 0 + 100.f + xOffset, 0 + 100.f + yOffset, 0 + 100.f)));
        linesVec.push_back(Line(Line3D(0 + 100.f + xOffset, 0 + 100.f + yOffset, 0 + 100.f, 0 + xOffset, 0 + 100.f + yOffset, 0 + 100.f)));
        linesVec.push_back(Line(Line3D(0 + xOffset, 0 + 100.f + yOffset, 0 + 100.f, 0 + xOffset, 0 + yOffset, 0 + 100.f)));

        linesVec.push_back(Line(Line3D(0 + xOffset, 0 + yOffset, 0, 0 + xOffset, 0 + yOffset, 0 + 100.f)));
        linesVec.push_back(Line(Line3D(0 + 100.f + xOffset, 0 + yOffset, 0, 0 + 100.f + xOffset, 0 + yOffset, 0 + 100.f)));
        linesVec.push_back(Line(Line3D(0 + 100.f + xOffset, 0 + 100.f + yOffset, 0, 0 + 100.f + xOffset, 0 + 100.f + yOffset, 0 + 100.f)));
        linesVec.push_back(Line(Line3D(0 + xOffset, 0 + 100.f + yOffset, 0, 0 + xOffset, 0 + 100.f + yOffset, 0 + 100.f)));


        linesVec.push_back(Line(Line3D(0 + 1, 0 + 1, 0, 0 + 100.f + 1, 0 + 1, 0)));
        linesVec.push_back(Line(Line3D(0 + 100.f + 1, 0 + 1, 0, 0 + 100.f + 1, 0 + 100.f + 1, 0)));
        linesVec.push_back(Line(Line3D(0 + 100.f + 1, 0 + 100.f + 1, 0, 0 + 1, 0 + 100.f + 1, 0)));
        linesVec.push_back(Line(Line3D(0 + 1, 0 + 100.f + 1, 0, 0 + 1, 0 + 1, 0)));

        linesVec.push_back(Line(Line3D(0 + 1, 0 + 1, 0 + 100.f, 0 + 100.f + 1, 0 + 1, 0 + 100.f)));
        linesVec.push_back(Line(Line3D(0 + 100.f + 1, 0 + 1, 0 + 100.f, 0 + 100.f + 1, 0 + 100.f + 1, 0 + 100.f)));
        linesVec.push_back(Line(Line3D(0 + 100.f + 1, 0 + 100.f + 1, 0 + 100.f, 0 + 1, 0 + 100.f + 1, 0 + 100.f)));
        linesVec.push_back(Line(Line3D(0 + 1, 0 + 100.f + 1, 0 + 100.f, 0 + 1, 0 + 1, 0 + 100.f)));

        linesVec.push_back(Line(Line3D(0 + 1, 0 + 1, 0, 0 + 1, 0 + 1, 0 + 100.f)));
        linesVec.push_back(Line(Line3D(0 + 100.f + 1, 0 + 1, 0, 0 + 100.f + 1, 0 + 1, 0 + 100.f)));
        linesVec.push_back(Line(Line3D(0 + 100.f + 1, 0 + 100.f + 1, 0, 0 + 100.f + 1, 0 + 100.f + 1, 0 + 100.f)));
        linesVec.push_back(Line(Line3D(0 + 1, 0 + 100.f + 1, 0, 0 + 1, 0 + 100.f + 1, 0 + 100.f)));

        for (auto line : linesVec){
        sf::Vertex v_line[] =
        {
            sf::Vertex(sf::Vector2f(line.xs + xyOffs, line.ys + xyOffs)),
            sf::Vertex(sf::Vector2f(line.xe + xyOffs, line.ye + xyOffs))
        };
        window.draw(v_line, 2, sf::Lines);
        }

        window.display();
    }
    return 0;
}