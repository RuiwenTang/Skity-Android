package org.lynxsdk.lynx.skity.dev.ui

import android.app.ActivityManager
import android.content.Context
import android.os.Debug
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.heightIn
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.produceState
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import kotlinx.coroutines.delay

@Composable
fun GpuInfoOverlay(
    statsProvider: () -> String,
    modifier: Modifier = Modifier
) {
    val context = LocalContext.current
    val overlayText by produceState(initialValue = "", key1 = context, key2 = statsProvider) {
        while (true) {
            val nativeText = statsProvider().trim()
            val appMemoryText = buildAppMemoryText(context)
            value = listOf(nativeText, appMemoryText)
                .filter { it.isNotBlank() }
                .joinToString(separator = "\n")
            delay(500L)
        }
    }

    if (overlayText.isBlank()) {
        return
    }

    Box(
        modifier = modifier
            .fillMaxWidth()
            .padding(12.dp)
            .background(Color(0xD9111725), RoundedCornerShape(16.dp))
            .padding(12.dp)
    ) {
        Column {
            Text(
                text = "GPU Overlay",
                color = Color(0xFFBAE6FD),
                style = MaterialTheme.typography.labelLarge.copy(
                    fontWeight = FontWeight.SemiBold
                )
            )
            Text(
                text = overlayText,
                color = Color(0xFFF8FAFC),
                fontFamily = FontFamily.Monospace,
                fontSize = 11.sp,
                lineHeight = 15.sp,
                modifier = Modifier
                    .padding(top = 6.dp)
                    .heightIn(max = 220.dp)
                    .verticalScroll(rememberScrollState())
            )
        }
    }
}

private fun buildAppMemoryText(context: Context): String {
    val memoryInfo = Debug.MemoryInfo().also(Debug::getMemoryInfo)
    val activityManager = context.getSystemService(Context.ACTIVITY_SERVICE) as? ActivityManager
    val systemMemory = ActivityManager.MemoryInfo().also { info ->
        activityManager?.getMemoryInfo(info)
    }

    val javaUsedBytes = Runtime.getRuntime().totalMemory() - Runtime.getRuntime().freeMemory()
    val nativeHeapBytes = Debug.getNativeHeapAllocatedSize()
    val totalPssBytes = memoryInfo.totalPss.toLong() * 1024L

    return buildString {
        append("App Memory: java ")
        append(formatBytes(javaUsedBytes))
        append("  native ")
        append(formatBytes(nativeHeapBytes))
        append("  pss ")
        append(formatBytes(totalPssBytes))
        if (activityManager != null) {
            append('\n')
            append("System Mem: avail ")
            append(formatBytes(systemMemory.availMem))
            append("  lowRam ")
            append(if (systemMemory.lowMemory) "yes" else "no")
        }
    }
}

private fun formatBytes(bytes: Long): String {
    if (bytes <= 0L) {
        return "0 B"
    }

    val units = arrayOf("B", "KB", "MB", "GB", "TB")
    var value = bytes.toDouble()
    var unitIndex = 0
    while (value >= 1024.0 && unitIndex < units.lastIndex) {
        value /= 1024.0
        unitIndex += 1
    }
    val decimals = if (unitIndex == 0) 0 else 1
    return "%,.${decimals}f %s".format(value, units[unitIndex])
}
