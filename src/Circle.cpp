#include "Circle.h"
Circle::Circle(int radius_, int circleWidth_, ofVec3f center_)
{
    radius=radius_;
    circleWidth=circleWidth_;
    center=center_;

    ofPoint p(center.x,center.y,1);
    ring.circle(p,radius_+circleWidth_/2);
    ring.circle(p,radius_-circleWidth_/2);
	ring.close();
    ofColor c(255, 255,255);
	ring.setFillColor(c);
	ring.setFilled(true);
}
Circle::~Circle()
{
    //dtor
}
void Circle::update(ofVec3f center_)
{
    center=center_;

    ring.clear();
    ofPoint p(center);
    ring.circle(p,radius+circleWidth/2);
    ring.circle(p,radius-circleWidth/2);
	ring.close();
    ofColor c(255, 255,255);    // ring color ! -> new parameter ?
	ring.setFillColor(c);
	ring.setFilled(true);
}

void Circle::setRadius(int radius_)
{
    radius=radius_;
}
int Circle::getRadius()
{
    return radius;
}

int Circle::getCircleWidth()
{
    return circleWidth;
}

ofPath Circle::getRing()
{
    return ring;
}
