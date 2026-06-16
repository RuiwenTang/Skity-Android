package org.lynxsdk.lynx.skity.dev

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import org.lynxsdk.lynx.skity.dev.ui.SkityLabTheme
import org.lynxsdk.lynx.skity.dev.ui.TextureShareScreen

class TextureShareActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            SkityLabTheme {
                TextureShareScreen()
            }
        }
    }
}
