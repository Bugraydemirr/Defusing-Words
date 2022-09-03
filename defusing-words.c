#include <stdio.h> // standart input output
#include <stdlib.h> // standart libraries
#include <string.h> // string library
#include <time.h> // time calculations
#include <stdbool.h> // boolean usage
#include <windows.h> // windows Sleep
#include <conio.h> // key handling

// painting definitions
#define SCREEN_WIDTH 62 // game screen width
#define SCREEN_HEIGHT 40 // game screen height

// global defined deactivation codes
char** deactivation_codes;

// game structure
typedef struct game_s
{
	// state:: 1: game running, 2: game paused, 3: game over
	int state;

	// plane x positon
	int plane_pos;

	// bomb x positon
	int bomb_pos_x;
	// bomb y positon
	int bomb_pos_y;

	// count of correctly typed letters
	int correct_letter_count;
	// count of missed letters
	int missed_letter_count;
	// total score
	int score;

	// index of current deactivation code of global defined deactivation codes
	int code_index;

	// currently showed code chars
	char current_code[10 + 1];

	// started time of game
	clock_t start_time;

	// is any game component changed? so render game again
	bool updated;
} Game;

// global define current playing game
Game* currentGame;

// return if file opened and saved succesfully
bool SaveGame(char* filename)
{
	// open file and write global defined currentGame object into file
	FILE* fp;

	fp = fopen(filename, "wb+");

	if (fp == NULL)
		return false;

	fwrite(currentGame, sizeof(Game), 1, fp);

	fclose(fp);

	return true;
}
// return if file opened and loaded succesfully
bool LoadGame(char* filename)
{
	// open file and read bytes into global defined currentGame object

	FILE* fp;

	fp = fopen(filename, "rb");

	if (fp == NULL)
		return false;

	fread(currentGame, sizeof(Game), 1, fp);

	fclose(fp);

	return true;
}

// show (print) load game stuffs
void ShowLoadGame()
{
	// get load game filename and try to open and load it, return loading succes
	// after loading, wait for 3 seconds and run the game
	system("CLS");
	printf("Enter load currentGame filename: ");
	char fileName[30];
	scanf("%s", fileName);
	if (LoadGame(fileName) == false)
	{
		printf("File not found!\n");
		exit(1);
	}
	printf("Game loaded.\n");
	printf("Game will continue in 3 seconds.\n");
	Sleep(3000);
}
// show (print) save game stuffs
void ShowSaveGame()
{
	// get save game filename and try to open and save it, return saving succes
	// after saving, wait for 3 seconds and run the game
	system("CLS");
	printf("Enter save currentGame filename: ");
	char fileName[30];
	scanf("%s", fileName);
	if (SaveGame(fileName))
	{
		printf("Game saved.\n");
	}
	else
	{
		printf("Game cannot saved.\n");
	}
	printf("Game will continue in 3 seconds.\n");
	Sleep(3000);
}

// delete current game and create new one
void CreateNewGame()
{
	// reinitialize values of structure
	currentGame->state = 1;

	currentGame->plane_pos = 0;
	currentGame->bomb_pos_x = 0;
	currentGame->bomb_pos_y = 0;

	currentGame->correct_letter_count = 0;
	currentGame->missed_letter_count = 0;
	currentGame->score = 0;

	currentGame->code_index = 0;

	currentGame->start_time = 0;

	// fill the deactivation_code value of struct with global value
	strcpy(currentGame->current_code, deactivation_codes[currentGame->code_index]);
}
// just exit the whole program
void ExitGame()
{
	exit(0);
}
// set game state into state_running and set start time with current clock and finally set updated into true for render game
void ContinueGame()
{
	currentGame->state = 1;
	currentGame->updated = true;
	currentGame->start_time = clock();
}
// set game state into state_paused and set updated into true for render pause menu
void PauseGame()
{
	currentGame->state = 2;
	currentGame->updated = true;
}
// set game state into state_gameover and set updated into true for render game over menu
void GameOver()
{
	currentGame->state = 3;
	currentGame->updated = true;
}

// print first line of plane ascii
void RenderPlane1()
{
	int i;
	for (i = 0; i < currentGame->plane_pos; ++i)
		printf(" ");
	printf("(|__.-. |");
}
// print other lines of plane ascii
void RenderPlane2()
{
	int i;
	for (i = 0; i < currentGame->plane_pos; ++i)
		printf(" ");
	printf("== ===_]+\n");
	for (i = 0; i < currentGame->plane_pos; ++i)
		printf(" ");

	printf("        |\n");
}
// print score side
void RenderScore()
{
	int i;
	for (i = 0; i < SCREEN_WIDTH - (currentGame->plane_pos + 10); ++i)
		printf(" ");
	printf("Score: %d\n", currentGame->score);
}
// print bomb into bomb position
void RenderBomb()
{
	int i;
	for (i = 0; i < currentGame->bomb_pos_x; ++i)
		printf(" ");
	printf("<|>\n");

	for (i = 0; i < currentGame->bomb_pos_x; ++i)
		printf(" ");
	printf(" |%s\n", currentGame->current_code);

	for (i = 0; i < currentGame->bomb_pos_x; ++i)
		printf(" ");
	printf(" *\n");
}
// print city side
void RenderCity()
{
	printf(
		" _____       ____      _______________   _______     ________\n"
		"|o o o|_____|    |____| []  []  []  []| | # # # |____|o o o o|\n"
		"|o o o|* * *|: ::|. . | []  []  []  []|o| # # # |. . |o o o o|\n"
		"|o o o|* * *|::  |. . | []  []  []  []|o| # # # |. . |o o o o|\n"
		"|o o o|**** |:  :| . .| []  []  []    |o| # # # |. . |o o o o|\n"
		"|_[]__|__[]_|_||_|__< |____________;;_|_|___[]__|_.|_|__[]___|"
	);
}
// print pause menu to middle of screen
void RenderMenu()
{
	static const int posx = 25;
	int i;
	for (i = 0; i < posx; ++i)
		printf(" ");
	printf("    Main Menu\n");

	for (i = 0; i < posx; ++i)
		printf(" ");
	printf("1. New Game\n");

	for (i = 0; i < posx; ++i)
		printf(" ");
	printf("2. Load Game\n");

	for (i = 0; i < posx; ++i)
		printf(" ");
	printf("3. Save Game\n");

	for (i = 0; i < posx; ++i)
		printf(" ");
	printf("4. Return Game (ESC)\n");

	for (i = 0; i < posx; ++i)
		printf(" ");
	printf("5. Quit Game\n");
}
// print game over menu to middle of screen
void RenderGameOverMenu()
{
	static const int posx = 25;
	int i;
	for (i = 0; i < posx; ++i)
		printf(" ");
	printf("    GAME OVER\n");

	for (i = 0; i < posx; ++i)
		printf(" ");
	printf("1. New Game\n");

	for (i = 0; i < posx; ++i)
		printf(" ");
	printf("2. Load Game\n");

	for (i = 0; i < posx; ++i)
		printf(" ");
	printf("3. Quit Game\n");
}

// render whole game
void RenderGame()
{
	// if we dont need to render game. Means no any game component changed, we dont need to render again
	if(!currentGame->updated)
		return;

	// firstly clear screen
	system("CLS");

	// render first line of plane
	RenderPlane1();
	// render score
	RenderScore();

	// render other sides of plane
	RenderPlane2();

	// defined for calculatings
	int i;


	if (currentGame->state == 1) // if game running render bomb
	{
		for (i = 0; i < currentGame->bomb_pos_y; ++i)
			printf("\n");
		RenderBomb();

		for (i = 0; i < SCREEN_HEIGHT - (3 + currentGame->bomb_pos_y + 3); ++i)
			printf("\n");
	}
	else if (currentGame->state == 2) // else if game paused, do not render bomb, render pause menu
	{
		for (i = 0; i < 15; ++i)
			printf("\n");
		RenderMenu();

		for (i = 0; i < SCREEN_HEIGHT - (6 + 15 + 3); ++i)
			printf("\n");
	}
	else if (currentGame->state == 3) // if game over, render menu
	{
		for (i = 0; i < 15; ++i)
			printf("\n");
		RenderGameOverMenu();

		for (i = 0; i < SCREEN_HEIGHT - (4 + 15 + 3); ++i)
			printf("\n");
	}

	// render city
	RenderCity();

	// set game updated into true
	currentGame->updated = false;
}
// update(refresh) game components it its time
void UpdateGame()
{
	clock_t ms_passed = clock() - currentGame->start_time; // calculate how much time passed from starting

	static const int plane_speed_frequency = 400; // once define for plane speed of frequency
	const int bomb_speed_frequency = 1000 - (currentGame->code_index / 5) * 150; // calculate bomb speed of frequency based on how many codewords completed
	//const int bomb_speed_frequency = 150;

	if (ms_passed % plane_speed_frequency == 0) // if its time to update plane
	{
		currentGame->updated = true; // set true for render game

		currentGame->plane_pos++; // increase the x position of plane
		if (currentGame->plane_pos == SCREEN_WIDTH - 10) // if its right side
			currentGame->plane_pos = 0; // move into left
	}

	if (ms_passed % bomb_speed_frequency == 0) // if its time to update bomb
	{
		currentGame->updated = true; // set true for render game

		currentGame->bomb_pos_y++; // increase the y position of bomb
		if (currentGame->bomb_pos_y > SCREEN_HEIGHT - 3 - 4)  // if its bottom side
			GameOver(); // gg
	}
}

// handle key during game running
void HandleKey(char key)
{
	if (currentGame->state == 2) // if game paused
	{
		if (key == 27 || key == '4') // if key equals ESC or 4(Return the game)
		{
			ContinueGame(); // continue game
		}
		else if (key == '1') // new game
		{
			CreateNewGame(); // create new game
		}
		else if (key == '2') // load game
		{
			ShowLoadGame(); // show load game window
			ContinueGame(); // and after continue new game
		}
		else if (key == '3') // save
		{
			ShowSaveGame(); // show save game window
			ContinueGame(); // and after continue game
		}
		else if (key == '5') // quit game
		{
			ExitGame();
		}
	}
	else if (currentGame->state == 1) // if running
	{
		if (key == 27) // if key equals ESC
		{
			PauseGame(); // pause game
		}
		else if (key == currentGame->current_code[0]) // else if key is correctly typed
		{
			currentGame->correct_letter_count++; // increase correctly typed count

			strcpy(currentGame->current_code, currentGame->current_code+1); // remove first char in current code

			if (strlen(currentGame->current_code) == 0) // if there is no more current code to type
			{
				currentGame->bomb_pos_x = currentGame->plane_pos + 2; // move bomb into middle of plane
				currentGame->bomb_pos_y = 0; // move bomb into top of the screen
				currentGame->code_index++; // increase code index of global variable
				strcpy(currentGame->current_code, deactivation_codes[currentGame->code_index]); // refresh current code based on global variable

				currentGame->score += currentGame->correct_letter_count - currentGame->missed_letter_count; // increase the score

				currentGame->correct_letter_count = currentGame->missed_letter_count = 0; // reset values
			}

			currentGame->updated = true; // game needs to be render
		}
		else // if key is missing typed
		{
			currentGame->missed_letter_count++; // just increase count of missed letters
		}
	}
	else if (currentGame->state == 3) // if game over
	{
		if (key == 27 || key == '3') // if key equals ESC or 3(Exit Game)
		{
			ExitGame();
		}
		else if (key == '1') // new game
		{
			CreateNewGame();
		}
		else if (key == '2') // load game
		{
			ShowLoadGame();
			ContinueGame();
		}
	}
}

// once read deactivation codes from given filename
bool ReadDeactivationCodes(const char* filename)
{
	char codeWord[10 + 1]; // buffer for code
	int line_index = 0; // index of line
	FILE* fp;

	fp = fopen(filename, "r");

	if (fp == NULL)
		return false;

	// just calculate the count of lines
	while (fscanf(fp, "%s", codeWord) == 1)
	{
		line_index++;
	}
	rewind(fp);

	// allocate necessary size from memory
	deactivation_codes = (char**) malloc(line_index * sizeof(char*));

	line_index = 0;
	while (fscanf(fp, "%s", codeWord) == 1)
	{
		// allocate necessary size from memory for each keyword
		deactivation_codes[line_index] = (char*) malloc((10 + 1) * sizeof(char));
		// copy from buffer to global variable
		strcpy(deactivation_codes[line_index], codeWord);
		line_index++;
	}

	fclose(fp);

	return true;
}
// shows main menu
int ShowMenu()
{
	printf("    Main Menu\n");
	printf("1. New Game\n");
	printf("2. Load Game\n");
	printf("3. Quit Game\n");

	int res;
	scanf("%d", &res);

	return res;
}

// main function to start program
int main(int argc, char const* argv[])
{
	// if we cant read deactivation code, just exit from program
	if (!ReadDeactivationCodes("deactivation_codes.txt"))
	{
		printf("Deactivation codes cannot read!\n");
		return 1;
	}

	currentGame = (Game*)malloc(sizeof(Game));

	// show menu and get users choise
	int res = ShowMenu();

	// do stuff depend on user choise
	switch (res)
	{
	case 1:
		CreateNewGame();
		break;
	case 2:
		ShowLoadGame();
		ContinueGame();
		break;
	case 3:
		return 0;
		break;
	default:
		return 1;
	}

	// continually
	while (true)
	{
		// check for keyboard type and if its, send into function to make stuff with this key
		while (_kbhit())
		{
			HandleKey(_getch());
		}
		// if game state is running, update and after render the game
		if (currentGame->state == 1)
		{
			UpdateGame();
			RenderGame();
		}
		// if game state is paused, just render game once (for menu)
		else if (currentGame->state == 2)
		{
			RenderGame();
		}
		// if game state is over, just render game once (for menu)
		else if (currentGame->state == 3)
		{
			RenderGame();
		}
	}

	return 0;
}
