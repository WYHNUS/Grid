#include "HelloWorldScene.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));
    
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);
    
    // create a TMX map
    map = TMXTiledMap::create("res/background.tmx");
    addChild(map, 0);
    // all tiles are aliased by default, let's set them anti-aliased
    for (const auto& child : map->getChildren())
    {
        static_cast<SpriteBatchNode*>(child)->getTexture()->setAntiAliasTexParameters();
    }
    
    TMXObjectGroup* objectGroup = map->getObjectGroup("Object-Players");
    CCASSERT(NULL != objectGroup, "'Object-Players' object group not found");
    auto playerSpawnPoint = objectGroup->getObject("SpawnPoint");
    CCASSERT(!playerSpawnPoint.empty(), "'SpawnPoint' object not found");
    
    int x = playerSpawnPoint["x"].asInt();
    int y = playerSpawnPoint["y"].asInt();
    player = Sprite::create("res/Player.png");
    player->setPosition(x, y);
    addChild(player);
    setViewPointCenter(player->getPosition());
    
    // add listener
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [&](Touch* touch, Event* event) ->bool {return true;};
    listener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    return true;
}

void HelloWorld::onTouchEnded(Touch* touch, Event* event)
{
    auto touchLocation = touch->getLocation();
    touchLocation = this->convertToNodeSpace(touchLocation);
    
    auto playerPos = player->getPosition();
    auto posDiff = touchLocation - playerPos;
    
    if (std::abs(posDiff.x) > std::abs(posDiff.y)) {
        if (posDiff.x > 0) {
            playerPos.x += map->getTileSize().width / 2;
        }
        else {
            playerPos.x -= map->getTileSize().width / 2;
        }
    } else {
        if (posDiff.y > 0) {
            playerPos.y += map->getTileSize().height / 2;
        }
        else {
            playerPos.y -= map->getTileSize().height / 2;
        }
    }
    
    // check if player within range of the map
    if (playerPos.x <= (map->getMapSize().width * map->getMapSize().width) &&
        playerPos.y <= (map->getMapSize().height * map->getMapSize().height) &&
        playerPos.y >= 0 && playerPos.x >= 0)
    {
        this->setPlayerPosition(playerPos);
    }
    
    this->setViewPointCenter(player->getPosition());
}

void HelloWorld::setPlayerPosition(cocos2d::Point position)
{
    player->setPosition(position.x, position.y);
}

void HelloWorld::setViewPointCenter(cocos2d::Point position)
{
    auto winSize = Director::getInstance()->getWinSize();
    
    int x = MAX(position.x, winSize.width / 2);
    int y = MAX(position.y, winSize.height / 2);
    x = MIN(x, (map->getMapSize().width * map->getTileSize().width) - winSize.width / 2);
    y = MIN(y, (map->getMapSize().height * map->getTileSize().height) - winSize.height / 2);
    auto actualPosition = Point(x, y);
    
    auto centerOfView = Point(winSize.width / 2, winSize.height / 2);
    auto viewPoint = centerOfView - actualPosition;
    this->setPosition(viewPoint);
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
