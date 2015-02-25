#include <CoreAudio/CoreAudioTypes.h>
#include "MainScene.h"
#include "SignalTypes.h"

const int AUDIO_BUFFERS = 16;

USING_NS_CC;

MainScene::MainScene(): _rawSignal(AUDIO_BUFFERS)
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
    
    
    initAudio();
    
    return true;
}

MainScene::~MainScene()
{

}

void MainScene::initAudio()
{
    _rawSignal.init();
    _rawSignal.start();
}

cocos2d::spritebuilder::ccReaderClickCallback MainScene::onResolveCCBClickSelector(const std::string &selectorName, cocos2d::Node* node)
{
    //CCBX_SELECTORRESOLVER_CLICK_GLUE(this, "settings", MainScene::onSettings);
    return nullptr;
}

bool MainScene::onAssignCCBMemberVariable(const std::string &memberVariableName, cocos2d::Node* node)
{
    //CCBX_MEMBERVARIABLEASSIGNER_GLUE("garage_new_item",_garage_new_item);
    return true;
}

