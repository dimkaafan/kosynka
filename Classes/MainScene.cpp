#include <CoreAudio/CoreAudioTypes.h>
//#include "GameHelper/AndroidFix.h"
#include "MainScene.h"
#include "SignalTypes.h"

const int AUDIO_BUFFERS = 16;
const size_t BUFFER_SIZE = 1024;

static std::random_device rd;
static std::default_random_engine randomEngine(rd());

std::string floatToStr(float val)
{
    std::string res = std::to_string(val);
    size_t pos = res.length();
    auto it = res.rbegin();
    for(;it != res.rend(); ++it)
        if(*it != '0')
            break;
        else
            pos--;
    if (pos != 0 && res.at(pos-1) == '.')
        pos--;
    res.erase(res.begin() + pos, res.end());
    return std::move(res);
}

USING_NS_CC;

MainScene::MainScene(): _rawSignal(AUDIO_BUFFERS, BUFFER_SIZE, 0.0001f)
,_count(nullptr)
,_pause(nullptr)
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
    
    auto bbox = _node_graphic->getBoundingBox();
    cocos2d::spritebuilder::CCBXReader *pointreader = cocos2d::spritebuilder::CCBXReader::createFromFile("Interface/dataPoint.ccbi");
    _points.reserve(BUFFER_SIZE);
    std::uniform_int_distribution<int> rndX(0, bbox.size.width-1);
    std::uniform_int_distribution<int> rndY(0, bbox.size.height-1);
    for(size_t i = 0; i < BUFFER_SIZE; i++)
    {
        cocos2d::Node * node = pointreader->createNode(this);
        auto pos = cocos2d::Vec2(rndX(randomEngine), rndY(randomEngine));
        node->setPosition(pos);
        _node_graphic->addChild(node);
        _points.push_back(node);
    }
    
    initAudio();
    
    return true;
}

MainScene::~MainScene()
{

}

void MainScene::initAudio()
{
    _rawSignal.init();
    _rawSignal.setOnRecieveFunction([this](long long count)
                                    {this->onRecieveSignal(count);});
                                        
    _rawSignal.start();
}

cocos2d::spritebuilder::ccReaderClickCallback MainScene::onResolveCCBClickSelector(const std::string &selectorName, cocos2d::Node* node)
{
    CCBX_SELECTORRESOLVER_CLICK_GLUE(this, "pause", MainScene::onPause);
    CCBX_SELECTORRESOLVER_CLICK_GLUE(this, "resume", MainScene::onResume);
    return nullptr;
}

bool MainScene::onAssignCCBMemberVariable(const std::string &memberVariableName, cocos2d::Node* node)
{
    CCBX_MEMBERVARIABLEASSIGNER_GLUE("count", _count);
    CCBX_MEMBERVARIABLEASSIGNER_GLUE("pause", _pause);
    CCBX_MEMBERVARIABLEASSIGNER_GLUE("node_graphic", _node_graphic);
    
    CCBX_MEMBERVARIABLEASSIGNER_GLUE("maxY", _maxY);
    CCBX_MEMBERVARIABLEASSIGNER_GLUE("minY", _minY);
    CCBX_MEMBERVARIABLEASSIGNER_GLUE("x_time", _xTime);
    
    return true;
}

void MainScene::onRecieveSignal(long long count)
{
    _count->setString(std::to_string(count));
    const auto& data = _rawSignal.getAvrSignal();
    auto sz = _node_graphic->getContentSize();

    auto minY = _rawSignal.getMinY();
    auto maxY = _rawSignal.getMaxY();
    _maxY->setString(std::to_string(maxY));
    _minY->setString(std::to_string(minY));
    _xTime->setString(floatToStr(_rawSignal.getXTime()));
    
    float fx = sz.width/data.size();
    
    float fy = (maxY - minY) > 0 ? sz.height/2/(maxY - minY) : 0;
    size_t maxIdx = std::max(data.size(), _points.size());
    for(size_t i = 0; i < maxIdx; i++)
    {
        auto& dataItem = data.at(i);
        auto pItem = _points.at(i);
        pItem->setPosition(fx*i, sz.height/2 + fy*dataItem);
        pItem->setVisible(true);
    }
    for(auto it = _points.begin() + maxIdx; it != _points.end(); ++it)
    {
        (*it)->setVisible(false);
    }
}

void MainScene::onPause(cocos2d::Ref* target)
{
    _rawSignal.pause();
    _pause->setVisible(false);
}

void MainScene::onResume(cocos2d::Ref* target)
{
    _rawSignal.start();
    _pause->setVisible(true);
}
