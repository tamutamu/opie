#ifndef __CONSTANTS_H
#define __CONSTANTS_H

#include <string>
using namespace std;
#define QString string

#ifdef QWS
#define IMAGES_PATH "/opt/QtPalmtop/pics/sfcave/data/"
#define SOUND_PATH "/opt/QtPalmtop/sounds/sfcave/"
#else
#define IMAGES_PATH "./images/"
#define SOUND_PATH "./sounds/"
#endif

// Width and height of app
#define WIDTH	240
#define HEIGHT	320

//Number of map segments
#define MAPSIZE 51

// Maximum number of blocks visible on screen at any one time
#define BLOCKSIZE 6

// length of players trail
#define TRAILSIZE 60

// Game States
#define STATE_QUIT			-1
#define STATE_PLAYING 		0
#define STATE_CRASHING 		1
#define STATE_CRASHED		2
#define STATE_NEWGAME		3
#define STATE_MENU			4
#define STATE_REPLAY		5
#define STATE_HELP          6

// Menu Options
#define MENU_STARTGAME 		1
#define MENU_REPLAYS 		2
#define MENU_OPTIONS 		3
#define MENU_HELP 			4
#define MENU_QUIT 			5
#define MENU_PLAY_REPLAY 	6
#define MENU_LOAD_REPLAY 	7
#define MENU_SAVE_REPLAY 	8
#define MENU_BACK 			9
#define MENU_GAME_TYPE 		10
#define MENU_DIFFICULTY 	11
#define MENU_CLEAR_SCORES 	12
#define MENU_GAME_SFCAVE	13
#define MENU_GAME_GATES		14
#define MENU_GAME_FLY		15
#define MENU_DIFFICULTY_EASY	16
#define MENU_DIFFICULTY_NORMAL	17
#define MENU_DIFFICULTY_HARD	18
#define MENU_SOUNDS				19
#define MENU_SOUND_ON			20
#define MENU_SOUND_OFF			21
#define MENU_MUSIC_ON			22
#define MENU_MUSIC_OFF			23

// Sounds
#define SND_EXPLOSION		0
#define SND_THRUST			1
#endif
