//
// Created by Víctor Jiménez Rugama on 12/24/21.
//
#include "../../include/view/mainView.h"
#include "../../include/utils/AppGraphics.h"
#include "../../include/view/repositoryView.h"
#include "../../include/view/homeView.h"
#include "../../include/view/downloadView.h"
#include "../../include/view/settingsView.h"
#include "../../include/utils/logger.h"
#include "../../include/ControllerManager.h"
#include "../../include/view/packageSearch.h"
#include <proto-include.h>
#include <thread>

bool mainView::updatingView;
int mainView::updateView() {

    mainScene->FrameBufferClear();

    updatingView = true;
    controllerManager->getCurrentView()->updateView();
    updatingView = false;

    tabView->updateTab();

    // Submit the frame buffer
    mainScene->SubmitFlip(frameID);
    mainScene->FrameWait(frameID);

    // Swap to the next buffer
    mainScene->FrameBufferSwap();
    frameID++;
    return controllerManager->getExit();
}

mainView::mainView(bool isFirstRun) : subViews()  {

    updatingView = false;
    bgColor = {255,255,255};
    // No buffering
    setvbuf(stdout, NULL, _IONBF, 0);

    // Create a 2D scene
    LOG << "Creating a scene" << "\n";

    mainScene = new Scene2D(frameWidth, frameHeight, frameDepth);

    if(!mainScene->Init(0xC000000, 2))
    {
        LOG << "Failed to initialize 2D scene" << "\n";
        for(;;);
    }

    // Initialize the font faces with arial (must be included in the package root!)
    std::string font(DATA_PATH);
    //font+="assets/fonts/Gontserrat-Regular.ttf";
    font+="assets/fonts/Noto_Sans/NotoSans-Regular.ttf";

    const char * fontC = font.c_str();
    // Set colors
    frameID = 0;

    //FOR SOME ODD REASON THIS DEBUGLOG JUST BREAKS ??
    LOG << "Initializing fontLarge (" << fontC << ")" << "\n";
    if(!mainScene->InitFont(&fontLarge, fontC, FONT_SIZE_LARGE))
    {
        LOG << "Failed to initialize large font '" << fontC << "'" << "\n";
        for(;;);
    }

    //FOR SOME ODD REASON THIS DEBUGLOG JUST BREAKS ??
    LOG << "Initializing fontMediumLarge (" << fontC << ")" << "\n";
    if(!mainScene->InitFont(&fontMediumLarge, fontC, FONT_SIZE_MEDIUM_LARGE))
    {
        LOG << "Failed to initialize medium-large font '" << fontC << "'" << "\n";
        for(;;);
    }


    //FOR SOME ODD REASON THIS DEBUGLOG JUST BREAKS ??
    LOG << "Initializing fontMedium (" << fontC << ")" << "\n";
    if(!mainScene->InitFont(&fontMedium, fontC, FONT_SIZE_MEDIUM))
    {
        LOG << "Failed to initialize large font '" << fontC << "'" << "\n";
        for(;;);
    }

    LOG << "Initializing fontSmall (" << fontC << ")" << "\n";
    if(!mainScene->InitFont(&fontSmall, fontC, FONT_SIZE_SMALL))
    {
        LOG << "Failed to initialize small font '" << fontC << "'" << "\n";
        for(;;);
    }


    //INITIALIZE VIEWS

    LOG << "Initializing tabView" << "\n";
    tabView = new class tabView(mainScene,frameWidth,frameHeight);

    LOG << "Settings subViews" << "\n";
    subViews[0] = new homeView(mainScene,fontMediumLarge,fontMedium,fontSmall,frameWidth,frameHeight);
    LOG << "Initialized homeView" << "\n";
    subViews[1] = new repositoryView(mainScene,fontLarge,fontMedium,fontSmall, frameWidth, frameHeight,isFirstRun);
    LOG << "Initialized repositoryView" << "\n";
    subViews[2] = new downloadView(mainScene,fontLarge,fontMedium,fontSmall, frameWidth, frameHeight);
    LOG << "Initialized downloadView" << "\n";
    subViews[3] = new packageSearch(mainScene,fontLarge,fontMedium,fontSmall, frameWidth, frameHeight);
    LOG << "Initialized packageSearch" << "\n";
    subViews[4] = new settingsView(mainScene,fontLarge,fontMedium,fontSmall, frameWidth, frameHeight);
    LOG << "Initialized settingsView" << "\n";

    LOG << "Initializing ControllerManager" << "\n";
    controllerManager = new ControllerManager(tabView,subViews);
    LOG << "Initializing Thread" << "\n";
    std::thread(&ControllerManager::initController, std::ref(*controllerManager)).detach();

}

mainView::~mainView() {
    delete controllerManager;
    LOG << "Deleted controllerManager";

    delete tabView;
    LOG << "Deleted tabView";

    for(auto & subView : subViews)
        delete subView;

    LOG << "Deleted subViews";

}
