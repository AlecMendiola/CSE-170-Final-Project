#pragma once

# include <sigogl/ui_button.h>
# include <sigogl/ws_viewer.h>

# include <raylib.h>

class GameViewer : public WsViewer {
// add any private variables in here 
	bool firstPlay, gameRunning, pause, gameOver; // game state stuff 
	bool up, down, left, right; // directional stuff 
	float xVel, yVel; // ship stuff 
	int score;
	// raylib stuff (for music)
	Sound fx, restart, zoom;
	Music bgm;
protected: // protected variables in here
	// consider importing the MyViewer variables into here 
public: // public in here 
	GameViewer(int x, int y, int w, int h, const char* l); // constructor
	virtual ~GameViewer(); // destructor
	void init_scene(); // Alec's methods all bunched up into one
	//void build_ui(); // may need this 
	void game_loop(); // main game loop
	void game_over();
	virtual int handle_keyboard(const GsEvent& e) override;
	virtual int handle_key_release(const GsEvent& e) override;
	//virtual int uievent(int e) override; // may need this 
};