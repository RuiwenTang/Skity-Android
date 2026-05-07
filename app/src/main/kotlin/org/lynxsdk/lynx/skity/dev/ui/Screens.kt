package org.lynxsdk.lynx.skity.dev.ui

import android.os.Build
import android.content.pm.ApplicationInfo
import android.content.pm.PackageManager
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.Button
import androidx.compose.material3.DropdownMenuItem
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.ExposedDropdownMenuBox
import androidx.compose.material3.ExposedDropdownMenuDefaults
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedButton
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Switch
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.DisposableEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalLifecycleOwner
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.compose.ui.viewinterop.AndroidView
import androidx.lifecycle.Lifecycle
import androidx.lifecycle.LifecycleEventObserver
import org.lynxsdk.lynx.skity.dev.BackendType
import org.lynxsdk.lynx.skity.dev.DemoScene
import org.lynxsdk.lynx.skity.dev.RenderQualitySettings
import org.lynxsdk.lynx.skity.dev.SharedRendererRegistry
import org.lynxsdk.lynx.skity.dev.SkityRenderSurfaceView
import org.lynxsdk.lynx.skity.dev.SkityNative
import org.lynxsdk.lynx.skity.dev.SkityVulkanSurfaceView
import org.lynxsdk.lynx.skity.dev.VulkanDebugSettings
import org.lynxsdk.lynx.skity.dev.VulkanPresentMode
import org.lynxsdk.lynx.skity.dev.VulkanPresentModeSettings
import androidx.compose.runtime.LaunchedEffect

@Composable
fun LauncherScreen(
    onOpenSceneGallery: () -> Unit,
    onOpenBackendCompare: () -> Unit,
    onOpenPerformanceTest: () -> Unit,
    onOpenCapabilityInfo: () -> Unit
) {
    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(20.dp)
    ) {
        Headline("Skity Lab")
        Body("A lightweight Android shell for scene preview, backend comparison, and skity integration validation.")
        Spacer(Modifier.height(20.dp))
        SurfaceCard(modifier = Modifier.fillMaxWidth()) {
            Column {
                CardTitle("Native Status")
                Body(SkityNative.getStatusSummary())
            }
        }
        Spacer(Modifier.height(20.dp))
        Button(modifier = Modifier.fillMaxWidth(), onClick = onOpenSceneGallery) {
            Text("Scene Gallery")
        }
        Spacer(Modifier.height(12.dp))
        Button(modifier = Modifier.fillMaxWidth(), onClick = onOpenBackendCompare) {
            Text("Backend Compare")
        }
        Spacer(Modifier.height(12.dp))
        Button(modifier = Modifier.fillMaxWidth(), onClick = onOpenPerformanceTest) {
            Text("Performance Test")
        }
        Spacer(Modifier.height(12.dp))
        OutlinedButton(modifier = Modifier.fillMaxWidth(), onClick = onOpenCapabilityInfo) {
            Text("Capability Info")
        }
    }
}

@Composable
fun SceneGalleryScreen() {
    var scene by remember { mutableStateOf(DemoScene.SHAPES) }
    var backend by remember { mutableStateOf(BackendType.AUTO) }
    val validationRequested = VulkanDebugSettings.validationRequested
    val msaaEnabled = RenderQualitySettings.isMsaaEnabled()
    val presentMode = VulkanPresentModeSettings.presentMode

    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(20.dp)
    ) {
        Headline("Scene Gallery")
        Body("Preview a single scene and switch the intended backend target for validation.")
        Spacer(Modifier.height(20.dp))
        SurfaceCard(modifier = Modifier.fillMaxWidth()) {
            Column {
                CardTitle("Scene")
                EnumDropdown(selected = scene, values = DemoScene.entries.toTypedArray(), itemLabel = { it.title }) {
                    scene = it
                }
                Spacer(Modifier.height(14.dp))
                CardTitle("Backend")
                EnumDropdown(selected = backend, values = BackendType.entries.toTypedArray(), itemLabel = { it.title }) {
                    backend = it
                }
                Spacer(Modifier.height(14.dp))
                Body(scene.description)
                Spacer(Modifier.height(8.dp))
                Body(backend.description)
            }
        }
        Spacer(Modifier.height(16.dp))
        MsaaCard(
            enabled = msaaEnabled,
            onEnabledChange = { enabled ->
                RenderQualitySettings.setMsaaEnabled(enabled)
                SharedRendererRegistry.vulkanSession.setMsaaSampleCount(
                    RenderQualitySettings.msaaSampleCount
                )
            }
        )
        Spacer(Modifier.height(16.dp))
        VulkanPresentModeCard(
            presentMode = presentMode,
            onPresentModeSelected = { mode ->
                VulkanPresentModeSettings.updatePresentMode(mode)
                SharedRendererRegistry.vulkanSession.setPresentMode(mode)
            }
        )
        Spacer(Modifier.height(16.dp))
        VulkanValidationCard(
            enabled = validationRequested,
            onEnabledChange = { enabled ->
                VulkanDebugSettings.updateValidationRequested(enabled)
                SharedRendererRegistry.vulkanSession.setValidationRequested(enabled)
            }
        )
        Spacer(Modifier.height(16.dp))
        ScenePreviewPanel(
            scene = scene,
            backend = backend,
            previewHeight = 260.dp
        )
    }
}

@Composable
fun BackendCompareScreen() {
    var scene by remember { mutableStateOf(DemoScene.SHAPES) }
    val validationRequested = VulkanDebugSettings.validationRequested
    val msaaEnabled = RenderQualitySettings.isMsaaEnabled()
    val presentMode = VulkanPresentModeSettings.presentMode

    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(20.dp)
    ) {
        Headline("Backend Compare")
        Body("Keep the same scene visible while comparing GLES and Vulkan expectations side by side.")
        Spacer(Modifier.height(18.dp))
        EnumDropdown(selected = scene, values = DemoScene.entries.toTypedArray(), itemLabel = { it.title }) {
            scene = it
        }
        Spacer(Modifier.height(12.dp))
        Body("Use this screen to compare the same scene across backends. Current scene: ${scene.title}.")
        Spacer(Modifier.height(16.dp))
        MsaaCard(
            enabled = msaaEnabled,
            onEnabledChange = { enabled ->
                RenderQualitySettings.setMsaaEnabled(enabled)
                SharedRendererRegistry.vulkanSession.setMsaaSampleCount(
                    RenderQualitySettings.msaaSampleCount
                )
            }
        )
        Spacer(Modifier.height(16.dp))
        VulkanPresentModeCard(
            presentMode = presentMode,
            onPresentModeSelected = { mode ->
                VulkanPresentModeSettings.updatePresentMode(mode)
                SharedRendererRegistry.vulkanSession.setPresentMode(mode)
            }
        )
        Spacer(Modifier.height(16.dp))
        VulkanValidationCard(
            enabled = validationRequested,
            onEnabledChange = { enabled ->
                VulkanDebugSettings.updateValidationRequested(enabled)
                SharedRendererRegistry.vulkanSession.setValidationRequested(enabled)
            }
        )
        Spacer(Modifier.height(18.dp))
        CardTitle("GLES Preview")
        Spacer(Modifier.height(8.dp))
        ScenePreviewPanel(
            scene = scene,
            backend = BackendType.GLES,
            previewHeight = 220.dp
        )
        Spacer(Modifier.height(18.dp))
        CardTitle("Vulkan Preview")
        Spacer(Modifier.height(8.dp))
        ScenePreviewPanel(
            scene = scene,
            backend = BackendType.VULKAN,
            previewHeight = 220.dp
        )
    }
}

@Composable
fun CapabilityInfoScreen() {
    val context = LocalContext.current
    val packageManager = context.packageManager
    val isDebuggable =
        (context.applicationInfo.flags and ApplicationInfo.FLAG_DEBUGGABLE) != 0
    val hasVulkan = packageManager.hasSystemFeature(PackageManager.FEATURE_VULKAN_HARDWARE_LEVEL)
    val hasExtensionPack = packageManager.hasSystemFeature(PackageManager.FEATURE_OPENGLES_EXTENSION_PACK)
    val deviceInfo = buildString {
        appendLine("Brand: ${Build.BRAND}")
        appendLine("Model: ${Build.MODEL}")
        appendLine("Device: ${Build.DEVICE}")
        appendLine("Android: ${Build.VERSION.RELEASE} (API ${Build.VERSION.SDK_INT})")
        append("ABIs: ${Build.SUPPORTED_ABIS.joinToString()}")
    }
    val backendInfo = buildString {
        appendLine("Current backend wiring:")
        appendLine("- GLES: Real skity rendering is active through GLSurfaceView.")
        appendLine("- Vulkan: Real skity rendering is active through SurfaceView when device support is available.")
        appendLine()
        appendLine("Platform capabilities:")
        appendLine("- Vulkan feature: ${if (hasVulkan) "available" else "not reported"}")
        append("- GLES extension pack: ${if (hasExtensionPack) "available" else "not reported"}")
    }

    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(20.dp)
    ) {
        Headline("Capability Info")
        InfoCard("Device Information", deviceInfo)
        Spacer(Modifier.height(14.dp))
        InfoCard("Backend Validation Notes", backendInfo)
        Spacer(Modifier.height(14.dp))
        InfoCard(
            "Validation Runtime Toggle",
            if (isDebuggable) {
                "Debug build: Vulkan validation can be requested at runtime from Scene Gallery and Backend Compare."
            } else {
                "Release build: the Vulkan validation toggle is ignored and no validation layers are requested."
            }
        )
        Spacer(Modifier.height(14.dp))
        InfoCard("Native Status", SkityNative.getStatusSummary())
    }
}

@Composable
internal fun InfoCard(title: String, body: String) {
    SurfaceCard(modifier = Modifier.fillMaxWidth()) {
        Column {
            CardTitle(title)
            Spacer(Modifier.height(10.dp))
            Body(body)
        }
    }
}

@Composable
internal fun MsaaCard(
    enabled: Boolean,
    onEnabledChange: (Boolean) -> Unit
) {
    SurfaceCard(modifier = Modifier.fillMaxWidth()) {
        Column {
            CardTitle("MSAA")
            Spacer(Modifier.height(10.dp))
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.SpaceBetween
            ) {
                Column(modifier = Modifier.fillMaxWidth(0.8f)) {
                    Body("Enable runtime multisample antialiasing for both backends. Current mode: ${if (enabled) "4x" else "Off"}")
                }
                Switch(
                    checked = enabled,
                    onCheckedChange = onEnabledChange
                )
            }
        }
    }
}

@Composable
internal fun VulkanValidationCard(
    enabled: Boolean,
    onEnabledChange: (Boolean) -> Unit
) {
    val context = LocalContext.current
    val isDebuggable =
        (context.applicationInfo.flags and ApplicationInfo.FLAG_DEBUGGABLE) != 0
    SurfaceCard(modifier = Modifier.fillMaxWidth()) {
        Column {
            CardTitle("Vulkan Validation")
            Spacer(Modifier.height(10.dp))
            if (isDebuggable) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Column(modifier = Modifier.fillMaxWidth(0.8f)) {
                        Body("Request Vulkan validation layers at runtime for the shared debug Vulkan renderer.")
                    }
                    Switch(
                        checked = enabled,
                        onCheckedChange = onEnabledChange
                    )
                }
            } else {
                Body("Release build ignores the Vulkan validation toggle and does not load validation layers.")
            }
        }
    }
}

@Composable
internal fun VulkanPresentModeCard(
    presentMode: VulkanPresentMode,
    onPresentModeSelected: (VulkanPresentMode) -> Unit
) {
    SurfaceCard(modifier = Modifier.fillMaxWidth()) {
        Column {
            CardTitle("Vulkan Present Mode")
            Spacer(Modifier.height(10.dp))
            Body("Choose the requested swapchain present mode for Vulkan previews. Unsupported modes may fall back to FIFO on the device.")
            Spacer(Modifier.height(12.dp))
            EnumDropdown(
                selected = presentMode,
                values = VulkanPresentMode.entries.toTypedArray(),
                itemLabel = { it.title },
                onSelected = onPresentModeSelected
            )
            Spacer(Modifier.height(10.dp))
            Body(presentMode.description)
        }
    }
}

@Composable
internal fun Headline(text: String) {
    Text(
        text = text,
        style = MaterialTheme.typography.headlineMedium.copy(
            fontWeight = FontWeight.Bold,
            fontSize = 30.sp
        )
    )
}

@Composable
internal fun CardTitle(text: String) {
    Text(
        text = text,
        style = MaterialTheme.typography.titleMedium.copy(fontWeight = FontWeight.Bold)
    )
}

@Composable
internal fun Body(text: String) {
    Text(
        text = text,
        style = MaterialTheme.typography.bodyMedium,
        color = MaterialTheme.colorScheme.onSurface.copy(alpha = 0.72f)
    )
}

@Composable
internal fun ScenePreviewPanel(
    scene: DemoScene,
    backend: BackendType,
    previewHeight: androidx.compose.ui.unit.Dp,
    onStatsProviderChanged: (((() -> String)?) -> Unit)? = null
) {
    val context = LocalContext.current
    val supportsVulkan = context.packageManager.hasSystemFeature(PackageManager.FEATURE_VULKAN_HARDWARE_LEVEL)
    when (backend) {
        BackendType.AUTO, BackendType.GLES -> GlesPreviewPanel(
            scene = scene,
            previewHeight = previewHeight,
            onStatsProviderChanged = onStatsProviderChanged
        )
        BackendType.VULKAN -> if (supportsVulkan) {
            VulkanPreviewPanel(
                scene = scene,
                previewHeight = previewHeight,
                onStatsProviderChanged = onStatsProviderChanged
            )
        } else {
            onStatsProviderChanged?.invoke(null)
            SurfaceCard(modifier = Modifier.fillMaxWidth()) {
                PreviewFallback(
                    title = "Vulkan unavailable",
                    body = "This device does not report Vulkan window-system support, so the Vulkan preview stays in fallback mode."
                )
            }
        }
    }
}

@Composable
private fun GlesPreviewPanel(
    scene: DemoScene,
    previewHeight: androidx.compose.ui.unit.Dp,
    onStatsProviderChanged: (((() -> String)?) -> Unit)? = null
) {
    val context = LocalContext.current
    @Suppress("DEPRECATION")
    val lifecycleOwner = LocalLifecycleOwner.current
    val view = remember {
        SkityRenderSurfaceView(context)
    }
    val msaaSampleCount = RenderQualitySettings.msaaSampleCount

    DisposableEffect(view, lifecycleOwner) {
        onStatsProviderChanged?.invoke { view.getOverlayDetails() }
        val observer = LifecycleEventObserver { _, event ->
            when (event) {
                Lifecycle.Event.ON_RESUME -> view.onResume()
                Lifecycle.Event.ON_PAUSE -> view.onPause()
                else -> Unit
            }
        }
        lifecycleOwner.lifecycle.addObserver(observer)

        onDispose {
            onStatsProviderChanged?.invoke(null)
            lifecycleOwner.lifecycle.removeObserver(observer)
            view.onPause()
            view.release()
        }
    }

    LaunchedEffect(view, msaaSampleCount) {
        view.setMsaaSampleCount(msaaSampleCount)
    }

    Column {
        SurfaceCard(modifier = Modifier.fillMaxWidth()) {
            AndroidView(
                modifier = Modifier
                    .fillMaxWidth()
                    .height(previewHeight),
                factory = {
                    view.apply {
                        setScene(scene)
                    }
                },
                update = {
                    it.setScene(scene)
                }
            )
        }
        Spacer(Modifier.height(10.dp))
        GpuInfoOverlay(
            statsProvider = { view.getOverlayDetails() }
        )
    }
}

@Composable
private fun VulkanPreviewPanel(
    scene: DemoScene,
    previewHeight: androidx.compose.ui.unit.Dp,
    onStatsProviderChanged: (((() -> String)?) -> Unit)? = null
) {
    val context = LocalContext.current
    @Suppress("DEPRECATION")
    val lifecycleOwner = LocalLifecycleOwner.current
    val view = remember {
        SkityVulkanSurfaceView(context)
    }
    val msaaSampleCount = RenderQualitySettings.msaaSampleCount

    DisposableEffect(view, lifecycleOwner) {
        onStatsProviderChanged?.invoke { view.getOverlayDetails() }
        val observer = LifecycleEventObserver { _, event ->
            when (event) {
                Lifecycle.Event.ON_RESUME -> view.onResumeRendering()
                Lifecycle.Event.ON_PAUSE -> view.onPauseRendering()
                else -> Unit
            }
        }
        lifecycleOwner.lifecycle.addObserver(observer)

        onDispose {
            onStatsProviderChanged?.invoke(null)
            lifecycleOwner.lifecycle.removeObserver(observer)
            view.onPauseRendering()
            view.release()
        }
    }

    LaunchedEffect(msaaSampleCount) {
        SharedRendererRegistry.vulkanSession.setMsaaSampleCount(msaaSampleCount)
    }

    Column {
        SurfaceCard(modifier = Modifier.fillMaxWidth()) {
            AndroidView(
                modifier = Modifier
                    .fillMaxWidth()
                    .height(previewHeight),
                factory = {
                    view.apply {
                        setScene(scene)
                    }
                },
                update = {
                    it.setScene(scene)
                }
            )
        }
        Spacer(Modifier.height(10.dp))
        GpuInfoOverlay(
            statsProvider = { view.getOverlayDetails() }
        )
    }
}

@Composable
private fun PreviewFallback(
    title: String,
    body: String
) {
    Box(modifier = Modifier.fillMaxSize()) {
        PreviewCanvas(
            scene = DemoScene.TEXT,
            backend = BackendType.VULKAN,
            modifier = Modifier.fillMaxSize()
        )
        Column(modifier = Modifier.padding(20.dp)) {
            CardTitle(title)
            Spacer(Modifier.height(10.dp))
            Body(body)
        }
    }
}

@Suppress("DEPRECATION")
@OptIn(ExperimentalMaterial3Api::class)
@Composable
internal fun <T> EnumDropdown(
    selected: T,
    values: Array<T>,
    itemLabel: (T) -> String,
    onSelected: (T) -> Unit
) {
    var expanded by remember { mutableStateOf(false) }

    ExposedDropdownMenuBox(
        expanded = expanded,
        onExpandedChange = { expanded = !expanded }
    ) {
        OutlinedTextField(
            value = itemLabel(selected),
            onValueChange = {},
            readOnly = true,
            modifier = Modifier
                .menuAnchor()
                .fillMaxWidth(),
            trailingIcon = { ExposedDropdownMenuDefaults.TrailingIcon(expanded = expanded) }
        )
        ExposedDropdownMenu(
            expanded = expanded,
            onDismissRequest = { expanded = false }
        ) {
            values.forEach { item ->
                DropdownMenuItem(
                    text = { Text(itemLabel(item)) },
                    onClick = {
                        expanded = false
                        onSelected(item)
                    }
                )
            }
        }
    }
}
