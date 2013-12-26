#include "testApp.h"
#include "ofxOsc.h"
//--------------------------------------------------------------
void testApp::setup()
{
    ofSetWindowTitle("VisualSynth");
    cout<<"***********************VisualSynth******************************"<<endl;
    OscReceiver.setup(8000);
    oscSender.setup("127.0.0.1",12000);
    profZ=1000; //a modifier, pourra être dans le fichier de config.
    for (int i=0; i<nbSynthsForBalls;i++)
    {
        synthsForBalls.push_back(true);
    }

    // image
    texBall.loadImage("/home/nico/of_v0.8.0_linux_release/apps/myApps/CrampTacle/cerf.png");

    // each ball has a plane
    ballPlane.set(100, 100); // initials values, change at the first display
    ballPlane.setPosition(200,200,10); //idem
    ballPlane.mapTexCoords(0, 0, texBall.getWidth(), texBall.getHeight());
}

//--------------------------------------------------------------
void testApp::update()
{
    ofBackground(50,50,255);

    receiveOscMessage();
    for(vector< ofPtr<Ball> >::iterator it = theBalls.begin(); it != theBalls.end(); ++it)
        {
            (*it)->update();
            sendOscInfos((*it));
        }
}

//--------------------------------------------------------------
void testApp::draw()
{
    ofSetHexColor(0xffffff);

    // we display each ball
    texBall.bind();
    for (int i=0;i<(int)theBalls.size();i++) {
        ballPlane.set((*theBalls[i]).getRadius(),(*theBalls[i]).getRadius());
        ballPlane.setPosition((*theBalls[i]).getPosition().x, (*theBalls[i]).getPosition().y,10);
        ballPlane.draw();
    }
    texBall.unbind();

    // we display each ring from each ball
    for (int i=0;i<(int)theBalls.size();i++) {
        for (int k=0; k<(*theBalls[i]).getNbCircles();k++)
        {
            (*((*theBalls[i]).getTheCircles()[k])).getRing().draw();
        }
    }

}
//--------------------------------------------------------------
void testApp::keyPressed(int key)
{
    //cout<<"key pressed : "<<key<<endl;
    if (key==70||key==102)
    {
        cout<<"toggle fullscreen"<<endl;
        ofToggleFullscreen();
    }
    if (key == 32) //space to create a ring around the last ball
    {
        int ballCircleMargin = 10;
        int ringWidth = 10;
        (*theBalls.back()).addCircle((*theBalls.back()).getRadius()+ballCircleMargin,ringWidth);
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key)
{

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y )
{

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{
    theBalls.push_back(ofPtr<Ball> (new Ball(x,y,0,attributeSynth())));
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo)
{

}
void testApp::receiveOscMessage()
{

    while(OscReceiver.hasWaitingMessages())
    {

        OscReceiver.getNextMessage(&OscReceivedMessage);
        cout<<"received a message : "<<ofToString(OscReceivedMessage.getAddress())<<endl;
        if(OscReceivedMessage.getAddress()=="/pad/1")
        {
            //cout<<"first Finger"<<endl;
            //Si jamais, on pourra toujours atribuer des paramètres différents en fonction du nombre de doigts.
            float xVal=OscReceivedMessage.getArgAsFloat(0)*ofGetWidth();
            float yVal=OscReceivedMessage.getArgAsFloat(1)*ofGetHeight();
            cout<<xVal<<" "<<yVal<<endl;
            theBalls.push_back(ofPtr<Ball> (new Ball (xVal,yVal,0,attributeSynth())));
        }
        else if(OscReceivedMessage.getAddress()=="/pad/2")//deuxième doigt
        {
            theBalls.push_back(ofPtr<Ball> (new Ball (OscReceivedMessage.getArgAsFloat(0)*ofGetWidth(),OscReceivedMessage.getArgAsFloat(1)*ofGetHeight(),0, attributeSynth())));
        }
        else if(OscReceivedMessage.getAddress()=="/pad/3")//troisième doigt
        {
            theBalls.push_back(ofPtr<Ball> (new Ball (OscReceivedMessage.getArgAsFloat(0)*ofGetWidth(),OscReceivedMessage.getArgAsFloat(1)*ofGetHeight(),0,attributeSynth())));
        }
        else if(OscReceivedMessage.getAddress()=="/pad/4")//quatrième doigt
        {
            theBalls.push_back(ofPtr<Ball> (new Ball (OscReceivedMessage.getArgAsFloat(0)*ofGetWidth(),OscReceivedMessage.getArgAsFloat(1)*ofGetHeight(),0,attributeSynth())));
        }
        else if(OscReceivedMessage.getAddress()=="/pad/5")//cinquième doigt
        {
            theBalls.push_back(ofPtr<Ball> (new Ball (OscReceivedMessage.getArgAsFloat(0)*ofGetWidth(),OscReceivedMessage.getArgAsFloat(1)*ofGetHeight(),0,attributeSynth())));
        }
        else if (OscReceivedMessage.getAddress()=="/1/multifader2/1")//durée de vie de la balle
        {
            lifeSpeed=ofMap(OscReceivedMessage.getArgAsFloat(0),0,1,0.9,1);
        }
        else if (OscReceivedMessage.getAddress()=="/1/multifader2/2")//vitesse de déplacement
        {
           ballNoiseFactor=(int)200*OscReceivedMessage.getArgAsFloat(0);
        }
        else if (OscReceivedMessage.getAddress()=="/1/multifader2/3")//position en z
        {
           zPos=ofMap(OscReceivedMessage.getArgAsFloat(0),0,1,0,profZ);
        }
        else
            {
                cout<<"I don't know this message "<<endl;
            }
    }
}
//fonction qui va envoyer par osc tout les paramètres de la balle.
void testApp::sendOscInfos(ofPtr<Ball>& ballToSend)
{
    int synthNumber=ballToSend->getSynthNumber();
    string address="/ball"+ofToString(synthNumber)+"/radius";
    ofxOscMessage msgToSend = ofxOscMessage();
    msgToSend.setAddress(address);
    //-----
    msgToSend.addFloatArg(ballToSend->getPosition().x);
    msgToSend.addFloatArg(ballToSend->getPosition().y);
    msgToSend.addFloatArg(ballToSend->getPosition().z);
    msgToSend.addFloatArg(ballToSend->getRadius());
    //-----
    //ajouter ici les autre paramètres pour envoyer toutes les infos dans une seule trame ...après tout l'osc c'est fait pour ca.
    oscSender.sendMessage(msgToSend);
   //cout<<"Sended : "<<value<<" @"<< address<<endl;
}
int testApp::attributeSynth()
{
    //!!! on ne pourra pas créer plus de balles que de synthés !
    int i;
    while(synthsForBalls[i]==false)
    {
        if(synthsForBalls[i]==true)
        {
            synthsForBalls[i]=false;
            break;
        }
        i=ofRandom(nbSynthsForBalls);
    }
    return i;
}
