/*
 * rheya/src/ofApp.cpp
 *
 * RHEYA
 * Copyright (C) 2019 - Rafa Diniz
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ofApp.h"

//Some mesh helper functions
//--------------------------------------------------------------
void ofApp::addFace(ofMesh& mesh, ofVec3f a, ofVec3f b, ofVec3f c) {
	ofVec3f normal = ((b - a).cross(c - a)).normalize();
	mesh.addNormal(normal);
	mesh.addVertex(a);
	mesh.addNormal(normal);
	mesh.addVertex(b);
	mesh.addNormal(normal);
	mesh.addVertex(c);
}

//--------------------------------------------------------------
void ofApp::addFace(ofMesh& mesh, ofVec3f a, ofVec3f b, ofVec3f c, ofVec3f d) {
	addFace(mesh, a, b, c);
	addFace(mesh, a, c, d);
}

//--------------------------------------------------------------
ofVec3f ofApp::getVertexFromImg(ofFloatImage& img, int x, int y) {
	return ofVec3f(x, y, 100 * img.getColor(x, y).getBrightness());
}


//--------------------------------------------------------------
void ofApp::setup() {

	ofSetVerticalSync(true);
	ofSetWindowShape(1300, 700);
	ofBackground(200);

	//---------primitives resolution set
	sphere.setResolution(100);
	box.setResolution(100);

	type.loadFont("lucon.ttf", 10);

	path = "rheya/rheya";

	//---------load images
	for (int i = 0; i < 407; i++) {

		if (i >= 0) {
			images[i] = path + "_0000" + ofToString(i) + ".png";
		}
		if (i >= 10) {
			images[i] = path + "_000" + ofToString(i) + ".png";
		}
		if (i >= 100) {
			images[i] = path + "_00" + ofToString(i) + ".png";
		}
		if (i >= 1000) {
			images[i] = path + "_0" + ofToString(i) + ".png";
		}
		//cout << '\n' << images[i];

	};
	
	//--------pitch controls
	pSphere.enableSmoothing(50.0f);
	pSphere.set(62);

	pRheya.enableSmoothing(50.0f);
	pRheya.set(54);


	//--------pan controls
	panControlWind.enableSmoothing(100.0f);
	panControlWind.set(0);

	panControlOcean.enableSmoothing(100.0f);
	panControlOcean.set(0);

	panControlSphere.enableSmoothing(100.0f);
	panControlSphere.set(0);

	panControlRheya.enableSmoothing(100.0f);
	panControlRheya.set(0);


	//--------filter controls
	filterControlWind.enableSmoothing(50.0f);
	filterControlWind.set(60.0f);

	filterControlOcean.enableSmoothing(50.0f);
	filterControlOcean.set(60.0f);

	filterControlSphere.enableSmoothing(50.0f);
	filterControlSphere.set(60.0f);

	filterControlRheya.enableSmoothing(50.0f);
	filterControlRheya.set(60.0f);


	//---------reverb parameters
	0.1f >> reverb.in_density();
	-0.9f >> reverb.in_damping();
	0.1f >> reverb.in_mod_amount();
	1.0f >> reverb.in_time();


	//---------route parameters
	pSphere >> synthSphere.in_pitch();
	pRheya >> synthRheya.in_pitch();

	ampControlWind >> ampWind.in_mod();
	ampControlOcean >> ampOcean.in_mod();
	ampControlSphere >> ampSphere.in_mod();
	ampControlRheya >> ampRheya.in_mod();
	
	filterControlWind >> filterWind.in_cutoff();
	filterControlOcean >> filterOcean.in_cutoff();
	filterControlSphere >> filterSphere.in_cutoff();
	filterControlRheya >> filterRheya.in_cutoff();

	panControlWind >> panWind.in_pan();
	panControlOcean >> panOcean.in_pan();
	panControlSphere >> panSphere.in_pan();
	panControlRheya >> panRheya.in_pan();


	//---------route signals generators
	synthRheya.out_saw();
	synthRheya.getSelectedOutput() >> ampRheya * dB(-24.0f) >> filterRheya.in_signal();

	noiseWind.getSelectedOutput() >> ampWind * dB(-24.0f) >> filterWind.in_signal();
	noiseOcean.getSelectedOutput() >> ampOcean * dB(-24.0f) >> filterOcean.in_signal();
	noiseSphere.getSelectedOutput() >> ampSphere * dB(-24.0f) >> filterSphere.in_signal();
	noiseRheya.getSelectedOutput() >> ampRheya * dB(-24.0f) >> filterRheya.in_signal();

	filterWind.getSelectedOutput() >> panWind.in_signal();
	filterWind.BandPass;
	1.0f >> filterWind.in_reso();

	filterOcean.getSelectedOutput() >> panOcean.in_signal();
	filterOcean.BandPass;
	1.0f >> filterOcean.in_reso();

	filterSphere.getSelectedOutput() >> reverb.getSelectedInput();
	filterSphere.BandPass;
	1.0f >> filterSphere.in_reso();

	filterRheya.getSelectedOutput() >> reverb.getSelectedInput();
	filterRheya.BandPass;
	1.0f >> filterRheya.in_reso();

	reverb.getSelectedOutput() >>  panRheya.in_signal();
	reverb.getSelectedOutput() >> panSphere.in_signal();
	//reverb.ch(1) >> amps[1] * dB(12.0f) >> panWind.in_signal();


	//-------pan to out sets 
	panWind.out_L() >> engine.audio_out(0);
	panWind.out_R() >> engine.audio_out(1);

	panOcean.out_L() >> engine.audio_out(0);
	panOcean.out_R() >> engine.audio_out(1);

	panSphere.out_L() >> engine.audio_out(0);
	panSphere.out_R() >> engine.audio_out(1);

	panRheya.out_L() >> engine.audio_out(0);
	panRheya.out_R() >> engine.audio_out(1);

	//---------engine config
	engine.listDevices();
	engine.setDeviceID(0); // REMEMBER TO SET THIS AT THE RIGHT INDEX!!!!
	engine.setup(44100, 512, 3);

	//---------initializing some globals
	sImg3 = 0;
	countF = 0;

	dif1 = 1000.0f;

	scl = 50;

	w1 = 0.0f;
	sw1 = 0.1f;
	h1 = 0.0f;
	sh1 = 0.11;

	SampOceanF = 1.0f;
	SpanOceanF = 1.0f;

	sRheyaPoint = 0.013f;
	sRheyaSize = 0.01f;

	alphaAnalyzer = 0;
	SalphaAnalyzer = 0.5f;

	randfWind = 53.0f;
	randfOcean = 71.0f;
	randfSphere = 58.0f;
    randfRheya = 58.0f;
	randpRheya = 49.0f;
}

//--------------------------------------------------------------
void ofApp::update(){

	ocean = true;
	suns = true;
	rheya = false;

	randP = ofNoise(ofMap(count, 0.0f, 1000.0f, 50.0f, 100.0f));

	//sound wind
	ampControlWind.set(randP);
	filterControlWind.set(ofMap(randP, 0.0f, 1.0f, randfWind, randfWind + 9.0f));
	panControlWind.set(ofMap(randP, 0.0f, 1.0f, -1.0f, 1.0f));

	//sound ocean
	ampControlOcean.set(ofInterpolateCosine(0.2f, 0.8f, ampOceanF));
	filterControlOcean.set(randfOcean);
	panControlOcean.set(ofInterpolateCosine(1.0f, -1.0f, panOceanF));

	//sound sphere
	ampControlSphere.set(0.5f);
	filterControlSphere.set(randfSphere);
	ofInterpolateCosine(0.2f, 0.8f, ofMap(h1, 0.0f, 700.0f, 0.0f, 1.0f)) >> ampSphere.in_mod();
	panControlSphere.set(ofInterpolateCosine(0.5f, -0.5f, ofMap(w1, 0.0f, 1000.0f, 0.0f, 1.0f)));
	
	cOcean = ofColor(50, 20);
	cSphere = ofColor(20, 40);

	count += scount;

	if (count < 0.0f) {
		count = 1000.0f;
	}
	if (count > 1000.0f) {
		count = 0.0f;
	};
	scount = 0.1;

	//time mod of random parameters 
	int timeRand = ofGetElapsedTimef();
	int modTRand = timeRand % (8 * 60);

	if (modTRand == 0) {
		randfWind = 53.0f + ofRandom(-3.0f,3.0f);
		randfOcean = 71.0f + ofRandom(-3.0f, 3.0f);
		randfSphere = 58.0f + ofRandom(-3.0f, 3.0f);
		randfRheya = 58.0f + ofRandom(-3.0f, 3.0f);
		randpRheya = 49.0f + ofRandom(-3.0f, 3.0f);
	};
	
	//time mod of suns tempest
	int timeSun = ofGetElapsedTimef();
	int modTSun = timeSun % (5 * 60);

	if ((modTSun >= 2 * 60) && (modTSun <= 5 * 60)) {

		rheya = false;

		pRheya.set(randpRheya);
		ampControlRheya.set(0.9f);
		filterControlRheya.set(ofMap(w1, 0.0f, 1000.0f, randfRheya, randfRheya + 36.0f));
		panControlRheya.set(0);

		cSphere = ofColor(
			ofMap(w1, 0, 1000.0f, 255.0f, 0.0f),
			ofMap(h1, 0, 700.0f, 255.0f, 0.0f),
			ofMap(w1, 0, 1000.0f, 0.0f, 255.0f),
			100);
		cBox = ofColor(
			ofMap(h1, 0, 700.0f, 255.0f, 0.0f),
			ofMap(w1, 0, 1000.0f, 255.0f, 0.0f),
			ofMap(h1, 0, 700.0f, 0.0f, 255.0f),
			100);

		cVol = ofColor(ofRandom(255), 200);

		ofSetBackgroundAuto(false);

	};

	//time mod of dark rain 
	int timeRain = ofGetElapsedTimef();
	int modTRain = timeRain % (6 * 60);

	if ((modTRain >= 3 * 60) && (modTRain <= 4 * 60 )) {

		rheya = false;

		//gateRheya.trigger(1.0f);
		pRheya.set(randpRheya + 36.0f);
		filterControlRheya.set(randfRheya + 36.0f);
		panControlRheya.set(0);
		
		//gateRev.trigger(ofInterpolateCosine(0.0f, 0.8f, oscF));
		cSphere = ofColor(ofRandom(255), 210);
		cBox = ofColor(ofRandom(255));
		cVol = ofColor(ofRandom(255),150);

		ofSetBackgroundAuto(false);

	};

	//time mod of Rheya composition
	int timeRheya = ofGetElapsedTimef();
	int modTRheya = timeRheya % (7 * 60);

	if ((modTRheya >= 4 * 60) && (modTRheya <= 5 * 60 )) {

		pSphere.set(randpRheya + 12);
		filterControlRheya.set(randfRheya + 12);

		rheya = true;

		cSphere = ofColor(20, 40);
		cBox = ofColor(100);
		cVol = ofColor(50, ofRandom(120));

		ofSetBackgroundAuto(true);
	};


	//------------------------mov parameters
	w1 += sw1;
	if ((w1 > 1000.0f) || (w1 < 0.0f)) {
		sw1 *= -1;
	};

	h1 += sh1;
	if ((h1 > 700.0f) || (h1 < 0.0f)) {
		sh1 *= -1;
	};


	panOceanF += SpanOceanF * CpanOceanF;
	if ((panOceanF > 1.0f) || (panOceanF < 0.0f)) {
		SpanOceanF *= -1.0f;
	};
	CpanOceanF = 0.01f;

	ampOceanF += SampOceanF * CampOceanF;
	if ((ampOceanF > 1.0f) || (ampOceanF < 0.0f)) {
		SampOceanF *= -1.0f;
	};
	CampOceanF = 0.013f;


	//time mod of alpha blending of the symbolic graphic analyzer
	int timeA = ofGetElapsedTimef();
	int modT = timeA % (10*60);

	if ((modT >= 5*60) && (modT <= 7*60)) {
		analyzer = true;
	}
	else {
		analyzer = false;
	};

	//load rheya img
	im.load(images[sImg3]);
	
	//set vertex ocean 
	shake += ofInterpolateCosine(0.005f, 0.01f, ampOceanF);
	float yoff = shake;
	for (int i = 0 ; i < ofGetHeight() / scl + 8; i++) {
		float xoff = 0.0f;
		for (int j = 0; j < ofGetWidth() / scl + 25; j++) {
			vZ[j][i] = ofMap(ofNoise(xoff, yoff), 0, 1, -100, 100);
			xoff += 0.04f;
		};
		yoff += 0.05f;
	};

}

//--------------------------------------------------------------
void ofApp::draw(){

	//-------------------object analyzer
	if (analyzer == true) {

		ofSetColor(40, 85);
		ofFill();
		ofRect(200, 100, ofGetWidth() - 400, ofGetHeight() - 200);

		ofNoFill();
		ofSetColor(220, 150);
		ofRect(200, 100, ofGetWidth() - 400, ofGetHeight() - 200);

		ofLine(
			ofMap(randP, 0.0f, 1.0f, 200.0f, ofGetWidth() - 200.0f),
			100,
			ofMap(randP, 0.0f, 1.0f, ofGetWidth() - 200.0f, 200.0f),
			ofGetHeight() - 100
		);
		ofLine(
			ofInterpolateCosine(ofGetWidth() - 200, 200, panOceanF),
			100,
			ofInterpolateCosine(ofGetWidth() - 200, 200, panOceanF),
			ofGetHeight() - 100
		);
		ofLine(
			200,
			ofInterpolateCosine(100, ofGetHeight() - 100, ampOceanF),
			ofGetWidth() - 200,
			ofInterpolateCosine(100, ofGetHeight() - 100, ampOceanF)
		);
	};
	
	//---------------ocean 
	if (ocean == true) {

		ofFill();

		ofPushMatrix();

		ofTranslate(ofGetWidth() - 600, ofGetHeight());
		ofRotateX(105);
		ofTranslate(-ofGetWidth(), -ofGetHeight());

		//ofSetColor(cOcean);
		ofSetLineWidth(1);

		for (int i = 0; i < ofGetHeight() / scl  + 8; i++) {
			for (int j = 0; j < ofGetWidth() / scl + 25; j++) {
				ofSetColor(cOcean,ofMap(i, 0, ofGetHeight() / scl + 5, 0,60));
				ofBox(j*scl , i*scl, vZ[j][i],46,46,46);
				//ofBox(j*scl, (i + 1)*scl,vZ[j][i + 1],45,45,45);
			};
		};
		ofPopMatrix();

		ofSetColor(100, 150);
		ofNoFill();
		ofSetLineWidth(1);

		//-----------------box

		ofPushStyle();
		ofPushMatrix();

		ofSetLineWidth(20);
		ofSetColor(cBox);
		ofTranslate(ofGetWidth() / 2 + ofRandom(-2.0f, 2.0f), ofGetHeight() / 2 + ofRandom(-2.0f, 2.0f));
		ofRotateX(ofMap(w1, 0.0f, 1000.0f, 360, 0));
		ofRotateZ(ofMap(h1, 0.0f, 700.0f, 360, 0));
		box.setScale(5 + ofMap(w1, 0, 1000.0f, 5.0f, 1.5f));
		box.drawVertices();

		ofPopStyle();
		ofPopMatrix();
	}
		      
	if (suns == true) {

		//----------------sphere

		ofPushStyle();
		ofPushMatrix();

		ofSetLineWidth(20);

		ofSetColor(cSphere);
		ofTranslate(ofGetWidth()/2  + ofRandom(-2.0f, 2.0f), ofGetHeight()/2 + ofRandom(-2.0f, 2.0f));
		ofRotateX(ofMap(w1, 0.0f, 1000.0f, 360, 0));
		ofRotateZ(ofMap(h1, 0.0f, 700.0f, 360, 0));
		sphere.setScale(5 + ofMap(w1, 0.0f, 1000.0f, 0, 8));
		sphere.drawVertices();
	
		ofPopMatrix();
		ofPopStyle();

	};

	//--------------rheya volumetric
	if (rheya == true) {

		ofPushMatrix();

		sImg3 = ofMap(h1, 0, ofGetHeight(), 0, 407);
		mesh.clear();
		img.load(images[sImg3]);
		mesh.setMode(OF_PRIMITIVE_LINES);
		ofSetLineWidth(1);
		rheyaPoint += sRheyaPoint;
		if ((rheyaPoint > 20.0f) || (rheyaPoint < 0.0f)) {
			sRheyaPoint *= -1;
		};
		int skip = 2 +  rheyaPoint;
		int width = img.getWidth();
		int height = img.getHeight();
		for (int y = 0; y < height - skip; y += skip) {
			for (int x = 0; x < width - skip; x += skip) {
				ofVec3f nw = getVertexFromImg(img, x, y);
				ofVec3f ne = getVertexFromImg(img, x + skip, y);
				ofVec3f sw = getVertexFromImg(img, x, y + skip);
				ofVec3f se = getVertexFromImg(img, x + skip, y + skip);
				addFace(mesh, nw, ne, se, sw);
				//mesh.addColor(ofFloatColor(0, 0, 0));
			}
		}

		rheyaSize += sRheyaSize;
		if ((rheyaSize > 12.7f) || (rheyaSize < 0.0f)) {
			sRheyaSize *= -1;
		};
		ofScale(1, 1, 0.8f + 12.7f - rheyaSize);
		ofTranslate(500, 100);
		ofSetColor(cVol);
		ofEnableDepthTest();
		mesh.draw();
		ofDisableDepthTest();
		
		ofPopMatrix();
	};
		
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	// if (key == ' ') {
	// 	ofImage imageS; //Declare image object
	// 	//Grab contents of the screen
	// 	imageS.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
	// 	frameSave++;
	// 	imageS.saveImage("screen" + ofToString(frameSave) + ".png"); //Save image to file
	// }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){}
