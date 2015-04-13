#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "spritebuilder/SpriteBuilder.h"
#include "SignalManager.h"

struct SignalWindow
{
    float start;// percent
    float width;// percent
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
    cocos2d::Label* _count = nullptr;
    cocos2d::Label* _maxY = nullptr;
    cocos2d::Label* _minY = nullptr;
    cocos2d::Label* _xTime = nullptr;
    cocos2d::Node* _pause = nullptr;
    cocos2d::Node* _node_graphic = nullptr;
    cocos2d::Node* _node_fft = nullptr;
    std::vector<cocos2d::Node*> _points;
    std::vector<cocos2d::Node*> _spectrPoints;
    
    float _xTimeSec = 0.2f;
    SignalWindow _signalWin={0.0f, 1.f};
    
    virtual cocos2d::spritebuilder::ccReaderClickCallback onResolveCCBClickSelector(const std::string &selectorName, cocos2d::Node* node) override;
    virtual bool onAssignCCBMemberVariable(const std::string &memberVariableName, cocos2d::Node* node) override;
    
    void onPause(cocos2d::Ref* target);
    void onResume(cocos2d::Ref* target);
    void onXTimeUp2(cocos2d::Ref* target);
    void onXTimeDown2(cocos2d::Ref* target);
    
    void onRecieveSignal(long long);
    void drawAxis(cocos2d::Node* node);
    
    void drawSignal();
    void drawSpectr();
    
    SignalManager _rawSignal;
};

#endif // __HELLOWORLD_SCENE_H__
