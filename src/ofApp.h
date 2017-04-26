#pragma once

#include "ofMain.h"
#include "ofxVideoRecorder.h"

#include "ofxTimeline.h"
#include "ofxTLEmptyTrack.h"
#include "ofxTLEmptyKeyframes.h"

// timeline names
#define TIMELINE_FEEDBACK_MIX "feedback mix"
#define TIMELINE_DRY_MIX "dry mix"
#define TIMELINE_CLEARFBOS "clear fbos"

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
    
    bool record;
    float framerate;
    string filename;
    string container;
    
    ofxVideoRecorder vidRecorder;
    void recordingComplete(ofxVideoRecorderOutputFileCompleteEventArgs& args);
    
    ofFbo record_fbo;
    ofPixels record_pix;
    
//    ofVideoPlayer micros; // currently the microscope dry footage
    ofxTLVideoTrack* videoTrack, *microsTrack; // testing iOS
        
    ofTexture greyTex, movieTex, lastTex;
    ofImage lastFrame, greyImg;
    
    ofShader flow, repos, blurX, blurY;
    ofFbo flow_fbo, repos_fbo, blurX_fbo, blurY_fbo;
    
    ofShader gaussian, hsv, contrast, kernel_shader, bump;
    ofFbo gaussian_fbo, hsv_fbo, contrast_fbo, kernel_fbo, bump_fbo;
    
    ofShader fbmixer, drymixer;
    ofFbo fbmixer_fbo, drymixer_fbo;

    bool enableBump;
    bool iosdraw;
    bool drawTimeline;
    ofxTimeline timeline;
    ofxTLEmptyTrack* emptyTrack;
    ofxTLEmptyKeyframes* emptyKeyframes;
    
    void receivedBang(ofxTLBangEventArgs& bang);
    
    ///////// for ios ////////
    float downsampleAmt;
    int ds_w, ds_h;
    
    ofShader geo_shader, displace_shader, blur_shader;
    ofFbo blur_fbo, displace_fbo, dest_fbo;
    
    ofImage displacement_map;
    ofTexture displaceTex;
    
    ofPlanePrimitive plane;
    
    ofLight pointLight;
    ofLight directionalLight;
    ofLight spotLight;
    
    ofEasyCam cam;
    ofTexture movTex;
};
