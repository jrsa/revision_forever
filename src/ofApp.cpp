#include "ofApp.h"
#include "timeline_tracks.h"

void ofApp::loadShaders() {
    flow.load("flow.vert", "flow.frag");
    repos.load("repos.vert", "repos.frag");
    blurX.load("generic.vert", "blurX.frag");
    blurY.load("generic.vert", "blurY.frag");
    gaussian.load("translate.vert", "gaussian.frag");
    fbmixer.load("generic.vert", "mixer_ex.frag");
}


void ofApp::clearFbos() {
    flow_fbo.begin();
    ofClear(0, 0, 0, 0);
    flow_fbo.end();
    repos_fbo.begin();
    ofClear(0, 0, 0, 0);
    repos_fbo.end();
    blurX_fbo.begin();
    ofClear(0, 0, 0, 0);
    blurX_fbo.end();
    blurY_fbo.begin();
    ofClear(0, 0, 0, 0);
    blurY_fbo.end();
    gaussian_fbo.begin();
    ofClear(0, 0, 0, 0);
    gaussian_fbo.end();
    fbmixer_fbo.begin();
    ofClear(0, 0, 0, 0);
    fbmixer_fbo.end();
    record_fbo.begin();
    ofClear(0, 0, 0, 0);
    record_fbo.end();
}

void ofApp::loadFbos(int w, int h) {
    vid_in.allocate(w, h, GL_RGBA);
    lastTex.allocate(w, h, GL_RGBA);
    flow_fbo.allocate(w, h, GL_RGBA);
    flow_fbo.setUseTexture(true);
    repos_fbo.allocate(w, h, GL_RGBA);
    repos_fbo.setUseTexture(true);
    blurX_fbo.allocate(w, h, GL_RGBA);
    blurX_fbo.setUseTexture(true);
    blurY_fbo.allocate(w, h, GL_RGBA);
    blurY_fbo.setUseTexture(true);
    gaussian_fbo.allocate(w, h, GL_RGBA);
    gaussian_fbo.setUseTexture(true);
    fbmixer_fbo.allocate(w, h, GL_RGBA);
    fbmixer_fbo.setUseTexture(true);
    
    record_fbo.allocate(w, h, GL_RGB);
    record_fbo.setUseTexture(true);
    record_pix.allocate(w, h, 3);
}


void ofApp::setup() {
    framerate = 23.98;
    
    timeline.setup();
    timeline.setLoopType(OF_LOOP_NORMAL);
    
    timeline.addCurves(TRACK_FEEDBACK_MIX, ofRange(0, 1));
    timeline.addCurves(TRACK_REPOS, ofRange(10, 80));
    timeline.addFlags(TRACK_CLEARFBOS);
    ofAddListener(timeline.events().bangFired, this, &ofApp::receivedBang);
    
    drawTimeline = false;
    
    videoTrack = timeline.addVideoTrack(TRACK_VID, "/Users/jrsa/Desktop/SCRATCH/me.mov");
    ofPtr<ofVideoPlayer> pplyr = videoTrack->getPlayer();
    
    framerate = pplyr->getTotalNumFrames() / pplyr->getDuration();
    timeline.setFrameRate(framerate);
    timeline.setDurationInFrames(pplyr->getTotalNumFrames());
    pplyr->setUseTexture(true);
    w = pplyr->getWidth();
    h = pplyr->getHeight();
    
    out_w = w;
    out_h = h;
    
    record_fbo.allocate(w, h, GL_RGB);
    record_fbo.setUseTexture(true);
    record_pix.allocate(w, h, 3);
    
    ofSetWindowShape(out_w, out_h);
    ofSetFrameRate(framerate);
    
    vidRecorder.setVideoCodec("prores");
    ofAddListener(vidRecorder.outputFileCompleteEvent, this, &ofApp::recordingComplete);
    record = false;
    filename = "moshcroscope";
    container = ".mov";
    
    loadShaders();
    loadFbos(w, h);
    clearFbos();
}


void ofApp::update() {
    if (record) {
        vidRecorder.addFrame(record_pix);
    }
    
    ofPtr<ofVideoPlayer> plr = videoTrack->getPlayer();
    if(plr->isFrameNew()) {
        lastTex = vid_in;
        vid_in = plr->getTexture();
    }
}

void ofApp::draw() {
#if 0 //testing programmable pipeline stuff
    // compute hsflow values of video input
    // ----------------------------------------------------------------------------
    flow_fbo.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    flow_fbo.begin();
    
    ofClear(0, 0, 0, 0);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    
    flow.begin();
    flow.setUniform2f("scale", 12.0, 12.0);
    flow.setUniform2f("offset", 100.0, 100.0);
    flow.setUniform1f("lambda", 100.0);
    flow.setUniformTexture("tex1", lastTex, 1);
    
    vid_in.draw(0, 0);
    flow.end();
    
    glDisable(GL_BLEND);
    flow_fbo.end();

    // blur hsflow map
    // ----------------------------------------------------------------------------
    blurX_fbo.begin();
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);
    blurX.begin();
    blurX.setUniform1f("blurAmnt", 1.0);
    flow_fbo.draw(0, 0);
    blurX.end();
    glDisable(GL_BLEND);
    blurX_fbo.end();
    
    blurY_fbo.begin();
    blurY.begin();
    blurY.setUniform1f("blurAmnt", 1.0);
    
    blurX_fbo.draw(0, 0);
    blurY.end();
    blurY_fbo.end();
    
    // texture displacement/repositioning based on hsflow map
    // ----------------------------------------------------------------------------
    repos_fbo.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    repos_fbo.begin();
    ofClear(255, 255, 255, 0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR);
    repos.begin();
    float reposAmt = timeline.getValue(TRACK_REPOS);
    repos.setUniform2f("amt", reposAmt, reposAmt);

    // blurY contains the blurred flow map
    repos.setUniformTexture("tex1", blurY_fbo.getTexture(), 1);
    
    // the tex0 input is a mix of the video input and feedback
    fbmixer_fbo.draw(0, 0);
    repos.end();
    glDisable(GL_BLEND);
    repos_fbo.end();

    // gaussian blur in feedback path
    // ----------------------------------------------------------------------------
    gaussian_fbo.begin();
    gaussian.begin();
    gaussian.setUniform2f("zoom", 1.0, 1.0);
    gaussian.setUniform1f("blurAmnt", 1.0);
    repos_fbo.draw(0, 0);
    gaussian.end();
    gaussian_fbo.end();

    //MIXER between video input and feedback path
    // ----------------------------------------------------------------------------
    fbmixer_fbo.begin();
    fbmixer.begin();

    fbmixer.setUniformTexture("tex1", vid_in, 1);    
    fbmixer.setUniformTexture("tex2", blurY_fbo.getTexture(), 2);
    
    float mix = timeline.getValue(TRACK_FEEDBACK_MIX);
    fbmixer.setUniform1f("mixAmnt", mix);

    gaussian_fbo.draw(0 ,0);

    fbmixer.end();
    fbmixer_fbo.end();

    // fbmixer -> record (the output, also gets drawn to the screen)
    // ----------------------------------------------------------------------------
    record_fbo.begin();
    fbmixer_fbo.draw(0, 0);
    
    if(record) {
        record_fbo.readToPixels(record_pix);
    }
    record_fbo.end();
    record_fbo.draw(0, 0, out_w, out_h);
#else
    record_fbo.begin();

    // ofClear(0, 0, 0, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    flow.begin();
    flow.setUniform2f("scale", 12.0, 12.0);
    flow.setUniform2f("offset", 100.0, 100.0);
    flow.setUniform1f("lambda", 100.0);
    flow.setUniformTexture("tex1", lastTex, 1);

    vid_in.draw(0, 0);
    flow.end();

    glDisable(GL_BLEND);

    record_fbo.end();
    record_fbo.draw(0, 0, out_w, out_h);
#endif
    
    if (drawTimeline) {
        timeline.draw();
    }
}


void ofApp::exit() {
    ofRemoveListener(vidRecorder.outputFileCompleteEvent, this, &ofApp::recordingComplete);
    vidRecorder.close();
}

void ofApp::recordingComplete(ofxVideoRecorderOutputFileCompleteEventArgs& args){
    cout << "The recorded video file is now complete." << endl;
}

void ofApp::keyPressed(int key) {
    
}

void ofApp::keyReleased(int key) {
    switch (key) {
        case 'c': {
            record = false;
            vidRecorder.close();
            break;
        }
        case 'r': {
            record = !record;
            if (record && !vidRecorder.isInitialized()){
                vidRecorder.setup(filename + ofGetTimestampString() + container, w, h, framerate);
                vidRecorder.start();
            }
            break;
        }
        case 's': {
            loadShaders();
            break;
        }
        case 't': {
            drawTimeline = !drawTimeline;
            break;
        }
        case 'u': {
            clearFbos();
            break;
        }
        case '0': {
            timeline.setCurrentTimeMillis(0);
            break;
        }
        default:
            break;
    }
}

void ofApp::receivedBang(ofxTLBangEventArgs &bang) {
    if (bang.track->getName() == TRACK_CLEARFBOS) {
        clearFbos();
    } 

#if 0
    else if (bang.track->getName() == "reset_video") {
        int frame = std::atoi(bang.flag.c_str());
        videoTrack->getPlayer()->setFrame(frame);
        videoTrack->getPlayer()->play();
    }
#endif
}


void ofApp::mouseMoved(int x, int y) {
    
}

void ofApp::mouseDragged(int x, int y, int button) {
    
}

void ofApp::mousePressed(int x, int y, int button) {
    
}

void ofApp::mouseReleased(int x, int y, int button) {
    
}

void ofApp::windowResized(int wi, int hei) {
}

void ofApp::gotMessage(ofMessage msg) {
    
}

void ofApp::dragEvent(ofDragInfo dragInfo) {
    
}
