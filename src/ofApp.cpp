#include "ofApp.h"
/*
for (size_t i = 0; i < 23; i++){
	vals[0][i] = false;
}

for (size_t i = 0; i < 23; i++){
	vals[1][i] = true;
}

vals[2][0] = true;
*/

//--------------------------------------------------------------
void ofApp::setup(){
	strcpy_s(senderName, 256, "Openframeworks NDI Sender"); // Set the sender name
	ofSetWindowTitle(senderName); // show it on the title bar

#ifdef _WIN64
	cout << "\nofxNDI example sender - 64 bit" << endl;
#else // _WIN64
	cout << "\nofxNDI example sender - 32 bit" << endl;
#endif // _WIN64

	cout << "NDI SDK copyright NewTek (http:\\NDI.NewTek.com)" << endl;

	// Set the dimensions of the sender output here
	// This is independent of the size of the display window
	senderWidth = 2184;
	senderHeight = 504;

	// Create an RGBA fbo for collection of data
	ndiFbo.allocate(senderWidth, senderHeight, GL_RGBA);

	// Optionally set fbo readback using OpenGL pixel buffers
	ndiSender.SetReadback(); // Change to false to compare

	// Optionally set NDI asynchronous sending
	// instead of clocked at the specified frame rate (60fps default)
	ndiSender.SetAsync();

	// Optionally set the framerate
	// Sending will clock at the set frame rate 
	// and over-ride asynchronous sending
	//
	// Can be set as a whole number, e.g. 60, 30, 25 etc
	// ndiSender.SetFrameRate(30);
	//
	// Or as a decimal number e.g. 29.97
	// ndiSender.SetFrameRate(29.97);
	//
	// Or as a fraction numerator and denominator
	// as specified by the NDI SDK - e.g. 
	// NTSC 1080 : 30000, 1001 for 29.97 fps
	// NTSC  720 : 60000, 1001 for 59.94fps
	// PAL  1080 : 30000, 1200 for 25fps
	// PAL   720 : 60000, 1200 for 50fps
	// ndiSender.SetFrameRate(30000, 1001);

	// Create a sender with default RGBA output format
	// ndiSender.CreateSender(senderName, senderWidth, senderHeight);

	// Optionally create a sender and specify the output colour format
	// NDIlib_FourCC_type_UYVY and NDIlib_FourCC_type_BGRA supported
	// NDI recommend using YCbCr if possible for higher performance and better quality
	ndiSender.CreateSender(senderName, senderWidth, senderHeight, NDIlib_FourCC_type_UYVY);

	cout << "Created NDI sender [" << senderName << "] (" << senderWidth << "x" << senderHeight << ")" << endl;

	// 3D drawing setup for the demo graphics
	glEnable(GL_DEPTH_TEST); // enable depth comparisons and update the depth buffer
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really Nice Perspective Calculations
	ofDisableArbTex(); // needed for textures to work

	ofSetSmoothLighting(true);
	pointLight.setDiffuseColor(ofFloatColor(.85, .85, .55));
	pointLight.setSpecularColor(ofFloatColor(1.f, 1.f, 1.f));

	pointLight2.setDiffuseColor(ofFloatColor(238.f / 255.f, 57.f / 255.f, 35.f / 255.f));
	pointLight2.setSpecularColor(ofFloatColor(.8f, .8f, .9f));

	pointLight3.setDiffuseColor(ofFloatColor(19.f / 255.f, 94.f / 255.f, 77.f / 255.f));
	pointLight3.setSpecularColor(ofFloatColor(18.f / 255.f, 150.f / 255.f, 135.f / 255.f));

	ambient.setAmbientColor(255);

	// shininess is a value between 0 - 128, 128 being the most shiny //
	material.setShininess(120);
	// the light highlight of the material //
	material.setSpecularColor(ofColor(255, 255, 255, 255));

	bWireframe = false;
	isAmbient = false;

	plane.set(rect,rect,15,15);

	for (size_t i = 0; i < 23; i++) {
		general[i] = plane.getMesh();
	}

	//presets
	if (!xml.load("settings.xml"))
		cout << "settings.xml loaded!" << endl;

	auto strokesXml = xml.find("//drawing/stroke");
	for (auto & stroke : strokesXml) {
		auto pts = stroke.getChildren("pt");
		for (auto & pt : pts) {
			auto x = pt.getAttribute("x").getIntValue();
		}
	}

	//sequencer
	bpm = 120;
	sequencer2.setup(10, bpm, 4);
	sequencer2.setPosition(0, 0, 300, 690);

	for (size_t i = 0; i < 23; i++){
		p[i].set("rect "+ofToString(i+1), false);
	}
	//p[0].set("rect 1" , false);
	//p[1].set("rect 2" , false);
	//p[2].set("rect 3" , false);

	for (size_t i = 0; i < 23; i++){
		sequencer2.addRow(&p[i]);
	}

	receiver.setup(PORT);

	isLight = true;

	sequencer2.start();
}

void ofApp::DrawGraphics() {

	ofDisableAlphaBlending(); // Or we can get trails with the rotating cube

	// Draw graphics into an fbo for the example
	ndiFbo.begin();

	// ======== your application draw goes here ========
	ofEnableDepthTest();
	ofClear(0, 0, 0, 255);

	ofEnableLighting();
	pointLight.enable();
	pointLight2.enable();
	pointLight3.enable();

	if (isAmbient) ambient.enable();
	else ambient.disable();

	ofSetColor(255);

	material.begin();


	//totem izquierdo
	for (size_t i = 0; i < 3; i++) {
		if (p[i]) {
			ofPushMatrix();
			ofTranslate(rect / 2, rect / 2 + (rect * i));
			if (bWireframe) general[i].drawWireframe();
			else general[i].draw();
			ofPopMatrix();
		}
	}


	//"L" izquierda
	for (size_t i = 0; i < 3; i++) {
		if (p[i + 3]) {
			ofPushMatrix();
			ofTranslate((rect * 2) + (rect*i), rect*2.5);
			if (bWireframe) general[(i + 3)].drawWireframe();
			else general[(i + 3)].draw();
			ofPopMatrix();
		}
	}
	if (p[6]) {
		ofPushMatrix();
		ofTranslate((rect * 4), rect*1.5);
		if (bWireframe) general[6].drawWireframe();
		else general[6].draw();
		ofPopMatrix();
	}
	if (p[7]) {
		ofPushMatrix();
		ofTranslate((rect * 4), rect*.5);
		if (bWireframe) general[7].drawWireframe();
		else general[7].draw();
		ofPopMatrix();
	}
	if (p[8]) {
		ofPushMatrix();
		ofTranslate((rect * 3), rect*.5);
		if (bWireframe) general[8].drawWireframe();
		else general[8].draw();
		ofPopMatrix();
	}

	//CRUZ centro
	for (size_t i = 0; i < 3; i++) {
		if (p[i+9]) {
			ofPushMatrix();
			ofTranslate(senderWidth / 2, rect / 2 + (rect * i));
			if (bWireframe) general[(i + 9)].drawWireframe();
			else general[(i + 9)].draw();
			ofPopMatrix();
		}
	}

	if (p[12]) {
		ofPushMatrix();
		ofTranslate((senderWidth / 2) - rect, senderHeight / 2);
		if (bWireframe) general[12].drawWireframe();
		else general[12].draw();
		ofPopMatrix();
	}

	if (p[13]) {
		ofPushMatrix();
		ofTranslate((senderWidth / 2) + rect, senderHeight / 2);
		if (bWireframe) general[13].drawWireframe();
		else general[13].draw();
		ofPopMatrix();
	}

	//"L" derecha
	for (size_t i = 0; i < 3; i++) {
		if (p[i + 14]) {
			ofPushMatrix();
			ofTranslate(senderWidth - ((rect * 2) + (rect*i)), rect*2.5);
			if (bWireframe) general[(i + 14)].drawWireframe();
			else general[(i + 14)].draw();
			ofPopMatrix();
		}
	}
	if (p[17]) {
		ofPushMatrix();
		ofTranslate(senderWidth - (rect * 4), rect*1.5);
		if (bWireframe) general[17].drawWireframe();
		else general[17].draw();
		ofPopMatrix();
	}
	if (p[18]) {
		ofPushMatrix();
		ofTranslate(senderWidth - (rect * 4), rect*.5);
		if (bWireframe) general[18].drawWireframe();
		else general[18].draw();
		ofPopMatrix();
	}
	if (p[19]) {
		ofPushMatrix();
		ofTranslate(senderWidth - (rect * 3), rect*.5);
		if (bWireframe) general[19].drawWireframe();
		else general[19].draw();
		ofPopMatrix();
	}

	//totem derecho
	for (size_t i = 0; i < 3; i++) {
		if (p[i + 20]) {
			ofPushMatrix();
			ofTranslate(senderWidth - (rect / 2), rect / 2 + (rect * i));
			if (bWireframe) general[(i + 20)].drawWireframe();
			else general[(i + 20)].draw();
			ofPopMatrix();
		}
	}


	material.end();
	
	ofDisableLighting();
	if (isLight) {
		ofFill();
		ofSetColor(pointLight.getDiffuseColor());
		pointLight.draw();
		ofSetColor(pointLight2.getDiffuseColor());
		pointLight2.draw();
		ofSetColor(pointLight3.getDiffuseColor());
		pointLight3.draw();
	}
	ofDisableDepthTest();

	// ============ end application draw ==============

	
	// End the fbo
	ndiFbo.end();


}

//--------------------------------------------------------------
void ofApp::update() {
	// check for waiting messages
	while (receiver.hasWaitingMessages()) {
		
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(m);

		// check for mouse moved message
		if (m.getAddress() == "/composition/tempocontroller/tempo") {
			
			// both the arguments are floats
			fBpm = m.getArgAsFloat(0);
			bpm = ofMap(fBpm, 0.0, 1.0, 20, 500);
			cout << "get BPM " << bpm << endl;
			int tolerancia = 5;
			if (bpm > sequencer2.getBpm() || bpm < sequencer2.getBpm()){
				sequencer2.setBpm(bpm);
			}
		}
	}

	sequencer2.update();

	pointLight.setPosition(
		(ofGetWidth()*.1) + cos(ofGetElapsedTimef()*.5)*(senderWidth*1),
		ofGetHeight() / 2,
		500);

	pointLight2.setPosition(
		(ofGetWidth()*.5) + cos(ofGetElapsedTimef()*.15)*(ofGetWidth()*.3),
		ofGetHeight()*.5 + sin(ofGetElapsedTimef()*.7)*(ofGetHeight()),
		-300);

	pointLight3.setPosition(
		cos(ofGetElapsedTimef()*1.5) * ofGetWidth()*.5,
		sin(ofGetElapsedTimef()*1.5f) * ofGetWidth()*.5,
		cos(ofGetElapsedTimef()*.2) * 250
	);

}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(0);

	// Option 1 : Send ofFbo
	// ofFbo and ofTexture must be RGBA
	// Pixel data extraction from fbo or texture
	// is optimised using ndiSender.SetReadback();
	DrawGraphics();
	ndiSender.SendImage(ndiFbo);

	// Draw the fbo result fitted to the display window
	//ndiFbo.draw(300, 0, 980, 226);

	ofEnableAlphaBlending();
	//sequencer.draw();
	sequencer2.draw();
	ofSetColor(ofColor(255, 200));
	ndiFbo.draw(0, 0, 980, 226);
	// Option 2 Send ofTexture
	// DrawGraphics();
	// ndiSender.SendImage(ndiFbo.getTexture());

	// Option 3 Send ofImage
	// ofImage or ofPixels are converted to RGBA if not already
	// ndiImage.draw(0, 0, ofGetWidth(), ofGetHeight());
	// ndiSender.SendImage(ndiImage);

	// Option 4 Send ofPixels
	// ndiImage.draw(0, 0, ofGetWidth(), ofGetHeight());
	// ndiSender.SendImage(ndiImage.getPixels());

	// Option 5 Send char buffer
	// The pixels must be rgba
	// if (ndiImage.getImageType() != OF_IMAGE_COLOR_ALPHA)
	//   ndiImage.setImageType(OF_IMAGE_COLOR_ALPHA);
	// ndiImage.draw(0, 0, ofGetWidth(), ofGetHeight());
	// ndiSender.SendImage(ndiImage.getPixels().getData(), senderWidth, senderHeight);

	// Show what it is sending
	if (ndiSender.SenderCreated()) {
		char str[256];
		sprintf_s(str, 256, "Sending as : [%s] (%dx%d)", senderName, senderWidth, senderHeight);
		ofDrawBitmapString(str, 20, 30);
		sprintf_s(str, 256, "OSC PORT %d", PORT);
		ofDrawBitmapString(str, 20, 45);
		sprintf_s(str, 256, "BPM %d", sequencer2.getBpm());
		ofDrawBitmapString(str, 20, 60);

		sprintf_s(str, 256, "REFERENCE");
		ofDrawBitmapString(str, 320, 300);
		sprintf_s(str, 256, "'S'	Wireframe");
		ofDrawBitmapString(str, 320, 315);
		sprintf_s(str, 256, "'L'	Draw lights");
		ofDrawBitmapString(str, 320, 330);
		sprintf_s(str, 256, "'W'	Barrido Derecho");
		ofDrawBitmapString(str, 320, 345);
		sprintf_s(str, 256, "'E'	Barrido Izquierdo");
		ofDrawBitmapString(str, 320, 360);
		sprintf_s(str, 256, "'Q'	FULL");
		ofDrawBitmapString(str, 320, 375);
		sprintf_s(str, 256, "'A'	Ambient Light");
		ofDrawBitmapString(str, 320, 390);
		sprintf_s(str, 256, "'R'	Random");
		ofDrawBitmapString(str, 320, 405);
		sprintf_s(str, 256, "'A'	Blackout");
		ofDrawBitmapString(str, 320, 420);
		// Show fps
		sprintf_s(str, 256, "fps: %3.3d (%4.2f)", (int)ofGetFrameRate(), ndiSender.GetFps());
		ofDrawBitmapString(str, ofGetWidth() - 140, 30);
	}
}



//--------------------------------------------------------------
void ofApp::exit() {
	// The sender must be released 
	// or NDI sender discovery will still find it
	ndiSender.ReleaseSender();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 's'){
		bWireframe = !bWireframe;
	}
	if (key == 'a') {
		isAmbient = !isAmbient;
	}
	if (key == 'w') {
		barrido_der();
	}
	if (key == 'e') {
		barrido_izq();
	}
	if (key == 'q') {
		on();
	}
	if (key == 'a') {
		off();
	}
	if (key == 'r') {
		randoms();
	}
	if (key == 'u') {
		barrido_up();
	}
	if (key == 'l') {
		isLight = !isLight;
	}
	
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

void ofApp::off() {
	sequencer2.setValue<bool>(0, 0, false);
	sequencer2.setValue<bool>(0, 1, false);
	sequencer2.setValue<bool>(0, 2, false);
	sequencer2.setValue<bool>(0, 3, false);
	sequencer2.setValue<bool>(0, 4, false);
	sequencer2.setValue<bool>(0, 5, false);
	sequencer2.setValue<bool>(0, 6, false);
	sequencer2.setValue<bool>(0, 7, false);
	sequencer2.setValue<bool>(0, 8, false);
	sequencer2.setValue<bool>(0, 9, false);

	sequencer2.setValue<bool>(1, 0, false);
	sequencer2.setValue<bool>(1, 1, false);
	sequencer2.setValue<bool>(1, 2, false);
	sequencer2.setValue<bool>(1, 3, false);
	sequencer2.setValue<bool>(1, 4, false);
	sequencer2.setValue<bool>(1, 5, false);
	sequencer2.setValue<bool>(1, 6, false);
	sequencer2.setValue<bool>(1, 7, false);
	sequencer2.setValue<bool>(1, 8, false);
	sequencer2.setValue<bool>(1, 9, false);

	sequencer2.setValue<bool>(2, 0, false);
	sequencer2.setValue<bool>(2, 1, false);
	sequencer2.setValue<bool>(2, 2, false);
	sequencer2.setValue<bool>(2, 3, false);
	sequencer2.setValue<bool>(2, 4, false);
	sequencer2.setValue<bool>(2, 5, false);
	sequencer2.setValue<bool>(2, 6, false);
	sequencer2.setValue<bool>(2, 7, false);
	sequencer2.setValue<bool>(2, 8, false);
	sequencer2.setValue<bool>(2, 9, false);

	sequencer2.setValue<bool>(3, 0, false);
	sequencer2.setValue<bool>(3, 1, false);
	sequencer2.setValue<bool>(3, 2, false);
	sequencer2.setValue<bool>(3, 3, false);
	sequencer2.setValue<bool>(3, 4, false);
	sequencer2.setValue<bool>(3, 5, false);
	sequencer2.setValue<bool>(3, 6, false);
	sequencer2.setValue<bool>(3, 7, false);
	sequencer2.setValue<bool>(3, 8, false);
	sequencer2.setValue<bool>(3, 9, false);

	sequencer2.setValue<bool>(4, 0, false);
	sequencer2.setValue<bool>(4, 1, false);
	sequencer2.setValue<bool>(4, 2, false);
	sequencer2.setValue<bool>(4, 3, false);
	sequencer2.setValue<bool>(4, 4, false);
	sequencer2.setValue<bool>(4, 5, false);
	sequencer2.setValue<bool>(4, 6, false);
	sequencer2.setValue<bool>(4, 7, false);
	sequencer2.setValue<bool>(4, 8, false);
	sequencer2.setValue<bool>(4, 9, false);

	sequencer2.setValue<bool>(5, 0, false);
	sequencer2.setValue<bool>(5, 1, false);
	sequencer2.setValue<bool>(5, 2, false);
	sequencer2.setValue<bool>(5, 3, false);
	sequencer2.setValue<bool>(5, 4, false);
	sequencer2.setValue<bool>(5, 5, false);
	sequencer2.setValue<bool>(5, 6, false);
	sequencer2.setValue<bool>(5, 7, false);
	sequencer2.setValue<bool>(5, 8, false);
	sequencer2.setValue<bool>(5, 9, false);

	sequencer2.setValue<bool>(6, 0, false);
	sequencer2.setValue<bool>(6, 1, false);
	sequencer2.setValue<bool>(6, 2, false);
	sequencer2.setValue<bool>(6, 3, false);
	sequencer2.setValue<bool>(6, 4, false);
	sequencer2.setValue<bool>(6, 5, false);
	sequencer2.setValue<bool>(6, 6, false);
	sequencer2.setValue<bool>(6, 7, false);
	sequencer2.setValue<bool>(6, 8, false);
	sequencer2.setValue<bool>(6, 9, false);

	sequencer2.setValue<bool>(7, 0, false);
	sequencer2.setValue<bool>(7, 1, false);
	sequencer2.setValue<bool>(7, 2, false);
	sequencer2.setValue<bool>(7, 3, false);
	sequencer2.setValue<bool>(7, 4, false);
	sequencer2.setValue<bool>(7, 5, false);
	sequencer2.setValue<bool>(7, 6, false);
	sequencer2.setValue<bool>(7, 7, false);
	sequencer2.setValue<bool>(7, 8, false);
	sequencer2.setValue<bool>(7, 9, false);

	sequencer2.setValue<bool>(8, 0, false);
	sequencer2.setValue<bool>(8, 1, false);
	sequencer2.setValue<bool>(8, 2, false);
	sequencer2.setValue<bool>(8, 3, false);
	sequencer2.setValue<bool>(8, 4, false);
	sequencer2.setValue<bool>(8, 5, false);
	sequencer2.setValue<bool>(8, 6, false);
	sequencer2.setValue<bool>(8, 7, false);
	sequencer2.setValue<bool>(8, 8, false);
	sequencer2.setValue<bool>(8, 9, false);

	sequencer2.setValue<bool>(9, 0, false);
	sequencer2.setValue<bool>(9, 1, false);
	sequencer2.setValue<bool>(9, 2, false);
	sequencer2.setValue<bool>(9, 3, false);
	sequencer2.setValue<bool>(9, 4, false);
	sequencer2.setValue<bool>(9, 5, false);
	sequencer2.setValue<bool>(9, 6, false);
	sequencer2.setValue<bool>(9, 7, false);
	sequencer2.setValue<bool>(9, 8, false);
	sequencer2.setValue<bool>(9, 9, false);

	sequencer2.setValue<bool>(10, 0, false);
	sequencer2.setValue<bool>(10, 1, false);
	sequencer2.setValue<bool>(10, 2, false);
	sequencer2.setValue<bool>(10, 3, false);
	sequencer2.setValue<bool>(10, 4, false);
	sequencer2.setValue<bool>(10, 5, false);
	sequencer2.setValue<bool>(10, 6, false);
	sequencer2.setValue<bool>(10, 7, false);
	sequencer2.setValue<bool>(10, 8, false);
	sequencer2.setValue<bool>(10, 9, false);

	sequencer2.setValue<bool>(11, 0, false);
	sequencer2.setValue<bool>(11, 1, false);
	sequencer2.setValue<bool>(11, 2, false);
	sequencer2.setValue<bool>(11, 3, false);
	sequencer2.setValue<bool>(11, 4, false);
	sequencer2.setValue<bool>(11, 5, false);
	sequencer2.setValue<bool>(11, 6, false);
	sequencer2.setValue<bool>(11, 7, false);
	sequencer2.setValue<bool>(11, 8, false);
	sequencer2.setValue<bool>(11, 9, false);

	sequencer2.setValue<bool>(12, 0, false);
	sequencer2.setValue<bool>(12, 1, false);
	sequencer2.setValue<bool>(12, 2, false);
	sequencer2.setValue<bool>(12, 3, false);
	sequencer2.setValue<bool>(12, 4, false);
	sequencer2.setValue<bool>(12, 5, false);
	sequencer2.setValue<bool>(12, 6, false);
	sequencer2.setValue<bool>(12, 7, false);
	sequencer2.setValue<bool>(12, 8, false);
	sequencer2.setValue<bool>(12, 9, false);

	sequencer2.setValue<bool>(13, 0, false);
	sequencer2.setValue<bool>(13, 1, false);
	sequencer2.setValue<bool>(13, 2, false);
	sequencer2.setValue<bool>(13, 3, false);
	sequencer2.setValue<bool>(13, 4, false);
	sequencer2.setValue<bool>(13, 5, false);
	sequencer2.setValue<bool>(13, 6, false);
	sequencer2.setValue<bool>(13, 7, false);
	sequencer2.setValue<bool>(13, 8, false);
	sequencer2.setValue<bool>(13, 9, false);

	sequencer2.setValue<bool>(14, 0, false);
	sequencer2.setValue<bool>(14, 1, false);
	sequencer2.setValue<bool>(14, 2, false);
	sequencer2.setValue<bool>(14, 3, false);
	sequencer2.setValue<bool>(14, 4, false);
	sequencer2.setValue<bool>(14, 5, false);
	sequencer2.setValue<bool>(14, 6, false);
	sequencer2.setValue<bool>(14, 7, false);
	sequencer2.setValue<bool>(14, 8, false);
	sequencer2.setValue<bool>(14, 9, false);

	sequencer2.setValue<bool>(15, 0, false);
	sequencer2.setValue<bool>(15, 1, false);
	sequencer2.setValue<bool>(15, 2, false);
	sequencer2.setValue<bool>(15, 3, false);
	sequencer2.setValue<bool>(15, 4, false);
	sequencer2.setValue<bool>(15, 5, false);
	sequencer2.setValue<bool>(15, 6, false);
	sequencer2.setValue<bool>(15, 7, false);
	sequencer2.setValue<bool>(15, 8, false);
	sequencer2.setValue<bool>(15, 9, false);

	sequencer2.setValue<bool>(16, 0, false);
	sequencer2.setValue<bool>(16, 1, false);
	sequencer2.setValue<bool>(16, 2, false);
	sequencer2.setValue<bool>(16, 3, false);
	sequencer2.setValue<bool>(16, 4, false);
	sequencer2.setValue<bool>(16, 5, false);
	sequencer2.setValue<bool>(16, 6, false);
	sequencer2.setValue<bool>(16, 7, false);
	sequencer2.setValue<bool>(16, 8, false);
	sequencer2.setValue<bool>(16, 9, false);

	sequencer2.setValue<bool>(17, 0, false);
	sequencer2.setValue<bool>(17, 1, false);
	sequencer2.setValue<bool>(17, 2, false);
	sequencer2.setValue<bool>(17, 3, false);
	sequencer2.setValue<bool>(17, 4, false);
	sequencer2.setValue<bool>(17, 5, false);
	sequencer2.setValue<bool>(17, 6, false);
	sequencer2.setValue<bool>(17, 7, false);
	sequencer2.setValue<bool>(17, 8, false);
	sequencer2.setValue<bool>(17, 9, false);

	sequencer2.setValue<bool>(18, 0, false);
	sequencer2.setValue<bool>(18, 1, false);
	sequencer2.setValue<bool>(18, 2, false);
	sequencer2.setValue<bool>(18, 3, false);
	sequencer2.setValue<bool>(18, 4, false);
	sequencer2.setValue<bool>(18, 5, false);
	sequencer2.setValue<bool>(18, 6, false);
	sequencer2.setValue<bool>(18, 7, false);
	sequencer2.setValue<bool>(18, 8, false);
	sequencer2.setValue<bool>(18, 9, false);

	sequencer2.setValue<bool>(19, 0, false);
	sequencer2.setValue<bool>(19, 1, false);
	sequencer2.setValue<bool>(19, 2, false);
	sequencer2.setValue<bool>(19, 3, false);
	sequencer2.setValue<bool>(19, 4, false);
	sequencer2.setValue<bool>(19, 5, false);
	sequencer2.setValue<bool>(19, 6, false);
	sequencer2.setValue<bool>(19, 7, false);
	sequencer2.setValue<bool>(19, 8, false);
	sequencer2.setValue<bool>(19, 9, false);

	sequencer2.setValue<bool>(20, 0, false);
	sequencer2.setValue<bool>(20, 1, false);
	sequencer2.setValue<bool>(20, 2, false);
	sequencer2.setValue<bool>(20, 3, false);
	sequencer2.setValue<bool>(20, 4, false);
	sequencer2.setValue<bool>(20, 5, false);
	sequencer2.setValue<bool>(20, 6, false);
	sequencer2.setValue<bool>(20, 7, false);
	sequencer2.setValue<bool>(20, 8, false);
	sequencer2.setValue<bool>(20, 9, false);

	sequencer2.setValue<bool>(21, 0, false);
	sequencer2.setValue<bool>(21, 1, false);
	sequencer2.setValue<bool>(21, 2, false);
	sequencer2.setValue<bool>(21, 3, false);
	sequencer2.setValue<bool>(21, 4, false);
	sequencer2.setValue<bool>(21, 5, false);
	sequencer2.setValue<bool>(21, 6, false);
	sequencer2.setValue<bool>(21, 7, false);
	sequencer2.setValue<bool>(21, 8, false);
	sequencer2.setValue<bool>(21, 9, false);

	sequencer2.setValue<bool>(22, 0, false);
	sequencer2.setValue<bool>(22, 1, false);
	sequencer2.setValue<bool>(22, 2, false);
	sequencer2.setValue<bool>(22, 3, false);
	sequencer2.setValue<bool>(22, 4, false);
	sequencer2.setValue<bool>(22, 5, false);
	sequencer2.setValue<bool>(22, 6, false);
	sequencer2.setValue<bool>(22, 7, false);
	sequencer2.setValue<bool>(22, 8, false);
	sequencer2.setValue<bool>(22, 9, false);
}
void ofApp::on() {
	sequencer2.setValue<bool>(0, 0, true);
	sequencer2.setValue<bool>(0, 1, true);
	sequencer2.setValue<bool>(0, 2, true);
	sequencer2.setValue<bool>(0, 3, true);
	sequencer2.setValue<bool>(0, 4, true);
	sequencer2.setValue<bool>(0, 5, true);
	sequencer2.setValue<bool>(0, 6, true);
	sequencer2.setValue<bool>(0, 7, true);
	sequencer2.setValue<bool>(0, 8, true);
	sequencer2.setValue<bool>(0, 9, true);

	sequencer2.setValue<bool>(1, 0, true);
	sequencer2.setValue<bool>(1, 1, true);
	sequencer2.setValue<bool>(1, 2, true);
	sequencer2.setValue<bool>(1, 3, true);
	sequencer2.setValue<bool>(1, 4, true);
	sequencer2.setValue<bool>(1, 5, true);
	sequencer2.setValue<bool>(1, 6, true);
	sequencer2.setValue<bool>(1, 7, true);
	sequencer2.setValue<bool>(1, 8, true);
	sequencer2.setValue<bool>(1, 9, true);

	sequencer2.setValue<bool>(2, 0, true);
	sequencer2.setValue<bool>(2, 1, true);
	sequencer2.setValue<bool>(2, 2, true);
	sequencer2.setValue<bool>(2, 3, true);
	sequencer2.setValue<bool>(2, 4, true);
	sequencer2.setValue<bool>(2, 5, true);
	sequencer2.setValue<bool>(2, 6, true);
	sequencer2.setValue<bool>(2, 7, true);
	sequencer2.setValue<bool>(2, 8, true);
	sequencer2.setValue<bool>(2, 9, true);

	sequencer2.setValue<bool>(3, 0, true);
	sequencer2.setValue<bool>(3, 1, true);
	sequencer2.setValue<bool>(3, 2, true);
	sequencer2.setValue<bool>(3, 3, true);
	sequencer2.setValue<bool>(3, 4, true);
	sequencer2.setValue<bool>(3, 5, true);
	sequencer2.setValue<bool>(3, 6, true);
	sequencer2.setValue<bool>(3, 7, true);
	sequencer2.setValue<bool>(3, 8, true);
	sequencer2.setValue<bool>(3, 9, true);

	sequencer2.setValue<bool>(4, 0, true);
	sequencer2.setValue<bool>(4, 1, true);
	sequencer2.setValue<bool>(4, 2, true);
	sequencer2.setValue<bool>(4, 3, true);
	sequencer2.setValue<bool>(4, 4, true);
	sequencer2.setValue<bool>(4, 5, true);
	sequencer2.setValue<bool>(4, 6, true);
	sequencer2.setValue<bool>(4, 7, true);
	sequencer2.setValue<bool>(4, 8, true);
	sequencer2.setValue<bool>(4, 9, true);

	sequencer2.setValue<bool>(5, 0, true);
	sequencer2.setValue<bool>(5, 1, true);
	sequencer2.setValue<bool>(5, 2, true);
	sequencer2.setValue<bool>(5, 3, true);
	sequencer2.setValue<bool>(5, 4, true);
	sequencer2.setValue<bool>(5, 5, true);
	sequencer2.setValue<bool>(5, 6, true);
	sequencer2.setValue<bool>(5, 7, true);
	sequencer2.setValue<bool>(5, 8, true);
	sequencer2.setValue<bool>(5, 9, true);

	sequencer2.setValue<bool>(6, 0, true);
	sequencer2.setValue<bool>(6, 1, true);
	sequencer2.setValue<bool>(6, 2, true);
	sequencer2.setValue<bool>(6, 3, true);
	sequencer2.setValue<bool>(6, 4, true);
	sequencer2.setValue<bool>(6, 5, true);
	sequencer2.setValue<bool>(6, 6, true);
	sequencer2.setValue<bool>(6, 7, true);
	sequencer2.setValue<bool>(6, 8, true);
	sequencer2.setValue<bool>(6, 9, true);

	sequencer2.setValue<bool>(7, 0, true);
	sequencer2.setValue<bool>(7, 1, true);
	sequencer2.setValue<bool>(7, 2, true);
	sequencer2.setValue<bool>(7, 3, true);
	sequencer2.setValue<bool>(7, 4, true);
	sequencer2.setValue<bool>(7, 5, true);
	sequencer2.setValue<bool>(7, 6, true);
	sequencer2.setValue<bool>(7, 7, true);
	sequencer2.setValue<bool>(7, 8, true);
	sequencer2.setValue<bool>(7, 9, true);

	sequencer2.setValue<bool>(8, 0, true);
	sequencer2.setValue<bool>(8, 1, true);
	sequencer2.setValue<bool>(8, 2, true);
	sequencer2.setValue<bool>(8, 3, true);
	sequencer2.setValue<bool>(8, 4, true);
	sequencer2.setValue<bool>(8, 5, true);
	sequencer2.setValue<bool>(8, 6, true);
	sequencer2.setValue<bool>(8, 7, true);
	sequencer2.setValue<bool>(8, 8, true);
	sequencer2.setValue<bool>(8, 9, true);

	sequencer2.setValue<bool>(9, 0, true);
	sequencer2.setValue<bool>(9, 1, true);
	sequencer2.setValue<bool>(9, 2, true);
	sequencer2.setValue<bool>(9, 3, true);
	sequencer2.setValue<bool>(9, 4, true);
	sequencer2.setValue<bool>(9, 5, true);
	sequencer2.setValue<bool>(9, 6, true);
	sequencer2.setValue<bool>(9, 7, true);
	sequencer2.setValue<bool>(9, 8, true);
	sequencer2.setValue<bool>(9, 9, true);

	sequencer2.setValue<bool>(10, 0, true);
	sequencer2.setValue<bool>(10, 1, true);
	sequencer2.setValue<bool>(10, 2, true);
	sequencer2.setValue<bool>(10, 3, true);
	sequencer2.setValue<bool>(10, 4, true);
	sequencer2.setValue<bool>(10, 5, true);
	sequencer2.setValue<bool>(10, 6, true);
	sequencer2.setValue<bool>(10, 7, true);
	sequencer2.setValue<bool>(10, 8, true);
	sequencer2.setValue<bool>(10, 9, true);

	sequencer2.setValue<bool>(11, 0, true);
	sequencer2.setValue<bool>(11, 1, true);
	sequencer2.setValue<bool>(11, 2, true);
	sequencer2.setValue<bool>(11, 3, true);
	sequencer2.setValue<bool>(11, 4, true);
	sequencer2.setValue<bool>(11, 5, true);
	sequencer2.setValue<bool>(11, 6, true);
	sequencer2.setValue<bool>(11, 7, true);
	sequencer2.setValue<bool>(11, 8, true);
	sequencer2.setValue<bool>(11, 9, true);

	sequencer2.setValue<bool>(12, 0, true);
	sequencer2.setValue<bool>(12, 1, true);
	sequencer2.setValue<bool>(12, 2, true);
	sequencer2.setValue<bool>(12, 3, true);
	sequencer2.setValue<bool>(12, 4, true);
	sequencer2.setValue<bool>(12, 5, true);
	sequencer2.setValue<bool>(12, 6, true);
	sequencer2.setValue<bool>(12, 7, true);
	sequencer2.setValue<bool>(12, 8, true);
	sequencer2.setValue<bool>(12, 9, true);

	sequencer2.setValue<bool>(13, 0, true);
	sequencer2.setValue<bool>(13, 1, true);
	sequencer2.setValue<bool>(13, 2, true);
	sequencer2.setValue<bool>(13, 3, true);
	sequencer2.setValue<bool>(13, 4, true);
	sequencer2.setValue<bool>(13, 5, true);
	sequencer2.setValue<bool>(13, 6, true);
	sequencer2.setValue<bool>(13, 7, true);
	sequencer2.setValue<bool>(13, 8, true);
	sequencer2.setValue<bool>(13, 9, true);

	sequencer2.setValue<bool>(14, 0, true);
	sequencer2.setValue<bool>(14, 1, true);
	sequencer2.setValue<bool>(14, 2, true);
	sequencer2.setValue<bool>(14, 3, true);
	sequencer2.setValue<bool>(14, 4, true);
	sequencer2.setValue<bool>(14, 5, true);
	sequencer2.setValue<bool>(14, 6, true);
	sequencer2.setValue<bool>(14, 7, true);
	sequencer2.setValue<bool>(14, 8, true);
	sequencer2.setValue<bool>(14, 9, true);

	sequencer2.setValue<bool>(15, 0, true);
	sequencer2.setValue<bool>(15, 1, true);
	sequencer2.setValue<bool>(15, 2, true);
	sequencer2.setValue<bool>(15, 3, true);
	sequencer2.setValue<bool>(15, 4, true);
	sequencer2.setValue<bool>(15, 5, true);
	sequencer2.setValue<bool>(15, 6, true);
	sequencer2.setValue<bool>(15, 7, true);
	sequencer2.setValue<bool>(15, 8, true);
	sequencer2.setValue<bool>(15, 9, true);

	sequencer2.setValue<bool>(16, 0, true);
	sequencer2.setValue<bool>(16, 1, true);
	sequencer2.setValue<bool>(16, 2, true);
	sequencer2.setValue<bool>(16, 3, true);
	sequencer2.setValue<bool>(16, 4, true);
	sequencer2.setValue<bool>(16, 5, true);
	sequencer2.setValue<bool>(16, 6, true);
	sequencer2.setValue<bool>(16, 7, true);
	sequencer2.setValue<bool>(16, 8, true);
	sequencer2.setValue<bool>(16, 9, true);

	sequencer2.setValue<bool>(17, 0, true);
	sequencer2.setValue<bool>(17, 1, true);
	sequencer2.setValue<bool>(17, 2, true);
	sequencer2.setValue<bool>(17, 3, true);
	sequencer2.setValue<bool>(17, 4, true);
	sequencer2.setValue<bool>(17, 5, true);
	sequencer2.setValue<bool>(17, 6, true);
	sequencer2.setValue<bool>(17, 7, true);
	sequencer2.setValue<bool>(17, 8, true);
	sequencer2.setValue<bool>(17, 9, true);

	sequencer2.setValue<bool>(18, 0, true);
	sequencer2.setValue<bool>(18, 1, true);
	sequencer2.setValue<bool>(18, 2, true);
	sequencer2.setValue<bool>(18, 3, true);
	sequencer2.setValue<bool>(18, 4, true);
	sequencer2.setValue<bool>(18, 5, true);
	sequencer2.setValue<bool>(18, 6, true);
	sequencer2.setValue<bool>(18, 7, true);
	sequencer2.setValue<bool>(18, 8, true);
	sequencer2.setValue<bool>(18, 9, true);

	sequencer2.setValue<bool>(19, 0, true);
	sequencer2.setValue<bool>(19, 1, true);
	sequencer2.setValue<bool>(19, 2, true);
	sequencer2.setValue<bool>(19, 3, true);
	sequencer2.setValue<bool>(19, 4, true);
	sequencer2.setValue<bool>(19, 5, true);
	sequencer2.setValue<bool>(19, 6, true);
	sequencer2.setValue<bool>(19, 7, true);
	sequencer2.setValue<bool>(19, 8, true);
	sequencer2.setValue<bool>(19, 9, true);

	sequencer2.setValue<bool>(20, 0, true);
	sequencer2.setValue<bool>(20, 1, true);
	sequencer2.setValue<bool>(20, 2, true);
	sequencer2.setValue<bool>(20, 3, true);
	sequencer2.setValue<bool>(20, 4, true);
	sequencer2.setValue<bool>(20, 5, true);
	sequencer2.setValue<bool>(20, 6, true);
	sequencer2.setValue<bool>(20, 7, true);
	sequencer2.setValue<bool>(20, 8, true);
	sequencer2.setValue<bool>(20, 9, true);

	sequencer2.setValue<bool>(21, 0, true);
	sequencer2.setValue<bool>(21, 1, true);
	sequencer2.setValue<bool>(21, 2, true);
	sequencer2.setValue<bool>(21, 3, true);
	sequencer2.setValue<bool>(21, 4, true);
	sequencer2.setValue<bool>(21, 5, true);
	sequencer2.setValue<bool>(21, 6, true);
	sequencer2.setValue<bool>(21, 7, true);
	sequencer2.setValue<bool>(21, 8, true);
	sequencer2.setValue<bool>(21, 9, true);

	sequencer2.setValue<bool>(22, 0, true);
	sequencer2.setValue<bool>(22, 1, true);
	sequencer2.setValue<bool>(22, 2, true);
	sequencer2.setValue<bool>(22, 3, true);
	sequencer2.setValue<bool>(22, 4, true);
	sequencer2.setValue<bool>(22, 5, true);
	sequencer2.setValue<bool>(22, 6, true);
	sequencer2.setValue<bool>(22, 7, true);
	sequencer2.setValue<bool>(22, 8, true);
	sequencer2.setValue<bool>(22, 9, true);
}
void ofApp::barrido_izq() {
	//totem izq
	sequencer2.setValue<bool>(0, 0, false);
	sequencer2.setValue<bool>(0, 1, false);
	sequencer2.setValue<bool>(0, 2, false);
	sequencer2.setValue<bool>(0, 3, false);
	sequencer2.setValue<bool>(0, 4, true);
	sequencer2.setValue<bool>(0, 5, false);
	sequencer2.setValue<bool>(0, 6, false);
	sequencer2.setValue<bool>(0, 7, false);
	sequencer2.setValue<bool>(0, 8, false);
	sequencer2.setValue<bool>(0, 9, true);

	sequencer2.setValue<bool>(1, 0, false);
	sequencer2.setValue<bool>(1, 1, false);
	sequencer2.setValue<bool>(1, 2, false);
	sequencer2.setValue<bool>(1, 3, false);
	sequencer2.setValue<bool>(1, 4, true);
	sequencer2.setValue<bool>(1, 5, false);
	sequencer2.setValue<bool>(1, 6, false);
	sequencer2.setValue<bool>(1, 7, false);
	sequencer2.setValue<bool>(1, 8, false);
	sequencer2.setValue<bool>(1, 9, true);

	sequencer2.setValue<bool>(2, 0, false);
	sequencer2.setValue<bool>(2, 1, false);
	sequencer2.setValue<bool>(2, 2, false);
	sequencer2.setValue<bool>(2, 3, false);
	sequencer2.setValue<bool>(2, 4, true);
	sequencer2.setValue<bool>(2, 5, false);
	sequencer2.setValue<bool>(2, 6, false);
	sequencer2.setValue<bool>(2, 7, false);
	sequencer2.setValue<bool>(2, 8, false);
	sequencer2.setValue<bool>(2, 9, true);
	//---------------------------------------

	//L izq
	sequencer2.setValue<bool>(3, 0, false);
	sequencer2.setValue<bool>(3, 1, false);
	sequencer2.setValue<bool>(3, 2, false);
	sequencer2.setValue<bool>(3, 3, true);
	sequencer2.setValue<bool>(3, 4, false);
	sequencer2.setValue<bool>(3, 5, false);
	sequencer2.setValue<bool>(3, 6, false);
	sequencer2.setValue<bool>(3, 7, false);
	sequencer2.setValue<bool>(3, 8, true);
	sequencer2.setValue<bool>(3, 9, false);

	sequencer2.setValue<bool>(4, 0, false);
	sequencer2.setValue<bool>(4, 1, false);
	sequencer2.setValue<bool>(4, 2, false);
	sequencer2.setValue<bool>(4, 3, true);
	sequencer2.setValue<bool>(4, 4, false);
	sequencer2.setValue<bool>(4, 5, false);
	sequencer2.setValue<bool>(4, 6, false);
	sequencer2.setValue<bool>(4, 7, false);
	sequencer2.setValue<bool>(4, 8, true);
	sequencer2.setValue<bool>(4, 9, false);

	sequencer2.setValue<bool>(5, 0, false);
	sequencer2.setValue<bool>(5, 1, false);
	sequencer2.setValue<bool>(5, 2, false);
	sequencer2.setValue<bool>(5, 3, true);
	sequencer2.setValue<bool>(5, 4, false);
	sequencer2.setValue<bool>(5, 5, false);
	sequencer2.setValue<bool>(5, 6, false);
	sequencer2.setValue<bool>(5, 7, false);
	sequencer2.setValue<bool>(5, 8, true);
	sequencer2.setValue<bool>(5, 9, false);

	sequencer2.setValue<bool>(6, 0, false);
	sequencer2.setValue<bool>(6, 1, false);
	sequencer2.setValue<bool>(6, 2, false);
	sequencer2.setValue<bool>(6, 3, true);
	sequencer2.setValue<bool>(6, 4, false);
	sequencer2.setValue<bool>(6, 5, false);
	sequencer2.setValue<bool>(6, 6, false);
	sequencer2.setValue<bool>(6, 7, false);
	sequencer2.setValue<bool>(6, 8, true);
	sequencer2.setValue<bool>(6, 9, false);

	sequencer2.setValue<bool>(7, 0, false);
	sequencer2.setValue<bool>(7, 1, false);
	sequencer2.setValue<bool>(7, 2, false);
	sequencer2.setValue<bool>(7, 3, true);
	sequencer2.setValue<bool>(7, 4, false);
	sequencer2.setValue<bool>(7, 5, false);
	sequencer2.setValue<bool>(7, 6, false);
	sequencer2.setValue<bool>(7, 7, false);
	sequencer2.setValue<bool>(7, 8, true);
	sequencer2.setValue<bool>(7, 9, false);

	sequencer2.setValue<bool>(8, 0, false);
	sequencer2.setValue<bool>(8, 1, false);
	sequencer2.setValue<bool>(8, 2, false);
	sequencer2.setValue<bool>(8, 3, true);
	sequencer2.setValue<bool>(8, 4, false);
	sequencer2.setValue<bool>(8, 5, false);
	sequencer2.setValue<bool>(8, 6, false);
	sequencer2.setValue<bool>(8, 7, false);
	sequencer2.setValue<bool>(8, 8, true);
	sequencer2.setValue<bool>(8, 9, false);
	//---------------------------------------

	//cruz centro
	sequencer2.setValue<bool>(9, 0, false);
	sequencer2.setValue<bool>(9, 1, false);
	sequencer2.setValue<bool>(9, 2, true);
	sequencer2.setValue<bool>(9, 3, false);
	sequencer2.setValue<bool>(9, 4, false);
	sequencer2.setValue<bool>(9, 5, false);
	sequencer2.setValue<bool>(9, 6, false);
	sequencer2.setValue<bool>(9, 7, true);
	sequencer2.setValue<bool>(9, 8, false);
	sequencer2.setValue<bool>(9, 9, false);

	sequencer2.setValue<bool>(10, 0, false);
	sequencer2.setValue<bool>(10, 1, false);
	sequencer2.setValue<bool>(10, 2, true);
	sequencer2.setValue<bool>(10, 3, false);
	sequencer2.setValue<bool>(10, 4, false);
	sequencer2.setValue<bool>(10, 5, false);
	sequencer2.setValue<bool>(10, 6, false);
	sequencer2.setValue<bool>(10, 7, true);
	sequencer2.setValue<bool>(10, 8, false);
	sequencer2.setValue<bool>(10, 9, false);

	sequencer2.setValue<bool>(11, 0, false);
	sequencer2.setValue<bool>(11, 1, false);
	sequencer2.setValue<bool>(11, 2, true);
	sequencer2.setValue<bool>(11, 3, false);
	sequencer2.setValue<bool>(11, 4, false);
	sequencer2.setValue<bool>(11, 5, false);
	sequencer2.setValue<bool>(11, 6, false);
	sequencer2.setValue<bool>(11, 7, true);
	sequencer2.setValue<bool>(11, 8, false);
	sequencer2.setValue<bool>(11, 9, false);

	sequencer2.setValue<bool>(12, 0, false);
	sequencer2.setValue<bool>(12, 1, false);
	sequencer2.setValue<bool>(12, 2, true);
	sequencer2.setValue<bool>(12, 3, false);
	sequencer2.setValue<bool>(12, 4, false);
	sequencer2.setValue<bool>(12, 5, false);
	sequencer2.setValue<bool>(12, 6, false);
	sequencer2.setValue<bool>(12, 7, true);
	sequencer2.setValue<bool>(12, 8, false);
	sequencer2.setValue<bool>(12, 9, false);

	sequencer2.setValue<bool>(13, 0, false);
	sequencer2.setValue<bool>(13, 1, false);
	sequencer2.setValue<bool>(13, 2, true);
	sequencer2.setValue<bool>(13, 3, false);
	sequencer2.setValue<bool>(13, 4, false);
	sequencer2.setValue<bool>(13, 5, false);
	sequencer2.setValue<bool>(13, 6, false);
	sequencer2.setValue<bool>(13, 7, true);
	sequencer2.setValue<bool>(13, 8, false);
	sequencer2.setValue<bool>(13, 9, false);
	//---------------------------------------

	//L derecha
	sequencer2.setValue<bool>(14, 0, false);
	sequencer2.setValue<bool>(14, 1, true);
	sequencer2.setValue<bool>(14, 2, false);
	sequencer2.setValue<bool>(14, 3, false);
	sequencer2.setValue<bool>(14, 4, false);
	sequencer2.setValue<bool>(14, 5, false);
	sequencer2.setValue<bool>(14, 6, true);
	sequencer2.setValue<bool>(14, 7, false);
	sequencer2.setValue<bool>(14, 8, false);
	sequencer2.setValue<bool>(14, 9, false);

	sequencer2.setValue<bool>(15, 0, false);
	sequencer2.setValue<bool>(15, 1, true);
	sequencer2.setValue<bool>(15, 2, false);
	sequencer2.setValue<bool>(15, 3, false);
	sequencer2.setValue<bool>(15, 4, false);
	sequencer2.setValue<bool>(15, 5, false);
	sequencer2.setValue<bool>(15, 6, true);
	sequencer2.setValue<bool>(15, 7, false);
	sequencer2.setValue<bool>(15, 8, false);
	sequencer2.setValue<bool>(15, 9, false);

	sequencer2.setValue<bool>(16, 0, false);
	sequencer2.setValue<bool>(16, 1, true);
	sequencer2.setValue<bool>(16, 2, false);
	sequencer2.setValue<bool>(16, 3, false);
	sequencer2.setValue<bool>(16, 4, false);
	sequencer2.setValue<bool>(16, 5, false);
	sequencer2.setValue<bool>(16, 6, true);
	sequencer2.setValue<bool>(16, 7, false);
	sequencer2.setValue<bool>(16, 8, false);
	sequencer2.setValue<bool>(16, 9, false);

	sequencer2.setValue<bool>(17, 0, false);
	sequencer2.setValue<bool>(17, 1, true);
	sequencer2.setValue<bool>(17, 2, false);
	sequencer2.setValue<bool>(17, 3, false);
	sequencer2.setValue<bool>(17, 4, false);
	sequencer2.setValue<bool>(17, 5, false);
	sequencer2.setValue<bool>(17, 6, true);
	sequencer2.setValue<bool>(17, 7, false);
	sequencer2.setValue<bool>(17, 8, false);
	sequencer2.setValue<bool>(17, 9, false);

	sequencer2.setValue<bool>(18, 0, false);
	sequencer2.setValue<bool>(18, 1, true);
	sequencer2.setValue<bool>(18, 2, false);
	sequencer2.setValue<bool>(18, 3, false);
	sequencer2.setValue<bool>(18, 4, false);
	sequencer2.setValue<bool>(18, 5, false);
	sequencer2.setValue<bool>(18, 6, true);
	sequencer2.setValue<bool>(18, 7, false);
	sequencer2.setValue<bool>(18, 8, false);
	sequencer2.setValue<bool>(18, 9, false);

	sequencer2.setValue<bool>(19, 0, false);
	sequencer2.setValue<bool>(19, 1, true);
	sequencer2.setValue<bool>(19, 2, false);
	sequencer2.setValue<bool>(19, 3, false);
	sequencer2.setValue<bool>(19, 4, false);
	sequencer2.setValue<bool>(19, 5, false);
	sequencer2.setValue<bool>(19, 6, true);
	sequencer2.setValue<bool>(19, 7, false);
	sequencer2.setValue<bool>(19, 8, false);
	sequencer2.setValue<bool>(19, 9, false);
	//---------------------------------------

	//totem derecho
	sequencer2.setValue<bool>(20, 0, true);
	sequencer2.setValue<bool>(20, 1, false);
	sequencer2.setValue<bool>(20, 2, false);
	sequencer2.setValue<bool>(20, 3, false);
	sequencer2.setValue<bool>(20, 4, false);
	sequencer2.setValue<bool>(20, 5, true);
	sequencer2.setValue<bool>(20, 6, false);
	sequencer2.setValue<bool>(20, 7, false);
	sequencer2.setValue<bool>(20, 8, false);
	sequencer2.setValue<bool>(20, 9, false);

	sequencer2.setValue<bool>(21, 0, true);
	sequencer2.setValue<bool>(21, 1, false);
	sequencer2.setValue<bool>(21, 2, false);
	sequencer2.setValue<bool>(21, 3, false);
	sequencer2.setValue<bool>(21, 4, false);
	sequencer2.setValue<bool>(21, 5, true);
	sequencer2.setValue<bool>(21, 6, false);
	sequencer2.setValue<bool>(21, 7, false);
	sequencer2.setValue<bool>(21, 8, false);
	sequencer2.setValue<bool>(21, 9, false);

	sequencer2.setValue<bool>(22, 0, true);
	sequencer2.setValue<bool>(22, 1, false);
	sequencer2.setValue<bool>(22, 2, false);
	sequencer2.setValue<bool>(22, 3, false);
	sequencer2.setValue<bool>(22, 4, false);
	sequencer2.setValue<bool>(22, 5, true);
	sequencer2.setValue<bool>(22, 6, false);
	sequencer2.setValue<bool>(22, 7, false);
	sequencer2.setValue<bool>(22, 8, false);
	sequencer2.setValue<bool>(22, 9, false);
}
void ofApp::barrido_up() {
	//totem izq
	sequencer2.setValue<bool>(0, 0, false);
	sequencer2.setValue<bool>(0, 1, false);
	sequencer2.setValue<bool>(0, 2, false);
	sequencer2.setValue<bool>(0, 3, false);
	sequencer2.setValue<bool>(0, 4, true);
	sequencer2.setValue<bool>(0, 5, false);
	sequencer2.setValue<bool>(0, 6, false);
	sequencer2.setValue<bool>(0, 7, false);
	sequencer2.setValue<bool>(0, 8, false);
	sequencer2.setValue<bool>(0, 9, true);

	sequencer2.setValue<bool>(1, 0, false);
	sequencer2.setValue<bool>(1, 1, false);
	sequencer2.setValue<bool>(1, 2, false);
	sequencer2.setValue<bool>(1, 3, false);
	sequencer2.setValue<bool>(1, 4, true);
	sequencer2.setValue<bool>(1, 5, false);
	sequencer2.setValue<bool>(1, 6, false);
	sequencer2.setValue<bool>(1, 7, false);
	sequencer2.setValue<bool>(1, 8, false);
	sequencer2.setValue<bool>(1, 9, true);

	sequencer2.setValue<bool>(2, 0, false);
	sequencer2.setValue<bool>(2, 1, false);
	sequencer2.setValue<bool>(2, 2, false);
	sequencer2.setValue<bool>(2, 3, false);
	sequencer2.setValue<bool>(2, 4, true);
	sequencer2.setValue<bool>(2, 5, false);
	sequencer2.setValue<bool>(2, 6, false);
	sequencer2.setValue<bool>(2, 7, false);
	sequencer2.setValue<bool>(2, 8, false);
	sequencer2.setValue<bool>(2, 9, true);
	//---------------------------------------

	//L izq
	sequencer2.setValue<bool>(3, 0, false);
	sequencer2.setValue<bool>(3, 1, false);
	sequencer2.setValue<bool>(3, 2, false);
	sequencer2.setValue<bool>(3, 3, true);
	sequencer2.setValue<bool>(3, 4, false);
	sequencer2.setValue<bool>(3, 5, false);
	sequencer2.setValue<bool>(3, 6, false);
	sequencer2.setValue<bool>(3, 7, false);
	sequencer2.setValue<bool>(3, 8, true);
	sequencer2.setValue<bool>(3, 9, false);

	sequencer2.setValue<bool>(4, 0, false);
	sequencer2.setValue<bool>(4, 1, false);
	sequencer2.setValue<bool>(4, 2, false);
	sequencer2.setValue<bool>(4, 3, true);
	sequencer2.setValue<bool>(4, 4, false);
	sequencer2.setValue<bool>(4, 5, false);
	sequencer2.setValue<bool>(4, 6, false);
	sequencer2.setValue<bool>(4, 7, false);
	sequencer2.setValue<bool>(4, 8, true);
	sequencer2.setValue<bool>(4, 9, false);

	sequencer2.setValue<bool>(5, 0, false);
	sequencer2.setValue<bool>(5, 1, false);
	sequencer2.setValue<bool>(5, 2, false);
	sequencer2.setValue<bool>(5, 3, true);
	sequencer2.setValue<bool>(5, 4, false);
	sequencer2.setValue<bool>(5, 5, false);
	sequencer2.setValue<bool>(5, 6, false);
	sequencer2.setValue<bool>(5, 7, false);
	sequencer2.setValue<bool>(5, 8, true);
	sequencer2.setValue<bool>(5, 9, false);

	sequencer2.setValue<bool>(6, 0, false);
	sequencer2.setValue<bool>(6, 1, false);
	sequencer2.setValue<bool>(6, 2, false);
	sequencer2.setValue<bool>(6, 3, true);
	sequencer2.setValue<bool>(6, 4, false);
	sequencer2.setValue<bool>(6, 5, false);
	sequencer2.setValue<bool>(6, 6, false);
	sequencer2.setValue<bool>(6, 7, false);
	sequencer2.setValue<bool>(6, 8, true);
	sequencer2.setValue<bool>(6, 9, false);

	sequencer2.setValue<bool>(7, 0, false);
	sequencer2.setValue<bool>(7, 1, false);
	sequencer2.setValue<bool>(7, 2, false);
	sequencer2.setValue<bool>(7, 3, true);
	sequencer2.setValue<bool>(7, 4, false);
	sequencer2.setValue<bool>(7, 5, false);
	sequencer2.setValue<bool>(7, 6, false);
	sequencer2.setValue<bool>(7, 7, false);
	sequencer2.setValue<bool>(7, 8, true);
	sequencer2.setValue<bool>(7, 9, false);

	sequencer2.setValue<bool>(8, 0, false);
	sequencer2.setValue<bool>(8, 1, false);
	sequencer2.setValue<bool>(8, 2, false);
	sequencer2.setValue<bool>(8, 3, true);
	sequencer2.setValue<bool>(8, 4, false);
	sequencer2.setValue<bool>(8, 5, false);
	sequencer2.setValue<bool>(8, 6, false);
	sequencer2.setValue<bool>(8, 7, false);
	sequencer2.setValue<bool>(8, 8, true);
	sequencer2.setValue<bool>(8, 9, false);
	//---------------------------------------

	//cruz centro
	sequencer2.setValue<bool>(9, 0, false);
	sequencer2.setValue<bool>(9, 1, false);
	sequencer2.setValue<bool>(9, 2, true);
	sequencer2.setValue<bool>(9, 3, false);
	sequencer2.setValue<bool>(9, 4, false);
	sequencer2.setValue<bool>(9, 5, false);
	sequencer2.setValue<bool>(9, 6, false);
	sequencer2.setValue<bool>(9, 7, true);
	sequencer2.setValue<bool>(9, 8, false);
	sequencer2.setValue<bool>(9, 9, false);

	sequencer2.setValue<bool>(10, 0, false);
	sequencer2.setValue<bool>(10, 1, false);
	sequencer2.setValue<bool>(10, 2, true);
	sequencer2.setValue<bool>(10, 3, false);
	sequencer2.setValue<bool>(10, 4, false);
	sequencer2.setValue<bool>(10, 5, false);
	sequencer2.setValue<bool>(10, 6, false);
	sequencer2.setValue<bool>(10, 7, true);
	sequencer2.setValue<bool>(10, 8, false);
	sequencer2.setValue<bool>(10, 9, false);

	sequencer2.setValue<bool>(11, 0, false);
	sequencer2.setValue<bool>(11, 1, false);
	sequencer2.setValue<bool>(11, 2, true);
	sequencer2.setValue<bool>(11, 3, false);
	sequencer2.setValue<bool>(11, 4, false);
	sequencer2.setValue<bool>(11, 5, false);
	sequencer2.setValue<bool>(11, 6, false);
	sequencer2.setValue<bool>(11, 7, true);
	sequencer2.setValue<bool>(11, 8, false);
	sequencer2.setValue<bool>(11, 9, false);

	sequencer2.setValue<bool>(12, 0, false);
	sequencer2.setValue<bool>(12, 1, false);
	sequencer2.setValue<bool>(12, 2, true);
	sequencer2.setValue<bool>(12, 3, false);
	sequencer2.setValue<bool>(12, 4, false);
	sequencer2.setValue<bool>(12, 5, false);
	sequencer2.setValue<bool>(12, 6, false);
	sequencer2.setValue<bool>(12, 7, true);
	sequencer2.setValue<bool>(12, 8, false);
	sequencer2.setValue<bool>(12, 9, false);

	sequencer2.setValue<bool>(13, 0, false);
	sequencer2.setValue<bool>(13, 1, false);
	sequencer2.setValue<bool>(13, 2, true);
	sequencer2.setValue<bool>(13, 3, false);
	sequencer2.setValue<bool>(13, 4, false);
	sequencer2.setValue<bool>(13, 5, false);
	sequencer2.setValue<bool>(13, 6, false);
	sequencer2.setValue<bool>(13, 7, true);
	sequencer2.setValue<bool>(13, 8, false);
	sequencer2.setValue<bool>(13, 9, false);
	//---------------------------------------

	//L derecha
	sequencer2.setValue<bool>(14, 0, false);
	sequencer2.setValue<bool>(14, 1, true);
	sequencer2.setValue<bool>(14, 2, false);
	sequencer2.setValue<bool>(14, 3, false);
	sequencer2.setValue<bool>(14, 4, false);
	sequencer2.setValue<bool>(14, 5, false);
	sequencer2.setValue<bool>(14, 6, true);
	sequencer2.setValue<bool>(14, 7, false);
	sequencer2.setValue<bool>(14, 8, false);
	sequencer2.setValue<bool>(14, 9, false);

	sequencer2.setValue<bool>(15, 0, false);
	sequencer2.setValue<bool>(15, 1, true);
	sequencer2.setValue<bool>(15, 2, false);
	sequencer2.setValue<bool>(15, 3, false);
	sequencer2.setValue<bool>(15, 4, false);
	sequencer2.setValue<bool>(15, 5, false);
	sequencer2.setValue<bool>(15, 6, true);
	sequencer2.setValue<bool>(15, 7, false);
	sequencer2.setValue<bool>(15, 8, false);
	sequencer2.setValue<bool>(15, 9, false);

	sequencer2.setValue<bool>(16, 0, false);
	sequencer2.setValue<bool>(16, 1, true);
	sequencer2.setValue<bool>(16, 2, false);
	sequencer2.setValue<bool>(16, 3, false);
	sequencer2.setValue<bool>(16, 4, false);
	sequencer2.setValue<bool>(16, 5, false);
	sequencer2.setValue<bool>(16, 6, true);
	sequencer2.setValue<bool>(16, 7, false);
	sequencer2.setValue<bool>(16, 8, false);
	sequencer2.setValue<bool>(16, 9, false);

	sequencer2.setValue<bool>(17, 0, false);
	sequencer2.setValue<bool>(17, 1, true);
	sequencer2.setValue<bool>(17, 2, false);
	sequencer2.setValue<bool>(17, 3, false);
	sequencer2.setValue<bool>(17, 4, false);
	sequencer2.setValue<bool>(17, 5, false);
	sequencer2.setValue<bool>(17, 6, true);
	sequencer2.setValue<bool>(17, 7, false);
	sequencer2.setValue<bool>(17, 8, false);
	sequencer2.setValue<bool>(17, 9, false);

	sequencer2.setValue<bool>(18, 0, false);
	sequencer2.setValue<bool>(18, 1, true);
	sequencer2.setValue<bool>(18, 2, false);
	sequencer2.setValue<bool>(18, 3, false);
	sequencer2.setValue<bool>(18, 4, false);
	sequencer2.setValue<bool>(18, 5, false);
	sequencer2.setValue<bool>(18, 6, true);
	sequencer2.setValue<bool>(18, 7, false);
	sequencer2.setValue<bool>(18, 8, false);
	sequencer2.setValue<bool>(18, 9, false);

	sequencer2.setValue<bool>(19, 0, false);
	sequencer2.setValue<bool>(19, 1, true);
	sequencer2.setValue<bool>(19, 2, false);
	sequencer2.setValue<bool>(19, 3, false);
	sequencer2.setValue<bool>(19, 4, false);
	sequencer2.setValue<bool>(19, 5, false);
	sequencer2.setValue<bool>(19, 6, true);
	sequencer2.setValue<bool>(19, 7, false);
	sequencer2.setValue<bool>(19, 8, false);
	sequencer2.setValue<bool>(19, 9, false);
	//---------------------------------------

	//totem derecho
	sequencer2.setValue<bool>(20, 0, true);
	sequencer2.setValue<bool>(20, 1, false);
	sequencer2.setValue<bool>(20, 2, false);
	sequencer2.setValue<bool>(20, 3, false);
	sequencer2.setValue<bool>(20, 4, false);
	sequencer2.setValue<bool>(20, 5, true);
	sequencer2.setValue<bool>(20, 6, false);
	sequencer2.setValue<bool>(20, 7, false);
	sequencer2.setValue<bool>(20, 8, false);
	sequencer2.setValue<bool>(20, 9, false);

	sequencer2.setValue<bool>(21, 0, true);
	sequencer2.setValue<bool>(21, 1, false);
	sequencer2.setValue<bool>(21, 2, false);
	sequencer2.setValue<bool>(21, 3, false);
	sequencer2.setValue<bool>(21, 4, false);
	sequencer2.setValue<bool>(21, 5, true);
	sequencer2.setValue<bool>(21, 6, false);
	sequencer2.setValue<bool>(21, 7, false);
	sequencer2.setValue<bool>(21, 8, false);
	sequencer2.setValue<bool>(21, 9, false);

	sequencer2.setValue<bool>(22, 0, true);
	sequencer2.setValue<bool>(22, 1, false);
	sequencer2.setValue<bool>(22, 2, false);
	sequencer2.setValue<bool>(22, 3, false);
	sequencer2.setValue<bool>(22, 4, false);
	sequencer2.setValue<bool>(22, 5, true);
	sequencer2.setValue<bool>(22, 6, false);
	sequencer2.setValue<bool>(22, 7, false);
	sequencer2.setValue<bool>(22, 8, false);
	sequencer2.setValue<bool>(22, 9, false);
}
void ofApp::barrido_der() {
	//totem izq
	sequencer2.setValue<bool>(0, 0, true);
	sequencer2.setValue<bool>(0, 1, false);
	sequencer2.setValue<bool>(0, 2, false);
	sequencer2.setValue<bool>(0, 3, false);
	sequencer2.setValue<bool>(0, 4, false);
	sequencer2.setValue<bool>(0, 5, true);
	sequencer2.setValue<bool>(0, 6, false);
	sequencer2.setValue<bool>(0, 7, false);
	sequencer2.setValue<bool>(0, 8, false);
	sequencer2.setValue<bool>(0, 9, false);

	sequencer2.setValue<bool>(1, 0, true);
	sequencer2.setValue<bool>(1, 1, false);
	sequencer2.setValue<bool>(1, 2, false);
	sequencer2.setValue<bool>(1, 3, false);
	sequencer2.setValue<bool>(1, 4, false);
	sequencer2.setValue<bool>(1, 5, true);
	sequencer2.setValue<bool>(1, 6, false);
	sequencer2.setValue<bool>(1, 7, false);
	sequencer2.setValue<bool>(1, 8, false);
	sequencer2.setValue<bool>(1, 9, false);

	sequencer2.setValue<bool>(2, 0, true);
	sequencer2.setValue<bool>(2, 1, false);
	sequencer2.setValue<bool>(2, 2, false);
	sequencer2.setValue<bool>(2, 3, false);
	sequencer2.setValue<bool>(2, 4, false);
	sequencer2.setValue<bool>(2, 5, true);
	sequencer2.setValue<bool>(2, 6, false);
	sequencer2.setValue<bool>(2, 7, false);
	sequencer2.setValue<bool>(2, 8, false);
	sequencer2.setValue<bool>(2, 9, false);
	//---------------------------------------

	//L izq
	sequencer2.setValue<bool>(3, 0, false);
	sequencer2.setValue<bool>(3, 1, true);
	sequencer2.setValue<bool>(3, 2, false);
	sequencer2.setValue<bool>(3, 3, false);
	sequencer2.setValue<bool>(3, 4, false);
	sequencer2.setValue<bool>(3, 5, false);
	sequencer2.setValue<bool>(3, 6, true);
	sequencer2.setValue<bool>(3, 7, false);
	sequencer2.setValue<bool>(3, 8, false);
	sequencer2.setValue<bool>(3, 9, false);

	sequencer2.setValue<bool>(4, 0, false);
	sequencer2.setValue<bool>(4, 1, true);
	sequencer2.setValue<bool>(4, 2, false);
	sequencer2.setValue<bool>(4, 3, false);
	sequencer2.setValue<bool>(4, 4, false);
	sequencer2.setValue<bool>(4, 5, false);
	sequencer2.setValue<bool>(4, 6, true);
	sequencer2.setValue<bool>(4, 7, false);
	sequencer2.setValue<bool>(4, 8, false);
	sequencer2.setValue<bool>(4, 9, false);

	sequencer2.setValue<bool>(5, 0, false);
	sequencer2.setValue<bool>(5, 1, true);
	sequencer2.setValue<bool>(5, 2, false);
	sequencer2.setValue<bool>(5, 3, false);
	sequencer2.setValue<bool>(5, 4, false);
	sequencer2.setValue<bool>(5, 5, false);
	sequencer2.setValue<bool>(5, 6, true);
	sequencer2.setValue<bool>(5, 7, false);
	sequencer2.setValue<bool>(5, 8, false);
	sequencer2.setValue<bool>(5, 9, false);

	sequencer2.setValue<bool>(6, 0, false);
	sequencer2.setValue<bool>(6, 1, true);
	sequencer2.setValue<bool>(6, 2, false);
	sequencer2.setValue<bool>(6, 3, false);
	sequencer2.setValue<bool>(6, 4, false);
	sequencer2.setValue<bool>(6, 5, false);
	sequencer2.setValue<bool>(6, 6, true);
	sequencer2.setValue<bool>(6, 7, false);
	sequencer2.setValue<bool>(6, 8, false);
	sequencer2.setValue<bool>(6, 9, false);

	sequencer2.setValue<bool>(7, 0, false);
	sequencer2.setValue<bool>(7, 1, true);
	sequencer2.setValue<bool>(7, 2, false);
	sequencer2.setValue<bool>(7, 3, false);
	sequencer2.setValue<bool>(7, 4, false);
	sequencer2.setValue<bool>(7, 5, false);
	sequencer2.setValue<bool>(7, 6, true);
	sequencer2.setValue<bool>(7, 7, false);
	sequencer2.setValue<bool>(7, 8, false);
	sequencer2.setValue<bool>(7, 9, false);

	sequencer2.setValue<bool>(8, 0, false);
	sequencer2.setValue<bool>(8, 1, true);
	sequencer2.setValue<bool>(8, 2, false);
	sequencer2.setValue<bool>(8, 3, false);
	sequencer2.setValue<bool>(8, 4, false);
	sequencer2.setValue<bool>(8, 5, false);
	sequencer2.setValue<bool>(8, 6, true);
	sequencer2.setValue<bool>(8, 7, false);
	sequencer2.setValue<bool>(8, 8, false);
	sequencer2.setValue<bool>(8, 9, false);
	//---------------------------------------

	//cruz centro
	sequencer2.setValue<bool>(9, 0, false);
	sequencer2.setValue<bool>(9, 1, false);
	sequencer2.setValue<bool>(9, 2, true);
	sequencer2.setValue<bool>(9, 3, false);
	sequencer2.setValue<bool>(9, 4, false);
	sequencer2.setValue<bool>(9, 5, false);
	sequencer2.setValue<bool>(9, 6, false);
	sequencer2.setValue<bool>(9, 7, true);
	sequencer2.setValue<bool>(9, 8, false);
	sequencer2.setValue<bool>(9, 9, false);

	sequencer2.setValue<bool>(10, 0, false);
	sequencer2.setValue<bool>(10, 1, false);
	sequencer2.setValue<bool>(10, 2, true);
	sequencer2.setValue<bool>(10, 3, false);
	sequencer2.setValue<bool>(10, 4, false);
	sequencer2.setValue<bool>(10, 5, false);
	sequencer2.setValue<bool>(10, 6, false);
	sequencer2.setValue<bool>(10, 7, true);
	sequencer2.setValue<bool>(10, 8, false);
	sequencer2.setValue<bool>(10, 9, false);

	sequencer2.setValue<bool>(11, 0, false);
	sequencer2.setValue<bool>(11, 1, false);
	sequencer2.setValue<bool>(11, 2, true);
	sequencer2.setValue<bool>(11, 3, false);
	sequencer2.setValue<bool>(11, 4, false);
	sequencer2.setValue<bool>(11, 5, false);
	sequencer2.setValue<bool>(11, 6, false);
	sequencer2.setValue<bool>(11, 7, true);
	sequencer2.setValue<bool>(11, 8, false);
	sequencer2.setValue<bool>(11, 9, false);

	sequencer2.setValue<bool>(12, 0, false);
	sequencer2.setValue<bool>(12, 1, false);
	sequencer2.setValue<bool>(12, 2, true);
	sequencer2.setValue<bool>(12, 3, false);
	sequencer2.setValue<bool>(12, 4, false);
	sequencer2.setValue<bool>(12, 5, false);
	sequencer2.setValue<bool>(12, 6, false);
	sequencer2.setValue<bool>(12, 7, true);
	sequencer2.setValue<bool>(12, 8, false);
	sequencer2.setValue<bool>(12, 9, false);

	sequencer2.setValue<bool>(13, 0, false);
	sequencer2.setValue<bool>(13, 1, false);
	sequencer2.setValue<bool>(13, 2, true);
	sequencer2.setValue<bool>(13, 3, false);
	sequencer2.setValue<bool>(13, 4, false);
	sequencer2.setValue<bool>(13, 5, false);
	sequencer2.setValue<bool>(13, 6, false);
	sequencer2.setValue<bool>(13, 7, true);
	sequencer2.setValue<bool>(13, 8, false);
	sequencer2.setValue<bool>(13, 9, false);
	//---------------------------------------

	//L derecha
	sequencer2.setValue<bool>(14, 0, false);
	sequencer2.setValue<bool>(14, 1, false);
	sequencer2.setValue<bool>(14, 2, false);
	sequencer2.setValue<bool>(14, 3, true);
	sequencer2.setValue<bool>(14, 4, false);
	sequencer2.setValue<bool>(14, 5, false);
	sequencer2.setValue<bool>(14, 6, false);
	sequencer2.setValue<bool>(14, 7, false);
	sequencer2.setValue<bool>(14, 8, true);
	sequencer2.setValue<bool>(14, 9, false);

	sequencer2.setValue<bool>(15, 0, false);
	sequencer2.setValue<bool>(15, 1, false);
	sequencer2.setValue<bool>(15, 2, false);
	sequencer2.setValue<bool>(15, 3, true);
	sequencer2.setValue<bool>(15, 4, false);
	sequencer2.setValue<bool>(15, 5, false);
	sequencer2.setValue<bool>(15, 6, false);
	sequencer2.setValue<bool>(15, 7, false);
	sequencer2.setValue<bool>(15, 8, true);
	sequencer2.setValue<bool>(15, 9, false);

	sequencer2.setValue<bool>(16, 0, false);
	sequencer2.setValue<bool>(16, 1, false);
	sequencer2.setValue<bool>(16, 2, false);
	sequencer2.setValue<bool>(16, 3, true);
	sequencer2.setValue<bool>(16, 4, false);
	sequencer2.setValue<bool>(16, 5, false);
	sequencer2.setValue<bool>(16, 6, false);
	sequencer2.setValue<bool>(16, 7, false);
	sequencer2.setValue<bool>(16, 8, true);
	sequencer2.setValue<bool>(16, 9, false);

	sequencer2.setValue<bool>(17, 0, false);
	sequencer2.setValue<bool>(17, 1, false);
	sequencer2.setValue<bool>(17, 2, false);
	sequencer2.setValue<bool>(17, 3, true);
	sequencer2.setValue<bool>(17, 4, false);
	sequencer2.setValue<bool>(17, 5, false);
	sequencer2.setValue<bool>(17, 6, false);
	sequencer2.setValue<bool>(17, 7, false);
	sequencer2.setValue<bool>(17, 8, true);
	sequencer2.setValue<bool>(17, 9, false);

	sequencer2.setValue<bool>(18, 0, false);
	sequencer2.setValue<bool>(18, 1, false);
	sequencer2.setValue<bool>(18, 2, false);
	sequencer2.setValue<bool>(18, 3, true);
	sequencer2.setValue<bool>(18, 4, false);
	sequencer2.setValue<bool>(18, 5, false);
	sequencer2.setValue<bool>(18, 6, false);
	sequencer2.setValue<bool>(18, 7, false);
	sequencer2.setValue<bool>(18, 8, true);
	sequencer2.setValue<bool>(18, 9, false);

	sequencer2.setValue<bool>(19, 0, false);
	sequencer2.setValue<bool>(19, 1, false);
	sequencer2.setValue<bool>(19, 2, false);
	sequencer2.setValue<bool>(19, 3, true);
	sequencer2.setValue<bool>(19, 4, false);
	sequencer2.setValue<bool>(19, 5, false);
	sequencer2.setValue<bool>(19, 6, false);
	sequencer2.setValue<bool>(19, 7, false);
	sequencer2.setValue<bool>(19, 8, true);
	sequencer2.setValue<bool>(19, 9, false);
	//---------------------------------------

	//totem derecho
	sequencer2.setValue<bool>(20, 0, false);
	sequencer2.setValue<bool>(20, 1, false);
	sequencer2.setValue<bool>(20, 2, false);
	sequencer2.setValue<bool>(20, 3, false);
	sequencer2.setValue<bool>(20, 4, true);
	sequencer2.setValue<bool>(20, 5, false);
	sequencer2.setValue<bool>(20, 6, false);
	sequencer2.setValue<bool>(20, 7, false);
	sequencer2.setValue<bool>(20, 8, false);
	sequencer2.setValue<bool>(20, 9, true);

	sequencer2.setValue<bool>(21, 0, false);
	sequencer2.setValue<bool>(21, 1, false);
	sequencer2.setValue<bool>(21, 2, false);
	sequencer2.setValue<bool>(21, 3, false);
	sequencer2.setValue<bool>(21, 4, true);
	sequencer2.setValue<bool>(21, 5, false);
	sequencer2.setValue<bool>(21, 6, false);
	sequencer2.setValue<bool>(21, 7, false);
	sequencer2.setValue<bool>(21, 8, false);
	sequencer2.setValue<bool>(21, 9, true);

	sequencer2.setValue<bool>(22, 0, false);
	sequencer2.setValue<bool>(22, 1, false);
	sequencer2.setValue<bool>(22, 2, false);
	sequencer2.setValue<bool>(22, 3, false);
	sequencer2.setValue<bool>(22, 4, true);
	sequencer2.setValue<bool>(22, 5, false);
	sequencer2.setValue<bool>(22, 6, false);
	sequencer2.setValue<bool>(22, 7, false);
	sequencer2.setValue<bool>(22, 8, false);
	sequencer2.setValue<bool>(22, 9, true);
}
void ofApp::randoms() {
	//totem izq
	sequencer2.setValue<bool>(0, 0, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(0, 1, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(0, 2, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(0, 3, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(0, 4, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(0, 5, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(0, 6, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(0, 7, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(0, 8, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(0, 9, ofInRange(ofRandomf(), 0, 1));

	sequencer2.setValue<bool>(1, 0, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(1, 1, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(1, 2, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(1, 3, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(1, 4, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(1, 5, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(1, 6, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(1, 7, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(1, 8, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(1, 9, ofInRange(ofRandomf(), 0, 1));

	sequencer2.setValue<bool>(2, 0, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(2, 1, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(2, 2, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(2, 3, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(2, 4, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(2, 5, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(2, 6, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(2, 7, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(2, 8, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(2, 9, ofInRange(ofRandomf(), 0, 1));
	//---------------------------------------

	//L izq
	sequencer2.setValue<bool>(3, 0, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(3, 1, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(3, 2, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(3, 3, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(3, 4, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(3, 5, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(3, 6, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(3, 7, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(3, 8, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(3, 9, ofInRange(ofRandomf(), 0, 1));

	sequencer2.setValue<bool>(4, 0, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(4, 1, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(4, 2, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(4, 3, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(4, 4, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(4, 5, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(4, 6, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(4, 7, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(4, 8, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(4, 9, ofInRange(ofRandomf(), 0, 1));

	sequencer2.setValue<bool>(5, 0, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(5, 1, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(5, 2, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(5, 3, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(5, 4, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(5, 5, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(5, 6, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(5, 7, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(5, 8, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(5, 9, ofInRange(ofRandomf(), 0, 1));

	sequencer2.setValue<bool>(6, 0, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(6, 1, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(6, 2, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(6, 3, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(6, 4, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(6, 5, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(6, 6, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(6, 7, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(6, 8, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(6, 9, ofInRange(ofRandomf(), 0, 1));

	sequencer2.setValue<bool>(7, 0, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(7, 1, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(7, 2, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(7, 3, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(7, 4, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(7, 5, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(7, 6, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(7, 7, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(7, 8, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(7, 9, ofInRange(ofRandomf(), 0, 1));

	sequencer2.setValue<bool>(8, 0, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(8, 1, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(8, 2, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(8, 3, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(8, 4, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(8, 5, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(8, 6, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(8, 7, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(8, 8, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(8, 9, ofInRange(ofRandomf(), 0, 1));
	//---------------------------------------

	//cruz centro
	sequencer2.setValue<bool>(9, 0, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(9, 1, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(9, 2, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(9, 3, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(9, 4, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(9, 5, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(9, 6, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(9, 7, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(9, 8, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(9, 9, ofInRange(ofRandomf(), 0, 1));

	sequencer2.setValue<bool>(10, 0, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(10, 1, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(10, 2, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(10, 3, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(10, 4, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(10, 5, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(10, 6, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(10, 7, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(10, 8, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(10, 9, ofInRange(ofRandomf(), 0, 1));

	sequencer2.setValue<bool>(11, 0, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(11, 1, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(11, 2, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(11, 3, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(11, 4, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(11, 5, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(11, 6, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(11, 7, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(11, 8, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(11, 9, ofInRange(ofRandomf(), 0, 1));

	sequencer2.setValue<bool>(12, 0, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(12, 1, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(12, 2, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(12, 3, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(12, 4, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(12, 5, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(12, 6, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(12, 7, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(12, 8, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(12, 9, ofInRange(ofRandomf(), 0, 1));

	sequencer2.setValue<bool>(13, 0, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(13, 1, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(13, 2, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(13, 3, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(13, 4, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(13, 5, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(13, 6, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(13, 7, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(13, 8, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(13, 9, ofInRange(ofRandomf(), 0, 1));
	//---------------------------------------

	//L derecha
	sequencer2.setValue<bool>(14, 0, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(14, 1, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(14, 2, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(14, 3, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(14, 4, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(14, 5, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(14, 6, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(14, 7, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(14, 8, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(14, 9, ofInRange(ofRandomf(), 0, 1));

	sequencer2.setValue<bool>(15, 0, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(15, 1, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(15, 2, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(15, 3, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(15, 4, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(15, 5, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(15, 6, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(15, 7, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(15, 8, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(15, 9, ofInRange(ofRandomf(), 0, 1));

	sequencer2.setValue<bool>(16, 0, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(16, 1, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(16, 2, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(16, 3, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(16, 4, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(16, 5, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(16, 6, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(16, 7, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(16, 8, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(16, 9, ofInRange(ofRandomf(), 0, 1));

	sequencer2.setValue<bool>(17, 0, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(17, 1, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(17, 2, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(17, 3, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(17, 4, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(17, 5, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(17, 6, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(17, 7, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(17, 8, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(17, 9, ofInRange(ofRandomf(), 0, 1));

	sequencer2.setValue<bool>(18, 0, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(18, 1, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(18, 2, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(18, 3, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(18, 4, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(18, 5, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(18, 6, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(18, 7, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(18, 8, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(18, 9, ofInRange(ofRandomf(), 0, 1));

	sequencer2.setValue<bool>(19, 0, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(19, 1, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(19, 2, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(19, 3, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(19, 4, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(19, 5, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(19, 6, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(19, 7, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(19, 8, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(19, 9, ofInRange(ofRandomf(), 0, 1));
	//---------------------------------------

	//totem derecho
	sequencer2.setValue<bool>(20, 0, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(20, 1, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(20, 2, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(20, 3, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(20, 4, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(20, 5, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(20, 6, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(20, 7, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(20, 8, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(20, 9, ofInRange(ofRandomf(), 0, 1));

	sequencer2.setValue<bool>(21, 0, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(21, 1, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(21, 2, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(21, 3, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(21, 4, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(21, 5, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(21, 6, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(21, 7, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(21, 8, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(21, 9, ofInRange(ofRandomf(), 0, 1));

	sequencer2.setValue<bool>(22, 0, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(22, 1, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(22, 2, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(22, 3, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(22, 4, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(22, 5, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(22, 6, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(22, 7, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(22, 8, ofInRange(ofRandomf(), 0, 1));
	sequencer2.setValue<bool>(22, 9, ofInRange(ofRandomf(), 0, 1));
}