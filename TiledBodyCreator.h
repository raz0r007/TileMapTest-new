#ifndef __TILED_BODY_CREATOR_H__
#define __TILED_BODY_CREATOR_H__

#include "cocos2d.h"
#include <Box2D/Box2D.h>

USING_NS_CC;

class FixtureDef {
public:
    FixtureDef()
    : next(nullptr) {}
    
    ~FixtureDef() {
        delete next;
        delete fixture.shape;
    }
    
    FixtureDef *next;
    b2FixtureDef fixture;
    int callbackData;
};

class BodyDef {
public:
    BodyDef()
    : fixtures(nullptr) {}
    
    ~BodyDef() {
        if (fixtures)
            delete fixtures;
    }
    
    FixtureDef *fixtures;
    Point anchorPoint;
};

class TiledBodyCreator
{
public:
    static void initCollisionMap(cocos2d::TMXTiledMap* map, b2World* world);
	static FixtureDef* createFixture(ValueMap object);

	static FixtureDef* createPolygon(ValueMap object);
	static FixtureDef* createPolyline(ValueMap object);
	static FixtureDef* createCircle(ValueMap object);
	static FixtureDef* createRect(ValueMap object);
	
};

#endif // __TILED_BODY_CREATOR_H__
