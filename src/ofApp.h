#pragma once

#include "ofMain.h"
#include "ofxNDI.h" // ofxNDI classes
#include "ofxSequencer.h"
#include "ofxOsc.h"

#define PORT 12345

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void exit();

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
		

		ofxNDIsender ndiSender;    // NDI sender
		char senderName[256];      // Sender name
		unsigned int senderWidth;  // Width of the sender output
		unsigned int senderHeight; // Height of the sender output
		ofFbo ndiFbo;              // Fbo used for graphics and sending
		ofImage textureImage;      // Texture image for the 3D cube graphics
		float rotX, rotY;          // Cube rotation increment
		ofImage ndiImage;          // Test image for sending
		void DrawGraphics();       // Rotating cube draw

		int rect = 168;

		ofVboMesh general[23]; //Totem derecho

		ofLight pointLight;
		ofLight pointLight2;
		ofLight pointLight3;
		ofLight ambient;

		ofMaterial material;

		ofPlanePrimitive plane;

		bool bWireframe;
		bool isAmbient;

		ofxSequencer sequencer2;

		ofParameter<bool> p[23];

		bool vals[4][23];

		ofXml xml;
		
		ofxOscReceiver receiver;
		float fBpm;
		int bpm;

		void barrido_der();
		void barrido_izq();
		void barrido_up();
		void off();
		void on();
		void randoms();

		bool isLight;

};
