#include <SFML/Graphics.hpp>
#include <iostream>
#include "Map.h"

using namespace std;
using namespace sf;

String TileMap[HEIGHT_MAP] = {
	"     b        b     ",
	"    bbbbbbbbbbbb    ",
	"   bbbbbbbbbbbbbb   ",
	"   bbb bbbbbb bbb   ",
	"   bbbbbbbbbbbbbb   ",
	"    bbbbbbbbbbbb    ",
	"      bb    bb      ",
	"     bb      bb     ",
	"    bb        bb    ",
	"                    ",
	"                    ",
	"                    ",
	"                    ",
	"                    ",
	"                    "
};

class Ball {
public:
	float xball, yball, xplate, yplate;
	bool gamestart;
	String File;
	Image image;
	Texture texture;
	Sprite sprite;
	Ball(String F) {
		File = F;
		image.loadFromFile("image/" + File);
		texture.loadFromImage(image);
		sprite.setTexture(texture);
	}
	void game(float xplate, float yplate, float xball, float yball ) {
		if (gamestart == 0) {
			sprite.setPosition(xplate, yplate);
		}
		else if (gamestart == 1) {
			sprite.move(xball, yball);
		}
	}
};

float plate(float xplate, float width) {
	float speed = 7;
	if (xplate == 0) {
		if (Keyboard::isKeyPressed(Keyboard::D) or Keyboard::isKeyPressed(Keyboard::Right)) {
			return speed;
		}
		else {
			return 0;
		}
	}
	else if (xplate + 100 >= width) {
		if (Keyboard::isKeyPressed(Keyboard::A) or Keyboard::isKeyPressed(Keyboard::Left)) {
			return -speed;
		}
		else {
			return 0;
		}
	}
	else {
		if (Keyboard::isKeyPressed(Keyboard::A) or Keyboard::isKeyPressed(Keyboard::Left)) {
			return -speed;
		}
		if (Keyboard::isKeyPressed(Keyboard::D) or Keyboard::isKeyPressed(Keyboard::Right)) {
			return speed;
		}
		else {
			return 0;
		}
	}
}

int main()
{
	bool l = 1;
	float xplate, xmove, speed = 4;
	Ball ball("ball.png");
	ball.xball = 0; ball.yball = 0; ball.xplate = 0; ball.yplate = 0; ball.gamestart = 0;
	RenderWindow window(sf::VideoMode(640, 480), "Arcanoid");
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);

	Texture platetexture, map;
	platetexture.loadFromFile("image/plate.png");
	map.loadFromFile("image/block.png");

	Sprite platesprite, s_map;
	platesprite.setTexture(platetexture);//передаём в него объект Texture (текстуры)
	platesprite.scale(0.5, 0.5);
	platesprite.setPosition(0, 455);//задаем начальные координаты появления спрайта

	s_map.setTexture(map);


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
		auto realballPos = window.mapCoordsToPixel(ball.sprite.getPosition());
		xplate = realplatePos.x;
		xmove = plate(xplate, width);
		platesprite.move(xmove, 0);
		if (ball.gamestart == 0) {
			ball.xplate = realplatePos.x + 40;
			ball.yplate = realplatePos.y - 20;
			ball.game(ball.xplate, ball.yplate, ball.xball, ball.yball);
			ball.sprite.setPosition(ball.xplate, ball.yplate);
			if (Keyboard::isKeyPressed(Keyboard::Space)) {
				ball.gamestart = 1;
				ball.xball = -speed;
				ball.yball = -speed;
			}
		}
		else if (ball.gamestart == 1) {
			if (realballPos.x <= 0) {
				ball.xball = ball.xball * -1;
				l = 0;
			}
			else if (realballPos.x + 20 >= width) {
				ball.xball = ball.xball * -1;
				l = 0;
			}
			if (realballPos.y <= 0) {
				ball.yball = ball.yball * -1;
				l = 0;
			}
			else if (realballPos.y + 20 >= height) {
				ball.gamestart = 0;
				l = 0;
			}
			if (realballPos.y + 20 == realplatePos.y and realballPos.x > realplatePos.x - 10 and realballPos.x < realplatePos.x + 100 and l == 0) {
				ball.yball = ball.yball * -1;
				l = 1;
			}
			if (realballPos.y + 10 >= realplatePos.y and realballPos.x + 10 <= realplatePos.x and l == 0) {
				ball.xball = ball.xball * -1;
				l = 1;
			}
			else if (realballPos.y + 10 >= realplatePos.y and realballPos.x <= realplatePos.x + 100 and l == 0) {
				ball.xball = ball.xball * -1;
				l = 1;
			}
			ball.game(ball.xplate, ball.yplate, ball.xball, ball.yball);
			ball.sprite.move(ball.xball, ball.yball);
		}
		/*int x, y, a, b;
		x = realplatePos.x;
		y = realplatePos.y;
		a = realballPos.x;
		b = realballPos.y;*/
		cout << ball.xplate << " " << ball.yplate << " " << ball.xball << " " << ball.yball << endl;

		window.clear(Color(255, 255, 255, 255));
		for (int i = 0; i < HEIGHT_MAP; i++)
			for (int j = 0; j < WIDTH_MAP; j++)
			{
				if (TileMap[i][j] == 'b') {
					s_map.setTextureRect(IntRect(0, 0, 32, 32));
				}
				if (TileMap[i][j] == ' ') {
					s_map.setTextureRect(IntRect(32, 0, 64, 32));
				}


				s_map.setPosition(j * 32, i * 32);//по сути раскидывает квадратики, превращая в карту. то есть задает каждому из них позицию. если убрать, то вся карта нарисуется в одном квадрате 32*32 и мы увидим один квадрат

				window.draw(s_map);//рисуем квадратики на экран
			}
		window.draw(platesprite);
		window.draw(ball.sprite);
		window.display();

	}

	return 0;
}