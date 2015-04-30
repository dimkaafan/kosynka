#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "spritebuilder/SpriteBuilder.h"

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
    cocos2d::Node* _pause = nullptr;
    cocos2d::Node* _resume = nullptr;
    cocos2d::Node* _node_graphic = nullptr;
    cocos2d::Node* _node_fft = nullptr;
    cocos2d::Node* _scrollNode = nullptr;
    cocos2d::EventListener* _listener = nullptr;

    
    virtual cocos2d::spritebuilder::ccReaderClickCallback onResolveCCBClickSelector(const std::string &selectorName, cocos2d::Node* node) override;
    virtual bool onAssignCCBMemberVariable(const std::string &memberVariableName, cocos2d::Node* node) override;
    
    void onPause(cocos2d::Ref* target);
    void onResume(cocos2d::Ref* target);
    
    bool onTouchBegan(cocos2d::Touch*, cocos2d::Event*);
    void onTouchMoved(cocos2d::Touch*, cocos2d::Event*);
    void onTouchEnded(cocos2d::Touch*, cocos2d::Event*);
    void onTouchCancelled(cocos2d::Touch*, cocos2d::Event*);
    
    void onTouchesBegan(const std::vector<cocos2d::Touch*>&, cocos2d::Event*);
    void onTouchesMoved(const std::vector<cocos2d::Touch*>&, cocos2d::Event*);
    void onTouchesEnded(const std::vector<cocos2d::Touch*>&, cocos2d::Event*);
    void onTouchesCancelled(const std::vector<cocos2d::Touch*>&, cocos2d::Event*);
    
    bool isNodeContainTouches(const std::vector<cocos2d::Touch*>& touches, cocos2d::Node* node);
};

#endif // __HELLOWORLD_SCENE_H__
