package com.example.arm_demo

class MyGLRenderer : GLSurfaceView.Renderer {

    external fun nativeInit()
    external fun nativeResize(width: Int, height: Int)
    external fun nativeDrawFrame()

    override fun onSurfaceCreated(unused: GL10, config: EGLConfig) {
        nativeInit()
    }

    override fun onSurfaceChanged(unused: GL10, width: Int, height: Int) {
        nativeResize(width, height)
    }

    override fun onDrawFrame(unused: GL10) {
        nativeDrawFrame()
    }
}