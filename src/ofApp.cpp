#include "ofApp.h"


struct PeseudoRandom {
	virtual ~PeseudoRandom() {}

	virtual uint32_t generate() = 0;
	virtual double uniform() = 0;
	virtual double uniform(double a, double b) = 0;
};

struct Xor : public PeseudoRandom {
	Xor() {

	}
	Xor(uint32_t seed) {
		_y = std::max(seed, 1u);
	}

	// 0 <= x <= 0x7FFFFFFF
	uint32_t generate() {
		_y = _y ^ (_y << 13); _y = _y ^ (_y >> 17);
		uint32_t value = _y = _y ^ (_y << 5); // 1 ~ 0xFFFFFFFF(4294967295
		return value >> 1;
	}
	// 0.0 <= x < 1.0
	double uniform() {
		return double(generate()) / double(0x80000000);
	}
	double uniform(double a, double b) {
		return a + (b - a) * double(uniform());
	}
public:
	uint32_t _y = 2463534242;
};

//--------------------------------------------------------------
void ofApp::setup(){
	_imgui.setup();

	_camera.setNearClip(0.01f);
	_camera.setFarClip(500.0f);
	_camera.setDistance(5.0f);

	_sampler.init_random(std::mt19937(_seed));
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

	ofEnableDepthTest();

	ofClear(0);

	_camera.begin();
	ofPushMatrix();
	ofRotateY(90.0f);
	ofSetColor(64);
	ofDrawGridPlane(1, 10);
	ofPopMatrix();

	ofPushMatrix();
	ofDrawAxis(5);
	ofPopMatrix();

	_mesh.clear();
	_mesh.setMode(OF_PRIMITIVE_POINTS);
	if (_random == 0) {
		Xor random(_seed);
		for (int i = 0; i < _count; ++i) {
			float x = random.uniform();
			float y = random.uniform();

			_mesh.addVertex(ofVec3f(x, y, 0.0f));
		}
	}
	else {
		for (int i = 0; i < _count; ++i) {
			float x = _sampler.sample(2, i);
			float y = _sampler.sample(3, i);

			_mesh.addVertex(ofVec3f(x, y, 0.0f));
		}
	}


	ofSetColor(255);
	_mesh.draw();

	_camera.end();


	ofDisableDepthTest();
	ofSetColor(255);


	_imgui.begin();

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ofVec4f(0.2f, 0.2f, 0.5f, 0.5f));
	ImGui::SetNextWindowPos(ofVec2f(50, 30), ImGuiSetCond_Once);
	ImGui::SetNextWindowSize(ofVec2f(500, 200), ImGuiSetCond_Once);

	ImGui::Begin("Config Panel");
	ImGui::Text("fps: %.2f", ofGetFrameRate());
	ImGui::RadioButton("simple random", &_random, 0);
	ImGui::RadioButton("quasi random", &_random, 1);
	if (ImGui::InputInt("seed", &_seed)) {
		_sampler.init_random(std::mt19937(_seed));
	}
	if (_seed == 0) {
		_seed = 1;
	}
	ImGui::InputInt("count", &_count);
	
	auto wp = ImGui::GetWindowPos();
	auto ws = ImGui::GetWindowSize();
	ofRectangle win(wp.x, wp.y, ws.x, ws.y);

	ImGui::End();
	ImGui::PopStyleColor();

	_imgui.end();

	if (win.inside(ofGetMouseX(), ofGetMouseY())) {
		_camera.disableMouseInput();
	}
	else {
		_camera.enableMouseInput();
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
