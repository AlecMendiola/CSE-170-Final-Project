/* SPACE ACE
Collaborators: Franz Anthony Varela, William Su, An Pham, and Alec Mendiola
Prepare for the best game you've ever seen 
*/

# include "game_viewer.h"
#include <sigogl/ws_run.h>

int main(int argc, char** argv) {
	GameViewer* g = new GameViewer(-1, -1, 1920, 1080, "Space Ace");
	g->cmd(WsViewer::VCmdAsIs);
	g->view_all();
	g->show();
	ws_run();
	return 1;
}
