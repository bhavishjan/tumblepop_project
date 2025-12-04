#include <iostream>
#include <fstream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>

using namespace sf;
using namespace std;

int screen_x = 1134;
int screen_y = 896;

void display_level(RenderWindow &window, char **lvl, Texture &bgTex, Sprite &bgSprite, Texture &blockTexture, Sprite &blockSprite, const int height, const int width, const int cell_size)
{
	window.draw(bgSprite);

	for (int i = 0; i < height; i += 1)
	{
		for (int j = 0; j < width; j += 1)
		{

			if (lvl[i][j] == '#')
			{
				blockSprite.setPosition(j * cell_size, i * cell_size);
				window.draw(blockSprite);
			}
		}
	}
}

void player_gravity(char **lvl, float &offset_y, float &velocityY, bool &onGround,
                    const float &gravity, float &terminal_Velocity,
                    float &player_x, float &player_y,
                    const int cell_size, int &Pheight, int &Pwidth)
{
	// enforce top bound
    if (player_y < 0) 
	{
        player_y = 0;
        velocityY = 0;
    }
    offset_y = player_y + velocityY;

    // Horizontal range
    int leftCell  = (int)(player_x) / cell_size;
    int rightCell = (int)(player_x + Pwidth - 1) / cell_size;

    // Bottom cell only (feet)
    int bottomCell = (int)(offset_y + Pheight) / cell_size;

    // Check if feet are landing on a platform
    onGround = false;
    for (int x = leftCell; x <= rightCell; x++) {
        if (lvl[bottomCell][x] == '#') {
            // Only snap if feet are touching top of platform
            if (player_y + Pheight <= bottomCell * cell_size) {
                offset_y = bottomCell * cell_size - Pheight;
                velocityY = 0;
                onGround = true;
            }
            break;
        }
    }

    if (!onGround) {
        velocityY += gravity;
        if (velocityY > terminal_Velocity)
            velocityY = terminal_Velocity;
    }

    player_y = offset_y;
}



int main()
{

	RenderWindow window(VideoMode(screen_x, screen_y), "GAME", Style::Default);
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);

	// level specifics
	const int cell_size = 64;
	const int height = 14;
	const int width = 18;
	char **lvl;

	// level and background textures and sprites
	Texture bgTex;
	Sprite bgSprite;
	Texture blockTexture;
	Sprite blockSprite;

	bgTex.loadFromFile("Data/bg.png");
	bgSprite.setTexture(bgTex);
	bgSprite.setPosition(0, 0);

	blockTexture.loadFromFile("Data/block1.png");
	blockSprite.setTexture(blockTexture);

	// player data
	float player_x = 500;
	float player_y = 150;

	float speed = 5;

	const float jumpStrength = -20; // Initial jump velocity
	const float gravity = 1;		// Gravity acceleration

	bool isJumping = false; // Track if jumping

	bool up_collide = false;
	bool left_collide = false;
	bool right_collide = false;

	Texture PlayerTexture;
	Sprite PlayerSprite;

	bool onGround = false;

	float offset_x = 0;
	float offset_y = 0;
	float velocityY = 0;

	float terminal_Velocity = 20;

	int PlayerHeight = 117;
	int PlayerWidth = 93;

	bool up_button = false;

	char top_left = '\0';
	char top_right = '\0';
	char top_mid = '\0';

	char left_mid = '\0';
	char right_mid = '\0';

	char bottom_left = '\0';
	char bottom_right = '\0';
	char bottom_mid = '\0';

	char bottom_left_down = '\0';
	char bottom_right_down = '\0';
	char bottom_mid_down = '\0';

	char top_right_up = '\0';
	char top_mid_up = '\0';
	char top_left_up = '\0';

	PlayerTexture.loadFromFile("Data/player1_walk1.png");
	PlayerSprite.setTexture(PlayerTexture);
	PlayerSprite.setScale(3, 3);
	PlayerSprite.setPosition(player_x, player_y);

	//creating level array
	lvl = new char* [height];
	for (int i = 0; i < height; i += 1)
	{
		lvl[i] = new char[width];
		for (int j = 0; j < width; ++j)
        	lvl[i][j] = '.';   // empty cell
	}

	// Ground
	for (int j = 0; j < width; j++)
    	lvl[13][j] = '#';
	
	
	
	// Left Down Platform
	lvl[10][1] = '#';
	lvl[10][2] = '#';
	lvl[10][3] = '#';

	// Right Down Platform
	lvl[10][14] = '#';
	lvl[10][15] = '#';
	lvl[10][16] = '#';

	// Platform
	lvl[7][1] = '#';
	lvl[7][2] = '#';
	lvl[7][3] = '#';

	//Platform
	lvl[7][14] = '#';
	lvl[7][15] = '#';
	lvl[7][16] = '#';

	//Platform
	lvl[4][1] = '#';
	lvl[4][2] = '#';
	lvl[4][3] = '#';

	//Platform
	lvl[4][14] = '#';
	lvl[4][15] = '#';
	lvl[4][16] = '#';

	// Mid Top Platform
	lvl[3][7] = '#';
	lvl[3][8] = '#';
	lvl[3][9] = '#';
	lvl[3][10] = '#';

	// Mid Mid Platform
	lvl[6][7] = '#';
	lvl[6][8] = '#';
	lvl[6][9] = '#';
	lvl[6][10] = '#';

	// Mid Down Platform
	lvl[9][7] = '#';
	lvl[9][8] = '#';
	lvl[9][9] = '#';
	lvl[9][10] = '#';
	
	
	// Music initialisation
	Music lvlMusic;

	lvlMusic.openFromFile("Data/mus.ogg");
	lvlMusic.setVolume(20);
	lvlMusic.play();
	lvlMusic.setLoop(true);

	// Intro screen
	Texture introTexture;
	introTexture.loadFromFile("Data/intro.jpg");
	Sprite introSprite;
	introSprite.setTexture(introTexture);

	// Start button
	Texture startTex;
	startTex.loadFromFile("Data/start_button.png");
	Sprite startSprite;
	startSprite.setTexture(startTex);
	startSprite.setScale(0.5, 0.5);
	startSprite.setPosition(screen_x / 2 - 120, screen_y / 2 + 300);

	// New screen texture
	Texture newScreenTex;
	newScreenTex.loadFromFile("Data/newscreen.png");
	Sprite newScreenSprite;
	newScreenSprite.setTexture(newScreenTex);

	Texture player1_walk1, player1_walk2;
	Texture player2_walk1, player2_walk2;

	player1_walk1.loadFromFile("Data/player1_walk1.png");
	player1_walk2.loadFromFile("Data/player1_walk2.png");
	player2_walk1.loadFromFile("Data/player2_walk1.png");
	player2_walk2.loadFromFile("Data/player2_walk2.png");

	Texture* walk1;
	Texture* walk2;

	// Player1
	Texture player1tex;
	player1tex.loadFromFile("Data/player1_walk1.png");
	Sprite player1sprite;
	player1sprite.setTexture(player1tex);
	player1sprite.setScale(4, 4);
	player1sprite.setPosition(300, 300);

	// Player2
	Texture player2tex;
	player2tex.loadFromFile("Data/player2_walk1.png");
	Sprite player2sprite;
	player2sprite.setTexture(player2tex);
	player2sprite.setScale(4, 4);
	player2sprite.setPosition(700, 300);

	// press1
	Texture press1tex;
	press1tex.loadFromFile("Data/press1.png");
	Sprite press1sprite;
	press1sprite.setTexture(press1tex);
	press1sprite.setScale(0.5, 0.5);
	press1sprite.setPosition(270, 450);

	// press2
	Texture press2tex;
	press2tex.loadFromFile("Data/press2.png");
	Sprite press2sprite;
	press2sprite.setTexture(press2tex);
	press2sprite.setScale(0.5, 0.5);
	press2sprite.setPosition(660, 450);

	// State flag
	bool onIntro = true;
	bool player_select = false;

	//Intro Loop
		while (window.isOpen())
	{
		Event ev;
		while (window.pollEvent(ev))
		{
			if (ev.type == Event::Closed)
				window.close();

			if (ev.type == Event::KeyPressed)
			{
				if (ev.key.code == Keyboard::Escape)
					window.close();

				if (onIntro && ev.key.code == Keyboard::Enter)
					onIntro = false;
				if (!onIntro && ev.key.code == Keyboard::Num1)
				{
					player_select = true;
					walk1 = &player1_walk1;
    				walk2 = &player1_walk2;
					PlayerSprite.setTexture(*walk1);
				}
				if (!onIntro && ev.key.code == Keyboard::Num2)
				{
					player_select = true;
					walk1 = &player2_walk1;
    				walk2 = &player2_walk2;
					PlayerSprite.setTexture(*walk1);
				}
			}
			
		}
		
		window.clear();
		if (onIntro)
		{
			window.draw(introSprite);
			window.draw(startSprite);
		}
		else
		{
			window.draw(newScreenSprite);
			window.draw(player1sprite);
			window.draw(player2sprite);
			window.draw(press1sprite);
			window.draw(press2sprite);
		}
		window.display();
		if (player_select)
				break;
	}

	Event ev;
	// Main loop
	while (window.isOpen())
	{
		static int walkCounter = 0;
		const int walkSpeed = 10;
		Event ev;
		while (window.pollEvent(ev))
		{
			if (ev.type == Event::Closed)
				window.close();
		}

		// Smooth movement and jump
		if (Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Left)) // move left
		{
			player_x -= 5;
			if (player_x < 0) // left bound
				player_x = 0;  
		}
		if (Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Right)) // move right
		{
			player_x += 5;
			if (player_x + PlayerWidth > screen_x)  // right bound
        		player_x = screen_x - PlayerWidth;
		}

		if ((Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::Space)) && onGround) // jump
		{
			velocityY = -20;
			onGround = false;
		}

		if (Keyboard::isKeyPressed(Keyboard::Escape)) // close
			window.close();
		
		if (Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Left) ||
			Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Right))
		{
			walkCounter++;
			if (walkCounter / walkSpeed % 2 == 0)
				PlayerSprite.setTexture(*walk1);
			else
				PlayerSprite.setTexture(*walk2);
		}
		else
		{
			walkCounter = 0;
			PlayerSprite.setTexture(*walk1); // idle frame
		}

		// Clear and draw
		window.clear();
		display_level(window, lvl, bgTex, bgSprite, blockTexture, blockSprite, height, width, cell_size);
		player_gravity(lvl, offset_y, velocityY, onGround, gravity, terminal_Velocity, player_x, player_y, cell_size, PlayerHeight, PlayerWidth);
		PlayerSprite.setPosition(player_x, player_y);
		window.draw(PlayerSprite);
		window.display();
	}

	// stopping music and deleting level array
	lvlMusic.stop();
	for (int i = 0; i < height; i++)
	{
		delete[] lvl[i];
	}
	delete[] lvl;
	return 0;
}
