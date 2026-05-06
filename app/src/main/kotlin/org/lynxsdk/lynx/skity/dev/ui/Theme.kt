package org.lynxsdk.lynx.skity.dev.ui

import androidx.compose.foundation.isSystemInDarkTheme
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.darkColorScheme
import androidx.compose.material3.lightColorScheme
import androidx.compose.runtime.Composable
import androidx.compose.ui.graphics.Color

private val LightColors = lightColorScheme(
    primary = Color(0xFF0F766E),
    onPrimary = Color.White,
    secondary = Color(0xFFF59E0B),
    onSecondary = Color.Black,
    background = Color(0xFFF4F1EA),
    onBackground = Color(0xFF172033),
    surface = Color(0xFFFFFBF5),
    onSurface = Color(0xFF172033),
    outline = Color(0x330F172A)
)

private val DarkColors = darkColorScheme(
    primary = Color(0xFF90CAF9),
    onPrimary = Color.Black,
    secondary = Color(0xFFF59E0B),
    onSecondary = Color.Black,
    background = Color(0xFF0F172A),
    onBackground = Color(0xFFF8FAFC),
    surface = Color(0xFF172033),
    onSurface = Color(0xFFF8FAFC),
    outline = Color(0x3390CAF9)
)

@Composable
fun SkityLabTheme(
    darkTheme: Boolean = isSystemInDarkTheme(),
    content: @Composable () -> Unit
) {
    MaterialTheme(
        colorScheme = if (darkTheme) DarkColors else LightColors,
        content = content
    )
}
