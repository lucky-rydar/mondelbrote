#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Main.hpp>
#include <thread>
#include <mutex>
#include <vector>
#include <future>

using namespace std;
using namespace sf;

class Complex
{
private:
	double r, i;
public:
	Complex(double r, double i)
	{
		this->r = r; this->i = i;
	}
	Complex operator+(const Complex& c)
	{
		return Complex(r + c.r, i + c.i);
	}
	Complex operator+(const double& c)
	{
		return Complex(r + c, i);
	}
	Complex operator*(const Complex& c)
	{
		return Complex(r * c.r - i * c.i, 2 * r * c.i);
	}
	double abs() { return sqrt(r * r + i * i); }
};

Vector2<double> my_size(1200, 800);
Vector2<double> c_move(0, 0);
double crop = 100;

mutex mu_draw;

VertexArray draw_line(RenderWindow* win, int y)
{
	VertexArray line;
	line.setPrimitiveType(sf::Points);

	for (double x = 0; x < my_size.x; x++)
	{
		Complex z(0, 0);
		int i = 0;
		while (i < 500 && z.abs() < 16)
		{
			z = z * z + Complex(((x - my_size.x / 2) / crop) + c_move.x, ((y - my_size.y / 2) / crop) + c_move.y);

			i++;
		}
		float r = (0.1 + i * 0.03 * 0.1) * 255;
		float g = (0.2 + i * 0.03 * 0.2) * 255;
		float b = (0.3 + i * 0.03 * 0.3) * 255;

		line.append(Vertex(Vector2f(x, y), Color(r, g, b)));
	}

	return line;
}

void draw(RenderWindow* win)
{
	vector<std::future<VertexArray>> drawers;

	for (int y = 0; y < my_size.y; y++)
		drawers.push_back(async(draw_line, win, y));

	for (int i = 0; i < drawers.size(); i++)
		win->draw(drawers[i].get());

}

int main()
{
	RenderWindow window(VideoMode(my_size.x, my_size.y), "Mondelbrote");
	double speed_scale = 30;


	while (window.isOpen())
	{
		Event e;
		while (window.pollEvent(e))
		{
			if (e.type == Event::Closed)
				window.close();
		}

		auto begin = chrono::steady_clock::now();

		if(Keyboard::isKeyPressed(Keyboard::Up))
			crop *= 1.1;
		else if (Keyboard::isKeyPressed(Keyboard::Down))
			crop /= 1.1;
		
		if (Keyboard::isKeyPressed(Keyboard::A))
			c_move.x -= (1 / crop) * speed_scale;
		else if(Keyboard::isKeyPressed(Keyboard::D))
			c_move.x += (1 / crop) * speed_scale;

		if (Keyboard::isKeyPressed(Keyboard::W))
			c_move.y -= (1 / crop) * speed_scale;
		else if (Keyboard::isKeyPressed(Keyboard::S))
			c_move.y += (1 / crop) * speed_scale;

		window.clear(Color::Black);
		draw(&window);
		window.display();

		cout << "ms: " << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - begin).count() << endl;
	}
}