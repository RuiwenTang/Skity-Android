package org.lynxsdk.lynx.skity.dev

import android.content.Intent
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import org.lynxsdk.lynx.skity.dev.ui.LauncherScreen
import org.lynxsdk.lynx.skity.dev.ui.SkityLabTheme

class LauncherActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            SkityLabTheme {
                LauncherScreen(
                    onOpenSceneGallery = {
                        startActivity(Intent(this, SceneGalleryActivity::class.java))
                    },
                    onOpenBackendCompare = {
                        startActivity(Intent(this, BackendCompareActivity::class.java))
                    },
                    onOpenCapabilityInfo = {
                        startActivity(Intent(this, CapabilityInfoActivity::class.java))
                    }
                )
            }
        }
    }
}
