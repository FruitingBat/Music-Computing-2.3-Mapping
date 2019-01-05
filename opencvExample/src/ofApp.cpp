#include "ofApp.h"

//--------------------------------------------------------------

void ofApp::mediaSort() {

	stringstream s; // holds file path

	if (sampleCount > 7) { // reset sample counter
		sampleCount = 0;
	};


	if (sampleCount == 0) { // if one cycle if sample counter is complete

		tempo = (int)ofRandom(3, 11); // random tempo of sequencer

		if (videoCount > 7) { // reset video counter
			videoCount = 0;
		};

		if (videoCount == 0) { // if one cycle of video counter is complete
			for (int i = 1; i < 2; i++) {
				videoSelect = (int)ofRandom(-1, 8); // random video
				if (videoList[7] == videoSelect) { // checks if current video is the same as previous video
					i = 0;
				}
			}
		}

		else {
			videoSelect = (int)ofRandom(-1, 8); // random video
			for (int k = 0; k < videoCount; k++) {
				if (videoList[k] == videoSelect) { // checks if current video is same as all videos so far
					videoSelect = (int)ofRandom(-1, 8);
					k = 0;
				}
			}
		}

		videoList[videoCount] = videoSelect; // logs videos loaded

		s.str("");
		s << "videos/" << videoSelect << ".mov"; // create file path

		vidPlayer.load(ofToDataPath(s.str())); // load video and initialise it
		vidPlayer.play();
		vidPlayer.setVolume(0);
		vidPlayer.setLoopState(OF_LOOP_NORMAL);

		videoCount++; // advance video count

		for (int i = 1; i < 2; i++) {
			sampleSelect = (int)ofRandom(-1, 8); // randomly choose a part to change
			if (sampleList[7] == sampleSelect) { // checks if current part is the same as previous part
				i = 0;
			}
		}

		sampleList[sampleCount] = sampleSelect; // logs which part was changed

		for (int i = 1; i < 2; i++) {
			folderSelect = (int)ofRandom(-1, 8);  // randomly chooses sample
			s.str("");
			s << "samples/" << sampleSelect << "/" << folderSelect << ".wav"; // create file path
			if (historyList[sampleSelect][7] == folderSelect) { // checks if current sample is the same as previous sample
				i = 0;
			}
		}
	}

	else {
		for (int i = 0; i < sampleCount; i++) {
			sampleSelect = (int)ofRandom(-1, 8); // randomly choose a part to change
			if (sampleList[i] == sampleSelect) { // checks if current part is the same as all parts so far
				i = 0;
			}
		}

		sampleList[sampleCount] = sampleSelect; // logs which part was changed

		for (int i = 0; i < sampleCount; i++) {
			folderSelect = (int)ofRandom(-1, 8); // randomly chooses sample
			s.str("");
			s << "samples/" << sampleSelect << "/" << folderSelect << ".wav"; // create file path
			if (historyList[sampleSelect][sampleCount] == folderSelect) { // checks if current sample is the same as all samples so far
				i = 0;
			}
		}
	}

	historyList[sampleSelect][sampleCount] = folderSelect; // logs outcome

	sample[sampleSelect].load(ofToDataPath(s.str())); // load sample
	sample[sampleSelect].normalise(1); // noramlise sample

	sampleCount++; // advance sample count
}

//--------------------------------------------------------------
void ofApp::setup() {

	// intialise settings

	ofSetFrameRate(30);

	sampleRate = 44100;
	bufferSize = 512;

	for (int i = 0; i < 8; i++) {
		mediaSort(); // load media
	}

	colorImg.allocate(vidPlayer.getWidth(), vidPlayer.getHeight());
	grayImage.allocate(vidPlayer.getWidth(), vidPlayer.getHeight());
	grayBg.allocate(vidPlayer.getWidth(), vidPlayer.getHeight());
	grayDiff.allocate(vidPlayer.getWidth(), vidPlayer.getHeight());

	bLearnBakground = true;
	threshold = 80;

	ofSoundStreamSetup(2, 0, this, sampleRate, bufferSize, 4); // start sound
}

//--------------------------------------------------------------
string ofApp::binaryConvert(int num) { // converts number into binary string of 16
	string out;
	int binary[16] = { 32768, 16384, 8192, 4096, 2048, 1024, 512, 256, 128, 64, 32, 16, 8, 4, 2, 1 };

	for (int i = 0; i < 16; i++) { // checks if to point in string should be '1' or '0'
		if (num >= binary[i]) {
			num = num - binary[i];

			out = ('1' + out); // combine numbers backwards as lower values are '1' more oftern making them more suitable to be at start of sequence 
		}
		else {
			out = ('0' + out);
		}
	}
	return out;

}

//--------------------------------------------------------------
void ofApp::update() {
	ofBackground(0, 0, 0); // clear screen

	// checks if frame has updated
	bool bNewFrame = false;

	vidPlayer.update();
	bNewFrame = vidPlayer.isFrameNew();

	if (bNewFrame) {

		colorImg.setFromPixels(vidPlayer.getPixels()); // get colour image

		grayImage = colorImg; // turn colour image into gray image
		if (bLearnBakground == true) {
			grayBg = grayImage;		// copys the pixels from gray image into gray background (operator overloading)
			bLearnBakground = false;
		}

		// take the abs value of the difference between background and incoming and then threshold
		grayDiff.absDiff(grayBg, grayImage);
		grayDiff.threshold(threshold);

		// find contours which are between the size of 20 pixels and 1/3 the w*h pixels..
		contourFinder.findContours(grayDiff, 20, (vidPlayer.getWidth(), vidPlayer.getHeight()) / 3, 10, true);	// find holes
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	// draw the background and the thresholded difference
	ofSetHexColor(0xffffff);

	ofFill();
	ofSetHexColor(0x333333);

	grayDiff.draw(0, 0, ofGetWidth(), ofGetHeight());

	// draw the whole contour finder
	contourFinder.draw(0, 0, ofGetWidth(), ofGetHeight());

	for (int i = 0; i < contourFinder.nBlobs; i++) { // converts contour vectors

		for (int i = 0; i < contourFinder.nBlobs; i++) {

			int j = 0;

			j = i;

			while (j >= 8) {
				j -= 8;
			}

			if (j < 0) {
				j = 0;
			}

			// converts x axis of contour center into sample panning
			finderX[j] = (contourFinder.blobs[i].boundingRect.getCenter().x / vidPlayer.getWidth());
			
			// converts y axis of bottom left of contour into binary sequence
			finderY[j] = ((contourFinder.blobs[i].boundingRect.getBottomLeft().y / vidPlayer.getHeight()) * 65535); 

			binaryY[j] = binaryConvert(finderY[j]);

			for (int k = 0; k < 16; k++) {
				charHit[j][k] = binaryY[j].at(k);
				hit[j][k] = (charHit[j][k] - '0');
			}
		}
	}


	// info test
	/* ofSetHexColor(0xffffff);
	stringstream reportStr;
	reportStr << "bg subtraction and blob detection" << endl
		<< "press ' ' to capture bg" << endl
		<< "threshold " << threshold << " (press: +/-)" << endl
		<< "num blobs found " << contourFinder.nBlobs << ", fps: " << ofGetFrameRate();
	ofDrawBitmapString(reportStr.str(), 10, 10);
	*/

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	// program keyboard controls
	switch (key) {
	case '+':
		threshold++;
		if (threshold > 255) threshold = 255;
		break;
	case '-':
		threshold--;
		if (threshold < 0) threshold = 0;
		break;
	case 'f':
		ofToggleFullscreen();
		break;
	}
}


//--------------------------------------------------------------
void ofApp::limiter() { // stops clipping

	if (leftOut[0] >= 1) {
		leftOut[0] = 1;
	}
	else if (leftOut[0] <= -1) {
		leftOut[0] = -1;
	}
	if (leftOut[1] >= 1) {
		leftOut[1] = 1;
	}
	else if (leftOut[1] <= -1) {
		leftOut[1] = -1;
	}

	if (rightOut[0] >= 1) {
		rightOut[0] = 1;
	}
	else if (rightOut[0] <= -1) {
		rightOut[0] = -1;
	}
	if (rightOut[1] >= 1) {
		rightOut[1] = 1;
	}
	else if (rightOut[1] <= -1) {
		rightOut[1] = -1;
	}
}

//--------------------------------------------------------------
void ofApp::audioOut(float * output, int bufferSize, int nChannels) {
	for (int i = 0; i < bufferSize; i++) { // produce audio

		// clear sound ouput
		left = 0; 
		right = 0;

		currentCount = (int)timer.phasor(tempo); //this sets up a metronome that ticks 8 times a second

		for (int j = 0; j < 8; j++) { // counts through each part
			if (j <= contourFinder.nBlobs) { // only advance part if its assigned contour has a value
				if (lastCount[j] != currentCount) {//if we have a new timer int this sample, play the sound

					sampleTrigger[j] = (hit[j][playHead[j] % 16]);//get the value out of the array for the kick
					playHead[j]++;//iterate the playhead
					lastCount[j] = 0;//reset the metrotest

					if (sampleTrigger[j] == 1) {//if the sequence has a 1 in it
						sample[j].trigger();//reset the playback position of the sample to 0 (the beginning)
						leftPan[j] = (1 - finderX[j]) * 0.2; // sets up panning volume based on x contour position 
						rightPan[j] = (finderX[j]) * 0.2;
					}
				}
			}

			left += (sample[j].playOnce() * leftPan[j]); // apply advanced parts to output and apply panning
			right += (sample[j].playOnce() * rightPan[j]);

		}

		leftMix.stereo(left, leftOut, 0.5); // mix stereo to mono
		rightMix.stereo(right, rightOut, 0.5);

		limiter(); // apply limiter 

		output[i*nChannels] = leftOut[0] + leftOut[1]; // mix stereo to mono
		output[i*nChannels + 1] = rightOut[0] + rightOut[1];

		if (lastResetCount != currentCount) {
			resetCount += 1; // advance reset count
			lastResetCount = 0;
		}

		if (resetCount >= 128) {
			mediaSort(); // load new media after 127 cycles of sequencer metronome 
			bLearnBakground = true; // take snapshot of grayscale and make it the background
			resetCount = 0;
		}
	}

	for (int j = 0; j < 8; j++) {
		sampleTrigger[j] = 0; // resest samples position 
	}
}
