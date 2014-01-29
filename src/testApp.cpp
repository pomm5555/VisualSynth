#include "testApp.h"
#include "ofxOsc.h"
//--------------------------------------------------------------
void testApp::setup()
{
    ofSetWindowTitle("VisualSynth");
    cout<<"***********************VisualSynth******************************"<<endl;
    OscReceiver.setup(8000);
    readXmlSetup();
    OscReceiver.setup(oscReceivePort);
    oscSender.setup(oscSendAddress,oscSendPort);
    profZ=1000; //a modifier, pourra être dans le fichier de config.
    for (int i=0; i<nbSynthsForBalls;i++)
    {
        synthsForBalls.push_back(true);
    }
    ballNoiseFactor=200;
    lifeSpeed=0.995;
    minDistToLink=100;
    maxDistToUnlink=200;

    int immortal = 1;
    int permanentBallCenterX1 = 0.25*ofGetWindowWidth();
    int permanentBallCenterX2 = 0.75*ofGetWindowWidth();
    int permanentBallCenterY = 0.5*ofGetWindowHeight();

    permanentBalls.push_back(ofPtr<Ball> (new Ball(-1,permanentBallCenterX1,permanentBallCenterY,0,0,pathToImages,200,0,immortal,400)));
    permanentBalls.push_back(ofPtr<Ball> (new Ball(-1,permanentBallCenterX2,permanentBallCenterY,0,0,pathToImages,200,0,immortal,400)));
}

//--------------------------------------------------------------
void testApp::update()
{
    receiveOscMessage();
    for(vector< ofPtr<Ball> >::iterator it = theBalls.begin(); it != theBalls.end(); ++it)
        {
            (*it)->update();
            sendOscInfos((*it));
            for(vector< ofPtr<Ball> >::iterator sit = theBalls.begin(); sit != theBalls.end(); ++sit)
            {
                if((*sit)->getPosition().distance((*it)->getPosition())<minDistToLink)
                {
                    (*it)->addLink((*sit)->getRefNumber());
                }
                else if((*it)->checkLink((*sit)->getRefNumber())&&(*sit)->getPosition().distance((*it)->getPosition())>maxDistToUnlink)
                {
                    (*it)->removeLink((*sit)->getRefNumber());
                    //cout<<"link removed ["<<(*sit)->getRefNumber()<<","<<(*it)->getRefNumber()<<"]"<<endl;
                }
            }
            if((*it)->checkIfDead()==true)
            {
                 theBalls.erase(it);
                 break;
            }
        }

    for(vector< ofPtr<Ball> >::iterator it = permanentBalls.begin(); it != permanentBalls.end(); ++it)
        {
            if ((*it)->getRadius() > 180 )
            {
                (*it)->setRadius((*it)->getRadius()+ofRandom(-2,2));
            }

            else
                (*it)->setRadius((*it)->getRadius()+ofRandom(2,6));

        }
}

//--------------------------------------------------------------
void testApp::draw()
{
    ofBackground(0);
    ofSetHexColor(0xffffff);
    for(vector< ofPtr<Ball> >::iterator it = theBalls.begin(); it != theBalls.end(); ++it)
        {
            (*it)->draw();
            for(vector< ofPtr<Ball> >::iterator sit = it; sit != theBalls.end(); ++sit)
            {
                if((*it)->checkLink((*sit)->getRefNumber()))
                {
                    ofVec3f p1,p2,d,n;
                    p1=(*it)->getPosition();
                    p2=(*sit)->getPosition();
                    d = p2-p1;  // vecteur p1p2
                    n.x = d.y;  // n : vecteur normal à d
                    n.y = -d.x;
                    ofPoint cp1 = ofPoint(p1 + 0.3*d+0.4*n); // point de contrôle
                    ofPolyline link;
                    link.quadBezierTo(p1+0*d,cp1,p2-0*d);
                    link.draw();
                }
            }
        }
    // we display each ball and each link

    for(vector< ofPtr<Ball> >::iterator it = permanentBalls.begin(); it != permanentBalls.end(); ++it)
        {
            (*it)->draw();
        }
}
//--------------------------------------------------------------
void testApp::keyPressed(int key)
{
    cout<<"key pressed : "<<key<<endl;
    if (key==70||key==102)
    {
        cout<<"toggle fullscreen"<<endl;
        ofToggleFullscreen();
    }

    if (key == 32) //space to create a ring around the last ball
    {
        if (theBalls.size() != 0)
        {
             int ringWidth = 5;
            (*theBalls.back()).addCircle((*theBalls.back()).getRadius(),ringWidth);
        }
    }
    if (key == 114||key==82)// r or R
    {
        cout<<"all ball unloaded"<<endl;
       theBalls.clear();
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
    int synthNbr=attributeSynth();
    cout<<"Synth "<<synthNbr<<endl;
    cout<<"NF"<<ballNoiseFactor<<endl;
    cout<<"LS"<<lifeSpeed<<endl;
    theBalls.push_back(ofPtr<Ball> (new Ball(refNumber,x,y,0,synthNbr,pathToImages,ofRandom(10,50),ballNoiseFactor,lifeSpeed)));
    refNumber+=1;
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
        //cout<<"received a message : "<<ofToString(OscReceivedMessage.getAddress())<<endl;
        if(OscReceivedMessage.getAddress()=="/pad/1"||OscReceivedMessage.getAddress()=="/1/multixy1/1")
        {
            //cout<<"first Finger"<<endl;
            //Si jamais, on pourra toujours atribuer des paramètres différents en fonction du nombre de doigts.
            float xVal=OscReceivedMessage.getArgAsFloat(0)*ofGetWidth();
            float yVal=OscReceivedMessage.getArgAsFloat(1)*ofGetHeight();
            cout<<xVal<<" "<<yVal<<endl;
            theBalls.push_back(ofPtr<Ball> (new Ball (refNumber,xVal,yVal,zPos,attributeSynth(),pathToImages,ofRandom(10,50),ballNoiseFactor,lifeSpeed)));
            refNumber+=1;
        }
        else if(OscReceivedMessage.getAddress()=="/pad/2"||OscReceivedMessage.getAddress()=="/1/multixy/2")//deuxième doigt
        {
            theBalls.push_back(ofPtr<Ball> (new Ball (refNumber,OscReceivedMessage.getArgAsFloat(0)*ofGetWidth(),OscReceivedMessage.getArgAsFloat(1)*ofGetHeight(),zPos,attributeSynth(),pathToImages,ofRandom(10,50),ballNoiseFactor,lifeSpeed)));
            refNumber+=1;
        }
        else if(OscReceivedMessage.getAddress()=="/pad/3"||OscReceivedMessage.getAddress()=="/1/multixy/3")//troisième doigt
        {
            theBalls.push_back(ofPtr<Ball> (new Ball (refNumber,OscReceivedMessage.getArgAsFloat(0)*ofGetWidth(),OscReceivedMessage.getArgAsFloat(1)*ofGetHeight(),zPos,attributeSynth(),pathToImages,ofRandom(10,50),ballNoiseFactor,lifeSpeed)));
            refNumber+=1;
        }
        else if(OscReceivedMessage.getAddress()=="/pad/4"||OscReceivedMessage.getAddress()=="/1/multixy/4")//quatrième doigt
        {
            theBalls.push_back(ofPtr<Ball> (new Ball (refNumber,OscReceivedMessage.getArgAsFloat(0)*ofGetWidth(),OscReceivedMessage.getArgAsFloat(1)*ofGetHeight(),zPos,attributeSynth(),pathToImages,ofRandom(10,50),ballNoiseFactor,lifeSpeed)));
            refNumber+=1;
        }
        else if(OscReceivedMessage.getAddress()=="/pad/5"||OscReceivedMessage.getAddress()=="/1/multixy/5")//cinquième doigt
        {
            theBalls.push_back(ofPtr<Ball> (new Ball (refNumber,OscReceivedMessage.getArgAsFloat(0)*ofGetWidth(),OscReceivedMessage.getArgAsFloat(1)*ofGetHeight(),zPos,attributeSynth(),pathToImages,ofRandom(10,50),ballNoiseFactor,lifeSpeed)));
            refNumber+=1;
        }
        else if (OscReceivedMessage.getAddress()=="/1/multifader1/1")//durée de vie de la balle
        {
            lifeSpeed=ofMap(OscReceivedMessage.getArgAsFloat(0),0,1,0.9,1);
        }
        else if (OscReceivedMessage.getAddress()=="/1/multifader1/2")//vitesse de déplacement
        {
           ballNoiseFactor=(int)200*OscReceivedMessage.getArgAsFloat(0);
        }
        else if (OscReceivedMessage.getAddress()=="/1/multifader1/3")//position en z
        {
           zPos=ofMap(OscReceivedMessage.getArgAsFloat(0),0,1,0,profZ);
        }
        else
            {
                cout<<"I don't know this message :"<<ofToString(OscReceivedMessage.getAddress())<<endl;
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
    int i=ofRandom(nbSynthsForBalls);
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
void testApp::readXmlSetup()
{
    ofFile file;
    //TODO block the process if the file is not there or is corrupted....
    //Test the file existance throw an error if not
    if(file.doesFileExist("config.xml",true)==true)
    {
        cout<<"config.xml found"<<endl;
        cout<<"*****************************************************************"<<endl<<endl;
    }
    else
    {
        cout<<"no config.xml file, default receive port : 8000"<<endl;
        oscReceivePort=8000;
        OscReceiver.setup(oscReceivePort);
        return;
    }

    file.open("config.xml");
    ofBuffer buffer=file.readToBuffer();
    ofXml configFile;
    configFile.loadFromBuffer(buffer.getText());
    configFile.setTo("osc");
    oscReceivePort=configFile.getIntValue("receivePort");
    cout<<"receivePort :"<<oscReceivePort<<endl;
    oscSendPort=configFile.getIntValue("sendPort");
    cout<<"sendPort :"<<oscSendPort<<endl;
    oscSendAddress=configFile.getValue("sendIp");
    cout<<"sendAddress :"<<oscSendAddress<<endl;
    configFile.setTo("../screen"); // go up and then down
    profZ=configFile.getIntValue("profondeurZ");
    cout<<"profZ :"<<profZ<<endl;
    configFile.setTo("../synth"); // go up and then down
    nbSynthsForBalls=configFile.getIntValue("nbSynthsForBalls");
    cout<<"nbSynthsForBalls :"<<nbSynthsForBalls<<endl;
    configFile.setTo("../textures");
    pathToImages=configFile.getValue("path");
    cout<<"path to Images :"<<pathToImages<<endl;

    file.close();
    buffer.clear();
    configFile.clear();
    cout<<endl<<"*****************************************************************"<<endl;
    cout<<"XML files read, objects created" <<endl;
}

