#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

class HudLayer: public cocos2d::Layer
{
public:
    void numCollectedChanged(int numCollected);
    virtual bool init();
    CREATE_FUNC(HudLayer);
    
    cocos2d::Label* scoreLabel;
};

class HelloWorld : public cocos2d::Layer
{
private:
    // hard-coded scale factor to enable retina display (can be deleted if the pictures are for retina devices)
    float retinaFactor = 2.13;
    
    cocos2d::TMXTiledMap* map;
    cocos2d::TMXLayer* background;
    cocos2d::TMXLayer* foreground;
    cocos2d::TMXLayer* blockMap;
    cocos2d::Sprite* player;
    
    int numCollected;
    static HudLayer* hud;
    
    cocos2d::Point tileCoordForPosition(cocos2d::Point position);
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
    
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    
    void setViewPointCenter(cocos2d::Point position);
    void setPlayerPosition(cocos2d::Point position);
};

#endif // __HELLOWORLD_SCENE_H__
