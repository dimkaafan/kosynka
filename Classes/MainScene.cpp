#include <CoreAudio/CoreAudioTypes.h>
#include <math.h>
//#include "GameHelper/AndroidFix.h"
#include "MainScene.h"
#include "SignalTypes.h"
#include "Utils.h"

const size_t BUFFER_SIZE = 2048;

USING_NS_CC;

MainScene::MainScene(): _rawSignal(BUFFER_SIZE, 0.0001f, _xTimeSec)
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
    initAudio();
    
    auto it = _drawSignals.insert({_node_graphic, SignalDesc(_node_graphic, _rawSignal.getAvrSignal(), _signalWin)});
    if (it.second)
        it.first->second._axisNode = drawAxis(_node_graphic, cocos2d::Rect(0,0, _rawSignal.getXTime(), 0));
    _drawSignals.insert({_node_fft, SignalDesc(_node_fft, _rawSignal.getAvrSignal(), _spektrWin)});
    _pause->setVisible(!_rawSignal.isPaused());
    _resume->setVisible(_rawSignal.isPaused());
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
    _max_frequency->setString(GameUtils::floatToStr(diap.first) + " - " + GameUtils::floatToStr(diap.second));
}

cocos2d::spritebuilder::ccReaderClickCallback MainScene::onResolveCCBClickSelector(const std::string &selectorName, cocos2d::Node* node)
{
    CCBX_SELECTORRESOLVER_CLICK_GLUE(this, "pause", MainScene::onPause);
    CCBX_SELECTORRESOLVER_CLICK_GLUE(this, "resume", MainScene::onResume);
    CCBX_SELECTORRESOLVER_CLICK_GLUE(this, "xtime_up2", MainScene::onXTimeUp2);
    CCBX_SELECTORRESOLVER_CLICK_GLUE(this, "xtime_down2", MainScene::onXTimeDown2);
    return nullptr;
}

bool MainScene::onAssignCCBMemberVariable(const std::string &memberVariableName, cocos2d::Node* node)
{
    CCBX_MEMBERVARIABLEASSIGNER_GLUE("pause", _pause);
    CCBX_MEMBERVARIABLEASSIGNER_GLUE("resume", _resume);
    
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
    drawSignal();
    drawSpectr();
}

void MainScene::drawSignal()
{
    const auto& signal = _rawSignal.getAvrSignal();

    auto sz = _node_graphic->getContentSize();
    _maxY->setString(std::to_string(signal.maxY));
    _minY->setString(std::to_string(signal.minY));
    
    float pointTime = _rawSignal.getTimeInPoint();
    float pow10 = GameUtils::get10power(pointTime);

    std::string pref = "";
    if (pow10 > -6)
    {
        pow10 = 3;
        pref = " ms";
        pointTime *= ::powf(10.f, pow10);
    }
    std::string timeStr = GameUtils::floatToStr(_rawSignal.getXTime());
    timeStr += " (" + GameUtils::floatToStr(pointTime) + pref + ")";
    _xTime->setString(timeStr);

    drawData(signal, _signalWin, _node_graphic, _points);
}

void MainScene::drawSpectr()
{
    const auto& spectr = _rawSignal.getSpectr();
    drawData(spectr.amplituda, _spektrWin, _node_fft, _spectrPoints);
}

void MainScene::drawData(const RoundBuff& source, const SignalWindow& viewWin, cocos2d::Node* target, std::vector<cocos2d::Node*>& dest)
{
    auto minY = source.minY;
    auto maxY = source.maxY;
    
    auto sz = target->getContentSize();
    float indentY = 10;
    float indentX = 10;
    
    float fx = (sz.width - 2*indentX)/(source.size()*viewWin._width);
    float fy = (maxY - minY) > 0 ? (sz.height - 2*indentY)/(maxY - minY) : 0;
    size_t maxIdx = std::max(source.size(), dest.size());
    size_t minSignalIdx = static_cast<size_t>(roundf(source.size()*viewWin._start));
    size_t maxSignalIdx = minSignalIdx + static_cast<size_t>(roundf(source.size()*viewWin._width));
    
    float yy = indentY - fy*minY;
    
    for(size_t i = 0; i < maxIdx; i++)
    {
        size_t xIdx = i < source.pos ? i + maxIdx : i;
        xIdx -= source.pos;
        auto pItem = dest.at(i);
        if (xIdx >= minSignalIdx && xIdx < maxSignalIdx)
        {
            auto& dataItem = source.data.at(i);
            pItem->setPosition(fx*(xIdx - minSignalIdx) + indentX, yy + fy*dataItem);
            pItem->setVisible(true);
        }
        else
            pItem->setVisible(false);
    }
    for(auto it = dest.begin() + maxIdx; it != dest.end(); ++it)
    {
        (*it)->setVisible(false);
    }

}

void MainScene::onPause(cocos2d::Ref* target)
{
    _rawSignal.pause();
    _pause->setVisible(!_rawSignal.isPaused());
    _resume->setVisible(_rawSignal.isPaused());
}

void MainScene::onResume(cocos2d::Ref* target)
{
    _rawSignal.start();
    _pause->setVisible(!_rawSignal.isPaused());
    _resume->setVisible(_rawSignal.isPaused());
}

void MainScene::onXTimeUp2(cocos2d::Ref* target)
{
    if (_rawSignal.setXTime(_xTimeSec*2.f))
    {
        auto it = _drawSignals.find(_node_graphic);
        SignalDesc& sgnDesc = it->second;
        sgnDesc._axisNode->removeFromParent();
        sgnDesc._axisNode = drawAxis(_node_graphic, cocos2d::Rect(0,0, _rawSignal.getXTime(), 0));
        _xTimeSec *= 2.f;
        updateFrequency();
    }
}

void MainScene::onXTimeDown2(cocos2d::Ref* target)
{
    if (_rawSignal.setXTime(_xTimeSec/2.f))
    {
        auto it = _drawSignals.find(_node_graphic);
        SignalDesc& sgnDesc = it->second;
        sgnDesc._axisNode->removeFromParent();
        sgnDesc._axisNode = drawAxis(_node_graphic, cocos2d::Rect(0,0, _rawSignal.getXTime(), 0));
        _xTimeSec /= 2.f;
        updateFrequency();
    }
}

bool MainScene::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event*)
{
    _scrollNode = nullptr;
    if (isNodeContainTouches({touch}, _node_graphic))
        _scrollNode = _node_graphic;
    else if (isNodeContainTouches({touch}, _node_fft))
        _scrollNode = _node_fft;
    return _scrollNode != nullptr;
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

void MainScene::onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
{
    if (touches.size() < 3)
    {
        _scrollNode = nullptr;
        for(auto& item : _drawSignals)
        {
            if (isNodeContainTouches(touches, item.first))
            {
                _scrollNode = item.first;
                break;
            }
        }
    }
}

void MainScene::onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
{
    auto it = _drawSignals.find(_scrollNode);
    if (it != _drawSignals.end())
    {
        if (touches.size() == 1)// move window
        {
            auto cs = it->first->getContentSize();
            it->second._win.shiftPos(-touches.front()->getDelta().x/cs.width);
            if (_rawSignal.isPaused())
            {
                drawSignal();
                drawSpectr();
            }
        }
        else if (touches.size() == 2)
        {
            float prevWidth = fabs(touches.at(0)->getPreviousLocation().x - touches.at(1)->getPreviousLocation().x);
            float width = fabs(touches.at(0)->getLocation().x - touches.at(1)->getLocation().x);
            it->second._win.changeWidth(width > prevWidth ? -0.1f : 0.1f);
            if (_rawSignal.isPaused())
            {
                drawSignal();
                drawSpectr();
            }
        }
    }
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

cocos2d::Node* MainScene::drawAxis(cocos2d::Node* node, const cocos2d::Rect& graphRect)
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
    
    float xTime = graphRect.size.width;
    float scX = bbox.size.width/xTime;
    float pow10 = GameUtils::get10Factor(xTime);
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
    node->addChild(axisNode);
    return axisNode;
}
