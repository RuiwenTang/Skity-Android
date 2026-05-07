package org.lynxsdk.lynx.skity.dev.ui

import android.content.pm.ApplicationInfo
import android.os.Build
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.Button
import androidx.compose.material3.OutlinedButton
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateListOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.unit.dp
import org.lynxsdk.lynx.skity.dev.BackendType
import org.lynxsdk.lynx.skity.dev.DemoScene
import org.lynxsdk.lynx.skity.dev.RenderQualitySettings
import org.lynxsdk.lynx.skity.dev.SharedRendererRegistry
import org.lynxsdk.lynx.skity.dev.VulkanDebugSettings
import org.lynxsdk.lynx.skity.dev.VulkanPresentModeSettings
import kotlinx.coroutines.delay
import kotlin.math.roundToInt

private data class BenchmarkSample(
    val fps: Double,
    val frameTimeMs: Double,
    val pssMb: Double?
)

private enum class BenchmarkWorkload(
    val title: String,
    val description: String,
    val scene: DemoScene
) {
    VECTOR_STRESS(
        "Vector Stress",
        "Repeated filled and stroked paths with dense striping and transform-heavy curves.",
        DemoScene.STRESS_PATHS
    ),
    TEXT_STRESS(
        "Text Stress",
        "Dense repeated glyph placement with varied size and baseline positions.",
        DemoScene.TEXT_CLOUD
    ),
    LAYER_STRESS(
        "Layer Blend",
        "Translucent overlapping geometry with compositing-heavy overlap.",
        DemoScene.LAYERS
    ),
    CLIP_STRESS(
        "Clip Stack",
        "Nested clip regions with constrained overdraw in multiple subregions.",
        DemoScene.CLIPS
    )
}

private enum class BenchmarkDuration(val seconds: Int, val title: String) {
    SHORT(5, "5s"),
    MEDIUM(10, "10s"),
    LONG(20, "20s")
}

@Composable
fun PerformanceTestScreen() {
    val context = LocalContext.current
    val isDebuggable =
        (context.applicationInfo.flags and ApplicationInfo.FLAG_DEBUGGABLE) != 0
    var workload by remember { mutableStateOf(BenchmarkWorkload.VECTOR_STRESS) }
    var backend by remember { mutableStateOf(BackendType.GLES) }
    var duration by remember { mutableStateOf(BenchmarkDuration.MEDIUM) }
    var running by remember { mutableStateOf(false) }
    var elapsedSeconds by remember { mutableStateOf(0) }
    var statsProvider by remember { mutableStateOf<(() -> String)?>(null) }
    val samples = remember { mutableStateListOf<BenchmarkSample>() }
    val validationRequested = VulkanDebugSettings.validationRequested
    val msaaEnabled = RenderQualitySettings.isMsaaEnabled()
    val presentMode = VulkanPresentModeSettings.presentMode

    LaunchedEffect(running, duration, statsProvider) {
        if (!running) {
            return@LaunchedEffect
        }

        samples.clear()
        elapsedSeconds = 0
        val totalSteps = duration.seconds * 2
        repeat(totalSteps) { step ->
            delay(500L)
            statsProvider?.invoke()?.let { overlay ->
                parseBenchmarkSample(overlay)?.let { samples.add(it) }
            }
            elapsedSeconds = ((step + 1) / 2.0).roundToInt()
        }
        running = false
    }

    val summary = remember(samples.toList(), workload, backend, duration, validationRequested, presentMode) {
        buildBenchmarkSummary(
            workload = workload,
            backend = backend,
            duration = duration,
            validationRequested = validationRequested,
            samples = samples
        )
    }

    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(20.dp)
    ) {
        Headline("Performance Test")
        Body("Run a heavier rendering workload with a fixed backend and duration, then compare the aggregated frame metrics.")
        Spacer(Modifier.height(20.dp))
        SurfaceCard(modifier = Modifier.fillMaxWidth()) {
            Column {
                CardTitle("Benchmark Settings")
                Spacer(Modifier.height(10.dp))
                CardTitle("Workload")
                EnumDropdown(
                    selected = workload,
                    values = BenchmarkWorkload.entries.toTypedArray(),
                    itemLabel = { it.title },
                    onSelected = { workload = it }
                )
                Spacer(Modifier.height(14.dp))
                Body(workload.description)
                Spacer(Modifier.height(14.dp))
                CardTitle("Backend")
                EnumDropdown(
                    selected = backend,
                    values = arrayOf(BackendType.GLES, BackendType.VULKAN),
                    itemLabel = { it.title },
                    onSelected = { backend = it }
                )
                Spacer(Modifier.height(14.dp))
                CardTitle("Duration")
                EnumDropdown(
                    selected = duration,
                    values = BenchmarkDuration.entries.toTypedArray(),
                    itemLabel = { it.title },
                    onSelected = { duration = it }
                )
                Spacer(Modifier.height(14.dp))
                MsaaCard(
                    enabled = msaaEnabled,
                    onEnabledChange = { enabled ->
                        RenderQualitySettings.setMsaaEnabled(enabled)
                        SharedRendererRegistry.vulkanSession.setMsaaSampleCount(
                            RenderQualitySettings.msaaSampleCount
                        )
                    }
                )
                if (backend == BackendType.VULKAN) {
                    Spacer(Modifier.height(14.dp))
                    VulkanPresentModeCard(
                        presentMode = presentMode,
                        onPresentModeSelected = { mode ->
                            VulkanPresentModeSettings.updatePresentMode(mode)
                            SharedRendererRegistry.vulkanSession.setPresentMode(mode)
                        }
                    )
                    Spacer(Modifier.height(14.dp))
                    VulkanValidationCard(
                        enabled = validationRequested,
                        onEnabledChange = { enabled ->
                            VulkanDebugSettings.updateValidationRequested(enabled)
                            SharedRendererRegistry.vulkanSession.setValidationRequested(enabled)
                        }
                    )
                }
            }
        }
        Spacer(Modifier.height(16.dp))
        SurfaceCard(modifier = Modifier.fillMaxWidth()) {
            Column {
                CardTitle("Run")
                Spacer(Modifier.height(10.dp))
                Body(
                    if (running) {
                        "Sampling ${workload.title} on ${backend.title} for ${duration.seconds}s. Elapsed: ${elapsedSeconds}s."
                    } else {
                        "Ready to run ${workload.title} on ${backend.title} for ${duration.seconds}s."
                    }
                )
                Spacer(Modifier.height(12.dp))
                Button(
                    modifier = Modifier.fillMaxWidth(),
                    enabled = !running,
                    onClick = { running = true }
                ) {
                    Text("Start Benchmark")
                }
                Spacer(Modifier.height(8.dp))
                OutlinedButton(
                    modifier = Modifier.fillMaxWidth(),
                    enabled = running || samples.isNotEmpty(),
                    onClick = {
                        running = false
                        elapsedSeconds = 0
                        samples.clear()
                    }
                ) {
                    Text("Reset")
                }
            }
        }
        Spacer(Modifier.height(16.dp))
        ScenePreviewPanel(
            scene = workload.scene,
            backend = backend,
            previewHeight = 260.dp,
            onStatsProviderChanged = { provider ->
                statsProvider = provider
            }
        )
        Spacer(Modifier.height(16.dp))
        InfoCard("Benchmark Summary", summary)
        Spacer(Modifier.height(16.dp))
        InfoCard(
            "Test Context",
            buildString {
                appendLine("Device: ${Build.BRAND} ${Build.MODEL}")
                appendLine("Android: ${Build.VERSION.RELEASE} (API ${Build.VERSION.SDK_INT})")
                appendLine("ABI: ${Build.SUPPORTED_ABIS.joinToString()}")
                append("Debuggable build: ${if (isDebuggable) "yes" else "no"}")
            }
        )
    }
}

private fun parseBenchmarkSample(text: String): BenchmarkSample? {
    val fpsMatch = Regex("""FPS:\s*([0-9.]+)\s+Frame:\s*([0-9.]+)""")
        .find(text) ?: return null
    val fps = fpsMatch.groupValues[1].toDoubleOrNull() ?: return null
    val frame = fpsMatch.groupValues[2].toDoubleOrNull() ?: return null
    val pss = Regex("""pss\s+([0-9.]+)\s+MB""", RegexOption.IGNORE_CASE)
        .find(text)
        ?.groupValues
        ?.getOrNull(1)
        ?.toDoubleOrNull()
    return BenchmarkSample(fps = fps, frameTimeMs = frame, pssMb = pss)
}

private fun buildBenchmarkSummary(
    workload: BenchmarkWorkload,
    backend: BackendType,
    duration: BenchmarkDuration,
    validationRequested: Boolean,
    samples: List<BenchmarkSample>
): String {
    if (samples.isEmpty()) {
        return buildString {
            appendLine("Workload: ${workload.title}")
            appendLine("Backend: ${backend.title}")
            appendLine("Duration: ${duration.seconds}s")
            if (backend == BackendType.VULKAN) {
                appendLine("Present mode: ${VulkanPresentModeSettings.presentMode.title}")
            }
            append("Run the benchmark to collect aggregate results.")
        }
    }

    val avgFps = samples.map { it.fps }.average()
    val avgFrame = samples.map { it.frameTimeMs }.average()
    val p95Frame = samples.map { it.frameTimeMs }.sorted().let { sorted ->
        val index = ((sorted.size - 1) * 0.95).toInt().coerceIn(sorted.indices)
        sorted[index]
    }
    val avgPss = samples.mapNotNull { it.pssMb }.takeIf { it.isNotEmpty() }?.average()

    return buildString {
        appendLine("Workload: ${workload.title}")
        appendLine("Backend: ${backend.title}")
        appendLine("Duration: ${duration.seconds}s")
        if (backend == BackendType.VULKAN) {
            appendLine("Present mode: ${VulkanPresentModeSettings.presentMode.title}")
            appendLine("Validation requested: ${if (validationRequested) "on" else "off"}")
        }
        appendLine("Samples: ${samples.size}")
        appendLine("Average FPS: ${"%.1f".format(avgFps)}")
        appendLine("Average frame time: ${"%.2f".format(avgFrame)} ms")
        appendLine("P95 frame time: ${"%.2f".format(p95Frame)} ms")
        if (avgPss != null) {
            append("Average app PSS: ${"%.1f".format(avgPss)} MB")
        }
    }
}
