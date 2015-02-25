#include "AppDelegate.h"
#include "MainScene.h"

USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    GLViewImpl *glview = static_cast<GLViewImpl*>(director->getOpenGLView());
    if(!glview) {
#if defined CC_TARGET_OS_MAC || defined _WINDOWS
        int screeWidth = 600;
        int screeHeight = 800;
        GLContextAttrs attrib{8, 8, 8, 0, 16, 0};
        GLView::setGLContextAttrs(attrib);
        
        glview = GLViewImpl::createWithRect("My Game", cocos2d::Rect(0,0, screeWidth, screeHeight));
        if(glview->getRetinaFactor()==2)
        {
            glview->enableRetina(true);
            glview->setFrameSize(screeWidth*2, screeHeight*2);
        }
#else
        glview = GLViewImpl::create("My Game");
#endif

        director->setOpenGLView(glview);
    }
    
    cocos2d::spritebuilder::CCBXReader::createParams("");
    if(director->getWinSize().height<1334)
    {
        FileUtils::getInstance()->setSearchResolutionsOrder({"resources-phonehd"});
        spritebuilder::CCBXReader::setResolutionScale(2.0);
    }
    else
    {
        FileUtils::getInstance()->setSearchResolutionsOrder({"resources-tablethd"});
        spritebuilder::CCBXReader::setResolutionScale(4.0);
    }

    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

    // create a scene. it's an autorelease object
    auto scene = MainScene::createScene();

    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
