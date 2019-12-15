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


float posBoundaryX = 100.0f;
float posBoundaryY = 100.0f;
float posBoundaryZ = 100.0f;

float negBoundaryX = -100.0f;
float negBoundaryY = -100.0f;
float negBoundaryZ = -100.0f;

bool firstPlay = true;


MyViewer::MyViewer ( int x, int y, int w, int h, const char* l ) : WsViewer(x,y,w,h,l)
{
	_nbut=0;
	_animating=false;
	build_ui ();
	createTitle();
	build_scene ();
	addShip();
	addSun();
	addWalls();
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

	SnModel* floor = new SnModel;
	GsModel& model = *floor->model();

	floor->model()->V.size(4);
	floor->model()->F.size(2);
	floor->model()->N.size(4);

	floor->model()->V[0].set(posBoundaryX, -1, posBoundaryZ);
	floor->model()->V[1].set(negBoundaryX, -1, posBoundaryZ);
	floor->model()->V[2].set(negBoundaryX, -1, negBoundaryZ);
	floor->model()->V[3].set(posBoundaryX, -1, negBoundaryZ);


	floor->model()->F[0].set(0, 1, 3);
	floor->model()->F[1].set(2, 3, 1);


	GsVec frontAxis = GsVec(0, 1, 1);

	floor->model()->rotate(GsQuat(frontAxis, float(GS_2PI / 2)));
	floor->model()->translate(GsVec(0, 0, posBoundaryZ * 2 +1));
	floor->model()->invert_faces();

	rootg()->add(floor);

	GsModel::Group& textureGroup = *floor->model()->G.push();
	textureGroup.fi = 0;
	textureGroup.fn = model.F.size();
	textureGroup.dmap = new GsModel::Texture;
	textureGroup.dmap->fname.set("../../Models/SkyboxTextures/StarscapeTitle.png");

	model.M.push().init();

	
	model.T.push(GsPnt2(posBoundaryX, posBoundaryZ));
	model.T.push(GsPnt2(negBoundaryX, negBoundaryZ));
	model.T.push(GsPnt2(negBoundaryX, posBoundaryZ));
	model.T.push(GsPnt2(posBoundaryX, negBoundaryZ));

	model.set_mode(GsModel::Smooth, GsModel::PerGroupMtl);
	model.textured = true;

	

}

void MyViewer::fixedCam()
{
	//GsCamera* camera = new GsCamera;
	camera().eye.set(GsVec(0.0f, 5.0f, -20.0f));
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

	floor->model()->V[0].set(posBoundaryX, -1, posBoundaryZ);
	floor->model()->V[1].set(negBoundaryX, -1, posBoundaryZ);
	floor->model()->V[2].set(negBoundaryX, -1, negBoundaryZ);
	floor->model()->V[3].set(posBoundaryX, -1, negBoundaryZ);

	floor->model()->F[0].set(0, 1, 3);
	floor->model()->F[1].set(2, 3, 1);

	GsVec frontAxis = GsVec(1, 1, 0);

	floor->model()->rotate(GsQuat(frontAxis, float(GS_2PI / 2)));
	floor->model()->translate(GsVec(posBoundaryX, 0, posBoundaryZ));


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

	floor->model()->V[0].set(posBoundaryX, -1, posBoundaryZ);
	floor->model()->V[1].set(negBoundaryX, -1, posBoundaryZ);
	floor->model()->V[2].set(negBoundaryX, -1, negBoundaryZ);
	floor->model()->V[3].set(posBoundaryX, -1, negBoundaryZ);

	floor->model()->F[0].set(0, 1, 3);
	floor->model()->F[1].set(2, 3, 1);

	GsVec frontAxis = GsVec(-1, -1, 0);

	floor->model()->rotate(GsQuat(frontAxis, float(GS_2PI / 2)));
	floor->model()->translate(GsVec(negBoundaryX+1, 0, posBoundaryZ));
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


void MyViewer::addShip()
{
	SnGroup* shipGroup = new SnGroup;
	SnTransform* shipTransform = new SnTransform;
	SnModel* shipModel = new SnModel;
	shipGroup->add(shipModel);
	shipGroup->add(shipTransform);
	GsBox* shipBox = new GsBox;

	GsLight* lightTest = new GsLight;

	/*lightTest->init();
	lightTest->diffuse = GsColor::black;
	WsViewer::set_light->*/


	shipModel->model()->init();
	shipModel->model()->load("../../Models/ShipModels/ThirdPerson/Custom Edited - Star Fox Customs - Arwing SNES HighPoly/SNES Arwing - High poly/Snes_Arwing.obj");



	shipModel->model()->get_bounding_box(*shipBox);

	/*gsout << shipBox->maxsize() << gsnl;
	gsout << shipBox->size() << gsnl;
	gsout << shipBox->center() << gsnl;*/



	rootg()->add(shipGroup);






}

void MyViewer::addSun()
{

	SnGroup* sunGroup = new SnGroup;
	SnTransform* sunTransform = new SnTransform;
	SnModel* sunModel = new SnModel;
	sunGroup->add(sunModel);
	sunGroup->add(sunTransform);

	//sunModel->model()->load("../../Models/MiscModels/sun/sun.obj");


	rootg()->add(sunModel);


}

void MyViewer::addWalls()
{



}

void MyViewer::build_scene ()
{

	SnModel* cube = new SnModel;
	cube->model()->make_box( GsBox( GsPnt(-5,-5,-5),GsPnt(5,5,5) ) );

	//rootg()->add(cube);
	

}




int MyViewer::handle_keyboard ( const GsEvent &e )
{
	int ret = WsViewer::handle_keyboard ( e ); // 1st let system check events
	if ( ret ) return ret;

	//SnModel torusEdit = (SnModel*)rootg()->get(0);

	switch ( e.key )
	{	case GsEvent::KeyEsc : gs_exit(); return 1;

	case 32: {

		if (firstPlay == true) {
			fixedCam();
			return 1;
		}
		else {
			return 1;
		}
		return 1;
	}



		default: gsout<<"Key pressed: "<<e.key<<gsnl;
	}

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
