//
//  HelloApp.cpp
//  Cornell University Game Library (CUGL)
//
//  This is the implementation file for the custom application. This is the
//  definition of your root (and in this case only) class.
//
//  CUGL zlib License:
//      This software is provided 'as-is', without any express or implied
//      warranty.  In no event will the authors be held liable for any damages
//      arising from the use of this software.
//
//      Permission is granted to anyone to use this software for any purpose,
//      including commercial applications, and to alter it and redistribute it
//      freely, subject to the following restrictions:
//
//      1. The origin of this software must not be misrepresented; you must not
//      claim that you wrote the original software. If you use this software
//      in a product, an acknowledgment in the product documentation would be
//      appreciated but is not required.
//
//      2. Altered source versions must be plainly marked as such, and must not
//      be misrepresented as being the original software.
//
//      3. This notice may not be removed or altered from any source distribution.
//
//  Author: Walker White
//  Version: 1/8/17
//
// Include the class header, which includes all of the CUGL classes
#include "HelloApp.h"
#include <cugl/base/CUBase.h>

// Add support for simple random number generation
#include <cstdlib>
#include <ctime>

// This keeps us from having to write cugl:: all the time
using namespace cugl;

// The number of frames before moving the logo to a new position
#define TIME_STEP 60
// This is adjusted by screen aspect ratio to get the height
#define GAME_WIDTH 256

/**
 * The method called after OpenGL is initialized, but before running the application.
 *
 * This is the method in which all user-defined program intialization should
 * take place.  You should not create a new init() method.
 *
 * When overriding this method, you should call the parent method as the
 * very last line.  This ensures that the state will transition to FOREGROUND,
 * causing the application to run.
 */
void HelloApp::onStartup() {
    Size size = getDisplaySize();
    size *= GAME_WIDTH/size.width;
    
    // Create a scene graph the same size as the window
    _scene = Scene2::alloc(size.width, size.height);
    // Create a sprite batch (and background color) to render the scene
    _batch = SpriteBatch::alloc();
    setClearColor(Color4(229,229,229,255));
    
    // Create an asset manager to load all assets
    _assets = AssetManager::alloc();
    
    // You have to attach the individual loaders for each asset type
    _assets->attach<Texture>(TextureLoader::alloc()->getHook());
    _assets->attach<Font>(FontLoader::alloc()->getHook());
    _assets->attach<scene2::SceneNode>(Scene2Loader::alloc()->getHook());
    _assets->attach<WidgetValue>(WidgetLoader::alloc()->getHook());

    
    // This reads the given JSON file and uses it to load all other assets
    _assets->loadDirectory("json/assets.json");

    // Activate mouse or touch screen input as appropriate
    // We have to do this BEFORE the scene, because the scene has a button
#if defined (CU_TOUCH_SCREEN)
    Input::activate<Touchscreen>();
#else
    Input::activate<Mouse>();
#endif
    
    // Build the scene from these assets
    buildScene();
    Application::onStartup();
    
    // Report the safe area
    Rect bounds = Display::get()->getSafeBounds();
    CULog("Safe Area %sx%s",bounds.origin.toString().c_str(),
                            bounds.size.toString().c_str());

    bounds = getSafeBounds();
    CULog("Safe Area %sx%s",bounds.origin.toString().c_str(),
                            bounds.size.toString().c_str());

    bounds = getDisplayBounds();
    CULog("Full Area %sx%s",bounds.origin.toString().c_str(),
                            bounds.size.toString().c_str());
  
    Haptics::start();
}

/**
 * The method called when the application is ready to quit.
 *
 * This is the method to dispose of all resources allocated by this
 * application.  As a rule of thumb, everything created in onStartup()
 * should be deleted here.
 *
 * When overriding this method, you should call the parent method as the
 * very last line.  This ensures that the state will transition to NONE,
 * causing the application to be deleted.
 */
void HelloApp::onShutdown() {
    // Delete all smart pointers
    _logo = nullptr;
    _scene = nullptr;
    _batch = nullptr;
    _assets = nullptr;
    
    // Deativate input
#if defined CU_TOUCH_SCREEN
    Input::deactivate<Touchscreen>();
#else
    Input::deactivate<Mouse>();
#endif
    Application::onShutdown();
  
    Haptics::stop();
}

/**
 * The method called to update the application data.
 *
 * This is your core loop and should be replaced with your custom implementation.
 * This method should contain any code that is not an OpenGL call.
 *
 * When overriding this method, you do not need to call the parent method
 * at all. The default implmentation does nothing.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void HelloApp::update(float timestep) {}

/**
 * The method called to draw the application to the screen.
 *
 * This is your core loop and should be replaced with your custom implementation.
 * This method should OpenGL and related drawing calls.
 *
 * When overriding this method, you do not need to call the parent method
 * at all. The default implmentation does nothing.
 */
void HelloApp::draw() {
    // This takes care of begin/end
    _scene->render(_batch);
}

/**
 * Internal helper to build the scene graph.
 *
 * Scene graphs are not required.  You could manage all scenes just like
 * you do in 3152.  However, they greatly simplify scene management, and
 * have become standard in most game engines.
 */
void HelloApp::buildScene() {
    Size size = getDisplaySize();
    size *= GAME_WIDTH/size.width;
      
    auto main = _assets->get<scene2::SceneNode>("main");
    main->setContentSize(size);
    _scene->addChild(main);
   
  
    auto transient = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("main_play-transient"));
    transient->addListener([=](const std::string& name, bool down){
        if (down) return;
        Haptics::get()->playTransient(_intensity, _sharpness);
    });
    transient->activate();
    
    auto continuous = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("main_play-continuous"));
    continuous->addListener([=](const std::string& name, bool down){
        if (down) return;
        Haptics::get()->playContinuous(_intensity, _sharpness, _duration);
    });
    continuous->activate();
    
    auto set_value = [](const std::shared_ptr<scene2::Label>& label, float val) {
        std::stringstream stream;
        stream.precision(1);
        stream << std::fixed << val;
        label->setText(stream.str());
    };
    
    
    auto intensity_value = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("main_intensity_value"));
    set_value(intensity_value, _intensity);
    auto intensity_increase = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("main_intensity_increase"));
    auto intensity_decrease = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("main_intensity_decrease"));
    intensity_increase->addListener([=](const std::string& name, bool down){
        if (down) return;
        _intensity += 0.1;
        _intensity = std::max(0.0f, std::min(_intensity, 1.0f));
        set_value(intensity_value, _intensity);
        CULog("%f", _intensity);
    });
    intensity_decrease->addListener([=](const std::string& name, bool down){
        if (down) return;
        _intensity -= 0.1;
        _intensity = std::max(0.0f, std::min(_intensity, 1.0f));
        set_value(intensity_value, _intensity);
        CULog("%f", _intensity);
    });
    intensity_increase->activate();
    intensity_decrease->activate();
    
    auto sharpness_value = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("main_sharpness_value"));
    set_value(sharpness_value, _sharpness);
    auto sharpness_increase = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("main_sharpness_increase"));
    auto sharpness_decrease = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("main_sharpness_decrease"));
    sharpness_increase->addListener([=](const std::string& name, bool down){
        if (down) return;
        _sharpness += 0.1;
        _sharpness = std::max(0.0f, std::min(_sharpness, 1.0f));
        set_value(sharpness_value, _sharpness);
        CULog("%f", _sharpness);
    });
    sharpness_decrease->addListener([=](const std::string& name, bool down){
        if (down) return;
        _sharpness -= 0.1;
        _sharpness = std::max(0.0f, std::min(_sharpness, 1.0f));
        set_value(sharpness_value, _sharpness);
        CULog("%f", _sharpness);
    });
    sharpness_increase->activate();
    sharpness_decrease->activate();

    auto duration_value = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("main_duration_value"));
    set_value(duration_value, _duration);
    auto duration_increase = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("main_duration_increase"));
    auto duration_decrease = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("main_duration_decrease"));
    duration_increase->addListener([=](const std::string& name, bool down){
        if (down) return;
        _duration += 0.1;
        _duration = std::max(0.0f, _duration);
        set_value(duration_value, _duration);
        CULog("%f", _duration);
    });
    duration_decrease->addListener([=](const std::string& name, bool down){
        if (down) return;
        _duration -= 0.1;
        _duration = std::max(0.0f, _duration);
        set_value(duration_value, _duration);
        CULog("%f", _duration);
    });
    duration_increase->activate();
    duration_decrease->activate();

    
}
