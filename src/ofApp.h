#pragma once

#include "ofMain.h"
#include "ofxImGui.h"

#include "halton_sampler.h"
#include <random>

class ofApp : public ofBaseApp{
public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	ofEasyCam _camera;
	ofxImGui::Gui _imgui;
	int _seed = 1;
	int _count = 1000;

	ofMesh _mesh;
	int _random = false;
	Halton_sampler _sampler;
};
