package org.lynxsdk.lynx.skity.dev

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import org.lynxsdk.lynx.skity.dev.ui.BackendCompareScreen
import org.lynxsdk.lynx.skity.dev.ui.SkityLabTheme

class BackendCompareActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            SkityLabTheme {
                BackendCompareScreen()
            }
        }
    }
}
