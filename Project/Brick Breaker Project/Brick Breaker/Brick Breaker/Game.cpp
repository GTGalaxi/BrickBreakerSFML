/*
	
	Script by
	Mitchell Smith

*/

// includes
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include "Windows.h"

// game function
int main()
{
	// setting sound buffers for sound 1, 2
	sf::SoundBuffer buffer;
	sf::SoundBuffer buffer2;

	// setting up the sounds for the game
	sf::Sound sound;
	sf::Sound sound2;

	// setting up the music for the game
	sf::Music music;

	// load music clip
	if (!music.openFromFile("music.ogg"))
	{
	}

	// loop music clip
	music.setLoop(true);

	// lower volume of bg music
	music.setVolume(10);

	// if statements that loads sound 1
	if (!buffer.loadFromFile("bounce-00.wav"))
	{
	}

	// if statement that loads sound 2
	if (!buffer2.loadFromFile("explosion-03.wav"))
	{
	}

	// loading the buffers that were previously set for sounds 1, 2 and 3
	sound.setBuffer(buffer);
	sound2.setBuffer(buffer2);

	// declaring variables
	int ScreenWidth;
	int ScreenHeight;
	bool fullScreen = false;
	float Length;
	float Height;
	int Score = 0;
	bool Start = false;
	float xSpeed;
	float ySpeed;
	int lives = 3;
	float pad;
	float PaddleSpeed;
	int Win = 0;
	sf::Font font;
	std::string userInput="";
	std::string ResString = "";

	// crisper shapes by uping the antialiasing
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;

	// create game window with user set 
	sf::RenderWindow window;

	// limit framerate to 60
	window.setFramerateLimit(60);

	// talking to user
	std::cout << "Enter a custom Game Resolution. Leave blank to use default. \nInput format example: 640x480\nAny resolution above 640x480 is accepted, however for the game to look better a 4:3 or 16:9 resoltuion scale is recommended.\nType 'FULLSCREEN' to play in fullscreen mode.\n>>";

	// get user input
	std::getline(std::cin, userInput);

	// if they type FULLSCREEN
	if (userInput == "FULLSCREEN")
	{
		// getting monitor resolution for fullscreen mode
		ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
		
		// set window mode to fullscreen
		window.create(sf::VideoMode(ScreenWidth, ScreenHeight), "Brick Break", sf::Style::Fullscreen, settings);
	}

	// otherwise set resolution to user supplied
	else
	{
		// stringstream and pair for storing user input res as 2 ints for x and y
		std::istringstream ss(userInput);
		std::pair <int, int> Resolution;
		int count = 0;

		// set pair to resolution
		while (std::getline(ss, ResString, 'x')) {
			int userResXY = atoi(ResString.c_str());
			if (count == 0)
			{
				Resolution.first = userResXY;
				count = count + 1;
			}
			else
			{
				Resolution.second = userResXY;
			}
		}

		// if res is smaller than 640x480 set to default of 640x480
		if (Resolution.first <= 639 || Resolution.second <= 479)
		{
			ScreenWidth = 640;
			ScreenHeight = 480;
			Resolution.first = ScreenWidth;
			Resolution.second = ScreenHeight;
		}

		// otherwise use user supplied resolution
		else
		{
			ScreenWidth = Resolution.first;
			ScreenHeight = Resolution.second;
		}

		// output users res for debug
		std::cout << "Resolution: " << Resolution.first << "x" << Resolution.second << "\n\n//IGNORE \n";

		// set window mode to fullscreen
		window.create(sf::VideoMode(ScreenWidth, ScreenHeight), "Brick Break", sf::Style::Close, settings);

	}
	
	// attempt to load the font
	if (!font.loadFromFile("pixelmix.ttf"))
	{
		// output error if failure to load font occurs
		std::cout << "\n";
	}

	// declare the texts used in game
	sf::Text ScoreText;
	sf::Text LivesText;
	sf::Text EndScoreText;
	sf::Text LoseText;
	sf::Text WinText;

	// select the font
	ScoreText.setFont(font);
	LivesText.setFont(font);
	EndScoreText.setFont(font);
	LoseText.setFont(font);
	WinText.setFont(font);

	// setup text sizes and strings
	ScoreText.setString("Press Space to start!");
	LivesText.setString("Lives : " + std::to_string(lives));
	LoseText.setString("You Lose...");
	WinText.setString("You Win!");
	ScoreText.setCharacterSize(ScreenWidth*0.03);
	LivesText.setCharacterSize(ScreenWidth*0.03);
	EndScoreText.setCharacterSize(ScreenWidth*0.05);
	LoseText.setCharacterSize(ScreenWidth*0.07);
	WinText.setCharacterSize(ScreenWidth*0.07);

	// colour lose and win text
	LoseText.setColor(sf::Color::Red);
	WinText.setColor(sf::Color::Green);

	// position score and lives text
	ScoreText.setPosition(ScreenWidth * 0.01, 0);
	LivesText.setPosition(ScreenWidth * 0.99 - LivesText.getLocalBounds().width, 0);

	// setting up the size, colour, and position of the ball
	sf::CircleShape Ball(ScreenWidth*0.01);
	Ball.setFillColor(sf::Color(255, 255, 255));
	Ball.setPosition(ScreenWidth * 0.5 - Ball.getLocalBounds().width/ 2, ScreenHeight * 0.9 - Ball.getLocalBounds().height / 2);

	// setting up the size, colour, and position of the paddle
	sf::RectangleShape Paddle(sf::Vector2f(ScreenWidth * 0.15, ScreenHeight * 0.03));
	Paddle.setFillColor(sf::Color(100, 250, 50));
	Paddle.setPosition(ScreenWidth * 0.5 - Paddle.getLocalBounds().width /2, ScreenHeight - Paddle.getLocalBounds().height);

	// brick length and height
	Length = ScreenWidth * 0.11;
	Height = ScreenHeight * 0.05;

	// ball speed on x and y axis
	ySpeed = ScreenHeight * 0.0002;
	xSpeed = ScreenWidth * 0.0002;

	// setting up a gap between the bricks
	float gap = ((ScreenWidth / 9) / 11) * 1.5;

	// setting the value of the gap between the border and the bricks
	float Gap;
	Gap = (window.getSize().x - (8 - 1)*gap - 8 * Length) / 2;

	// setting up the size, colour, and position of the red bricks as well as making them visible
	std::vector <sf::RectangleShape> Brick(8);
	bool collidable[8];
	for (int i = 0; i < Brick.size(); ++i)
	{
		Brick[i].setSize(sf::Vector2f(Length, Height));
		Brick[i].setFillColor(sf::Color::Red);
		Brick[i].setPosition(Gap + i * (Length + gap), window.getSize().y * 0.11);
		collidable[i] = true;
	}

	// setting up the size, colour, and position of the orange bricks as well as making them visible
	std::vector <sf::RectangleShape> Brick2(8);
	bool collidable2[8];
	for (int i = 0; i < Brick2.size(); ++i)
	{
		Brick2[i].setSize(sf::Vector2f(Length, Height));
		Brick2[i].setFillColor(sf::Color(255, 127, 0));
		Brick2[i].setPosition(Gap + i * (Length + gap), window.getSize().y * 0.20);
		collidable2[i] = true;
	}

	// setting up the size, colour, and position of the yellow bricks as well as making them visible
	std::vector <sf::RectangleShape> Brick3(8);
	bool collidable3[8];
	for (int i = 0; i < Brick3.size(); ++i)
	{
		Brick3[i].setSize(sf::Vector2f(Length, Height));
		Brick3[i].setFillColor(sf::Color::Yellow);
		Brick3[i].setPosition(Gap + i * (Length + gap), window.getSize().y * 0.29);
		collidable3[i] = true;
	}
	
	// set paddle speed
	PaddleSpeed = ScreenWidth * 0.0003;

	// set padding of bricks and paddle
	pad = ScreenWidth * 0.001;

	// play background music
	music.play();

	// if the game window is open
	while (window.isOpen())
	{
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window.pollEvent(event))
		{
			// "close requested" event: close the window
			if (event.type == sf::Event::Closed)
				window.close();
		}

		// store bricks destroyed count
		Win = 0;

		// check for bricks in red row
		for (int i = 0; i < 8; ++i)
		{
			// if brick destroyed
			if (!collidable[i])
			{
				// add to count
				Win++;
			}
		}

		// check for bricks in orange row
		for (int i = 0; i < 8; ++i)
		{
			// if brick destroyed
			if (!collidable2[i])
			{
				// add to count
				Win++;
			}
		}

		// check for bricks in yellow row
		for (int i = 0; i < 8; ++i)
		{
			// if brick destroyed
			if (!collidable3[i])
			{
				// add to count
				Win++;
			}
		}

		// game loop if health is above 0 and there are still bricks to break
		if (lives > 0 &&  Win < 24) 
		{
			// check that paddle is within bounds
			if (Paddle.getPosition().x > 0)
			{
				// check for keyboard input
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
				{
					// left key is pressed: move the paddle
					Paddle.move(-PaddleSpeed, 0);
				}
			}

			// check that paddle is within bounds
			if (Paddle.getPosition().x < ScreenWidth - Paddle.getLocalBounds().width)
			{
				// check for keyboard input
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
				{
					// right key is pressed: move the paddle
					Paddle.move(PaddleSpeed, 0);
				}
			}

			// check if space bar has been pressed
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
			{
				// space bar pressed: fire ball
				Start = true;
			}

			// if space hasn't been pressed to fire ball
			if (Start == false)
			{
				// make ball follow player paddle ready to fire
				Ball.setPosition(sf::Vector2f(Paddle.getPosition().x + Paddle.getLocalBounds().width / 2 - Ball.getLocalBounds().width / 2, window.getSize().y*0.9f));
			}

			// if it has *^
			else
			{
				// start moving ball
				Ball.move(xSpeed, -ySpeed);

				// replace start string with score string
				ScoreText.setString("Score : " + std::to_string(Score));
			}

			// left border collision  detection
			if (Ball.getPosition().x < 1)
			{
				// play sound
				sound.play();

				// bounce
				xSpeed = -xSpeed;
			}

			// right border collision  detection
			if (Ball.getPosition().x  > window.getSize().x - (Ball.getLocalBounds().width * 1.2))
			{
				// play sound
				sound.play();

				// bounce
				xSpeed = -xSpeed;
			}

			// top border collision  detection
			if (Ball.getPosition().y < 1)
			{
				// play sound
				sound.play();

				// bounce
				ySpeed = -ySpeed;
			}

			// bottom border collision  detection and lost lives if the ball hits the floor
			if (Ball.getPosition().y > window.getSize().y - Ball.getLocalBounds().height)
			{
				// reset ball pos
				Ball.setPosition(sf::Vector2f(Paddle.getPosition().x + 30.5f, window.getSize().y*0.9f));

				// stop ball movement
				Start = false;

				// deduct life
				lives = lives - 1;

				// play sound
				sound2.play();

				// bounce for next start
				ySpeed = -ySpeed;
			}


			if (Ball.getGlobalBounds().intersects(Paddle.getGlobalBounds()))
			{
				// setting collision on the right X axis with the paddle
				if (Ball.getPosition().x > Paddle.getPosition().x + Length - pad) 
				{
					// stop ball getting stuck on paddle
					Paddle.move(-PaddleSpeed * 10, 0);

					// bounce
					xSpeed = -xSpeed;

					// play sound
					sound.play();
				}

				// setting collision on the left X axis with the paddle
				if (Ball.getPosition().x < Paddle.getPosition().x + pad)
				{
					// stop ball getting stuck on paddle
					Paddle.move(PaddleSpeed * 10, 0);

					// bounce
					xSpeed = -xSpeed;

					// play sound
					sound.play();
				}

				// setting collision on the Y axis with the paddle
				if (Ball.getPosition().y + Ball.getLocalBounds().height > Paddle.getPosition().y)
				{
					// bounce
					ySpeed = -ySpeed;

					// play sound
					sound.play();
				}
			}

			// for loop that does each of the below if statements to each of the bricks
			for (int i = 0; i < 8; ++i)
			{
				// ball collision with bricks, receives global boundaries
				if (Ball.getGlobalBounds().intersects(Brick[i].getGlobalBounds()) && collidable[i])
				{
					// setting collision on the X axis with the bricks
					if (Ball.getPosition().x > Brick[i].getPosition().x + Brick[i].getLocalBounds().width - pad || Ball.getPosition().x + Ball.getLocalBounds().width - pad < Brick[i].getPosition().x)
					{
						// bounce
						xSpeed = -xSpeed;

						//destroy brick
						collidable[i] = false;

						// play sound
						sound2.play();

						// add score
						Score = Score + 3;
					}

					// setting collision on the Y axis with the bricks
					if (Ball.getPosition().y > Brick[i].getPosition().y + Brick[i].getLocalBounds().height - pad || Ball.getPosition().y + Ball.getLocalBounds().height - pad < Brick[i].getPosition().y)
					{
						// bounce
						ySpeed = -ySpeed;

						// destroy brick
						collidable[i] = false;

						// play sound
						sound2.play();

						// add score
						Score = Score + 3;
					}

				}
			}


			// for loop that does each of the below if statements to each of the bricks
			for (int i = 0; i < 8; ++i)
			{
				// ball collision with bricks, receives global boundaries
				if (Ball.getGlobalBounds().intersects(Brick2[i].getGlobalBounds()) && collidable2[i])
				{

					// setting collision on the X axis with the bricks
					if (Ball.getPosition().x > Brick2[i].getPosition().x + Brick2[i].getLocalBounds().width - pad || Ball.getPosition().x + Ball.getLocalBounds().width - pad < Brick2[i].getPosition().x)
					{
						// bounce
						xSpeed = -xSpeed;

						// destroy brick
						collidable2[i] = false;

						// play sound
						sound2.play();

						// add score
						Score = Score + 2;
					}
					// setting collision on the Y axis with the bricks
					if (Ball.getPosition().y > Brick2[i].getPosition().y + Brick2[i].getLocalBounds().height - pad || Ball.getPosition().y + Ball.getLocalBounds().height - pad < Brick2[i].getPosition().y)
					{
						// bounce
						ySpeed = -ySpeed;

						// destroy brick
						collidable2[i] = false;

						// play sound
						sound2.play();

						// add score
						Score = Score + 2;
					}

				}
			}

			// for loop that does each of the below if statements to each of the bricks
			for (int i = 0; i < 8; ++i)
			{
				// ball collision with bricks, receives global boundaries
				if (Ball.getGlobalBounds().intersects(Brick3[i].getGlobalBounds()) && collidable3[i])
				{

					// setting collision on the X axis with the bricks
					if (Ball.getPosition().x > Brick3[i].getPosition().x + Brick3[i].getLocalBounds().width - pad || Ball.getPosition().x + Ball.getLocalBounds().width - pad < Brick3[i].getPosition().x)
					{
						// bounce
						xSpeed = -xSpeed;

						// destroy brick
						collidable3[i] = false;

						// play sound
						sound2.play();

						// add score
						Score = Score + 1;
					}
					// setting collision on the Y axis with the bricks
					if (Ball.getPosition().y > Brick3[i].getPosition().y + Brick3[i].getLocalBounds().height - pad || Ball.getPosition().y + Ball.getLocalBounds().height - pad < Brick3[i].getPosition().y)
					{
						// bounce
						ySpeed = -ySpeed;

						// destroy brick
						collidable3[i] = false;

						// play sound
						sound2.play();

						// add score
						Score = Score + 1;
					}

				}
			}

			// clear the window with black color
			window.clear(sf::Color::Black);

			// draw everything here...
			// window.draw(...);

			// go through brick array
			for (int i = 0; i < 8; ++i)
			{
				if (collidable[i])
				{
					// draw non-destroyed bricks
					window.draw(Brick[i]);
				}
			}

			// go through brick array
			for (int i = 0; i < 8; ++i)
			{
				if (collidable2[i])
				{
					// draw non-destroyed bricks
					window.draw(Brick2[i]);
				}
			}

			// go through brick array
			for (int i = 0; i < 8; ++i)
			{
				if (collidable3[i])
				{
					// draw non-destroyed bricks
					window.draw(Brick3[i]);
				}
			}

			// set lives text value
			LivesText.setString("Lives : " + std::to_string(lives));

			// draw objects on window
			window.draw(ScoreText);
			window.draw(LivesText);
			window.draw(Ball);
			window.draw(Paddle);

			// end the current frame
			window.display();

		}

		// if win condition reached
		else if (Win == 24 && lives > 0)
		{
			// clear the screen
			window.clear(sf::Color::Black);

			// setup text
			WinText.setPosition(ScreenWidth * 0.5 - WinText.getLocalBounds().width / 2, ScreenHeight * 0.5 - WinText.getLocalBounds().height);
			EndScoreText.setPosition(ScreenWidth * 0.5 - EndScoreText.getLocalBounds().width / 2, ScreenHeight * 0.55);
			EndScoreText.setString("Score : " + std::to_string(Score));

			// display text
			window.draw(WinText);
			window.draw(EndScoreText);
			window.display();
		}

		// if lose condition reached
		else if (lives == 0)
		{
			// clear the screen
			window.clear(sf::Color::Black);

			// setup text
			LoseText.setPosition(ScreenWidth * 0.5 - LoseText.getLocalBounds().width / 2, ScreenHeight * 0.5 - LoseText.getLocalBounds().height);
			EndScoreText.setPosition(ScreenWidth * 0.5 - EndScoreText.getLocalBounds().width / 2, ScreenHeight * 0.55);
			EndScoreText.setString("Score : " + std::to_string(Score));

			// display text
			window.draw(LoseText);
			window.draw(EndScoreText);
			window.display();
		}
	}

	return 0;
}