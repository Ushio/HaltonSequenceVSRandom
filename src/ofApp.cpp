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


/*x = 0~1*/
inline int float_to_index(float x, int indexCount) {
	int index = (int)(x * indexCount);
	index = std::max(index, 0);
	index = std::min(index, indexCount - 1);
	return index;
}
inline std::vector<int> random_index_samples(int sampleCount, int indexCount, int seed) {
	seed = std::max(1, seed);
	Xor random(seed);

	std::vector<int> r;
	r.resize(sampleCount);
	for (int i = 0; i < sampleCount; ++i) {
		r[i] = random.generate() % indexCount; // has modulo bias
	}
	return r;
}
inline std::vector<int> quasi_random_index_samples(int sampleCount, int indexCount, int seed) {
	seed = std::max(1, seed);
	Xor random(seed);

	Halton_sampler sampler;
	sampler.init_random(std::mt19937(seed));

	std::vector<int> r;
	r.resize(sampleCount);
	for (int i = 0; i < sampleCount; ++i) {
		r[i] = float_to_index(sampler.sample(2, i), indexCount);
	}
	return r;
}

class Kahan {
public:
	Kahan() {}
	Kahan(double value) {}

	void add(double x) {
		double y = x - _c;
		double t = _sum + y;
		_c = (t - _sum) - y;
		_sum = t;
	}
	void operator=(double x) {
		_sum = x;
		_c = 0.0;
	}
	void operator+=(double x) {
		add(x);
	}
	operator double() const {
		return _sum;
	}
private:
	double _sum = 0.0;
	double _c = 0.0;
};

// https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Online_algorithm
class IncrementalStatatics {
public:
	void addSample(double x) {
		_n++;
		double delta = x - _mean;
		_mean += delta / _n;
		double delta2 = x - _mean;
		_M2 += delta * delta2;
	}
	double variance() const {
		// return _M2 / (_n - 1);
		return _M2 / _n;
	}
	double avarage() const {
		return _mean;
	}

	IncrementalStatatics merge(const IncrementalStatatics &rhs) const {
		IncrementalStatatics r;

		double ma = _mean;
		double mb = rhs._mean;
		double N = _n;
		double M = rhs._n;
		double N_M = N + M;
		double a = N / N_M;
		double b = M / N_M;
		r._mean = a * ma + b * mb;
		r._M2 = _M2 + rhs._M2;
		r._n = N_M;

		return r;
	}
private:
	Kahan _mean = 0.0;
	Kahan _M2 = 0.0;
	int _n = 0;
};

//--------------------------------------------------------------
void ofApp::setup(){
	_imgui.setup();

	_camera.setNearClip(0.01f);
	_camera.setFarClip(500.0f);
	_camera.setDistance(5.0f);

	_sampler.init_random(std::mt19937(_seed));

	const int indexCount = 10000;
	auto samples_1 = random_index_samples(100000, indexCount, _seed);
	auto samples_2 = quasi_random_index_samples(100000, indexCount, _seed);

	{
		IncrementalStatatics st;
		int number_of_sample[indexCount] = {};
		for (int i = 0; i < samples_1.size(); ++i) {
			number_of_sample[samples_1[i]]++;
		}
		for (int i = 0; i < indexCount; ++i) {
			st.addSample(number_of_sample[i]);
		}
		printf("random\n");
		printf("avg     : %.4f\n", st.avarage());
		printf("variance: %.4f\n", st.variance());
	}
	
	{
		IncrementalStatatics st;
		int number_of_sample[indexCount] = {};
		for (int i = 0; i < samples_2.size(); ++i) {
			number_of_sample[samples_2[i]]++;
		}
		for (int i = 0; i < indexCount; ++i) {
			st.addSample(number_of_sample[i]);
		}
		printf("quasi-random\n");
		printf("avg     : %.4f\n", st.avarage());
		printf("variance: %.4f\n", st.variance());
	}
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
