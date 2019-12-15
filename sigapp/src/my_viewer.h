# pragma once

# include <sig/sn_poly_editor.h>
# include <sig/sn_lines2.h>

# include <sigogl/ui_button.h>
# include <sigogl/ws_viewer.h>
#include <sig/sn_model.h>
#include <sig/gs_model.h>


// Viewer for this example:
class MyViewer : public WsViewer
{  protected :
	enum MenuEv { EvNormals, EvAnimate, EvExit };
	UiCheckButton* _nbut;
	bool _animating;
   public :
	MyViewer ( int x, int y, int w, int h, const char* l );
	void build_ui ();
	void add_model ( SnShape* s, GsVec p );
	void createSkybox();
	void addShip();
	void addSun();
	void addWalls();
	void setFront(SnModel* floor, GsModel& model);
	void setFloor(SnModel* floor, GsModel& model);
	void setWallR(SnModel* floor, GsModel& model);
	void setWallL(SnModel* floor, GsModel& model);
	void createTitle();
	void fixedCam();
	void setCeiling(SnModel* floor, GsModel& model);


	void build_scene ();
	void show_normals ( bool view );
	void run_animation ();
	virtual int handle_keyboard ( const GsEvent &e ) override;
	virtual int uievent ( int e ) override;
};
