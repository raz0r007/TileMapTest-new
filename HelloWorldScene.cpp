/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "TiledBodyCreator.h"
#include "B2DebugDrawLayer.h"


USING_NS_CC;

b2World* _world;
TMXTiledMap *tileMap;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    Director::getInstance()->getScheduler()->scheduleUpdate(HelloWorld::getInstance(), 1, false); //update()
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
  
    
    //box2d world

    //b2Vec2 gravity(0, -9.8);
    b2Vec2 gravity(0, 0);
    _world = new b2World(gravity);
    _world->SetContactListener(this);
    _world->SetAllowSleeping(false);

    // Create map
    tileMap = new TMXTiledMap;
    tileMap->initWithTMXFile("RaceTrackMap64.tmx");
    tileMap->setPosition(Vec2(origin.x, origin.y));
    addChild(tileMap, 0);
 
    TiledBodyCreator::initCollisionMap(tileMap, _world);

    // Add the car in the TMX position..
    TMXObjectGroup *objectGroup = tileMap->getObjectGroup("Objects");
    auto &objects = objectGroup->getObjects();
    _car = Sprite::create("car.png");
    
    for (auto& obj : objects)
    {
        ValueMap& dict = obj.asValueMap();
        
        float x = dict["x"].asFloat();
        float y = dict["y"].asFloat();
        
        if(dict["name"].asString() == "myCar")
        {
            _car->setScale(0.09, 0.09);
            _car->setName("myCar");
            _car->setPosition(Vec2(x, y));
            tileMap->addChild(_car, 1);
            
            //Box2d car
            b2BodyDef bdCarDef;
            bdCarDef.type = b2_dynamicBody;
            bdCarDef.position = b2Vec2(x/SCALE_RATIO, y/SCALE_RATIO);
            bdCarDef.userData = _car;
            auto bCarBody = _world->CreateBody(&bdCarDef);
         //   bCarBody->SetLinearVelocity(b2Vec2(0,1));//1.f/SCALE_RATIO));
           // assert(bCarBody->GetUserData() == _car);

            // add fixture
            float w = _car->getBoundingBox().size.width;
            float h = _car->getBoundingBox().size.height;
            float boxWidth = (w/2) / SCALE_RATIO;
            float boxHeight = (h/2) / SCALE_RATIO;
            b2PolygonShape psCarShape;
            psCarShape.SetAsBox(boxWidth, boxHeight);
            bCarBody->CreateFixture(&psCarShape, 0);

            // Treat as Sensor and move by `setTransform`
            //   (would be useful for something like a moving "coin" pickup object)
//            b2FixtureDef fdCarFixture;
//            fdCarFixture.isSensor = true;
//            fdCarFixture.shape = &psCarShape;
//            bCarBody->CreateFixture(&fdCarFixture);
        }
    }

    
    tileMap->runAction(Follow::create(tileMap->getChildByName("myCar")));

    auto debugLayer = B2DebugDrawLayer::create(_world, SCALE_RATIO);
    debugLayer->setGlobalZOrder(99);
    tileMap->addChild(debugLayer);
    
    /*
    ///// TEST Physics Sim to show gravity/forces are working.
    // REQUIRES: `b2Vec2 gravity(0, -9.8);` *** OR *** set velocity
    b2BodyDef myBodyDef;
    myBodyDef.type = b2_dynamicBody; //this will be a dynamic body
    myBodyDef.position.Set(1, 7); //set the starting position
    myBodyDef.angle = 0; //set the starting angle

    b2Body* dynamicBody = _world->CreateBody(&myBodyDef);

    b2PolygonShape boxShape;
    boxShape.SetAsBox(1,1);

    b2FixtureDef boxFixtureDef;
    boxFixtureDef.shape = &boxShape;
    boxFixtureDef.density = 1;
    dynamicBody->CreateFixture(&boxFixtureDef);

    // NOTE: only if no gravity (otherwise will move too fast)
    dynamicBody->SetLinearVelocity(b2Vec2(0,-10));

    myBodyDef.type = b2_staticBody; //this will be a static body
    myBodyDef.position.Set(1, 1); //slightly lower position
    b2Body* staticBody = _world->CreateBody(&myBodyDef); //add body to world
    staticBody->CreateFixture(&boxFixtureDef); //add fixture to body
*/
    
    

    return true;
}


HelloWorld* HelloWorld::getInstance()
{
    static HelloWorld sharedInstance;
    return &sharedInstance;
}

void HelloWorld::update(float dt)
{
    if(_world != NULL)
    {
        Vec2 origin = Director::getInstance()->getVisibleOrigin();
        auto visibleSize = Director::getInstance()->getVisibleSize();
        
        int velocityIterations = 8;
        int positionIterations = 3;
        
        _world->Step(dt, velocityIterations, positionIterations);

        for (b2Body* b = _world->GetBodyList(); b != NULL; b = b->GetNext())
        {
            if (b->GetUserData() != NULL) {
                Sprite* s = (Sprite*)b->GetUserData();
                // NOTE: using `b->SetLinearVelocity` instead
                if(s->getName() == "myCar") {
                //    b->SetTransform(b2Vec2(s->getPositionX()/SCALE_RATIO, (s->getPositionY() + (visibleSize.height * 0.0035f))/SCALE_RATIO), 0);
                
                    b->SetLinearVelocity(b2Vec2(0, 2));
                }
                s->setPosition(b->GetPosition().x * SCALE_RATIO, b->GetPosition().y * SCALE_RATIO);
            }
        }
    }
}


void HelloWorld::BeginContact(b2Contact *contact)
{
    /*
    //test
    auto a = contact->GetFixtureA();
    auto b = contact->GetFixtureB();

    if(a->GetShape()->GetType() == b2Shape::Type::e_circle ||
       b->GetShape()->GetType() == b2Shape::Type::e_circle)
    {
        cocos2d::log("BeginContact: One of the fixtures was the circle.");
        if(contact->IsTouching())
        {
            cocos2d::log("BeginContact: And they're touching. Not just AABB overlap");
            if(a->GetBody()->GetUserData() == _car)
            {
                cocos2d::log("BeginContact: Fixture 'A' is the car.");
                // If circle was sensor, we could stop the car manually
                //a->GetBody()->SetLinearVelocity(b2Vec2(0,0));
            }
            else if(b->GetBody()->GetUserData() == _car)
            {
                cocos2d::log("BeginContact: Fixture 'A' is the car.");
                // If circle was sensor, we could stop the car manually
                //b->GetBody()->SetLinearVelocity(b2Vec2(0,0));
            }
            _car->setColor(Color3B::BLACK);
        }
    }
     */
}

void HelloWorld::EndContact(b2Contact *contact)
{
    //test
   // auto a = contact->GetFixtureA();
   // auto b = contact->GetFixtureB();
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
    #endif

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/
    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);
}
