# include "my_viewer.h"
# include <sigogl/ui_button.h>
# include <sigogl/ui_radio_button.h>
# include <sig/sn_primitive.h>
# include <sig/sn_transform.h>
# include <sig/sn_manipulator.h>
# include <sigogl/ws_run.h>
#include <sig/gs_camera.h>
#include <math.h>
#include <string>
#include <random>


float posBoundaryX = 100.0f;
float posBoundaryY = 100.0f;
float posBoundaryZ = 100.0f;

float negBoundaryX = -100.0f;
float negBoundaryY = -100.0f;
float negBoundaryZ = -100.0f;

float randVarianceX = 10.0f;
float randVarianceY = 5.0f;

bool firstPlay = true;
bool game_running = false;
bool pause = false;

//fps counter vars
bool fps_counter_enable = false;
bool ship_coords_enable = false;
bool mvmt_enable = true;
double fpssum = 0.0;
int count = 0;

//keystates for ship movement
bool up = false;
bool down = false;
bool left = false;
bool right = false;
bool qkey = false;
bool ekey = false;

//ship (x, y, z) location
float xloc = 0.0f;
float yloc = 0.0f;
float zloc = 0.0f; //not currently used
//ship velocity for kinematics
float xvel = 0.0f;
float yvel = 0.0f;
//ship rotation state
float yrot = 0.0f;
float xrot = 0.0f;
float zrot = 0.0f;
//for angular acceleration
float yangvel = 0.0f;
float xangvel = 0.0f;
float zangvel = 0.0f;






MyViewer::MyViewer ( int x, int y, int w, int h, const char* l ) : WsViewer(x,y,w,h,l)
{
	_nbut=0;
	_animating=false;
	build_ui ();
	createTitle();
	createSkybox();

}

void MyViewer::build_ui ()
{
	UiPanel *p, *sp;
	UiManager* uim = WsWindow::uim();
	p = uim->add_panel ( "", UiPanel::HorizLeft );
	p->add ( new UiButton ( "View", sp=new UiPanel() ) );
	{	UiPanel* p=sp; 
	}
	p->add ( new UiButton ( "Exit", EvExit ) ); p->top()->separate();
}

void MyViewer::add_model ( SnShape* s, GsVec p )
{

	
}

void MyViewer::createTitle()
{

	SnModel* titleModel = new SnModel;

	titleModel->model()->init();
	titleModel->model()->load("../../Models/SkyboxTextures/titleRotBig.obj");

	GsVec frontAxis = GsVec(0, 1.5f, 1);

	titleModel->model()->rotate(GsQuat(frontAxis, float(GS_2PI / 2)));
	titleModel->model()->translate(GsVec(0, 0, posBoundaryZ * 3.0f));
	//titleModel->model()->invert_faces();


	rootg()->add(titleModel);


}

void MyViewer::fixedCam()
{
	//GsCamera* camera = new GsCamera;
	camera().eye.set(GsVec(0.0f, 4.5f, -27.5f));
	render();
	ws_check();

}

void MyViewer::createSkybox()
{
	//SnGroup* wallGroup = new SnGroup;
	//SnModel* frontWall = new SnModel;
	//SnModel* sideWallL = new SnModel;
	//SnModel* sideWallR = new SnModel;
	//SnModel* floor = new SnModel;
	//SnModel* ceiling = new SnModel;
	//SnTransform* wallTrans = new SnTransform;

	//GsModel& frontModel = *frontWall->model();


	//wallGroup->add(frontWall);
	//wallGroup->add(wallTrans);
	//frontWall->model()->make_box(GsBox(GsPnt(-7, 0, 0), GsPnt(7, 7, 0.1f)));

	//GsModel::Group& textureGroup = *frontWall->model()->G.push();
	//textureGroup.fi = 0;
	//textureGroup.fn = frontWall->model()->F.size();
	//textureGroup.dmap = new GsModel::Texture;
	//textureGroup.dmap->fname.set("../../Models/SkyboxTextures/Starscape.png");


	///*frontWall->model()->T.push(GsPnt2());*/
	//for (int i = 0; i < frontWall->model()->V.size(); i++) {
	//	GsPnt2 map;
	//	map.x = frontWall->model()->V[i].x;
	//	map.y = frontWall->model()->V[i].y;
	//	//map.z = frontWall->model()->V[i].z;
	//	frontWall->model()->T.push(map);
	//}

	//frontWall->model()->set_mode(GsModel::Smooth, GsModel::PerGroupMtl);
	//frontWall->model()->textured = true;


	SnModel* floor = new SnModel;
	GsModel& model = *floor->model();
	setFront(floor, model);

	SnModel* ceiling = new SnModel;
	GsModel& modelC = *ceiling->model();
	setCeiling(ceiling, modelC);


	SnModel* wallR = new SnModel;
	GsModel& modelR = *wallR->model();
	setWallR(wallR, modelR);


	SnModel* wallL = new SnModel;
	GsModel& modelL = *wallL->model();
	setWallL(wallL, modelL);


	SnModel* realFloor = new SnModel;
	GsModel& modelRF = *realFloor->model();
	setFloor(realFloor, modelRF);




}



void MyViewer::setFront(SnModel* floor, GsModel& model)
{

	floor->model()->V.size(4);
	floor->model()->F.size(2);
	floor->model()->N.size(4);

	floor->model()->V[0].set(posBoundaryX, -1, posBoundaryZ);
	floor->model()->V[1].set(negBoundaryX, -1, posBoundaryZ);
	floor->model()->V[2].set(negBoundaryX, -1, negBoundaryZ);
	floor->model()->V[3].set(posBoundaryX, -1, negBoundaryZ);

	/*floor->model()->F[0].set(0, 2, 1);
	floor->model()->F[1].set(0, 3, 2);*/

	floor->model()->F[0].set(0, 1, 3);
	floor->model()->F[1].set(2, 3, 1);

	//floor->model()->F[0].set(0, 3, 1);
	//floor->model()->F[1].set(2, 1, 3);

	//floor->model()->rotate(GsQuat(0.0f,0.0f,1.0f,0.0f));



	GsVec frontAxis = GsVec(0, 1, 1);
	/*GsVec floorAxis = GsVec(0, 0, 1);
	GsVec ceilingAxis = GsVec(0, 0, -1);
	GsVec wallAxis = GsVec(1, 1, 1);*/

	floor->model()->rotate(GsQuat(frontAxis, float(GS_2PI / 2)));
	floor->model()->translate(GsVec(0, 0, posBoundaryZ * 2));

	rootg()->add(floor);

	GsModel::Group& textureGroup = *floor->model()->G.push();
	textureGroup.fi = 0;
	textureGroup.fn = model.F.size();
	textureGroup.dmap = new GsModel::Texture;
	textureGroup.dmap->fname.set("../../Models/SkyboxTextures/Starscape.png");

	model.M.push().init();

	/*model.T.push(GsPnt2(10,10));
	model.T.push(GsPnt2(-10, -10));
	model.T.push(GsPnt2(-10, 10));
	model.T.push(GsPnt2(10, -10));*/
	model.T.push(GsPnt2(posBoundaryX, posBoundaryZ));
	model.T.push(GsPnt2(negBoundaryX, negBoundaryZ));
	model.T.push(GsPnt2(negBoundaryX, posBoundaryZ));
	model.T.push(GsPnt2(posBoundaryX, negBoundaryZ));

	model.set_mode(GsModel::Smooth, GsModel::PerGroupMtl);
	model.textured = true;
}


//texture needs to be inversed
void MyViewer::setCeiling(SnModel* floor, GsModel& model)
{

	floor->model()->V.size(4);
	floor->model()->F.size(2);
	floor->model()->N.size(4);

	floor->model()->V[0].set(posBoundaryX, -1, posBoundaryZ);
	floor->model()->V[1].set(negBoundaryX, -1, posBoundaryZ);
	floor->model()->V[2].set(negBoundaryX, -1, negBoundaryZ);
	floor->model()->V[3].set(posBoundaryX, -1, negBoundaryZ);

	floor->model()->F[0].set(0, 1, 3);
	floor->model()->F[1].set(2, 3, 1);

	GsVec frontAxis = GsVec(0, 0, -1);

	floor->model()->rotate(GsQuat(frontAxis, float(GS_2PI / 2)));
	floor->model()->translate(GsVec(0, posBoundaryY -1, posBoundaryZ));
	floor->model()->invert_faces();

	rootg()->add(floor);

	GsModel::Group& textureGroup = *floor->model()->G.push();
	textureGroup.fi = 0;
	textureGroup.fn = model.F.size();
	textureGroup.dmap = new GsModel::Texture;
	textureGroup.dmap->fname.set("../../Models/SkyboxTextures/Starscape.png");

	model.M.push().init();

	model.T.push(GsPnt2(posBoundaryX, posBoundaryZ));
	model.T.push(GsPnt2(negBoundaryX, negBoundaryZ));
	model.T.push(GsPnt2(negBoundaryX, posBoundaryZ));
	model.T.push(GsPnt2(posBoundaryX, negBoundaryZ));

	model.set_mode(GsModel::Smooth, GsModel::PerGroupMtl);
	model.textured = true;
}


void MyViewer::setFloor(SnModel* floor, GsModel& model)
{

	floor->model()->V.size(4);
	floor->model()->F.size(2);
	floor->model()->N.size(4);

	floor->model()->V[0].set(posBoundaryX, -1, posBoundaryZ);
	floor->model()->V[1].set(negBoundaryX, -1, posBoundaryZ);
	floor->model()->V[2].set(negBoundaryX, -1, negBoundaryZ);
	floor->model()->V[3].set(posBoundaryX, -1, negBoundaryZ);

	floor->model()->F[0].set(0, 1, 3);
	floor->model()->F[1].set(2, 3, 1);

	GsVec frontAxis = GsVec(0, 0, 1);

	floor->model()->rotate(GsQuat(frontAxis, float(GS_2PI / 2)));
	floor->model()->translate(GsVec(0, negBoundaryY, posBoundaryZ));

	rootg()->add(floor);

	GsModel::Group& textureGroup = *floor->model()->G.push();
	textureGroup.fi = 0;
	textureGroup.fn = model.F.size();
	textureGroup.dmap = new GsModel::Texture;
	textureGroup.dmap->fname.set("../../Models/SkyboxTextures/Starscape.png");

	model.M.push().init();

	model.T.push(GsPnt2(posBoundaryX, posBoundaryZ));
	model.T.push(GsPnt2(negBoundaryX, negBoundaryZ));
	model.T.push(GsPnt2(negBoundaryX, posBoundaryZ));
	model.T.push(GsPnt2(posBoundaryX, negBoundaryZ));

	model.set_mode(GsModel::Smooth, GsModel::PerGroupMtl);
	model.textured = true;
}


//needs to be inversed
void MyViewer::setWallR(SnModel* floor, GsModel& model)
{

	floor->model()->V.size(4);
	floor->model()->F.size(2);
	floor->model()->N.size(4);

	/*floor->model()->V[0].set(posBoundaryX, -1, posBoundaryZ);
	floor->model()->V[1].set(negBoundaryX, -1, posBoundaryZ);
	floor->model()->V[2].set(negBoundaryX, -1, negBoundaryZ);
	floor->model()->V[3].set(posBoundaryX, -1, negBoundaryZ);*/

	/*floor->model()->V[0].set(1, 0, posBoundaryZ);
	floor->model()->V[1].set(1, 0, 0);
	floor->model()->V[2].set(1, posBoundaryY, posBoundaryZ);
	floor->model()->V[3].set(1, posBoundaryY, 0);*/


	floor->model()->V[0].set(1, negBoundaryY, posBoundaryZ);
	floor->model()->V[1].set(1, negBoundaryY, negBoundaryZ);
	floor->model()->V[2].set(1, posBoundaryY, posBoundaryZ);
	floor->model()->V[3].set(1, posBoundaryY, negBoundaryZ);



	floor->model()->F[0].set(3, 1, 0); 
	floor->model()->F[1].set(0, 2, 3); //correct

	GsVec frontAxis = GsVec(1, 1, 0);

	//floor->model()->rotate(GsQuat(frontAxis, float(GS_2PI / 2)));
	floor->model()->translate(GsVec(posBoundaryX - 1, 0, posBoundaryZ));


	rootg()->add(floor);

	GsModel::Group& textureGroup = *floor->model()->G.push();
	textureGroup.fi = 0;
	textureGroup.fn = model.F.size();
	textureGroup.dmap = new GsModel::Texture;
	textureGroup.dmap->fname.set("../../Models/SkyboxTextures/Starscape.png");

	model.M.push().init();

	model.T.push(GsPnt2(posBoundaryX, posBoundaryZ));
	model.T.push(GsPnt2(negBoundaryX, negBoundaryZ));
	model.T.push(GsPnt2(negBoundaryX, posBoundaryZ));
	model.T.push(GsPnt2(posBoundaryX, negBoundaryZ));

	model.set_mode(GsModel::Smooth, GsModel::PerGroupMtl);
	model.textured = true;
}


void MyViewer::setWallL(SnModel* floor, GsModel& model)
{

	floor->model()->V.size(4);
	floor->model()->F.size(2);
	floor->model()->N.size(4);

	/*floor->model()->V[0].set(posBoundaryX, -1, posBoundaryZ);
	floor->model()->V[1].set(negBoundaryX, -1, posBoundaryZ);
	floor->model()->V[2].set(negBoundaryX, -1, negBoundaryZ);
	floor->model()->V[3].set(posBoundaryX, -1, negBoundaryZ);*/

	/*floor->model()->V[0].set(1, 0, posBoundaryZ);
	floor->model()->V[1].set(1, 0, 0);
	floor->model()->V[2].set(1, posBoundaryY, posBoundaryZ);
	floor->model()->V[3].set(1, posBoundaryY, 0);*/


	floor->model()->V[0].set(1, negBoundaryY, posBoundaryZ);
	floor->model()->V[1].set(1, negBoundaryY, negBoundaryZ);
	floor->model()->V[2].set(1, posBoundaryY, posBoundaryZ);
	floor->model()->V[3].set(1, posBoundaryY, negBoundaryZ);



	floor->model()->F[0].set(3, 1, 0);
	floor->model()->F[1].set(0, 2, 3); //correct

	GsVec frontAxis = GsVec(1, 1, 0);

	//floor->model()->rotate(GsQuat(frontAxis, float(GS_2PI / 2)));
	floor->model()->invert_faces();

	floor->model()->translate(GsVec(negBoundaryX - 1, 0, posBoundaryZ));


	rootg()->add(floor);

	GsModel::Group& textureGroup = *floor->model()->G.push();
	textureGroup.fi = 0;
	textureGroup.fn = model.F.size();
	textureGroup.dmap = new GsModel::Texture;
	textureGroup.dmap->fname.set("../../Models/SkyboxTextures/Starscape.png");

	model.M.push().init();

	model.T.push(GsPnt2(posBoundaryX, posBoundaryZ));
	model.T.push(GsPnt2(negBoundaryX, negBoundaryZ));
	model.T.push(GsPnt2(negBoundaryX, posBoundaryZ));
	model.T.push(GsPnt2(posBoundaryX, negBoundaryZ));

	model.set_mode(GsModel::Smooth, GsModel::PerGroupMtl);
	model.textured = true;
}


void MyViewer::addShip()
{
	SnModel* shipModel = new SnModel;
	GsBox* shipBox = new GsBox;




	shipModel->model()->init();
	shipModel->model()->load("../../Models/ShipModels/ThirdPerson/Custom Edited - Star Fox Customs - Arwing SNES HighPoly/SNES Arwing - High poly/Snes_Arwing.obj");



	shipModel->model()->get_bounding_box(*shipBox);

	/*gsout << shipBox->maxsize() << gsnl;
	gsout << shipBox->size() << gsnl;*/
	//gsout << shipBox->center() << gsnl;

	rootg()->add(shipModel);
	//gsout << rootg()->size() << gsnl;

	/*
	for (int c = 0; c < 1000; c++) {
		ws_check();
		addAsteroid();
	}
	*/

}


void MyViewer::moveShip() //removed args
{
	SnModel* ship = (SnModel*)rootg()->get(6);
	float inc = 0.03f;
	if (up) {
		yvel += inc;

		if (yrot > -float(GS_PI) / 4.0f) {
			yangvel -= 0.01f;
		}
		else if (yrot <= float(GS_PI) / 4.0f) {
			yangvel = 0.005f;
		}

	} else if (down) {
		yvel -= inc;

		if (yrot < float(GS_PI) / 4.0f) {
			yangvel += 0.01f;
		}
		else if (yrot >= float(GS_PI) / 4.0f) {
			yangvel = -0.005f;
		}

	} else if (!up && !down) {
		//decelerate
		yvel *= 0.95f;

		if (yrot >= -0.05f && yrot <= 0.05f) {
			yangvel = 0.0f;
		} else if (yrot > 0.05f) { //if it is pointing up
			if (yangvel >= -0.1f) { //and it is rotating upwards
				yangvel = -0.1f; //start it downwards
			}
			else { //if it is already rotating downwards and still pointing up
				yangvel *= 0.99f; //decelerate the downward rotation
			}
		}
		else if (yrot < -0.05f) { //if it is pointing down
			if (yangvel <= 0.1f) { //and is rotating downward
				yangvel = 0.1f; //start it rotating upwards
			}
			else { //if it is pointing downwards and already rotating upwards
				yangvel *= 0.99f; //decelerate the upward rotation
			}
		}
	}

	if (left) {
		xvel += inc;

		if (xrot > -float(GS_PI) / 4.0f) {
			xangvel -= 0.01f;
		}
		else if (xrot <= float(GS_PI) / 4.0f) {
			xangvel = 0.005f;
		}

	} else if (right) {
		xvel -= inc;

		if (xrot < float(GS_PI) / 4.0f) {
			xangvel += 0.01f;
		}
		else if (xrot >= float(GS_PI) / 4.0f) {
			xangvel = -0.005f;
		}

	} else if (!left && !right) {
		//decelerate
		xvel *= 0.95f;

		if (xrot >= -0.05f && xrot <= 0.05f) {
			xangvel = 0.0f;
		}
		else if (xrot > 0.05f) { //if it is pointing up
			if (xangvel >= -0.1f) { //and it is rotating upwards
				xangvel = -0.1f; //start it downwards
			}
			else { //if it is already rotating downwards and still pointing up
				xangvel *= 0.99f; //decelerate the downward rotation
			}
		}
		else if (xrot < -0.05f) { //if it is pointing down
			if (xangvel <= 0.1f) { //and is rotating downward
				xangvel = 0.1f; //start it rotating upwards
			}
			else { //if it is pointing downwards and already rotating upwards
				xangvel *= 0.99f; //decelerate the upward rotation
			}
		}

	}

	if (qkey) {
		zangvel -= 0.01f;
	}
	else if (ekey) {
		zangvel += 0.01f;
	}
	else {
		zangvel *= 0.95f;
	}

	//rotation portion
	//movement portion
	ship->model()->translate(GsVec(-xloc, -yloc, 0.0f));
	ship->model()->rotate(GsQuat(GsVec(1.0f, 0.0f, 0.0f), yangvel));
	ship->model()->rotate(GsQuat(GsVec(0.0f, 1.0f, 0.0f), -xangvel));
	ship->model()->rotate(GsQuat(GsVec(0.0f, 0.0f, 1.0f), zangvel));
	ship->model()->translate(GsVec(xloc, yloc, 0.0f));
	yrot += yangvel;
	xrot += xangvel;

	if (mvmt_enable) {
		ship->model()->translate(GsVec(0, yvel, 0));
		ship->model()->translate(GsVec(xvel, 0, 0));
		yloc += yvel;
		xloc += xvel;
	}

	
	//ship->model()->rotate(GsQuat(GsVec(1.0, 0.0, 0.0), float(GS_PI)/10.0f));
	//render();
	//ws_check();
}

void MyViewer::addAsteroid() {


	//int easterEgg = gs_random(0, 300);


	SnModel* asteroidModel = new SnModel;

	SnModel* ship = new SnModel;

	GsBox* b = new GsBox;

	GsBox* asteroidBox = new GsBox;

	ship = (SnModel*)rootg()->get(6);

	ship->model()->get_bounding_box(*b);

	asteroidModel->model()->init();

	/*if (easterEgg == 28) {
		gsout << "EASTER EGG!" << gsnl;
		asteroidModel->model()->load("../../Models/MiscModels/bb-Yoda-Obj/bbY.obj");
	}*/

	//else{
		asteroidModel->model()->load("../../Models/AsteroidModels/asteroidLowPoly/asteroidSmall.obj");
	//}


	/*
	ship box
	13.2806
	10.4039 3.50116 13.2806
	-1.19209e-06 0.589332 0.4039
	*/

	asteroidModel->model()->translate(GsVec(gs_random(b->center().x - randVarianceX, b->center().x + randVarianceX), gs_random(b->center().y - randVarianceY, b->center().y + randVarianceY), posBoundaryZ * 2 + 1));

	asteroidModel->model()->get_bounding_box(*asteroidBox);

	//gsout << asteroidBox->center() << gsnl;

	rootg()->add(asteroidModel);

	moveAsteroid(asteroidModel, asteroidBox);
	

}


void MyViewer::moveAsteroid(SnModel* asteroid, GsBox* asteroidBox)
{
	float increment = -10.0f;
	do {
		asteroid->model()->translate(GsVec(0.0f, 0.0f, increment));
		asteroid->model()->get_bounding_box(*asteroidBox);
		render();
		ws_check();
		gs_sleep(50);

	} while (asteroidBox->center().z > -30);

	rootg()->remove(asteroid);
	//delete asteroid;
	//asteroid = NULL;


}






void MyViewer::build_scene ()
{
	

}


int MyViewer::handle_key_release(const GsEvent& e) {
	int ret = WsViewer::handle_key_release(e); // 1st let system check events
	if (ret) return ret;
	switch (e.key) {
		case GsEvent::KeyUp: {
			up = false;
			//gsout << "Up released \n";
			return 1;
		}

		case GsEvent::KeyDown: {
			down = false;
			//gsout << "Down released \n";
			return 1;
		}

		case GsEvent::KeyLeft: {
			left = false;
			//gsout << "Left released \n";
			return 1;
		}

		case GsEvent::KeyRight: {
			right = false;
			//gsout << "Right released \n";
			return 1;
		}

		case 'w': {
			up = false;
			//gsout << "Up released \n";
			return 1;
		}

		case 's': {
			down = false;
			//gsout << "Down released \n";
			return 1;
		}

		case 'a': {
			left = false;
			//gsout << "Left released \n";
			return 1;
		}

		case 'd': {
			right = false;
			//gsout << "Right released \n";
			return 1;
		}

		case 'q': {
			qkey = false;
			return 1;
		}

		case 'e': {
			ekey = false;
			return 1;
		}
	}
	return 0;

}

int MyViewer::handle_keyboard ( const GsEvent &e )
{
	int ret = WsViewer::handle_keyboard ( e ); // 1st let system check events
	if ( ret ) return ret;

	//SnModel torusEdit = (SnModel*)rootg()->get(0);
	switch (e.key) {
		case GsEvent::KeyUp: {
			up = true;
			//gsout << "Up pressed \n";
			return 1;
		}

		case GsEvent::KeyDown: {
			down = true;
			//gsout << "Down pressed \n";
			return 1;
		}

		case GsEvent::KeyLeft: {
			left = true;
			//gsout << "Left pressed \n";
			return 1;
		}

		case GsEvent::KeyRight: {
			right = true;
			//gsout << "Right pressed \n";
			return 1;
		}

		case 'w': {
			up = true;
			//gsout << "Up pressed \n";
			return 1;
		}

		case 's': {
			down = true;
			//gsout << "Down pressed \n";
			return 1;
		}

		case 'a': {
			left = true;
			//gsout << "Left pressed \n";
			return 1;
		}

		case 'd': {
			right = true;
			//gsout << "Right pressed \n";
			return 1;
		}
		
		case 'q': {
			qkey = true;
			return 1;
		}

		case 'e': {
			ekey = true;
			return 1;
		}

		case 32: {

			if (firstPlay == true) {
				fixedCam();
				//addShip();
				game_loop();
				return 1;
			}
			else {
				return 1;
			}
			return 1;
		}
	}

	/*
	switch ( e.key )
	{	case GsEvent::KeyEsc : gs_exit(); 
		return 1;
		case 32: {

			if (firstPlay == true) {
				fixedCam();
				addShip();
				return 1;
			}
			else {
				return 1;
			}
			return 1;
		}

		case 119: {
			moveShip('w');
			return 1;
		}
		case 115: {
			moveShip('s');
			return 1;
		}
		case 97: {
			moveShip('a');
			return 1;
		}
		case 100: {
			moveShip('d');
			return 1;
		}


			default: gsout<<"Key pressed: "<<e.key<<gsnl;
		}
		*/

	return 0;
}

int MyViewer::uievent ( int e )
{
	switch ( e )
	{	
		
		case EvExit: gs_exit();
	}
	return WsViewer::uievent(e);
}


void MyViewer::game_loop() {
	if (game_running) {
		gsout << "Game is already running! \n";
		return;
	}

	gsout << "Starting game loop... \n";
	game_running = true;

	addShip(); //call alec's ship spawning method

	pause = false;
	double t = 0;
	double lt = 0;
	double t0 = gs_time();

	//MAIN GAME LOGIC LOOP
	do {
		/* ========== sig has vsync so no need for artifical delay ===========
		while (t - lt < frdt) {
			ws_check();
			
		}*/

		t = gs_time() - t0;

		//FPS COUNTER, USE GLOBAL FLAG TO DISABLE
		if (fps_counter_enable) {
			if (count == 10) {
				fpssum += 1 / (t - lt);
				gsout << fpssum / 10.0 << gsnl;
				count = 1;
				fpssum = 0.0;
			}
			else {
				count++;
				fpssum += 1 / (t - lt);
			}
		}

		//SHIP COORDS, USE GLOBAL FLAG TO DISABLE
		if (ship_coords_enable) {
			if (count == 18) {
				gsout << "Ship is at x = " << xloc << " and y = " << yloc << gsnl;
				count = 1;
			}
			else {
				count++;
			}
		}

		lt = t; //reset time for next frame

		moveShip(); //call alec's ship movement method

		//DO NOT INCLUDE RENDER AND WS_CHECK IN ANY METHODS CALLED HERE
		render();
		ws_check();
	} while (game_running);

}
