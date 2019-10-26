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
	glPointSize(3);

	sphere.setResolution(80);
	box.setResolution(80);

	font.load("OCRAStd.otf", 80);

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

	//--------------shader set
#ifdef TARGET_OPENGLES
	shader.load("shadersES2/shader");
#else
	if (ofIsGLProgrammableRenderer()) {
		shader.load("shadersGL3/shader");
	}
	else {
		shader.load("shadersGL2/shader");
	}
#endif

	doShader = true;
	
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

	randfWind = 53.0f;
	randfOcean = 71.0f;
	randfSphere = 58.0f;
    randfRheya = 58.0f;
	randpRheya = 49.0f;

	sphereStruc = 1;

}

//--------------------------------------------------------------
void ofApp::update() {

	//time set
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	ocean = true;

	if (start == true) {

		timeColorBack = ofColor(
			ofMap(timeinfo->tm_min, 0, 60, 255, 0),
			ofMap(timeinfo->tm_hour, 0, 23, 255, 0),
			ofMap(timeinfo->tm_wday, 0, 6, 255, 0),
			255
		);

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
		cBox = ofColor(240, 20);

		count += scount;

		if (count < 0.0f) {
			count = 1000.0f;
		}
		if (count > 1000.0f) {
			count = 0.0f;
		};
		scount = 0.1;

		//time mod of random parameters 
		//int timeRand = ofGetElapsedTimef();
		//int modTRand = timeRand % (8 * 60);

		if (timeinfo->tm_mday == 0) {
			randfWind = 53.0f + ofMap(timeinfo->tm_mday, 0.0f, 7.0f, -8.0f, 8.0f);
			randfOcean = 71.0f + ofMap(timeinfo->tm_mday, 0.0f, 14.0f, -8.0f, 8.0f);
			randfSphere = 58.0f + ofMap(timeinfo->tm_mday, 0.0f, 19.0f, -8.0f, 8.0f);
			randfRheya = 58.0f + ofRandom(-3.0f, 3.0f);
			randpRheya = 49.0f + ofRandom(-3.0f, 3.0f);

			timeSun = ofRandom(0, 12);
			timeRain = ofRandom(0, 22);
			timeRheya = ofRandom(0, 23);
			minRheya = ofRandom(0, 30);
		};

		//time mod of suns tempest
		if ((timeinfo->tm_hour < timeSun + 4) && (timeinfo->tm_hour > timeSun - 1)) {
				
			int timeColorH = ofMap(timeinfo->tm_hour, 0, 24, 10, 205);
			int timeColorM = ofMap(timeinfo->tm_min, 0, 60, 0, 255);
			int timeColorS = ofMap(timeinfo->tm_sec, 0, 60, 0, 255);

			rheya = false;

			pRheya.set(randpRheya);
			ampControlRheya.set(0.9f);
			filterControlRheya.set(ofMap(w1, 0.0f, 1000.0f, randfRheya, randfRheya + 36.0f));
			panControlRheya.set(0);

			cSphere = ofColor(
				timeColorH,
				timeColorM,
				ofMap(w1, 0, 1000.0f, 0.0f, 255.0f),
				100
			);
			cBox = ofColor(
				timeColorH,
				timeColorM,
				ofMap(h1, 0, 700.0f, 0.0f, 255.0f),
				100
			);

			cVol = ofColor(ofRandom(255));

			//ofSetBackgroundAuto(false);

		};

		//time mod of dark rain 
		if ((timeinfo->tm_hour < timeRain + 1) && (timeinfo->tm_hour > timeRain - 1)) {

			rheya = false;

			pRheya.set(randpRheya + 66.0f);
			filterControlRheya.set(randfRheya +66.0f);
			panControlRheya.set(0);

			timeColorBack = ofColor(ofRandom(255),200);
			
			cSphere = ofColor(ofRandom(255), 210);
			cBox = ofColor(ofRandom(255));
			cVol = ofColor(ofRandom(255), 150);

			//ofSetBackgroundAuto(false);

		};

		//time mod of Rheya composition
		if ((timeinfo->tm_hour < timeRheya + 1) && (timeinfo->tm_hour > timeRheya - 1)) {

			cout << minRheya << endl;
			if ((timeinfo->tm_min > minRheya - 1) && (timeinfo->tm_min < minRheya + 15)) {
				pSphere.set(randpRheya + 12);
				filterControlRheya.set(randfRheya + 12);

				rheya = true;

				cSphere = ofColor(20, 40);
				cBox = ofColor(100);
				cVol = ofColor(50, ofRandom(120));
			}
			//ofSetBackgroundAuto(true);
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
		CpanOceanF = 0.001f;

		ampOceanF += SampOceanF * CampOceanF;
		if ((ampOceanF > 1.0f) || (ampOceanF < 0.0f)) {
			SampOceanF *= -1.0f;
		};
		CampOceanF = 0.0013f;

		//load rheya img
		im.load(images[sImg3]);

		//set vertex ocean 
		shake += ofInterpolateCosine(0.005f, 0.002f, ampOceanF);
		float yoff = shake;
		for (int i = 0; i < ofGetHeight() / scl + 8; i++) {
			float xoff = 0.0f;
			for (int j = 0; j < ofGetWidth() / scl + 25; j++) {
				vZ[j][i] = ofMap(ofNoise(xoff, yoff), 0, 1, -100, 100);
				xoff += 0.04f;
			};
			yoff += 0.05f;
		};
	};
};

//--------------------------------------------------------------
void ofApp::draw() {

	//-----------time set local
	time(&rawtime);
	timeinfo = localtime(&rawtime);


	for (int i = -timeinfo->tm_sec ; i < ofGetHeight() + timeinfo->tm_sec; i += 50) {
		int colorD = ofMap(i, 0, ofGetHeight(), 0, 255);
		ofSetColor(timeColorBack, colorD);
		ofDrawRectangle(0, i, ofGetWidth(), 50);
	};

	ofPushMatrix();

	ofSetColor(20, alphaTitle);
	ofScale(scaleT);
	font.drawString(title, posXt, posYt);

	string press = "pressione ENTER para iniciar";

	int timeStart = ofGetElapsedTimef();

	if (timeStart >= 3) {
		posXt = ofGetWidth() / 0.71;
		posYt = ofGetHeight()*1.75;
		scaleT = 0.3f;
		title = "rafa diniz";
	}

	if (timeStart >= 5) {
		posXt = ofGetWidth() / 0.64;
		title = "2020";
	}
	if (timeStart >= 7) {
		posXt = ofGetWidth() / 3.6f;
		posYt = ofGetHeight() / 2.8f;
		scaleT = 1.2f;
		title = "RHEYA";
	}
	if (timeStart >= 10) {
		ofSetColor(20, alphaTitle);
		ofScale(0.3);
		font.drawString(press, ofGetWidth() / 1.25f, 1200);
		freKey = true;
		//timeStart = 5;
	}

	ofPopMatrix();

	if (start == true) {
		
		countEsc++;

		alphaTitle = 0;

		ofPushMatrix();
         
		ofSetColor(10, alphaEsc);
		ofDrawRectangle(0, 0, 200, 20);

		string esc = "para sair pressione ESC";
		//int timeEsc = ofGetElapsedTimef() - timeStart;
		ofSetColor(240, alphaEsc);
		ofScale(0.1);
		font.drawString(esc, 50, 140);

		if (countEsc >= 5 * 60) {
			alphaEsc = 0;
		};

		//numbers in loop
		ofPushMatrix();
		ofScale(1.5);
		for (int i = 10; i < ofGetWidth() * 8; i += 500) {
			for (int h = 10; h < ofGetHeight() * 8; h += 200) {
				int aphaN = ofMap(h, 10, ofGetHeight()*8, 40, 15);
				ofSetColor(20, aphaN);
				font.drawString(ofToString(i+h + timeinfo->tm_sec), i, h);
			}
		}
		ofPopMatrix();

		ofPopMatrix();
	}

	//---------------ocean 
	if (ocean == true) {

		ofFill();

		ofPushMatrix();

		ofTranslate(ofGetWidth() - 600, ofGetHeight());
		ofRotateXDeg(105);
		ofTranslate(-ofGetWidth(), -ofGetHeight());

		//ofSetColor(cOcean);
		ofSetLineWidth(1);

		for (int i = 0; i < ofGetHeight() / scl + 8; i++) {
			for (int j = 0; j < ofGetWidth() / scl + 25; j++) {
				ofSetColor(cOcean, ofMap(i, 0, ofGetHeight() / scl + 5, 0, 60));
				ofDrawBox(j*scl + vZ[j][i], i*scl + vZ[j][i], vZ[j][i], 46, 46, 46);
				//ofBox(j*scl, (i + 1)*scl,vZ[j][i + 1],45,45,45);
			};
		};
		ofPopMatrix();


		ofSetColor(100, 150);
		ofNoFill();
		ofSetLineWidth(1);

		//-----------------box

		if (doShader) {
			shader.begin();
			//we want to pass in some varrying values to animate our type / color 
			shader.setUniform1f("time", ofGetElapsedTimef() * 1.1);
			//shader.setUniform1f("time", -ofGetElapsedTimef() * 5.18);
		}

		ofPushStyle();
		ofPushMatrix();

		ofSetLineWidth(20);
		ofSetColor(cBox);
		ofTranslate(ofGetWidth() / 2 + ofRandom(-2.0f, 2.0f), ofGetHeight() / 2 + ofRandom(-2.0f, 2.0f));
		ofRotateXDeg(ofMap(w1, 0.0f, 1000.0f, 360, 0));
		ofRotateZDeg(ofMap(h1, 0.0f, 700.0f, 360, 0));
		float timeScaleMs = ofMap(w1, 0, 1000, 5.0f, 1.5f);
		box.setScale(5 + timeScaleMs);
		box.drawVertices();

		ofPopStyle();
		ofPopMatrix();

		if (doShader) {
			shader.end();
		}
	}

	if (suns == true) {

		//----------------sphere
		if (doShader) {
			shader.begin();
			//we want to pass in some varrying values to animate our type / color 
			shader.setUniform1f("time", ofGetElapsedTimef() * 1.1);
			//shader.setUniform1f("time", -ofGetElapsedTimef() * 5.18);
		}

		ofPushStyle();
		ofPushMatrix();

		ofSetLineWidth(20);

		ofSetColor(cSphere);
		ofTranslate(ofGetWidth() / 2 + ofRandom(-2.0f, 2.0f), ofGetHeight() / 2 + ofRandom(-2.0f, 2.0f));
		ofRotateXDeg(ofMap(w1, 0.0f, 1000.0f, 360, 0));
		ofRotateZDeg(ofMap(h1, 0.0f, 700.0f, 360, 0));
		float timeScaleM = ofMap(timeinfo->tm_hour, 0, 23, 13.0f, 2.0f);
		sphere.setScale(timeScaleM);
		if (sphereStruc == 0) {
			sphere.drawFaces();
		}
		if (sphereStruc == 1) {
			sphere.drawVertices();
		}
		if (sphereStruc == 2) {
			sphere.drawWireframe();
		}
		
		ofPopMatrix();
		ofPopStyle();

		if (doShader) {
			shader.end();
		}

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
		int skip = 2 + rheyaPoint;
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

	//fade intro 
	if (start == true) {
		ofSetColor(220, 255 - countEsc);
		ofFill();
		ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
	}
	
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
	if (freKey == true) {
		if (key == OF_KEY_RETURN) {
			start = true;
		}
	}

	if (key == 's') {
		doShader = !doShader;
	}
	if (key == 'r') {
		sphereStruc = ofRandom(0,3);
	}
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
