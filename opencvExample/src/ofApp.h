#pragma once

#include "ofMain.h"

#include "ofxOpenCv.h"
#include "ofxMaxim.h"

class ofApp : public ofBaseApp {

public:
	void mediaSort();
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void limiter();
	void audioOut(float * output, int bufferSize, int nChannels);


	ofVideoPlayer vidPlayer;
	ofxCvColorImage colorImg;

	ofxCvGrayscaleImage grayImage;
	ofxCvGrayscaleImage grayBg;
	ofxCvGrayscaleImage grayDiff;

	ofxCvContourFinder 	contourFinder;

	maxiSample sample[8];
	maxiMix leftMix, rightMix;
	maxiOsc timer;

	string binaryConvert(int num);

	int threshold, sampleRate, bufferSize;
	bool bLearnBakground;

	int tempo;

	int folderSelect, historyList[8][8];
	int videoList[8], videoSelect, videoCount;
	int sampleList[8], sampleSelect, sampleCount;

	int resetCount, lastResetCount, currentCount, lastCount[8], playHead[8], hit[8][16];
	char charHit[8][16];

	int sampleTrigger[8];

	float finderX[8], finderY[8];
	string binaryY[8];

	float leftPan[8], rightPan[8];
	double left, right;
	double leftOut[2], rightOut[2];
};