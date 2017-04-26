#include "ofApp.h"
#include <cstdlib>

void ofApp::loadShaders() {
    flow.load("flow.vert", "flow.frag");
    repos.load("repos.vert", "repos.frag");
    blurX.load("generic.vert", "blurX.frag");
    blurY.load("generic.vert", "blurY.frag");
    gaussian.load("translate.vert", "gaussian.frag");
    hsv.load("generic.vert", "hsv.frag");
    contrast.load("generic.vert", "contrast.frag");
    kernel_shader.load("generic.vert", "jwarp.frag");
    bump.load("generic.vert", "bump.frag");
    fbmixer.load("generic.vert", "mixer_ex.frag");
    drymixer.load("generic.vert", "mixer.frag");
    
    geo_shader.load("displace.vert", "displaceTex.frag");
    blur_shader.load("generic.vert", "gaussian.frag");
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
    kernel_fbo.begin();
    ofClear(0, 0, 0, 0);
    kernel_fbo.end();
    hsv_fbo.begin();
    ofClear(0, 0, 0, 0);
    hsv_fbo.end();
    fbmixer_fbo.begin();
    ofClear(0, 0, 0, 0);
    fbmixer_fbo.end();
    drymixer_fbo.begin();
    ofClear(0, 0, 0, 0);
    drymixer_fbo.end();
    record_fbo.begin();
    ofClear(0, 0, 0, 0);
    record_fbo.end();
}

void ofApp::loadFbos(int w, int h) {
    greyTex.allocate(w, h, GL_RGBA);
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
    hsv_fbo.allocate(w, h, GL_RGBA);
    hsv_fbo.setUseTexture(true);
    contrast_fbo.allocate(w, h, GL_RGBA);
    contrast_fbo.setUseTexture(true);
    kernel_fbo.allocate(w, h, GL_RGBA);
    kernel_fbo.setUseTexture(true);
    bump_fbo.allocate(w, h, GL_RGBA);
    bump_fbo.setUseTexture(true);
    fbmixer_fbo.allocate(w, h, GL_RGBA);
    fbmixer_fbo.setUseTexture(true);
    drymixer_fbo.allocate(w, h, GL_RGBA);
    drymixer_fbo.setUseTexture(true);
    
    record_fbo.allocate(w, h, GL_RGB);
    record_fbo.setUseTexture(true);
    record_pix.allocate(w, h, 3);
    
    blur_fbo.allocate(ds_w, ds_h, GL_RGB);
    blur_fbo.setUseTexture(true);
    
    dest_fbo.allocate(w, h, GL_RGBA);
    dest_fbo.setUseTexture(true);
}


void ofApp::setup() {
//    framerate = 23.98;
    
    timeline.setup();
    timeline.setLoopType(OF_LOOP_NORMAL);
    timeline.setDurationInSeconds(180);
    
    timeline.addCurves(TIMELINE_FEEDBACK_MIX, ofRange(0, 1));
    timeline.addCurves(TIMELINE_DRY_MIX, ofRange(0, 1));
    timeline.addFlags(TIMELINE_CLEARFBOS);
    ofAddListener(timeline.events().bangFired, this, &ofApp::receivedBang);
    
    drawTimeline = false;
    iosdraw = false;
    enableBump = false;
    
    videoTrack = timeline.addVideoTrack("ios", "iphone_cap_1.mov");
    microsTrack = timeline.addVideoTrack("micro", "dry_dome_2ksquare.mov");
    
    // ios video
    ofPtr<ofVideoPlayer> pplyr = videoTrack->getPlayer();
    pplyr->getPlayer()->setLoopState(OF_LOOP_NORMAL);

    
    // microscope video
    pplyr = microsTrack->getPlayer();
//    ofPtr<ofVideoPlayer> microplyr = microsTrack->getPlayer();
    
    framerate = pplyr->getTotalNumFrames() / pplyr->getDuration();
    timeline.setFrameRate(framerate);
//    timeline.setDurationInFrames(pplyr->getTotalNumFrames());
//    timeline.setTimecontrolTrack(videoTrack);
    pplyr->setUseTexture(true);
    
    
    movTex.allocate(pplyr->getWidth(), pplyr->getHeight(), GL_RGBA);
    w = pplyr->getWidth();
    h = pplyr->getHeight();
    
    out_w = w;// /2;
    out_h = h;// /2;
    
    createPlane();
    iosdraw = true;
    
    downsampleAmt = 1.0/64.0;
    ds_w = pplyr->getWidth() * downsampleAmt;
    ds_h = pplyr->getHeight() * downsampleAmt;
    
    displacement_map.load("Displacements.png");
    displaceTex.allocate(displacement_map.getWidth(), displacement_map.getHeight(), GL_RGBA);
    displaceTex.loadData(displacement_map.getPixels());
    
    blur_fbo.allocate(ds_w, ds_h, GL_RGB);
    blur_fbo.setUseTexture(true);
    
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
    
    cam.setDistance(1280);
    
//    timeline.play();
}


void ofApp::update() {
//    if (record) {
//        vidRecorder.addFrame(record_pix);
//    }
    
    ofPtr<ofVideoPlayer> plr = videoTrack->getPlayer();
    
    if(plr->isFrameNew()) {
        movTex = plr->getTexture();
    }
    
    plr = microsTrack->getPlayer();
    if(plr->isFrameNew()) {
        lastTex = greyTex;
        greyTex = plr->getTexture();
    }
}

void ofApp::draw() {
//    if(record) {
        record_fbo.begin();
//    }
    
    if(iosdraw) {
        dest_fbo.begin();
        ofClear(0, 0, 0, 0);
        drawPlane();
        dest_fbo.end();
        
        drymixer_fbo.begin();
        drymixer.begin();
        drymixer.setUniformTexture("tex1", dest_fbo.getTexture(), 1);
        drymixer.setUniform1f("mixAmnt", timeline.getValue(TIMELINE_DRY_MIX));
        greyTex.draw(0, 0); // microscope footage
        drymixer.end();
        drymixer_fbo.end();
    }
    else {
        dest_fbo.begin();
        ofClear(0, 0, 0, 0);
//        drawPlane();
        dest_fbo.end();
        
        drymixer_fbo.begin();
        drymixer.begin();
        drymixer.setUniform1f("mixAmnt", 0.0);
        greyTex.draw(0, 0); // microscope footage
        drymixer.end();
        drymixer_fbo.end();
    }
    
    flow_fbo.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    flow_fbo.begin(); //flow
    
    ofClear(0, 0, 0, 0);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    
    flow.begin();
    flow.setUniform2f("scale", 12.0, 12.0);
    flow.setUniform2f("offset", 100.0, 100.0);
    flow.setUniform1f("lambda", 100.0);
    flow.setUniformTexture("tex1", lastTex, 1);
    
    drymixer_fbo.getTexture().draw(0, 0);
    
    flow.end();
    
    glDisable(GL_BLEND);
    flow_fbo.end();
    
    blurX_fbo.begin(); //blur x
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);
    
    blurX.begin();
    blurX.setUniform1f("blurAmnt", 1.0);
    
    flow_fbo.draw(0, 0);
    blurX.end();
    
    //flow_fbo.draw(0, 0); //how to draw flow shader solo?
    
    glDisable(GL_BLEND); //should this end before or after blurY?
    blurX_fbo.end();
    
    blurY_fbo.begin(); //blur y
    blurY.begin();
    blurY.setUniform1f("blurAmnt", 1.0);
    
    blurX_fbo.draw(0, 0);
    blurY.end();
    blurY_fbo.end();
    
    //repos
    repos_fbo.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    repos_fbo.begin();
    
    ofClear(255, 255, 255, 0);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR);
    
    repos.begin();
    repos.setUniform2f("amt", 50.0, 50.0);
    repos.setUniformTexture("tex1", blurY_fbo.getTexture(), 1);
    
    fbmixer_fbo.draw(0, 0);
    
    repos.end();
    
    glDisable(GL_BLEND);
    repos_fbo.end();
    
    gaussian_fbo.begin();
    gaussian.begin();
    
    gaussian.setUniform2f("zoom", 1.0, 1.0);
    gaussian.setUniform1f("blurAmnt", 1.0);
    
    repos_fbo.draw(0, 0);
    
    gaussian.end();
    gaussian_fbo.end();
    
    //CONTRAST
    contrast_fbo.begin();
    contrast.begin();
    
    contrast.setUniform1f("contrast", 1.00);
    
    gaussian_fbo.draw(0, 0);
    
    contrast.end();
    contrast_fbo.end();
    
    //KERNEL
    kernel_fbo.begin();
    kernel_shader.begin();
    
    //sharp
    float sharp_kernel[] = {
        0.5, -0.5,  0.5,
        -0.5, 3.0, -0.5,
        0.5, -0.5,  0.5
    };
    
    //edge //-2.0
    float edge_kernel[] = {
        2.0/8.0, 2.0/8.0, 2.0/8.0,
        2.0/8.0,  3.0,    2.0/8.0,
        2.0/8.0, 2.0/8.0, 2.0/8.0
    };
    
    kernel_shader.setUniform2f("step", 1.0, 1.0);
    kernel_shader.setUniform1fv("kernel", edge_kernel, 9);
    
    contrast_fbo.draw(0, 0);
    kernel_shader.end();
    kernel_fbo.end();
    
    //HSV
    hsv_fbo.begin();
    hsv.begin();
    
    hsv.setUniform1f("hue", 0.00);
    hsv.setUniform1f("sv",  1.01);
    
    kernel_fbo.draw(0, 0);
    
    hsv.end();
    hsv_fbo.end();
    
    //MIXER
    fbmixer_fbo.begin();
    fbmixer.begin();
    fbmixer.setUniformTexture("tex1", drymixer_fbo.getTexture(), 1);
    fbmixer.setUniformTexture("tex2", blurY_fbo.getTexture(), 2); // flow map controls mix amt
    
    float mix = timeline.getValue(TIMELINE_FEEDBACK_MIX);
    fbmixer.setUniform1f("mixAmnt", mix);
    
    hsv_fbo.draw(0, 0); // end of feedback chain
    fbmixer.end();
    fbmixer_fbo.end();

    if(enableBump) {
        bump_fbo.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
        bump_fbo.begin();
        bump.begin();
        bump.setUniform2f("res", w, h);
    //    float posX = cos(ofGetElapsedTimef()*0.1) * w + w*0.5;
    //    float posY = sin(ofGetElapsedTimef()*0.1) * h + h*0.5;
        bump.setUniform2f("mouse", mouseX, -mouseY);
    //    float normalStrength = ofMap(mix, 0.15, 0.9, 200.0, 1.0);
        bump.setUniform1f("normalStrength", 150.0);
        bump.setUniform1f("lightWidth", w*10);
        bump.setUniform1f("lightBrightness", 0.75);
        bump.setUniform1f("shinAmnt", 0.1);
        fbmixer_fbo.draw(0, 0);
        bump.end();
        bump_fbo.end();
    
        bump_fbo.draw(0, 0);
    } else {
        fbmixer_fbo.draw(0, 0);
    }
    
//    if(record) {
//        record_fbo.readToPixels(record_pix);
        record_fbo.end();
        record_fbo.draw(0, 0, out_w, out_h);
//    }
    
    if (drawTimeline) {
        timeline.draw();
    }
}

void ofApp::drawPlane() {
    blur_fbo.begin(); // draw screencap through blur
    blur_shader.begin();
    videoTrack->getPlayer()->draw(0, 0, ds_w, ds_h);
    blur_shader.end();
    blur_fbo.end();
    
    ofEnableDepthTest();
    ofPushMatrix();
    
    cam.begin();
    geo_shader.begin();
    
    float t = int(ofGetElapsedTimef() * 50.0) % int(displaceTex.getWidth() * 0.5);
    
    geo_shader.setUniform1f("time", t);
    geo_shader.setUniform2f("resolution", movTex.getWidth(), movTex.getHeight());
    geo_shader.setUniform1f("downsampleAmt", downsampleAmt);
    geo_shader.setUniform1f("displaceAmt", 350.0);
    geo_shader.setUniformTexture("tex0", blur_fbo.getTexture(), 0);
    geo_shader.setUniformTexture("tex1", movTex, 1);
    geo_shader.setUniformTexture("tex2", displaceTex, 2);
    
    plane.draw(); // draw plane, deformed and with footage
    
    geo_shader.end();
    cam.end();
    
    ofPopMatrix();
    ofDisableDepthTest();
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
        case 'b': {
            enableBump = !enableBump;
            break;
        }
        case 'c': {
            record = false;
            vidRecorder.close();
            break;
        }
        case 'i': {
            iosdraw = !iosdraw;
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
        default:
            break;
    }
}

void ofApp::receivedBang(ofxTLBangEventArgs &bang) {
    ofLogNotice("Bang fired from track " + bang.flag);
    

    clearFbos();

    int frame = std::atoi(bang.flag.c_str());
    microsTrack->getPlayer()->setFrame(frame);
    microsTrack->getPlayer()->play();
}

void ofApp::createPlane() {
    float planeScale = 0.75;
    float aspect = 9.0 / 16.0;
    
    int planeWidth = int(ofGetWidth() * planeScale * aspect);
    int planeHeight = ofGetHeight() * planeScale;
    int planeGridSize = 2;
    int planeColumns = planeWidth / planeGridSize;
    int planeRows = planeHeight / planeGridSize;
    
    plane.set(planeWidth, planeHeight, planeColumns, planeRows, OF_PRIMITIVE_TRIANGLES);
    plane.mapTexCoordsFromTexture(movTex);
    plane.enableNormals();
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
//    w = wi;
//    h = hei;
//    
//    loadFbos(w, h);
//    loadShaders();
//    clearFbos();
}

void ofApp::gotMessage(ofMessage msg) {
    
}

void ofApp::dragEvent(ofDragInfo dragInfo) {
    
}
