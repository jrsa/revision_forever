#include "ofMain.h"
#include "ofApp.h"

#define GL_MAJOR 2
#define GL_MINOR 1

//========================================================================
int main( ){

    ofGLWindowSettings settings;
    settings.setGLVersion(GL_MAJOR, GL_MINOR);
    ofCreateWindow(settings);
    ofRunApp(new ofApp());
}
