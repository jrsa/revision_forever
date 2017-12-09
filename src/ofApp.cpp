#include "ofApp.h"
#include "timeline_tracks.h"

void ofApp::loadShaders() {
    flow.load("flow.vert", "flow.frag");
    repos.load("repos.vert", "repos.frag");
    blurX.load("generic.vert", "blurX.frag");
    blurY.load("generic.vert", "blurY.frag");
    gaussian.load("translate.vert", "gaussian.frag");
    fbmixer.load("generic.vert", "mixer_ex.frag");
    noise.load("generic.vert", "noise.frag");
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
    ofFbo::Settings shared_fbo_settings;
    shared_fbo_settings.width = w;
    shared_fbo_settings.height = h;
    shared_fbo_settings.colorFormats.push_back(GL_RGB);
    shared_fbo_settings.minFilter = GL_LINEAR;
    shared_fbo_settings.maxFilter = GL_LINEAR;
    shared_fbo_settings.wrapModeHorizontal = GL_REPEAT;
    shared_fbo_settings.wrapModeVertical = GL_REPEAT;
    shared_fbo_settings.internalformat = GL_RGB;
    shared_fbo_settings.textureTarget = GL_TEXTURE_2D;

    ofTextureData texData;
    texData.textureTarget = GL_TEXTURE_2D;
    texData.glInternalFormat = GL_RGB;
    texData.width = w;
    texData.height = h;

    n_vid_in = 3;

    vid_in = new ofTexture[n_vid_in];
    for (int i = 0; i >= n_vid_in; i++) {
        vid_in[i].allocate(texData);
    }

    record_pix.allocate(w, h, 3);

    flow_fbo.allocate(shared_fbo_settings);
    repos_fbo.allocate(shared_fbo_settings);
    blurX_fbo.allocate(shared_fbo_settings);
    blurY_fbo.allocate(shared_fbo_settings);
    gaussian_fbo.allocate(shared_fbo_settings);
    fbmixer_fbo.allocate(shared_fbo_settings);

    record_fbo.allocate(w, h, GL_RGB);
}


void ofApp::setup() {
    framerate = 23.98;
    
    timeline.setup();
    timeline.setLoopType(OF_LOOP_NORMAL);
    
    timeline.addCurves(TRACK_FEEDBACK_MIX, ofRange(0, 1));
    timeline.addCurves(TRACK_REPOS, ofRange(-1, 1));
    timeline.addFlags(TRACK_CLEARFBOS);
    ofAddListener(timeline.events().bangFired, this, &ofApp::receivedBang);
    
    drawTimeline = false;
    
    videoTrack = timeline.addVideoTrack(TRACK_VID, "/Users/jrsa/Desktop/SCRATCH/me.mov");
    ofPtr<ofVideoPlayer> pplyr = videoTrack->getPlayer();
    
    framerate = pplyr->getTotalNumFrames() / pplyr->getDuration();
    timeline.setFrameRate(framerate);
    timeline.setDurationInFrames(pplyr->getTotalNumFrames());
    pplyr->setUseTexture(false);
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
    
    pplyr->play();
}

stringstream ss;
void ofApp::update() {
    if (record) {
        vidRecorder.addFrame(record_pix);
    }
    
    ofPtr<ofVideoPlayer> plr = videoTrack->getPlayer();
    if(plr->isFrameNew()) {
        vid_in[write_vid_in].loadData(plr->getPixels());

        write_vid_in = (write_vid_in + 1) % n_vid_in;
        read_vid_in = write_vid_in + 1;

        if (read_vid_in >= n_vid_in) {
            read_vid_in -= n_vid_in;
        }
    }

    ss.str("");
    ss << ofGetFrameRate() << std::setw(2) << " fps";
    ofSetWindowTitle(ss.str());
}

void ofApp::draw() {
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
    flow.setUniformTexture("tex1", vid_in[read_vid_in], 1);
    
    vid_in[write_vid_in].draw(0, 0);
    flow.end();
    
    glDisable(GL_BLEND);
    flow_fbo.end();

#if 1
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
#endif
    
    // texture displacement/repositioning based on hsflow map
    // ----------------------------------------------------------------------------
    repos_fbo.begin();
    ofClear(0, 0, 0, 0);
    if(seed) {
        noise.begin();
        fbmixer_fbo.draw(0, 0);
        noise.end();
        seed = false;
    } else {    
        repos.begin();
        float reposAmt = timeline.getValue(TRACK_REPOS);
        repos.setUniform2f("amt", reposAmt, reposAmt);

        repos.setUniformTexture("tex1", flow_fbo.getTexture(), 1);

        // the tex0 input is a mix of the video input and feedback
        fbmixer_fbo.draw(0, 0);
        repos.end();
    }
    repos_fbo.end();

    // gaussian blur in feedback path
    // ----------------------------------------------------------------------------
    gaussian_fbo.begin();
    gaussian.begin();
    gaussian.setUniform2f("zoom", 1.0, 1.0);
    gaussian.setUniform2f("dim", repos_fbo.getTexture().getWidth(), repos_fbo.getTexture().getHeight());
    gaussian.setUniform1f("blurAmnt", 1.0);
    repos_fbo.draw(0, 0);
    gaussian.end();
    gaussian_fbo.end();

    //MIXER between video input and feedback path
    // ----------------------------------------------------------------------------
    fbmixer_fbo.begin();
    fbmixer.begin();

    fbmixer.setUniformTexture("tex1", vid_in[write_vid_in], 1);
    fbmixer.setUniformTexture("tex2", flow_fbo.getTexture(), 2);
    
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
    record_fbo.draw(0, 0, ofGetWidth(), ofGetHeight());
    
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
        case 'R': {
            record = !record;
            if (record && !vidRecorder.isInitialized()){
                vidRecorder.setup(filename + ofGetTimestampString() + container, w, h, framerate);
                vidRecorder.start();
            }
            break;
        }
        case 'r': {
            loadShaders();
            break;
        }
        case 's': {
            seed = true;
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
