#include "HelloWorldScene.h"
#include "iostream"

USING_NS_CC;

HudLayer *HelloWorld::hud = NULL;

bool HudLayer::init()
{
    if (!Layer::init())
    {
        return false;
    }
    auto visibleSize = Director::getInstance()->getVisibleSize();
    scoreLabel = Label::createWithTTF("Score: 0", "fonts/Marker Felt.ttf", 18.0f, Size(150, 20),
                                      TextHAlignment::RIGHT, TextVAlignment::BOTTOM);
    scoreLabel->setColor(Color3B(255, 255, 255));
    int margin = 10;
    scoreLabel->setPosition(visibleSize.width - (scoreLabel->getDimensions().width / 2) - margin,
                       scoreLabel->getDimensions().height / 2 + margin);
    this->addChild(scoreLabel);
    
    return true;
}

void HudLayer::numCollectedChanged(int numCollected)
{
    char showStr[20];
    sprintf(showStr, "Score: %d", numCollected);
    scoreLabel->setString(showStr);
}

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();
    scene->addChild(layer);
    
    hud = HudLayer::create();
    scene->addChild(hud);

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
    // hard-coded scale factor to enable retina display (can be deleted if the pictures are for retina devices)
    map->setScale(retinaFactor);
    
    background = map->getLayer("BackgroundLayer");
    foreground = map->getLayer("Foreground");
    blockMap = map->getLayer("BlockLayer");
    blockMap->setVisible(false);
    this->addChild(map, 0);
    
    // all tiles are aliased by default, let's set them anti-aliased
    for (const auto& child : map->getChildren())
    {
        static_cast<SpriteBatchNode*>(child)->getTexture()->setAntiAliasTexParameters();
    }
    
    TMXObjectGroup* objectGroup = map->getObjectGroup("Object-Players");
    CCASSERT(NULL != objectGroup, "'Object-Players' object group not found");
    auto playerSpawnPoint = objectGroup->getObject("SpawnPoint");
    CCASSERT(!playerSpawnPoint.empty(), "'SpawnPoint' object not found");
    
    int x = playerSpawnPoint["x"].asInt() * retinaFactor;
    int y = playerSpawnPoint["y"].asInt() * retinaFactor - 5;   // hard-coded to fix display
    player = Sprite::create("res/Player.png");
    player->setScale(retinaFactor, retinaFactor);
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
            playerPos.x += map->getTileSize().width;
        }
        else {
            playerPos.x -= map->getTileSize().width;
        }
    } else {
        if (posDiff.y > 0) {
            playerPos.y += map->getTileSize().height;
        }
        else {
            playerPos.y -= map->getTileSize().height;
        }
    }
    
    // check if player within range of the map
    if (playerPos.x <= (map->getMapSize().width * map->getTileSize().width) &&
        playerPos.y <= (map->getMapSize().height * map->getTileSize().height) &&
        playerPos.y >= 0 && playerPos.x >= 0)
    {
        this->setPlayerPosition(playerPos);
    }
    
    this->setViewPointCenter(player->getPosition());
}

cocos2d::Point HelloWorld::tileCoordForPosition(cocos2d::Point position)
{
    int x = position.x / map->getTileSize().width;
    int y = ((map->getMapSize().height * map->getTileSize().height) - position.y) / map->getTileSize().height;
    return Point(x, y);
}

void HelloWorld::setPlayerPosition(cocos2d::Point position)
{
    cocos2d::Point tileCoord = this->tileCoordForPosition(position);
    int tileGrid = blockMap->getTileGIDAt(tileCoord);
    if (tileGrid) {
        auto property = map->getPropertiesForGID(tileGrid).asValueMap();
        if (!property.empty()) {
            if (property["Blockage"].asString() == "True") {
                return;
            } else if (property["Collectable"].asString() == "True") {
                blockMap->removeTileAt(tileCoord);
                foreground->removeTileAt(tileCoord);
                this->numCollected++;
                this->hud->HudLayer::numCollectedChanged(numCollected);
            }
        }
    }
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
