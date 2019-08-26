/*
 * rheya/src/ofApp.h
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

#pragma once

#include "ofMain.h"
#include "ofxPDSP.h"


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


		//mesh helper functions
		void addFace(ofMesh& mesh, ofVec3f a, ofVec3f b, ofVec3f c);
		void addFace(ofMesh& mesh, ofVec3f a, ofVec3f b, ofVec3f c, ofVec3f d);
		ofVec3f getVertexFromImg(ofFloatImage& img, int x, int y);

		//mesh
		ofFloatImage img;
		ofVboMesh mesh;
		ofLight light;

		//primitives
		ofSpherePrimitive sphere;
		ofBoxPrimitive box;
		ofPolyline line;
		
		ofColor color1;
		
		ofTrueTypeFont type;

		//image to frames
		ofImage im;

		//globals parameters to frames manipulation 
		string path;
		string imgT;
		string images[1005];
		int sImg3;
		int posX, posY;
		int sizeF;
		int countF;

		float dif1;


		//loop ocean set
		int cols, rows;
		int scl;
	    float vZ[100][100];
		float shake;

		//simple count to perlin noise activation
		float count;
		float scount;

		//move's oscillators 
		float w1;
		float sw1;
		float h1;
		float sh1;

		//globals sound parameters set 
		float ampOceanF;
		float SampOceanF;
		float CampOceanF;

		float panOceanF;
		float SpanOceanF;
		float CpanOceanF;

		float rheyaPoint;
		float sRheyaPoint;
		float rheyaSize;
		float sRheyaSize;

		//globals colors parameters set 
		int alphaAnalyzer;
		float SalphaAnalyzer;

		ofColor cOcean;
		ofColor cSphere;
		ofColor cBox;
		ofColor cVol;

		//boolean objects
		bool ocean = false;
		bool suns = false;
		bool rheya = false;
		bool analyzer = false;
		

		//globals random paramenters set
		float randP;

		float randfWind;
		float randfOcean;
		float randfSphere;
		float randfRheya;
		float randpRheya;

		int frameSave = 0;

		//PDSP instance declaration
		pdsp::Engine   engine;
		
		pdsp::VAOscillator synthSphere;
		pdsp::VAOscillator synthRheya;

		pdsp::BitNoise  noiseWind;
		pdsp::BitNoise  noiseOcean;
		pdsp::BitNoise  noiseSphere;
		pdsp::BitNoise  noiseRheya;
		
		pdsp::SVFilter filterWind;
		pdsp::SVFilter filterOcean;
		pdsp::SVFilter filterSphere;
		pdsp::SVFilter filterRheya;
		pdsp::BasiVerb reverb;

		pdsp::ComputerKeyboard keyboard;
		pdsp::Amp  ampWind;
		pdsp::Amp  ampOcean;
		pdsp::Amp  ampSphere;
		pdsp::Amp  ampRheya;
		pdsp::Amp  ampRev;

		pdsp::Panner panWind;
		pdsp::Panner panOcean;
		pdsp::Panner panSphere;
		pdsp::Panner panRheya;

		pdsp::ValueControl pSphere;
		pdsp::ValueControl pRheya;

		pdsp::ValueControl ampControlWind;
		pdsp::ValueControl ampControlOcean;
		pdsp::ValueControl ampControlSphere;
		pdsp::ValueControl ampControlRheya;

		pdsp::ValueControl filterControlWind;
		pdsp::ValueControl filterControlOcean;
		pdsp::ValueControl filterControlSphere;
		pdsp::ValueControl filterControlRheya;

		pdsp::ValueControl panControlWind;
		pdsp::ValueControl panControlOcean;
		pdsp::ValueControl panControlSphere;
		pdsp::ValueControl panControlRheya;

};
