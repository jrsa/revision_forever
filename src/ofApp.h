#pragma once

#include "ofMain.h"
#include "ofxVideoRecorder.h"

#include "ofxTimeline.h"
#include "ofxTLEmptyTrack.h"
#include "ofxTLEmptyKeyframes.h"

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    void exit();
    
    void loadShaders();
    void loadFbos(int w, int h);
    void clearFbos();
    
    void createPlane();
    void drawPlane();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    int w, h, out_w, out_h;
    float x_off, y_off, w_off, h_off;
    
    bool record, seed;
    float framerate;
    string filename;
    string container;
    
    ofxVideoRecorder vidRecorder;
    void recordingComplete(ofxVideoRecorderOutputFileCompleteEventArgs& args);
    
    ofFbo record_fbo;
    ofPixels record_pix;
    
    ofxTLVideoTrack *videoTrack;

    ofTexture *vid_in;
    int write_vid_in = 0;
    int read_vid_in = 0;
    int n_vid_in = 2;
    // ofImage lastFrame, greyImg;

    ofShader noise;

    ofShader flow, repos, blurX, blurY;
    ofFbo flow_fbo, repos_fbo, blurX_fbo, blurY_fbo;
    
    ofShader gaussian;
    ofFbo gaussian_fbo;
    
    ofShader fbmixer;
    ofFbo fbmixer_fbo;

    bool drawTimeline;
    ofxTimeline timeline;
    ofxTLEmptyTrack* emptyTrack;
    ofxTLEmptyKeyframes* emptyKeyframes;
    
    void receivedBang(ofxTLBangEventArgs& bang);
};
