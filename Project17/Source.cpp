
#include "Map.h"
#include <cmath>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <set>

using namespace std;
using namespace sf;

int TileMap[HEIGHT_MAP][WIDTH_MAP];

constexpr int windowWidth{ 640 }, windowHeight{ 480 };
float ballRadius{ 10.f }, ballVelocity{ 4.f };
float paddleWidth{ 100.f }, paddleHeight{ 20.f }, paddleVelocity{ 7.f };
constexpr float blockWidth{ 32.f }, blockHeight{ 32.f };
constexpr float bonusWidth{ 32.f }, bonusHeight{ 32.f };
constexpr int countBlocksX{ WIDTH_MAP }, countBlocksY{ HEIGHT_MAP };

int y = 1;

struct Ball
{
    CircleShape shape;
    Vector2f velocity{ -ballVelocity, -ballVelocity };
	int live = 0, game = 0, score = 0;

    Ball(float mX, float mY)
    {
        shape.setPosition(mX, mY);
        shape.setRadius(ballRadius);
        shape.setFillColor(Color::Red);
        shape.setOrigin(ballRadius, ballRadius);
    }

    void update()
    {
        shape.move(velocity);

        if (left() < 0)
            velocity.x = ballVelocity;
        else if (right() > windowWidth)
            velocity.x = -ballVelocity;

        if (top() < 20)
            velocity.y = ballVelocity;
        else if (bottom() > windowHeight) {
			live = live - 1;
			game = 0;
        }
            
    }

    float x() { return shape.getPosition().x; }
    float y() { return shape.getPosition().y; }
    float left() { return x() - shape.getRadius(); }
    float right() { return x() + shape.getRadius(); }
    float top() { return y() - shape.getRadius(); }
    float bottom() { return y() + shape.getRadius(); }
};

struct Rectangle
{
    RectangleShape shape;
    float x() { return shape.getPosition().x; }
    float y() { return shape.getPosition().y; }
    float left() { return x() - shape.getSize().x / 2.f; }
    float right() { return x() + shape.getSize().x / 2.f; }
    float top() { return y() - shape.getSize().y / 2.f; }
    float bottom() { return y() + shape.getSize().y / 2.f; }
};

struct Paddle : public Rectangle
{
    Vector2f velocity;

    Paddle(float mX, float mY)
    {
        shape.setPosition(mX, mY);
        shape.setSize({ paddleWidth, paddleHeight });
        shape.setFillColor(Color(127,51,0,255));
        shape.setOrigin(paddleWidth / 2.f, paddleHeight / 2.f);
    }

    void update()
    {
        shape.move(velocity);

        if (Keyboard::isKeyPressed(Keyboard::Key::Left) and left() > 0 or Keyboard::isKeyPressed(Keyboard::Key::A) and left() > 0)
            velocity.x = -paddleVelocity;
        else if (Keyboard::isKeyPressed(Keyboard::Key::Right) and right() < windowWidth or Keyboard::isKeyPressed(Keyboard::Key::D) and right() < windowWidth)
            velocity.x = paddleVelocity;
        else
            velocity.x = 0;
    }
};

struct Brick : public Rectangle
{
    bool destroyed{ false };

    Brick(float mX, float mY)
    {
        shape.setPosition(mX, mY);
        shape.setSize({ blockWidth, blockHeight });
        shape.setFillColor(Color::Blue);
        shape.setOrigin(blockWidth / 2.f, blockHeight / 2.f);
    }
};

template <class T1, class T2>
bool isIntersecting(T1& mA, T2& mB)
{
    return mA.right() >= mB.left() && mA.left() <= mB.right() &&
        mA.bottom() >= mB.top() && mA.top() <= mB.bottom();
}

void testCollision(Paddle& mPaddle, Ball& mBall)
{
    if (!isIntersecting(mPaddle, mBall)) return;

    mBall.velocity.y = -ballVelocity;
    if (mBall.x() < mPaddle.x())
        mBall.velocity.x = -ballVelocity;
    else
        mBall.velocity.x = ballVelocity;
}

void testCollision(Brick& mBrick, Ball& mBall)
{
	

    if (!isIntersecting(mBrick, mBall)) return;
    mBrick.destroyed = true;

    float overlapLeft{ mBall.right() - mBrick.left() };
    float overlapRight{ mBrick.right() - mBall.left() };
    float overlapTop{ mBall.bottom() - mBrick.top() };
    float overlapBottom{ mBrick.bottom() - mBall.top() };

    bool ballFromLeft(abs(overlapLeft) < abs(overlapRight));
    bool ballFromTop(abs(overlapTop) < abs(overlapBottom));

    float minOverlapX{ ballFromLeft ? overlapLeft : overlapRight };
    float minOverlapY{ ballFromTop ? overlapTop : overlapBottom };

	if (abs(minOverlapX) < abs(minOverlapY)) {
		mBall.velocity.x = ballFromLeft ? -ballVelocity : ballVelocity;
		mBall.score++;
		if (mBall.score != 0 and mBall.score % 100 == 0 and y == 1) {
			mBall.live++;
			y = 0;
		}
		else {
			y = 1;
		}
	} 
	else {
		mBall.velocity.y = ballFromTop ? -ballVelocity : ballVelocity;
		mBall.score++;
		if (mBall.score != 0 and mBall.score % 100 == 0 and y == 1) {
			mBall.live++;
			y = 0;
		}
		else {
			y = 1;
		}
	}
}

int menu(RenderWindow& window, int menucond) {
	Texture menuTexture1, menuTexture2, menuTexture3;
	menuTexture1.loadFromFile("image/text/Play.png");
	menuTexture2.loadFromFile("image/text/Table.png");
	menuTexture3.loadFromFile("image/text/Exit.png");
	Sprite menu1(menuTexture1), menu2(menuTexture2), menu3(menuTexture3);
	bool isMenu = 1;
	int menuNum = 0;
	menu1.setPosition(100, 30);
	menu2.setPosition(100, 90);
	menu3.setPosition(100, 150);

	while (isMenu) {
		menuNum = 0;
		window.clear(Color(255, 255, 255));

		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
		}

		if (IntRect(100, 30, 300, 50).contains(Mouse::getPosition(window))) {
			menuNum = 1;
		}
		if (IntRect(100, 90, 300, 50).contains(Mouse::getPosition(window))) {
			menuNum = 2;
		}
		if (IntRect(100, 150, 300, 50).contains(Mouse::getPosition(window))) {
			menuNum = 3;
		}

		if (Mouse::isButtonPressed(Mouse::Left))
		{
			if (menuNum == 1) {
				isMenu = 0;
				return 0;
			}
			if (menuNum == 2) {
				isMenu = 0;
				return 3;
			}
			if (menuNum == 3) {
				isMenu = 0;
				window.close();
				return 0;
			}

		}
		window.draw(menu1);
		window.draw(menu2);
		window.draw(menu3);
		window.display();
	}
}

int gameover(RenderWindow& window, int menucond) {
	Texture endtext, mentext, savetext;
	endtext.loadFromFile("image/gameover.png");
	mentext.loadFromFile("image/text/Menu.png");
	savetext.loadFromFile("image/text/save.png");
	Sprite end(endtext), men(mentext), save(savetext);
	end.setPosition(0, 0);
	end.scale(0.74, 0.70);
	men.setPosition(0, 440);
	save.setPosition(230, 440);
	bool isEnd = 1;
	int menuNum = 0;
	while (isEnd) {
		menuNum = 0;
		window.clear(Color(255, 255, 255));

		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
		}

		if (IntRect(0, 440, 100, 480).contains(Mouse::getPosition(window))) {
			menuNum = 1;
		}
		if (IntRect(230, 440, 640, 480).contains(Mouse::getPosition(window))) {
			menuNum = 2;
		}
		if (Mouse::isButtonPressed(Mouse::Left))
		{
			if (menuNum == 1) {
				isEnd = 0;
				return 1;
			}
			if (menuNum == 2) {
				return 4;
			}

		}
		window.draw(end);
		window.draw(men);
		window.draw(save);
		window.display();
	}

}

int win(RenderWindow& window, int score) {
	Texture endtext, mentext, savetext;
	endtext.loadFromFile("image/win.png");
	mentext.loadFromFile("image/text/Menu.png");
	savetext.loadFromFile("image/text/save.png");
	Sprite end(endtext), men(mentext), save(savetext);
	end.setPosition(0, 0);
	end.scale(0.74, 0.70);
	men.setPosition(0, 440);
	save.setPosition(230, 440);
	bool isEnd = 1;
	int menuNum = 0;
	while (isEnd) {
		menuNum = 0;
		window.clear(Color(255, 255, 255));

		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
		}

		if (IntRect(0, 440, 100, 480).contains(Mouse::getPosition(window))) {
			menuNum = 1;
		}
		if (IntRect(230, 440, 640, 480).contains(Mouse::getPosition(window))) {
			menuNum = 2;
		}
		if (Mouse::isButtonPressed(Mouse::Left))
		{
			if (menuNum == 1) {
				isEnd = 0;
				return 1;
			}
			if (menuNum == 2) {
				return 4;
			}

		}
		window.draw(end);
		window.draw(men);
		window.draw(save);
		window.display();
	}
}

int table(RenderWindow& window) {
	Font font;
	font.loadFromFile("ColumbiaRandom BoldItalic.ttf");
	Text number("", font, 20), record("", font, 20), name("", font, 20);
	number.setFillColor(Color::Black); record.setFillColor(Color::Black);

	int d = 1, t = 0;

	string s,a = "l";

	ifstream file("Record.csv");


	RectangleShape edge(Vector2f(630.f, 425.f)), line1(Vector2f(630.f, 1.f)), line11(Vector2f(1.f, 425.f));
	line1.setFillColor(Color::Black);
	
	line11.setFillColor(Color::Black);
	line11.setPosition(45, 5);

	edge.setPosition(5, 5);
	edge.setOutlineThickness(1.f);
	edge.setOutlineColor(Color::Black);

	Texture mentext;
	mentext.loadFromFile("image/text/Menu.png");
	Sprite men(mentext);
	men.setPosition(0, 440);

	while (true)
	{
		string v;
		getline(file, v);
		if (!file.eof())
			t++;
		else
			break;
	}

	bool isTable = 1;
	int menuNum = 0;

	while (isTable) {
		menuNum = 0;
		window.clear(Color(255, 255, 255));

		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		if (IntRect(0, 440, 100, 480).contains(Mouse::getPosition(window))) {
			menuNum = 1;
		}
		if (Mouse::isButtonPressed(Mouse::Left))
		{
			if (menuNum == 1) {
				isTable = 0;
				return 1;
			}

		}
		window.draw(men);
		window.draw(edge);
		for (int i = 1; i < 11; i++) {
			line1.setPosition(5, 43 * i);
			window.draw(line1);
		}
		window.draw(line11);
		for (int i = 1; i < 11; i++) {
			string num = to_string(i);
			number.setString(num);
			number.setPosition(10, 43 * (i - 1) + 5);
			window.draw(number);
		}
		file.seekg(0, std::ios::beg);
		for (int i = 1; i < t + 2; i++) {
			getline(file, s);
			
			for (int j = 0; j < s.length(); j++) {
				if (s[j] == *";") {

					s.erase(j, 1);
				}
			}
			if (a == s or s == " ") {
				break;
			}
			a = s;
			record.setString(s);
			record.setPosition(50, 43 * (i - 1) + 5);
			window.draw(record);
		}
		window.display();
	}
	file.close();
}

int save(RenderWindow& window, int score) {
	string name = "", a;


	Font font;
	font.loadFromFile("ColumbiaRandom BoldItalic.ttf");
	Text nametext(name, font, 50), scoretext("score " + to_string(score) + ": ", font, 50);
	nametext.setFillColor(Color::Black);
	nametext.setPosition(240, 200);
	scoretext.setFillColor(Color::Black);
	scoretext.setPosition(10, 200);
	
	ofstream file("Record.csv", ios_base::app);
	
	Texture mentext, savetext;
	mentext.loadFromFile("image/text/Menu.png");
	savetext.loadFromFile("image/text/save.png");
	Sprite men(mentext), save(savetext);
	men.setPosition(0, 0);
	save.setPosition(230, 0);
	
	bool isSave = 1;
	int menuNum = 0, t, min = 0, num;
	char buff[50];

	while (isSave) {
		menuNum = 0;
		window.clear(Color(255, 255, 255));

		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::TextEntered)
			{
				if (event.text.unicode < 128)
				{ 
					
					if (event.text.unicode == 8) {
						name = name.substr(0, name.size() - 1);
					}
					else {
						name = name + static_cast<char>(event.text.unicode);
					}
				}
			}
		}

		nametext.setString(name);

		if (IntRect(0, 0, 100, 32).contains(Mouse::getPosition(window))) {
			menuNum = 1;
		}
		if (IntRect(230, 0, 640, 32).contains(Mouse::getPosition(window))) {
			menuNum = 2;
		}
		if (Mouse::isButtonPressed(Mouse::Left))
		{
			if (menuNum == 1) {
				isSave = 0;
				return 1;
			}
			if (menuNum == 2) {
				isSave = 0;

				file <<"\n" <<  score << " ;" << name ;
				file.close();
				return 3;
			}

		}
		string scorstr = to_string(score);
		window.draw(men);
		window.draw(save);
		window.draw(nametext);
		window.draw(scoretext);
		window.display();
	}
}

int pause(RenderWindow& window, int menucond) {
	Texture mentext, resumetext;
	mentext.loadFromFile("image/text/Menu.png");
	resumetext.loadFromFile("image/text/resume.png");
	Sprite men(mentext), resume(resumetext);
	resume.setPosition(100, 200);
	men.setPosition(100, 300);
	bool isPause = 1;
	int menuNum = 0;
	while (isPause) {
		menuNum = 0;
		window.clear(Color(255, 255, 255));

		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
		}

		if (IntRect(100, 200, 300, 50).contains(Mouse::getPosition(window))) {
			menuNum = 1;
		}
		if (IntRect(100, 300, 300, 50).contains(Mouse::getPosition(window))) {
			menuNum = 2;
		}
		if (Mouse::isButtonPressed(Mouse::Left))
		{
			if (menuNum == 1) {
				isPause = 0;
				return 0;
			}
			if (menuNum == 2) {
				isPause = 0;
				return 1;
			}
		}
		window.draw(resume);
		window.draw(men);
		window.display();
	}
}

int main()
{
    Ball ball{ windowWidth / 2, windowHeight };
    Paddle paddle{ windowWidth / 2, windowHeight };
    vector<Brick> bricks;

	int score = 0, menucond = 1, num = 0, bonusst = 0, brick = 0, level = 1;
            
	Font font;
	font.loadFromFile("ColumbiaRandom BoldItalic.ttf");
	Text livetext("Live", font, 20);
	Text scoretext("Score", font, 20);
	livetext.setFillColor(Color::Black);
	livetext.setStyle(Text::Underlined);
	scoretext.setFillColor(Color::Black);
	scoretext.setStyle(Text::Underlined);

    RenderWindow window{ {windowWidth, windowHeight}, "Arkanoid", Style::Close};
    window.setFramerateLimit(60);

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }

		if (Keyboard::isKeyPressed(Keyboard::Escape)) {
			menucond = 2;
		}

		auto realpaddlepos = window.mapCoordsToPixel(paddle.shape.getPosition());

		if (ball.game == 0) {
			ball.shape.setPosition(realpaddlepos.x, realpaddlepos.y - 20);
			if (Keyboard::isKeyPressed(Keyboard::Space)) {
				ball.game = 1;
			}
		}
		else if(ball.game == 1){
			ball.update();
		}
         paddle.update();
         testCollision(paddle, ball);
		 for (auto& brick : bricks) {
			 testCollision(brick, ball);
		 }
         bricks.erase(remove_if(begin(bricks), end(bricks), [](const Brick& mBrick){return mBrick.destroyed;}),end(bricks));

		 if (bricks.size() == 0 and menucond == 0) {
			 menucond = 5;
			 level++;
			 paddleVelocity++;
		 }
		 cout << bricks.size() << endl;

		 string lives = to_string(ball.live), scores = to_string(ball.score);
		 livetext.setString("Live:" + lives);
		 scoretext.setString("Score:" + scores);
		 livetext.setPosition(570, 0);
		 scoretext.setPosition(5, 0);

			if (menucond == 0 and ball.live > 0) {
				window.clear(Color::White);
				window.draw(livetext);
				window.draw(scoretext);
				window.draw(ball.shape);
				window.draw(paddle.shape);
				for (auto& brick : bricks) {
					window.draw(brick.shape);
				}
			}
            else if (menucond == 1) {
				level = 1;
				bricks.clear();
                menucond = menu(window, menucond);
                ball.live = 3;
				ball.game = 0;
                ball.score = 0;
				paddleVelocity = 7.f;
                for (int i = 0; i < HEIGHT_MAP; i++) {
                    for (int j = 0; j < WIDTH_MAP; j++) {
                        TileMap[i][j] = Level1[i][j];
                    }
                }
				for (int iX = 0; iX < countBlocksX; ++iX){
					for (int iY = 0; iY < countBlocksY; ++iY) {
						if (TileMap[iY][iX] == 1) {
							bricks.emplace_back((iX) * (blockWidth + 3), (iY) * (blockHeight + 3));
						}
					}
				}
					


            }
            else if (menucond == 2) {
                menucond = pause(window, menucond);
            }
            else if (menucond == 3) {
                menucond = table(window);
            }
            else if (menucond == 4) {
                menucond = save(window, ball.score);
            }
			else if (menucond == 5) {
				bricks.clear();
				ball.game = 0;
				window.clear(Color::White);
				if (level == 2) {
					for (int i = 0; i < HEIGHT_MAP; i++) {
						for (int j = 0; j < WIDTH_MAP; j++) {
							TileMap[i][j] = Level2[i][j];
						}
					}
					for (int iX = 0; iX < countBlocksX; ++iX) {
						for (int iY = 0; iY < countBlocksY; ++iY) {
							if (TileMap[iY][iX] == 1) {
								bricks.emplace_back((iX) * (blockWidth + 3), (iY) * (blockHeight + 3));
							}
						}
					}
					menucond = 0;
				}
				else if (level == 3) {
					for (int i = 0; i < HEIGHT_MAP; i++) {
						for (int j = 0; j < WIDTH_MAP; j++) {
							TileMap[i][j] = Level3[i][j];
						}
					}
					for (int iX = 0; iX < countBlocksX; ++iX) {
						for (int iY = 0; iY < countBlocksY; ++iY) {
							if (TileMap[iY][iX] == 1) {
								bricks.emplace_back((iX) * (blockWidth + 3), (iY) * (blockHeight + 3));
							}
						}
					}
					menucond = 0;
				}
				else if (level == 4) {
					for (int i = 0; i < HEIGHT_MAP; i++) {
						for (int j = 0; j < WIDTH_MAP; j++) {
							TileMap[i][j] = Level4[i][j];
						}
					}
					for (int iX = 0; iX < countBlocksX; ++iX) {
						for (int iY = 0; iY < countBlocksY; ++iY) {
							if (TileMap[iY][iX] == 1) {
								bricks.emplace_back((iX) * (blockWidth + 3), (iY) * (blockHeight + 3));
							}
						}
					}
					menucond = 0;
				}
				else if (level == 5) {
					for (int i = 0; i < HEIGHT_MAP; i++) {
						for (int j = 0; j < WIDTH_MAP; j++) {
							TileMap[i][j] = Level5[i][j];
						}
					}
					for (int iX = 0; iX < countBlocksX; ++iX) {
						for (int iY = 0; iY < countBlocksY; ++iY) {
							if (TileMap[iY][iX] == 1) {
								bricks.emplace_back((iX) * (blockWidth + 3), (iY) * (blockHeight + 3));
							}
						}
					}
					menucond = 0;
				}
				else {
					menucond = win(window, ball.score);
				}
			}
            else {
                menucond = gameover(window, menucond);
            }
    window.display();
    }

    return 0;
}