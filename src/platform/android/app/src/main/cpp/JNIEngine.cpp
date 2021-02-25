#include <jni.h>
#include <assert.h>

#include "Engine.hpp"
#include "MainEngineController.hpp"

Engine* _engine = NULL;
MainEngineController* _mainEngineController = NULL;

extern "C"
JNIEXPORT void JNICALL
Java_com_gowen_ggj21_Engine_createDeviceDependentResources(JNIEnv *env, jobject thiz)
{
    _engine->createDeviceDependentResources();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_gowen_ggj21_Engine_onWindowSizeChanged(JNIEnv *env, jobject thiz, jint screen_width, jint screen_height, jint cursor_width, jint cursor_height)
{
    _engine->onWindowSizeChanged(screen_width, screen_height, cursor_width, cursor_height);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_gowen_ggj21_Engine_releaseDeviceDependentResources(JNIEnv *env, jobject thiz)
{
    _engine->releaseDeviceDependentResources();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_gowen_ggj21_Engine_onResume(JNIEnv *env, jobject thiz)
{
    _engine->onResume();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_gowen_ggj21_Engine_onPause(JNIEnv *env, jobject thiz)
{
    _engine->onPause();
}

extern "C"
JNIEXPORT int JNICALL
Java_com_gowen_ggj21_Engine_update(JNIEnv *env, jobject thiz, jdouble delta_time)
{
    return _engine->update(delta_time);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_gowen_ggj21_Engine_render(JNIEnv *env, jobject thiz)
{
    _engine->render();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_gowen_ggj21_Engine_onCursorDown(JNIEnv *env, jobject thiz, jfloat x, jfloat y)
{
    _engine->onCursorDown(x, y);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_gowen_ggj21_Engine_onCursorDragged(JNIEnv *env, jobject thiz, jfloat x, jfloat y)
{
    _engine->onCursorDragged(x, y);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_gowen_ggj21_Engine_onCursorUp(JNIEnv *env, jobject thiz, jfloat x, jfloat y)
{
    _engine->onCursorUp(x, y);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_gowen_ggj21_Engine_init(JNIEnv *env, jobject thiz, jobject asset_manager)
{
    assert(_mainEngineController == NULL);
    _mainEngineController = new MainEngineController(env, asset_manager);

    assert(_engine == NULL);
    _engine = new Engine(*_mainEngineController);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_gowen_ggj21_Engine_deinit(JNIEnv *env, jobject thiz)
{
    delete _engine;
    _engine = NULL;

    delete _mainEngineController;
    _mainEngineController = NULL;
}
