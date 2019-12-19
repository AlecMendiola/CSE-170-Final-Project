# include "game_viewer.h"

//# include <sigogl/ui_button.h>
//# include <sigogl/ui_radio_button.h> // may not need these if we dont have a UI
# include <sig/sn_primitive.h>
# include <sig/sn_transform.h>
//# include <sig/gs_camera.h>
# include <sigogl/ws_run.h>

# include <raylib.h> // for music 

# include <iostream>
# include <string>
# include <sstream>
# include <fstream> // writing/outputting high scores

# define POSBOUNDARYX  100.0f
# define POSBOUNDARYY  100.0f
# define POSBOUNDARYZ  100.0f

# define NEGBOUNDARYX  -100.0f
# define NEGBOUNDARYY  -100.0f
# define NEGBOUNDARYZ  -100.0f

GameViewer::GameViewer(int x, int y, int w, int h, const char*l) : WsViewer(x, y, w, h, l) {
	init_scene();
	score = 0;
	firstPlay = true;
	gameRunning = gameOver = false;
	pause = false;
	
	up = down = left = right = false;
	xVel = yVel = 0.0f;
	
	InitAudioDevice();
	bgm = LoadMusicStream("../src/assets/sounds/music/spaceboops.ogg");
	fx = LoadSound("../src/assets/sounds/fx/crash.wav");
	restart = LoadSound("../src/assets/sounds/fx/restart.wav");
	zoom = LoadSound("../src/assets/sounds/fx/zoom.wav");


}

GameViewer::~GameViewer() { // destructor to handle resource cleanup
	UnloadSound(fx);
	UnloadMusicStream(bgm);
	rootg()->remove_all();
}

void GameViewer::init_scene() { // mega function
	// BEGINNING OF ALEC'S SHIT 

	// createTitle()
	SnModel* titleModel = new SnModel;
	titleModel->model()->load("../src/assets/Models/SkyboxTextures/titleRotBig.obj");
	GsVec frontAxis = GsVec(0.0f, 1.5f, 1.0f);
	titleModel->model()->rotate(GsQuat(frontAxis, float(GS_2PI / 2)));
	titleModel->model()->translate(GsVec(0.0f, 0.0f, POSBOUNDARYZ * 3.0f));
	rootg()->add(titleModel);

	// setting up the scene 

	SnGroup* scene = new SnGroup;
	scene->separator(true);
	rootg()->add(scene);

	// createSkybox()...

	// - setFront()
	SnModel* floor = new SnModel;
	floor->model()->N.size(4);
	floor->model()->V.push(GsPnt(POSBOUNDARYX, -1.0f, POSBOUNDARYZ));
	floor->model()->V.push(GsPnt(NEGBOUNDARYX, -1.0f, POSBOUNDARYZ));
	floor->model()->V.push(GsPnt(NEGBOUNDARYX, -1.0f, NEGBOUNDARYZ));
	floor->model()->V.push(GsPnt(POSBOUNDARYX, -1.0f, NEGBOUNDARYZ));
	floor->model()->F.push(GsModel::Face(0, 1, 3));
	floor->model()->F.push(GsModel::Face(2, 3, 1));
	frontAxis = GsVec(0.0f, 1.0f, 1.0f);
	floor->model()->rotate(GsQuat(frontAxis, (float)(GS_2PI / 2.0)));
	floor->model()->translate(GsVec(0.0f, 0.0f, POSBOUNDARYZ * 2.0f));
	scene->add(floor); // added object to scene graph
	GsModel::Group& textureGroup1 = *floor->model()->G.push();
	textureGroup1.fi = 0;
	textureGroup1.fn = floor->model()->F.size();
	textureGroup1.dmap = new GsModel::Texture;
	textureGroup1.dmap->fname.set("../src/assets/Models/SkyboxTextures/Starscape.png");
	floor->model()->M.push().init();
	floor->model()->M.top() = GsMaterial::DefaultSpecular;
	floor->model()->T.push(GsPnt2(POSBOUNDARYX, POSBOUNDARYZ));
	floor->model()->T.push(GsPnt2(NEGBOUNDARYX, NEGBOUNDARYZ));
	floor->model()->T.push(GsPnt2(NEGBOUNDARYX, POSBOUNDARYZ));
	floor->model()->T.push(GsPnt2(POSBOUNDARYX, NEGBOUNDARYZ));
	floor->model()->set_mode(GsModel::Smooth, GsModel::PerGroupMtl);
	floor->model()->textured = true;

	// - setCeiling()
	SnModel* ceiling = new SnModel;
	ceiling->model()->N.size(4);
	ceiling->model()->V.push(GsPnt(POSBOUNDARYX, -1.0f, POSBOUNDARYZ));
	ceiling->model()->V.push(GsPnt(NEGBOUNDARYX, -1.0f, POSBOUNDARYZ));
	ceiling->model()->V.push(GsPnt(NEGBOUNDARYX, -1.0f, NEGBOUNDARYZ));
	ceiling->model()->V.push(GsPnt(POSBOUNDARYX, -1.0f, NEGBOUNDARYZ));
	ceiling->model()->F.push(GsModel::Face(0, 1, 3));
	ceiling->model()->F.push(GsModel::Face(2, 3, 1));
	frontAxis = GsVec(0.0f, 0.0f, -1.0f);
	ceiling->model()->rotate(GsQuat(frontAxis, (float)(GS_2PI / 2.0)));
	ceiling->model()->translate(GsVec(0.0f, POSBOUNDARYY - 1.0f, POSBOUNDARYZ));
	ceiling->model()->invert_faces();
	scene->add(ceiling); // added object to scene graph
	GsModel::Group& textureGroup2 = *ceiling->model()->G.push();
	textureGroup2.fi = 0;
	textureGroup2.fn = ceiling->model()->F.size();
	textureGroup2.dmap = new GsModel::Texture;
	textureGroup2.dmap->fname.set("../src/assets/Models/SkyboxTextures/Starscape.png");
	ceiling->model()->M.push().init();
	ceiling->model()->M.top() = GsMaterial::DefaultSpecular;
	ceiling->model()->T.push(GsPnt2(POSBOUNDARYX, POSBOUNDARYZ));
	ceiling->model()->T.push(GsPnt2(NEGBOUNDARYX, NEGBOUNDARYZ));
	ceiling->model()->T.push(GsPnt2(NEGBOUNDARYX, POSBOUNDARYZ));
	ceiling->model()->T.push(GsPnt2(POSBOUNDARYX, NEGBOUNDARYZ));
	ceiling->model()->set_mode(GsModel::Smooth, GsModel::PerGroupMtl);
	ceiling->model()->textured = true;

	// - setWallR()
	SnModel* wallR = new SnModel;
	wallR->model()->N.size(4);
	wallR->model()->V.push(GsPnt(1.0f, NEGBOUNDARYY, POSBOUNDARYZ));
	wallR->model()->V.push(GsPnt(1.0f, NEGBOUNDARYY, NEGBOUNDARYZ));
	wallR->model()->V.push(GsPnt(1.0f, POSBOUNDARYY, POSBOUNDARYZ));
	wallR->model()->V.push(GsPnt(1.0f, POSBOUNDARYY, NEGBOUNDARYZ));
	wallR->model()->F.push(GsModel::Face(3, 1, 0));
	wallR->model()->F.push(GsModel::Face(0, 2, 3));
	frontAxis = GsVec(1.0f, 1.0f, 0.0f);
	//wallR->model()->rotate(GsQuat(frontAxis, (float)(GS_2PI / 2.0)));
	wallR->model()->translate(GsVec(POSBOUNDARYX - 1.0f, 0.0f, POSBOUNDARYZ));
	//wallR->model()->invert_faces();
	scene->add(wallR); // added object to scene graph
	GsModel::Group& textureGroup3 = *wallR->model()->G.push();
	textureGroup3.fi = 0;
	textureGroup3.fn = wallR->model()->F.size();
	textureGroup3.dmap = new GsModel::Texture;
	textureGroup3.dmap->fname.set("../src/assets/Models/SkyboxTextures/Starscape.png");
	wallR->model()->M.push().init();
	wallR->model()->M.top() = GsMaterial::DefaultSpecular;
	wallR->model()->T.push(GsPnt2(POSBOUNDARYX, POSBOUNDARYZ));
	wallR->model()->T.push(GsPnt2(NEGBOUNDARYX, NEGBOUNDARYZ));
	wallR->model()->T.push(GsPnt2(NEGBOUNDARYX, POSBOUNDARYZ));
	wallR->model()->T.push(GsPnt2(POSBOUNDARYX, NEGBOUNDARYZ));
	wallR->model()->set_mode(GsModel::Smooth, GsModel::PerGroupMtl);
	wallR->model()->textured = true;

	// - setWallL()
	SnModel* wallL = new SnModel;
	wallL->model()->N.size(4);
	wallL->model()->V.push(GsPnt(1.0f, NEGBOUNDARYY, POSBOUNDARYZ));
	wallL->model()->V.push(GsPnt(1.0f, NEGBOUNDARYY, NEGBOUNDARYZ));
	wallL->model()->V.push(GsPnt(1.0f, POSBOUNDARYY, POSBOUNDARYZ));
	wallL->model()->V.push(GsPnt(1.0f, POSBOUNDARYY, NEGBOUNDARYZ));
	wallL->model()->F.push(GsModel::Face(3, 1, 0));
	wallL->model()->F.push(GsModel::Face(0, 2, 3));
	frontAxis = GsVec(1.0f, 1.0f, 0.0f);
	//wallL->model()->rotate(GsQuat(frontAxis, (float)(GS_2PI / 2.0)));
	wallL->model()->invert_faces();
	wallL->model()->translate(GsVec(NEGBOUNDARYX + 1.0f, 0.0f, POSBOUNDARYZ));
	scene->add(wallL); // added object to scene graph
	GsModel::Group& textureGroup5 = *wallL->model()->G.push();
	textureGroup5.fi = 0;
	textureGroup5.fn = wallL->model()->F.size();
	textureGroup5.dmap = new GsModel::Texture;
	textureGroup5.dmap->fname.set("../src/assets/Models/SkyboxTextures/Starscape.png");
	wallL->model()->M.push().init();
	wallL->model()->M.top() = GsMaterial::DefaultSpecular;
	wallL->model()->T.push(GsPnt2(POSBOUNDARYX, POSBOUNDARYZ));
	wallL->model()->T.push(GsPnt2(NEGBOUNDARYX, NEGBOUNDARYZ));
	wallL->model()->T.push(GsPnt2(NEGBOUNDARYX, POSBOUNDARYZ));
	wallL->model()->T.push(GsPnt2(POSBOUNDARYX, NEGBOUNDARYZ));
	wallL->model()->set_mode(GsModel::Smooth, GsModel::PerGroupMtl);
	wallL->model()->textured = true;

	// - setRealFloor()
	//SnModel* realFloor = new SnModel;
	//realFloor->model()->N.size(4);
	//realFloor->model()->V.push(GsPnt(POSBOUNDARYX, -1.0f, POSBOUNDARYZ));
	//realFloor->model()->V.push(GsPnt(NEGBOUNDARYX, -1.0f, POSBOUNDARYZ));
	//realFloor->model()->V.push(GsPnt(NEGBOUNDARYX, -1.0f, NEGBOUNDARYZ));
	//realFloor->model()->V.push(GsPnt(POSBOUNDARYX, -1.0f, NEGBOUNDARYZ));
	//realFloor->model()->F.push(GsModel::Face(0, 1, 3));
	//realFloor->model()->F.push(GsModel::Face(2, 3, 1));
	//frontAxis = GsVec(0.0f, 0.0f, 1.0f);
	//realFloor->model()->rotate(GsQuat(frontAxis, (float)(GS_2PI / 2.0)));
	//realFloor->model()->translate(GsVec(0.0f, NEGBOUNDARYY, POSBOUNDARYZ));
	//scene->add(realFloor); // added object to scene graph
	//GsModel::Group& textureGroup4 = *realFloor->model()->G.push();
	//textureGroup4.fi = 0;
	//textureGroup4.fn = realFloor->model()->F.size();
	//textureGroup4.dmap = new GsModel::Texture;
	//textureGroup4.dmap->fname.set("../src/assets/Models/SkyboxTextures/Starscape.png");
	//realFloor->model()->M.push().init();
	//realFloor->model()->M.top() = GsMaterial::DefaultSpecular;
	//realFloor->model()->T.push(GsPnt2(POSBOUNDARYX, POSBOUNDARYZ));
	//realFloor->model()->T.push(GsPnt2(NEGBOUNDARYX, NEGBOUNDARYZ));
	//realFloor->model()->T.push(GsPnt2(NEGBOUNDARYX, POSBOUNDARYZ));
	//realFloor->model()->T.push(GsPnt2(POSBOUNDARYX, NEGBOUNDARYZ));
	//realFloor->model()->set_mode(GsModel::Smooth, GsModel::PerGroupMtl);
	//realFloor->model()->textured = true;

	// ENDING OF ALEC'S SHIT
}

void GameViewer::game_loop() {
	if (gameRunning) { gsout << "Game is already running!" << gsnl; return; }
	gsout << "Starting game loop..." << gsnl;
	gameRunning = true;
	gameOver = false;
	pause = false;

	// initialize the game's parameters

	float xBoundary = 30.0f;
	float yBoundary = 30.0f;

	up = down = left = right = false;
	xVel = yVel = 0.0f;


	// addShip()
	SnModel* shipModel = new SnModel;
	shipModel->model()->load("../src/assets/Models/ShipModels/ThirdPerson/Custom Edited - Star Fox Customs - Arwing SNES HighPoly/SNES Arwing - High poly/Snes_Arwing.obj");
	rootg()->add(shipModel);
	shipModel->model()->translate(GsPnt(0.0f, 0.0f, 0.0f));
	GsBox* shipBoundingBox = new GsBox;

	// asteroids
	int asteroidCount = 0;
	double asteroidInterval = 5.0, asteroidSpeed = -1.0, speedIncrement = -1.0;
	float xVariance = 10.0f, yVariance = 5.0f, varianceDecrement = 0.5f;
	SnGroup* asteroids = new SnGroup;
	GsBox& asteroidBox = GsBox();
	rootg()->add(asteroids); // gonna keep a list of asteroids in the game 

	StopMusicStream(bgm);
	PlayMusicStream(bgm); // play the music 

	//gsout << "Highest Score: ";
	//std::fstream file;
	//file.open("high_scores.txt");
	//if (!file) {
	//	gsout << "ERROR" << gsnl;
	//}
	//else {
	//	std::string temp;
	//	std::stringstream ss;
	//	std::string name, hScore;
	//	file >> temp;
	//	std::getline(file, temp);
	//	ss << temp;
	//	std::cout << temp << std::endl;
	//	ss >> name >> hScore;
	//	//std::cout << name << " " << hScore << std::endl;
	//}


	//file >> temp;


	double frdt = 1.0 / 30.0;
	float increment = 0.03f, decrement = 0.95f, tilt = 0.0f;
	double t = 0.0, lt = 0.0, t0 = gs_time(), asteroidTimer = 0.0; 
	do {
		while (t - lt < frdt) { ws_check(); t = gs_time() - t0; } // allow frame time to pass
		// game logic updates in here

		// get the ship's coordinates and what not 
		shipModel->model()->get_bounding_box(*shipBoundingBox);

		// check for game loss

		// when you think about it... the asteroid at the front of the scene is the oldest asteroid 
		// so really we only need to check if the first asteroid in the scene is past the camera 
		if (asteroids->size() > 0) {
			for (int i = 0; i < asteroids->size(); i++) { // actually first check if you've lost 
				asteroids->get<SnModel>(0)->model()->get_bounding_box(asteroidBox);
				if (asteroidBox.intersects(*shipBoundingBox)) {
					PlaySound(fx);
					gameRunning = false;
					break;
				}
			}
			asteroids->get<SnModel>(0)->model()->get_bounding_box(asteroidBox);
			if (asteroidBox.center().z < -30.0f) { asteroids->remove(0); }
		}

		// decide whether to add another asteroid to the scene
		if (t > asteroidTimer + asteroidInterval) { 
			asteroidSpeed += speedIncrement;
			if (asteroidSpeed < -20.0) asteroidSpeed = -20;
			asteroidCount++;
			//if (asteroidCount > 20) asteroidCount = 20;
			xVariance -= varianceDecrement;
			if (xVariance < 0.0f) xVariance = 0.0f;
			yVariance -= varianceDecrement;
			if (yVariance < 0.0f) yVariance = 0.0f;
			//gsout << "Total amount of asteroids: " << asteroidCount << gsnl;
			asteroidTimer = t;
		}
		if (asteroids->size() < asteroidCount) { // if total amount of asteroids in play is still not enough 
			if (gs_random(0, 100) == gs_random(0, 100)) { // random check if to spawn an asteroid 
				//gsout << "merp" << gsnl;
				SnModel* asteroid = new SnModel;
				asteroids->add(asteroid);
				asteroid->model()->load("../src/assets/Models/AsteroidModels/asteroidLowPoly/asteroidSmall.obj");
				//asteroid->model()->translate(GsVec(gs_random(-xBoundary, xBoundary),gs_random(-yBoundary, yBoundary), POSBOUNDARYZ*2));
				asteroid->model()->translate(GsVec(gs_random(shipBoundingBox->center().x - xVariance, shipBoundingBox->center().x + xVariance),gs_random(shipBoundingBox->center().y - yVariance, shipBoundingBox->center().y + yVariance), POSBOUNDARYZ*2));
			}
		}

		// moveShip()
		if (up) yVel += increment;
		if (down) yVel -= increment;
		if (left) { xVel += increment; /*if (tilt + increment > (float)(GS_PI / 4)) tilt = (float)(GS_PI / 4); else tilt += increment;*/ }
		if (right) xVel -= increment;
		if (!up && !down) yVel *= decrement;
		if (!left && !right) { xVel *= decrement; /*tilt *= decrement;*/}

		// cap the speeds
		if (yVel > 0.5f) yVel = 0.5f; else if (yVel < -0.5f) yVel = -0.5f;
		if (xVel > 0.5f) xVel = 0.5f; else if (xVel < -0.5f) xVel = -0.5f;



		// update music stream
		UpdateMusicStream(bgm);

		lt = t;
		// drawing updates in here

		for (int i = 0; i < asteroids->size(); i++) { // move each of the asteroids
			asteroids->get <SnModel>(i)->model()->get_bounding_box(asteroidBox);
			if (asteroidBox.center().z < 0.0f) {
				if (!IsSoundPlaying(zoom)) PlaySound(zoom);
			}
			asteroids->get<SnModel>(i)->model()->translate(GsVec(0.0, 0.0, asteroidSpeed));
		}
		
		// update the ship's model translation
		if (shipBoundingBox->center().x + shipBoundingBox->dx()/2 > xBoundary) { // if it moves too far to the left
			xVel = 0.0f;
			shipModel->model()->translate(GsVec(-shipBoundingBox->dx()*0.01f, 0, 0));
		}
		else if (shipBoundingBox->center().x - shipBoundingBox->dx()/2 < -xBoundary) { // if it moves to ofar to the right 
			xVel = 0.0f;
			shipModel->model()->translate(GsVec(shipBoundingBox->dx() * 0.01f, 0, 0));
		}
		else if (shipBoundingBox->center().y + shipBoundingBox->dy()/2 > yBoundary) { // if it moves too far up
			yVel = 0.0f;
			shipModel->model()->translate(GsVec(0.0f, -shipBoundingBox->dy() * 0.01f, 0.0f));
		}
		else if (shipBoundingBox->center().y - shipBoundingBox->dy()/2 < -5.0f) { // if it moves too far down
			yVel = 0.0f;
			shipModel->model()->translate(GsVec(0.0f, shipBoundingBox->dy() * 0.01f, 0.0f));
		}
		else {
			shipModel->model()->translate(GsVec(xVel, yVel, 0.0f));
		}

		render();
		ws_check();
	} while (gameRunning);
	gsout << "You've survived for " << (int)t << " seconds!" << gsnl;
	score = (int)t;



	gameOver = true;
	game_over();

}

void GameViewer::game_over() {
	gsout << "You may press R to restart, or Feel free to exit with escape!" << gsnl;
}

int GameViewer::handle_keyboard(const GsEvent& e) {
	int ret = WsViewer::handle_keyboard(e);
	if (ret) return ret;
	switch (e.key) {
		case GsEvent::KeyEsc: {gs_exit(); return 1;} // exit 
		case GsEvent::KeySpace: {
			if (firstPlay) {
				rootg()->remove(0); // get rid of title screen 
				camera().eye.set(GsVec(0.0f, 4.5f, -27.5f)); 
				render(); 
				ws_check(); 
				firstPlay = false;
			} 
			game_loop(); 
			return 1; 
		} // start game 
		case GsEvent::KeyUp: {up = true; return 1;}
		case GsEvent::KeyDown: {down = true; return 1;}
		case GsEvent::KeyLeft: {left = true; return 1;}
		case GsEvent::KeyRight: {right = true; return 1;}

		//case 'f': {PlaySound(fx); return 1; } // boop
		case 'r': { // restart 
			if (gameOver) {
				StopSound(fx);
				PlaySound(restart);
				rootg()->remove(1);
				rootg()->remove(2);
				gameOver = false;
				game_loop();
			}
			return 1; 
		}
	}

	return 0;
}

int GameViewer::handle_key_release(const GsEvent& e) {
	int ret = WsViewer::handle_key_release(e);
	if (ret) return ret;
	switch (e.key) {
	case GsEvent::KeyUp: {up = false; return 1;}
	case GsEvent::KeyDown: {down = false; return 1;}
	case GsEvent::KeyLeft: {left = false; return 1;}
	case GsEvent::KeyRight: {right = false; return 1; }
	}

	return 0;
}

