#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "spritebuilder/SpriteBuilder.h"
#include "SignalManager.h"

class SignalWindow
{
public:
    float _start;// percent
    float _width;// percent
    float _widthMin;
    SignalWindow(float start, float width, float widthMin):_start(start), _width(width), _widthMin(widthMin){}
    void shiftPos(float delta)
    {
        _start += delta;
        if (_start < 0.f)
            _start = 0.f;
        else if (_start + _width > 1.f)
            _start = 1.f - _width;
    }
    void changeWidth(float delta)
    {
        _width += delta;
        if (_width > 1.f)
            _width = 1.f;
        else if ( _width < _widthMin)
            _width = _widthMin;
        _start = 1.f - _width;
    }
};

struct DrawRect
{
    cocos2d::Rect sourceRect;
    cocos2d::Rect destRect;
    float indentX;
    float indentY;
};

class MainScene : public cocos2d::Node, public cocos2d::spritebuilder::CCBXReaderOwner
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();

    void initAudio();
    ~MainScene();
    
    CREATE_FUNC(MainScene);
private:
    MainScene();
    cocos2d::Label* _maxY = nullptr;
    cocos2d::Label* _minY = nullptr;
    cocos2d::Label* _xTime = nullptr;
    cocos2d::Label* _max_frequency = nullptr;
    cocos2d::Node* _pause = nullptr;
    cocos2d::Node* _resume = nullptr;
    cocos2d::Node* _node_graphic = nullptr;
    cocos2d::Node* _node_fft = nullptr;
    cocos2d::Node* _scrollNode = nullptr;
    
    std::vector<cocos2d::Node*> _points;
    std::vector<cocos2d::Node*> _spectrPoints;
    cocos2d::EventListener* _listener = nullptr;
    
    struct SignalDesc
    {
        SignalDesc(cocos2d::Node* node, const RoundBuff& signal, SignalWindow& win):_node(node), _signal(signal), _win(win), _axisNode(nullptr)
        {}
        cocos2d::Node* _node;
        cocos2d::Node* _axisNode;
        const RoundBuff& _signal;
        SignalWindow& _win;
    };
    
    std::map<cocos2d::Node*, SignalDesc> _drawSignals;
    
    float _xTimeSec = 0.2f;
    SignalWindow _signalWin=SignalWindow(0.0f, 1.f, 0.1f);
    SignalWindow _spektrWin=SignalWindow(0.0f, 1.f, 0.1f);
    
    SignalManager _rawSignal;
    
    virtual cocos2d::spritebuilder::ccReaderClickCallback onResolveCCBClickSelector(const std::string &selectorName, cocos2d::Node* node) override;
    virtual bool onAssignCCBMemberVariable(const std::string &memberVariableName, cocos2d::Node* node) override;
    
    void onPause(cocos2d::Ref* target);
    void onResume(cocos2d::Ref* target);
    void onXTimeUp2(cocos2d::Ref* target);
    void onXTimeDown2(cocos2d::Ref* target);
    
    bool onTouchBegan(cocos2d::Touch*, cocos2d::Event*);
    void onTouchMoved(cocos2d::Touch*, cocos2d::Event*);
    void onTouchEnded(cocos2d::Touch*, cocos2d::Event*);
    void onTouchCancelled(cocos2d::Touch*, cocos2d::Event*);
    
    void onTouchesBegan(const std::vector<cocos2d::Touch*>&, cocos2d::Event*);
    void onTouchesMoved(const std::vector<cocos2d::Touch*>&, cocos2d::Event*);
    void onTouchesEnded(const std::vector<cocos2d::Touch*>&, cocos2d::Event*);
    void onTouchesCancelled(const std::vector<cocos2d::Touch*>&, cocos2d::Event*);
    
    bool isNodeContainTouches(const std::vector<cocos2d::Touch*>& touches, cocos2d::Node* node);
    
    void onRecieveSignal(long long);
    cocos2d::Node* drawAxis(cocos2d::Node* target, const DrawRect& signalBbox);
    
    void drawSignal();
    void drawSpectr();
    void drawData(const RoundBuff& source, const SignalWindow& viewWin, const DrawRect& drawRect, cocos2d::Node* target,std::vector<cocos2d::Node*>& dest);
    
    void updateFrequency();
    void updateSignalAxis();

};

#endif // __HELLOWORLD_SCENE_H__
