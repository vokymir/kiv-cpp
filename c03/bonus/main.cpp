#include <ios>
#include <iostream>
#include <ostream>

class Vector2D {
private:
    double m_x;
    double m_y;

public:
    Vector2D(const double x, const double y) : m_x(x), m_y(y){}
    double X() const {return m_x;}
    double Y() const {return m_y;}
    Vector2D operator+(const Vector2D& rhs) const{
        return Vector2D(m_x + rhs.m_x, m_y + rhs.m_y);
    }
    Vector2D operator-(const Vector2D& rhs) const{
        return Vector2D(m_x - rhs.m_x, m_y - rhs.m_y);
    }
    Vector2D& operator+=(const Vector2D& rhs){
        m_x += rhs.m_x;
        m_y += rhs.m_y;
        return *this;
    }
    Vector2D operator*(const double rhs) const{
        return Vector2D(m_x * rhs, m_y * rhs);
    }
};

// venku, protoze chci mit stream vlevo
std::ostream& operator<<(std::ostream& lhs, const Vector2D& rhs){
    auto old_precision = lhs.precision(1); // pretty vypis
    lhs << std::fixed << "(" << rhs.X() << ", " << rhs.Y() << ")";
    lhs.precision(old_precision);
    return lhs;
}

int main(int argc, char** argv){
    Vector2D origin(0,0);
    std::cout << "Origin: " << origin << std::endl;

    // right triangle vertexes
    std::cout << "=== POINTS ===" << std::endl;

    Vector2D point_a(0,3);
    Vector2D point_b(4,3);
    auto point_c = origin;
    std::cout << "Point A: " << point_a << std::endl;
    std::cout << "Point B: " << point_b << std::endl;
    std::cout << "Point C: " << point_c << std::endl;

    // another triangle
    Vector2D point_d = point_a + point_b;
    Vector2D point_e = point_d + point_c;
    auto point_f = point_a + point_b + point_c + point_d + point_e;
    std::cout << "Point D = A+B: " << point_d << std::endl;
    std::cout << "Point E = D+C: " << point_e << std::endl;
    std::cout << "Point F = A+B+C+D+E" << point_f << std::endl;


    // triangle edges
    std::cout << "=== SIDES ===" << std::endl;

    auto side_a = point_c - point_b;
    auto side_b = point_a - point_c;
    auto side_c = point_b - point_a;
    std::cout << "Side A (C-B): " << side_a << std::endl;
    std::cout << "Side B (A-C): " << side_b << std::endl;
    std::cout << "Side C (B-A): " << side_c << std::endl;

    // move the triangle up-left
    Vector2D mover(-5,5);
    std::cout << "=== MOVED ===" << std::endl << "By: " << mover << std::endl;

    point_a += mover;
    point_b += mover;
    point_c += mover;
    std::cout << "Point A: " << point_a << std::endl;
    std::cout << "Point B: " << point_b << std::endl;
    std::cout << "Point C: " << point_c << std::endl;


    // scale the triangle
    double scale = 3.0;
    std::cout << "=== RESIZ ===" << std::endl << "By: " << scale << std::endl;

    auto point_a1 = point_a * scale;
    auto point_b1 = point_b * scale;
    auto point_c1 = point_c * scale;
    std::cout << "Point A1: " << point_a1 << std::endl;
    std::cout << "Point B1: " << point_b1 << std::endl;
    std::cout << "Point C1: " << point_c1 << std::endl;

    return 0;
}
