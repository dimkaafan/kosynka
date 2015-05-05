#include <math.h>
//#include "GameHelper/AndroidFix.h"
#include "MainScene.h"
#include "Utils.h"

USING_NS_CC;

MainScene::MainScene()
{
}

Scene* MainScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = MainScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool MainScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Node::init() )
    {
        return false;
    }
    
    cocos2d::spritebuilder::CCBXReader *reader = cocos2d::spritebuilder::CCBXReader::createFromFile("Interface/MainScene.ccbi");
    cocos2d::Node * node = reader->createNode(this, cocos2d::spritebuilder::SceneScaleType::MAXSCALE);
    addChild(node);
    
   // auto bbox = _node_graphic->getBoundingBox();

    auto dispatcher = cocos2d::Director::getInstance()->getEventDispatcher();
    
#if 0
    auto touchListener = cocos2d::EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(MainScene::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(MainScene::onTouchMoved, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(MainScene::onTouchEnded, this);
    touchListener->onTouchCancelled = CC_CALLBACK_2(MainScene::onTouchCancelled, this);
    dispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
    _listener = touchListener;
#else
    auto touchesListener = cocos2d::EventListenerTouchAllAtOnce::create();
    touchesListener->onTouchesBegan = CC_CALLBACK_2(MainScene::onTouchesBegan, this);
    touchesListener->onTouchesMoved = CC_CALLBACK_2(MainScene::onTouchesMoved, this);
    touchesListener->onTouchesEnded = CC_CALLBACK_2(MainScene::onTouchesEnded, this);
    touchesListener->onTouchesCancelled = CC_CALLBACK_2(MainScene::onTouchesCancelled, this);
    dispatcher->addEventListenerWithSceneGraphPriority(touchesListener, this);
    _listener = touchesListener;
#endif
    return true;
}

MainScene::~MainScene()
{
    if (_listener)
        cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(_listener);
}


cocos2d::spritebuilder::ccReaderClickCallback MainScene::onResolveCCBClickSelector(const std::string &selectorName, cocos2d::Node* node)
{
    CCBX_SELECTORRESOLVER_CLICK_GLUE(this, "pause", MainScene::onPause);
    CCBX_SELECTORRESOLVER_CLICK_GLUE(this, "resume", MainScene::onResume);
    return nullptr;
}

bool MainScene::onAssignCCBMemberVariable(const std::string &memberVariableName, cocos2d::Node* node)
{
    CCBX_MEMBERVARIABLEASSIGNER_GLUE("pause", _pause);
    CCBX_MEMBERVARIABLEASSIGNER_GLUE("resume", _resume);
    
    CCBX_MEMBERVARIABLEASSIGNER_GLUE("node_signal", _node_graphic);
    CCBX_MEMBERVARIABLEASSIGNER_GLUE("node_fft", _node_fft);
    
    return true;
}


void MainScene::onPause(cocos2d::Ref* target)
{

}

void MainScene::onResume(cocos2d::Ref* target)
{

}


bool MainScene::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event*)
{
    _scrollNode = nullptr;

    return _scrollNode != nullptr;
}

void MainScene::onTouchMoved(cocos2d::Touch* touch, cocos2d::Event*)
{

}

void MainScene::onTouchEnded(cocos2d::Touch*, cocos2d::Event*)
{
    _scrollNode = nullptr;
}

void MainScene::onTouchCancelled(cocos2d::Touch*, cocos2d::Event*)
{
    _scrollNode = nullptr;
}

void MainScene::onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
{
    if (touches.size() < 3)
    {
        _scrollNode = nullptr;
    }
}

void MainScene::onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
{

}

void MainScene::onTouchesEnded(const std::vector<cocos2d::Touch*>&, cocos2d::Event*)
{
    
}

void MainScene::onTouchesCancelled(const std::vector<cocos2d::Touch*>&, cocos2d::Event*)
{
    
}

bool MainScene::isNodeContainTouches(const std::vector<cocos2d::Touch*>& touches, cocos2d::Node* node)
{
    cocos2d::Rect bbox = node->getBoundingBox();
    bbox.origin.set(0, 0);
    size_t touchCount = 0;
    for(auto& item: touches)
        if (bbox.containsPoint(node->convertTouchToNodeSpace(item)))
            touchCount++;
    return touches.size() == touchCount;
}
