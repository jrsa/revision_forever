#include "ofMain.h"
#include "ofApp.h"

#define GL_MAJOR 3
#define GL_MINOR 3

//========================================================================
int main( ){

    ofGLWindowSettings settings;
    settings.setGLVersion(GL_MAJOR, GL_MINOR);
    ofCreateWindow(settings);
    ofRunApp(new ofApp());
}
