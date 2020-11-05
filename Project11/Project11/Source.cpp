#include <SFML/Graphics.hpp>
#include <iostream>

using namespace std;
using namespace sf;

float plate(float xplate, float width) {
	if (xplate == 0) {
		if (Keyboard::isKeyPressed(Keyboard::D) or Keyboard::isKeyPressed(Keyboard::Right)) {
			return 5;
		}
		else {
			return 0;
		}
	}
	else if (xplate + 100 == width) {
		if (Keyboard::isKeyPressed(Keyboard::A) or Keyboard::isKeyPressed(Keyboard::Left)) {
			return -5;
		}
		else {
			return 0;
		}
	}
	else {
		if (Keyboard::isKeyPressed(Keyboard::A) or Keyboard::isKeyPressed(Keyboard::Left)) {
			return -5;
		}
		if (Keyboard::isKeyPressed(Keyboard::D) or Keyboard::isKeyPressed(Keyboard::Right)) {
			return 5;
		}
		else {
			return 0;
		}
	}
}

int main()
{
	int t = 0;
	float xplate, xmove, xball = 0, yball = 0;
	bool gamestart = 0;
	RenderWindow window(sf::VideoMode(640, 480), "Arcanoid");
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);
	Texture platetexture, balltexture;
	platetexture.loadFromFile("image/plate.png");
	balltexture.loadFromFile("image/ball.png");

	Sprite platesprite, ball;
	platesprite.setTexture(platetexture);//передаём в него объект Texture (текстуры)
	platesprite.scale(0.5, 0.5);
	platesprite.setPosition(0, 455);//задаем начальные координаты появления спрайта

	ball.setTexture(balltexture);


	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		auto size = window.getSize();
		float width = size.x;
		float height = size.y;
		

		auto realplatePos = window.mapCoordsToPixel(platesprite.getPosition());
		auto realballPos = window.mapCoordsToPixel(ball.getPosition());
		xplate = realplatePos.x;
		xmove = plate(xplate, width);
		platesprite.move(xmove, 0);
		if (gamestart == 0) {
			ball.setPosition(realplatePos.x + 40, realplatePos.y - 20);
			if (Keyboard::isKeyPressed(Keyboard::Space)) {
				gamestart = 1;
				xball = -5;
				yball = -5;
			}
			t = 0;
		}
		else if(gamestart == 1){
			if (realballPos.x <= 0) {
				xball = xball * -1;
			}
			else if (realballPos.x + 20 >= width) {
				xball = xball * -1;
			}
			if (realballPos.y <= 0) {
				yball = yball * -1;
			}
			else if (realballPos.y + 20 >= height) {
				gamestart = 0;
			}
			if (realballPos.y + 20 == realplatePos.y and realballPos.x > realplatePos.x and realballPos.x < realplatePos.x + 100 and t > 3) {
				yball = yball * -1;
			}
			if (realballPos.y + 10 >= realplatePos.y and realballPos.x + 20 <= realplatePos.x) {
				xball = xball * -1;
			}
			else if (realballPos.y + 10 >= realplatePos.y and realballPos.x <= realplatePos.x + 100) {
				xball = xball * -1;
			}
			ball.move(xball, yball);
			t++;
		}

		cout << realballPos.x << " " << realplatePos.x << " " << realballPos.y << " " << realplatePos.y << endl;

		window.clear(Color(255, 255, 255, 255));
		window.draw(platesprite);
		window.draw(ball);
		window.display();

	}

	return 0;
}