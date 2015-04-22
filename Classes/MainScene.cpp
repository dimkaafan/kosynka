#include <CoreAudio/CoreAudioTypes.h>
#include <math.h>
//#include "GameHelper/AndroidFix.h"
#include "MainScene.h"
#include "SignalTypes.h"

const size_t BUFFER_SIZE = 2048;


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

MainScene::MainScene(): _rawSignal(BUFFER_SIZE, 0.0001f, _xTimeSec)
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
    for(size_t i = 0; i < BUFFER_SIZE; i++)
    {
        cocos2d::Node * node = pointreader->createNode(this);
        _node_graphic->addChild(node);
        _points.push_back(node);
        cocos2d::Node * nodeSpectr = pointreader->createNode(this);
        _node_fft->addChild(nodeSpectr);
        _spectrPoints.push_back(nodeSpectr);
    }
    
#if 1
    auto dispatcher = cocos2d::Director::getInstance()->getEventDispatcher();
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
    _listener = touchListener;
#endif
    initAudio();
    drawAxis(_node_graphic);
    
    return true;
}

MainScene::~MainScene()
{
    if (_listener)
        cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(_listener);
}

void MainScene::initAudio()
{
    _rawSignal.init();
    _rawSignal.setOnRecieveFunction([this](long long count)
                                    {this->onRecieveSignal(count);});
    
    _rawSignal.start();
    updateFrequency();
}

void MainScene::updateFrequency()
{
    auto diap = _rawSignal.getFrequencyGap();
    _max_frequency->setString(floatToStr(diap.first) + " - " + floatToStr(diap.second));
}

cocos2d::spritebuilder::ccReaderClickCallback MainScene::onResolveCCBClickSelector(const std::string &selectorName, cocos2d::Node* node)
{
    CCBX_SELECTORRESOLVER_CLICK_GLUE(this, "pause", MainScene::onPause);
    CCBX_SELECTORRESOLVER_CLICK_GLUE(this, "resume", MainScene::onResume);
    CCBX_SELECTORRESOLVER_CLICK_GLUE(this, "xtime_up2", MainScene::onXTimeUp2);
    CCBX_SELECTORRESOLVER_CLICK_GLUE(this, "xtime_down2", MainScene::onXTimeDown2);
    CCBX_SELECTORRESOLVER_CLICK_GLUE(this, "xwin_up", MainScene::onXWinUp);
    CCBX_SELECTORRESOLVER_CLICK_GLUE(this, "xwin_down", MainScene::onXWinDown);
    return nullptr;
}

bool MainScene::onAssignCCBMemberVariable(const std::string &memberVariableName, cocos2d::Node* node)
{
    CCBX_MEMBERVARIABLEASSIGNER_GLUE("count", _count);
    CCBX_MEMBERVARIABLEASSIGNER_GLUE("pause", _pause);
    CCBX_MEMBERVARIABLEASSIGNER_GLUE("node_signal", _node_graphic);
    CCBX_MEMBERVARIABLEASSIGNER_GLUE("node_fft", _node_fft);
    
    CCBX_MEMBERVARIABLEASSIGNER_GLUE("maxY", _maxY);
    CCBX_MEMBERVARIABLEASSIGNER_GLUE("minY", _minY);
    CCBX_MEMBERVARIABLEASSIGNER_GLUE("x_time", _xTime);
    CCBX_MEMBERVARIABLEASSIGNER_GLUE("max_frequency", _max_frequency);
    
    return true;
}

void MainScene::onRecieveSignal(long long count)
{
    _count->setString(std::to_string(count));
    drawSignal();
    drawSpectr();
}

void MainScene::drawSignal()
{
    const auto& signal = _rawSignal.getAvrSignal();
    
    
    auto minY = _rawSignal.getMinY();
    auto maxY = _rawSignal.getMaxY();
    
    auto sz = _node_graphic->getContentSize();
    _maxY->setString(std::to_string(maxY));
    _minY->setString(std::to_string(minY));
    _xTime->setString(floatToStr(_rawSignal.getXTime()));
    
    float pointTime = _rawSignal.getTimeInPoint();
    float pow10 = ::log10f(pointTime);
    if (pow10 > 0)
        pow10 = static_cast<int>(pow10);
    else if (pow10 < 0)
        pow10 = static_cast<int>(pow10 - 1.f);

    std::string pref = "";
    if (pow10 > -6)
    {
        pow10 = 3;
        pref = " ms";
        pointTime *= ::powf(10.f, pow10);
    }
    std::string timeStr = _xTime->getString();
    timeStr += "(" + floatToStr(pointTime) + pref + ")";
    _xTime->setString(timeStr);


    float fx = sz.width/(signal.data.size()*_signalWin._width);
    float fy = (maxY - minY) > 0 ? sz.height/2/(maxY - minY) : 0;
    size_t maxIdx = std::max(signal.size(), _points.size());
    size_t minSignalIdx = static_cast<size_t>(roundf(signal.size()*_signalWin._start));
    size_t maxSignalIdx = minSignalIdx + static_cast<size_t>(roundf(signal.size()*_signalWin._width));
    
    for(size_t i = 0; i < maxIdx; i++)
    {
        size_t xIdx = i < signal.pos ? i + maxIdx : i;
        xIdx -= signal.pos;
        auto pItem = _points.at(i);
        if (xIdx >= minSignalIdx && xIdx < maxSignalIdx)
        {
            auto& dataItem = signal.data.at(i);
            pItem->setPosition(fx*(xIdx - minSignalIdx) , sz.height/2 + fy*dataItem);
            pItem->setVisible(true);
        }
        else
            pItem->setVisible(false);
    }
    for(auto it = _points.begin() + maxIdx; it != _points.end(); ++it)
    {
        (*it)->setVisible(false);
    }
}

void MainScene::drawSpectr()
{
    const auto& spectr = _rawSignal.getSpectr();
    auto sz = _node_fft->getContentSize();
    
    float indent = 10.f;
    size_t maxIdx = spectr.data.size()/2;
    float fx = (sz.width - 2*indent)/maxIdx;
    float fy = (spectr.maxAmplutide - spectr.minAmplutide) > 0 ? (sz.height - 2*indent)/(spectr.maxAmplutide - spectr.minAmplutide) : 0;
    for(size_t i = 0; i < maxIdx; i++)
    {
        auto& dataItem = spectr.data.at(i);
        auto pItem = _spectrPoints.at(i);
        pItem->setPosition(fx*i + indent, fy*dataItem.re + indent);
        pItem->setVisible(true);
    }
    for(auto it = _spectrPoints.begin() + maxIdx; it != _spectrPoints.end(); ++it)
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

void MainScene::onXTimeUp2(cocos2d::Ref* target)
{
    if (_rawSignal.setXTime(_xTimeSec*2.f))
    {
        _xTimeSec *= 2.f;
        updateFrequency();
    }
}

void MainScene::onXTimeDown2(cocos2d::Ref* target)
{
    if (_rawSignal.setXTime(_xTimeSec/2.f))
    {
        _xTimeSec /= 2.f;
        updateFrequency();
    }
}

void MainScene::onXWinUp(cocos2d::Ref* target)
{
    _signalWin.changeWidth(0.1f);
    if (_rawSignal.isPaused())
        drawSignal();
}

void MainScene::onXWinDown(cocos2d::Ref* target)
{
    _signalWin.changeWidth(-0.1f);
    if (_rawSignal.isPaused())
        drawSignal();
}

bool MainScene::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event*)
{
    cocos2d::Rect bbox = _node_graphic->getBoundingBox();
    bbox.origin.set(0, 0);
    if (bbox.containsPoint(_node_graphic->convertTouchToNodeSpace(touch)))
    {
        _scrollNode = _node_graphic;
        return true;
    }
    bbox = _node_fft->getBoundingBox();
    bbox.origin.set(0, 0);
    if (bbox.containsPoint(_node_fft->convertTouchToNodeSpace(touch)))
    {
        _scrollNode = _node_fft;
        return true;
    }
    return false;
}

void MainScene::onTouchMoved(cocos2d::Touch* touch, cocos2d::Event*)
{
    if (_scrollNode)
    {
        auto cs = _scrollNode->getContentSize();
        _signalWin.shiftPos(-touch->getDelta().x/cs.width);
        if (_rawSignal.isPaused())
            drawSignal();
    }
}

void MainScene::onTouchEnded(cocos2d::Touch*, cocos2d::Event*)
{
    _scrollNode = nullptr;
}

void MainScene::onTouchCancelled(cocos2d::Touch*, cocos2d::Event*)
{
    _scrollNode = nullptr;
}

void MainScene::onTouchesBegan(const std::vector<cocos2d::Touch*>&, cocos2d::Event*)
{
    
}

void MainScene::onTouchesMoved(const std::vector<cocos2d::Touch*>&, cocos2d::Event*)
{
    
}

void MainScene::onTouchesEnded(const std::vector<cocos2d::Touch*>&, cocos2d::Event*)
{
    
}

void MainScene::onTouchesCancelled(const std::vector<cocos2d::Touch*>&, cocos2d::Event*)
{
    
}

void MainScene::drawAxis(cocos2d::Node* node)
{
    DrawNode* axisNode =DrawNode::create();
    auto bbox = node->getBoundingBox();
    float deltaBreak = 5;
    //draw x
    float  x = 0;
    while(x < bbox.size.width)
    {
        axisNode->drawLine(Vec2(x, bbox.size.height/2),Vec2(x + deltaBreak, bbox.size.height/2),Color4F(1,0,0,1));
        x += 2*deltaBreak;
    }
    
    float xTime = _rawSignal.getXTime();
    float scX = bbox.size.width/xTime;
    float pow10 = ::log10f(xTime);
    if (pow10 > 0)
        pow10 = static_cast<int>(pow10);
    else if (pow10 < 0)
        pow10 = static_cast<int>(pow10 - 1.f);
    pow10 = ::powf(10.f, pow10);
    int maxX = ceil(xTime/pow10);
    scX *= pow10;
    for(int i = 0; i < maxX; i++)
    {
        float y = 0;
        while (y < bbox.size.height)
        {
            axisNode->drawLine(Vec2(i*scX, y), Vec2(i*scX, y + deltaBreak), Color4F(1,0,0,1));
            y += 2*deltaBreak;
        }
    }
    _node_graphic->addChild(axisNode);
}
