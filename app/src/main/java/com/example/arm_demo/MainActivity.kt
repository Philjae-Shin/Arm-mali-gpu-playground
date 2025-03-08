package com.example.arm_demo

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.TextView
import com.example.arm_demo.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var glView: GLSurfaceView

    companion object {
        init {
            System.loadLibrary("MyPhysicsDemo") // JNI 라이브러리 로드
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        glView = GLSurfaceView(this)
        // OpenGL ES 3.0
        glView.setEGLContextClientVersion(3)
        glView.setRenderer(MyGLRenderer())
        setContentView(glView)
    }

    public override fun onPause() {
        super.onPause()
        glView.onPause()
    }

    public override fun onResume() {
        super.onResume()
        glView.onResume()
    }
}

    /**
     * A native method that is implemented by the 'arm_demo' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    companion object {
        // Used to load the 'arm_demo' library on application startup.
        init {
            System.loadLibrary("arm_demo")
        }
    }
}