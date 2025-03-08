#include <jni.h>
#include <string>
#include "renderer.h"

extern "C" {

JNIEXPORT void JNICALL
Java_com_example_myphysicsdemo_MyGLRenderer_nativeInit(JNIEnv* env, jobject thiz) {
    initRenderer();
}

JNIEXPORT void JNICALL
Java_com_example_myphysicsdemo_MyGLRenderer_nativeResize(JNIEnv* env, jobject thiz,
                                                         jint width, jint height) {
    resizeRenderer(width, height);
}

JNIEXPORT void JNICALL
Java_com_example_myphysicsdemo_MyGLRenderer_nativeDrawFrame(JNIEnv* env, jobject thiz) {
    drawFrame();
}

}