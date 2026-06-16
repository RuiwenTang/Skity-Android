package org.lynxsdk.lynx.skity.dev.ui

import android.os.Build
import android.content.pm.ApplicationInfo
import android.content.pm.PackageManager
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.defaultMinSize
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.widthIn
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.ui.draw.clip
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.Button
import androidx.compose.material3.DropdownMenu
import androidx.compose.material3.DropdownMenuItem
import androidx.compose.material3.AlertDialog
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.LocalMinimumInteractiveComponentSize
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedButton
import androidx.compose.material3.Switch
import androidx.compose.material3.Text
import androidx.compose.material3.TextButton
import androidx.compose.runtime.Composable
import androidx.compose.runtime.CompositionLocalProvider
import androidx.compose.runtime.DisposableEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalLifecycleOwner
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.style.TextOverflow
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.compose.ui.draw.scale
import androidx.compose.ui.viewinterop.AndroidView
import androidx.lifecycle.Lifecycle
import androidx.lifecycle.LifecycleEventObserver
import org.lynxsdk.lynx.skity.dev.BackendType
import org.lynxsdk.lynx.skity.dev.DemoScene
import org.lynxsdk.lynx.skity.dev.RenderQualitySettings
import org.lynxsdk.lynx.skity.dev.SharedRendererRegistry
import org.lynxsdk.lynx.skity.dev.SkityRenderSurfaceView
import org.lynxsdk.lynx.skity.dev.SkityNative
import org.lynxsdk.lynx.skity.dev.SkityTextureShareSurfaceView
import org.lynxsdk.lynx.skity.dev.SkityVulkanSurfaceView
import org.lynxsdk.lynx.skity.dev.VulkanDebugSettings
import org.lynxsdk.lynx.skity.dev.VulkanFramePacingMode
import org.lynxsdk.lynx.skity.dev.VulkanFramePacingSettings
import org.lynxsdk.lynx.skity.dev.VulkanMinImageCount
import org.lynxsdk.lynx.skity.dev.VulkanMinImageCountSettings
import org.lynxsdk.lynx.skity.dev.VulkanPresentMode
import org.lynxsdk.lynx.skity.dev.VulkanPresentModeSettings
import androidx.compose.runtime.LaunchedEffect

private val ScreenPadding = 12.dp
private val CardSpacing = 10.dp
private val SectionSpacing = 12.dp
private val CompactRowSpacing = 10.dp
private val RenderOptionsCardPadding = PaddingValues(horizontal = 6.dp, vertical = 1.dp)
private val RenderOptionsItemSpacing = 0.dp

private enum class RenderSettingInfo(
    val title: String,
    val body: String
) {
    MSAA(
        "MSAA",
        "Enable runtime multisample antialiasing for both backends. When enabled, the current mode is 4x MSAA."
    ),
    PRESENT_MODE(
        "Vulkan Present Mode",
        "Choose the requested Vulkan swapchain present mode. Unsupported modes may fall back to FIFO on the device."
    ),
    SWAPCHAIN_IMAGES(
        "Vulkan Swapchain Images",
        "Choose the requested minimum Vulkan swapchain image count. The driver may clamp the final value to surface limits."
    ),
    FRAME_PACING(
        "Vulkan Frame Pacing",
        "Choose whether Vulkan frames are triggered by Android Choreographer or by a render-thread self-paced loop. Presenter pacing is useful for swapchain experiments, especially with FIFO."
    ),
    VALIDATION(
        "Vulkan Validation",
        "Request Vulkan validation layers at runtime for the shared debug Vulkan renderer. Release builds ignore this toggle."
    )
}

@Composable
fun LauncherScreen(
    onOpenSceneGallery: () -> Unit,
    onOpenBackendCompare: () -> Unit,
    onOpenPerformanceTest: () -> Unit,
    onOpenCapabilityInfo: () -> Unit,
    onOpenTextureShare: () -> Unit
) {
    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(ScreenPadding)
    ) {
        Headline("Skity Lab")
        Body("A lightweight Android shell for scene preview, backend comparison, and skity integration validation.")
        Spacer(Modifier.height(16.dp))
        SurfaceCard(modifier = Modifier.fillMaxWidth()) {
            Column {
                CardTitle("Native Status")
                Body(SkityNative.getStatusSummary())
            }
        }
        Spacer(Modifier.height(16.dp))
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
        Spacer(Modifier.height(12.dp))
        Button(modifier = Modifier.fillMaxWidth(), onClick = onOpenTextureShare) {
            Text("GL-Vulkan Texture Share")
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
    val minImageCount = VulkanMinImageCountSettings.minImageCount
    val framePacingMode = VulkanFramePacingSettings.mode

    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(ScreenPadding)
    ) {
        Headline("Scene Gallery")
        Body("Preview one scene with a tighter control panel so the renderer gets more room.")
        Spacer(Modifier.height(SectionSpacing))
        SurfaceCard(modifier = Modifier.fillMaxWidth()) {
            Column {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.spacedBy(CompactRowSpacing)
                ) {
                    Column(modifier = Modifier.weight(1f)) {
                        CardTitle("Scene")
                        Spacer(Modifier.height(4.dp))
                        EnumDropdown(
                            selected = scene,
                            values = DemoScene.entries.toTypedArray(),
                            itemLabel = { it.title }
                        ) {
                            scene = it
                        }
                    }
                    Column(modifier = Modifier.weight(1f)) {
                        CardTitle("Backend")
                        Spacer(Modifier.height(4.dp))
                        EnumDropdown(
                            selected = backend,
                            values = BackendType.entries.toTypedArray(),
                            itemLabel = { it.title }
                        ) {
                            backend = it
                        }
                    }
                }
                Spacer(Modifier.height(4.dp))
                FinePrint(scene.description)
                Spacer(Modifier.height(1.dp))
                FinePrint(backend.description)
            }
        }
        Spacer(Modifier.height(CardSpacing))
        RenderOptionsCard(
            msaaEnabled = msaaEnabled,
            onMsaaEnabledChange = { enabled ->
                RenderQualitySettings.setMsaaEnabled(enabled)
                SharedRendererRegistry.vulkanSession.setMsaaSampleCount(
                    RenderQualitySettings.msaaSampleCount
                )
            },
            showMinImageCount = true,
            minImageCount = minImageCount,
            onMinImageCountSelected = { count ->
                VulkanMinImageCountSettings.updateMinImageCount(count)
                SharedRendererRegistry.vulkanSession.setMinImageCount(count.imageCount)
            },
            showFramePacing = true,
            framePacingMode = framePacingMode,
            onFramePacingModeSelected = { mode ->
                VulkanFramePacingSettings.updateMode(mode)
                SharedRendererRegistry.vulkanSession.setFramePacingMode(mode)
            },
            showPresentMode = true,
            presentMode = presentMode,
            onPresentModeSelected = { mode ->
                VulkanPresentModeSettings.updatePresentMode(mode)
                SharedRendererRegistry.vulkanSession.setPresentMode(mode)
            },
            showValidation = true,
            validationEnabled = validationRequested,
            onValidationEnabledChange = { enabled ->
                VulkanDebugSettings.updateValidationRequested(enabled)
                SharedRendererRegistry.vulkanSession.setValidationRequested(enabled)
            }
        )
        Spacer(Modifier.height(SectionSpacing))
        ScenePreviewPanel(
            scene = scene,
            backend = backend,
            previewHeight = 380.dp
        )
    }
}

@Composable
fun BackendCompareScreen() {
    var scene by remember { mutableStateOf(DemoScene.SHAPES) }
    val validationRequested = VulkanDebugSettings.validationRequested
    val msaaEnabled = RenderQualitySettings.isMsaaEnabled()
    val presentMode = VulkanPresentModeSettings.presentMode
    val minImageCount = VulkanMinImageCountSettings.minImageCount
    val framePacingMode = VulkanFramePacingSettings.mode

    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(ScreenPadding)
    ) {
        Headline("Backend Compare")
        Body("Compare the same scene across backends without letting controls dominate the screen.")
        Spacer(Modifier.height(SectionSpacing))
        SurfaceCard(modifier = Modifier.fillMaxWidth()) {
            Column {
                CardTitle("Scene")
                Spacer(Modifier.height(4.dp))
                EnumDropdown(
                    selected = scene,
                    values = DemoScene.entries.toTypedArray(),
                    itemLabel = { it.title }
                ) {
                    scene = it
                }
                Spacer(Modifier.height(2.dp))
                FinePrint("Current scene: ${scene.title}")
            }
        }
        Spacer(Modifier.height(CardSpacing))
        RenderOptionsCard(
            msaaEnabled = msaaEnabled,
            onMsaaEnabledChange = { enabled ->
                RenderQualitySettings.setMsaaEnabled(enabled)
                SharedRendererRegistry.vulkanSession.setMsaaSampleCount(
                    RenderQualitySettings.msaaSampleCount
                )
            },
            showMinImageCount = true,
            minImageCount = minImageCount,
            onMinImageCountSelected = { count ->
                VulkanMinImageCountSettings.updateMinImageCount(count)
                SharedRendererRegistry.vulkanSession.setMinImageCount(count.imageCount)
            },
            showFramePacing = true,
            framePacingMode = framePacingMode,
            onFramePacingModeSelected = { mode ->
                VulkanFramePacingSettings.updateMode(mode)
                SharedRendererRegistry.vulkanSession.setFramePacingMode(mode)
            },
            showPresentMode = true,
            presentMode = presentMode,
            onPresentModeSelected = { mode ->
                VulkanPresentModeSettings.updatePresentMode(mode)
                SharedRendererRegistry.vulkanSession.setPresentMode(mode)
            },
            showValidation = true,
            validationEnabled = validationRequested,
            onValidationEnabledChange = { enabled ->
                VulkanDebugSettings.updateValidationRequested(enabled)
                SharedRendererRegistry.vulkanSession.setValidationRequested(enabled)
            }
        )
        Spacer(Modifier.height(SectionSpacing))
        CardTitle("GLES Preview")
        Spacer(Modifier.height(6.dp))
        ScenePreviewPanel(
            scene = scene,
            backend = BackendType.GLES,
            previewHeight = 280.dp
        )
        Spacer(Modifier.height(SectionSpacing))
        CardTitle("Vulkan Preview")
        Spacer(Modifier.height(6.dp))
        ScenePreviewPanel(
            scene = scene,
            backend = BackendType.VULKAN,
            previewHeight = 280.dp
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
            .padding(ScreenPadding)
    ) {
        Headline("Capability Info")
        InfoCard("Device Information", deviceInfo)
        Spacer(Modifier.height(CardSpacing))
        InfoCard("Backend Validation Notes", backendInfo)
        Spacer(Modifier.height(CardSpacing))
        InfoCard(
            "Validation Runtime Toggle",
            if (isDebuggable) {
                "Debug build: Vulkan validation can be requested at runtime from Scene Gallery and Backend Compare."
            } else {
                "Release build: the Vulkan validation toggle is ignored and no validation layers are requested."
            }
        )
        Spacer(Modifier.height(CardSpacing))
        InfoCard("Native Status", SkityNative.getStatusSummary())
    }
}

@Composable
internal fun InfoCard(title: String, body: String) {
        SurfaceCard(modifier = Modifier.fillMaxWidth()) {
            Column {
                CardTitle(title)
                Spacer(Modifier.height(4.dp))
                Body(body)
            }
        }
}

@Composable
internal fun RenderOptionsCard(
    msaaEnabled: Boolean,
    onMsaaEnabledChange: (Boolean) -> Unit,
    showMinImageCount: Boolean,
    minImageCount: VulkanMinImageCount,
    onMinImageCountSelected: (VulkanMinImageCount) -> Unit,
    showFramePacing: Boolean,
    framePacingMode: VulkanFramePacingMode,
    onFramePacingModeSelected: (VulkanFramePacingMode) -> Unit,
    showPresentMode: Boolean,
    presentMode: VulkanPresentMode,
    onPresentModeSelected: (VulkanPresentMode) -> Unit,
    showValidation: Boolean,
    validationEnabled: Boolean,
    onValidationEnabledChange: (Boolean) -> Unit
) {
    var infoDialog by remember { mutableStateOf<RenderSettingInfo?>(null) }

    SurfaceCard(
        modifier = Modifier.fillMaxWidth(),
        contentPadding = RenderOptionsCardPadding
    ) {
        CompositionLocalProvider(
            LocalMinimumInteractiveComponentSize provides Dp.Unspecified
        ) {
            Column(
                verticalArrangement = Arrangement.spacedBy(RenderOptionsItemSpacing)
            ) {
                CardTitle("Renderer Options")
                CompactSwitchRow(
                    title = "MSAA",
                    checked = msaaEnabled,
                    onCheckedChange = onMsaaEnabledChange,
                    onInfoClick = { infoDialog = RenderSettingInfo.MSAA }
                )
                if (showMinImageCount) {
                    CompactDropdownRow(
                        title = "Swapchain",
                        onInfoClick = { infoDialog = RenderSettingInfo.SWAPCHAIN_IMAGES }
                    ) { modifier ->
                        EnumDropdown(
                            selected = minImageCount,
                            values = VulkanMinImageCount.entries.toTypedArray(),
                            itemLabel = { it.title },
                            modifier = modifier,
                            textStyle = MaterialTheme.typography.bodySmall.copy(
                                fontSize = 11.sp
                            ),
                            onSelected = onMinImageCountSelected
                        )
                    }
                }
                if (showFramePacing) {
                    CompactDropdownRow(
                        title = "Pacing",
                        onInfoClick = { infoDialog = RenderSettingInfo.FRAME_PACING }
                    ) { modifier ->
                        EnumDropdown(
                            selected = framePacingMode,
                            values = VulkanFramePacingMode.entries.toTypedArray(),
                            itemLabel = { it.title },
                            modifier = modifier,
                            textStyle = MaterialTheme.typography.bodySmall.copy(
                                fontSize = 11.sp
                            ),
                            onSelected = onFramePacingModeSelected
                        )
                    }
                }
                if (showPresentMode) {
                    CompactDropdownRow(
                        title = "Present Mode",
                        onInfoClick = { infoDialog = RenderSettingInfo.PRESENT_MODE }
                    ) { modifier ->
                        EnumDropdown(
                            selected = presentMode,
                            values = VulkanPresentMode.entries.toTypedArray(),
                            itemLabel = { it.title },
                            modifier = modifier,
                            textStyle = MaterialTheme.typography.bodySmall.copy(
                                fontSize = 11.sp
                            ),
                            onSelected = onPresentModeSelected
                        )
                    }
                }
                if (showValidation) {
                    CompactValidationRow(
                        enabled = validationEnabled,
                        onEnabledChange = onValidationEnabledChange,
                        onInfoClick = { infoDialog = RenderSettingInfo.VALIDATION }
                    )
                }
            }
        }
    }

    infoDialog?.let { info ->
        AlertDialog(
            onDismissRequest = { infoDialog = null },
            confirmButton = {
                TextButton(onClick = { infoDialog = null }) {
                    Text("Close")
                }
            },
            title = { Text(info.title) },
            text = { Text(info.body) }
        )
    }
}

@Composable
private fun CompactSwitchRow(
    title: String,
    checked: Boolean,
    onCheckedChange: (Boolean) -> Unit,
    onInfoClick: () -> Unit
) {
    Row(
        modifier = Modifier.fillMaxWidth(),
        horizontalArrangement = Arrangement.spacedBy(3.dp),
        verticalAlignment = Alignment.CenterVertically
    ) {
        Column(modifier = Modifier.weight(1f)) {
            Row(
                horizontalArrangement = Arrangement.spacedBy(0.dp),
                verticalAlignment = Alignment.CenterVertically
            ) {
                CompactOptionTitle(title)
                CompactInfoButton(onClick = onInfoClick)
            }
        }
        Switch(
            modifier = Modifier.scale(0.76f),
            checked = checked,
            onCheckedChange = onCheckedChange
        )
    }
}

@Composable
private fun CompactDropdownRow(
    title: String,
    onInfoClick: () -> Unit,
    content: @Composable (Modifier) -> Unit
) {
    Row(
        modifier = Modifier.fillMaxWidth(),
        horizontalArrangement = Arrangement.spacedBy(3.dp),
        verticalAlignment = Alignment.CenterVertically
    ) {
        Row(
            modifier = Modifier.weight(1f),
            horizontalArrangement = Arrangement.spacedBy(0.dp),
            verticalAlignment = Alignment.CenterVertically
        ) {
            CompactOptionTitle(title)
            CompactInfoButton(onClick = onInfoClick)
        }
        content(Modifier.widthIn(min = 104.dp, max = 148.dp))
    }
}

@Composable
private fun CompactValidationRow(
    enabled: Boolean,
    onEnabledChange: (Boolean) -> Unit,
    onInfoClick: () -> Unit
) {
    val context = LocalContext.current
    val isDebuggable =
        (context.applicationInfo.flags and ApplicationInfo.FLAG_DEBUGGABLE) != 0

    if (isDebuggable) {
        CompactSwitchRow(
            title = "Validation",
            checked = enabled,
            onCheckedChange = onEnabledChange,
            onInfoClick = onInfoClick
        )
    } else {
        Column {
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.CenterVertically
            ) {
                CompactOptionTitle("Validation")
                CompactInfoButton(onClick = onInfoClick)
            }
            Body("Release only")
        }
    }
}

@Composable
private fun CompactOptionTitle(text: String) {
    Text(
        text = text,
        style = MaterialTheme.typography.bodySmall.copy(
            fontWeight = FontWeight.SemiBold,
            fontSize = 11.sp
        )
    )
}

@Composable
private fun CompactInfoButton(onClick: () -> Unit) {
    TextButton(
        onClick = onClick,
        modifier = Modifier.defaultMinSize(minWidth = 1.dp, minHeight = 1.dp),
        contentPadding = PaddingValues(horizontal = 1.dp, vertical = 0.dp)
    ) {
        Text(
            text = "Info",
            style = MaterialTheme.typography.labelSmall.copy(
                fontSize = 9.sp
            )
        )
    }
}

@Composable
private fun FinePrint(text: String) {
    Text(
        text = text,
        style = MaterialTheme.typography.bodySmall.copy(
            fontSize = 10.sp
        ),
        color = MaterialTheme.colorScheme.onSurface.copy(alpha = 0.66f)
    )
}

@Composable
internal fun Headline(text: String) {
    Text(
        text = text,
        style = MaterialTheme.typography.headlineMedium.copy(
            fontWeight = FontWeight.Bold,
            fontSize = 19.sp
        )
    )
}

@Composable
internal fun CardTitle(text: String) {
    Text(
        text = text,
        style = MaterialTheme.typography.titleSmall.copy(
            fontWeight = FontWeight.Bold,
            fontSize = 13.sp
        )
    )
}

@Composable
internal fun Body(text: String) {
    Text(
        text = text,
        style = MaterialTheme.typography.bodySmall.copy(
            fontSize = 11.sp
        ),
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
        BackendType.VULKAN, BackendType.TEXTURE_SHARE -> if (supportsVulkan) {
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
            Spacer(Modifier.height(6.dp))
            Body(body)
        }
    }
}

@Composable
internal fun <T> EnumDropdown(
    selected: T,
    values: Array<T>,
    itemLabel: (T) -> String,
    modifier: Modifier = Modifier,
    textStyle: TextStyle = MaterialTheme.typography.bodySmall.copy(
        fontSize = 10.sp
    ),
    fieldHeight: Dp = 24.dp,
    onSelected: (T) -> Unit
) {
    var expanded by remember { mutableStateOf(false) }

    val shape = RoundedCornerShape(10.dp)

    Box(modifier = modifier) {
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .height(fieldHeight)
                .clip(shape)
                .border(1.dp, MaterialTheme.colorScheme.outline, shape)
                .background(MaterialTheme.colorScheme.surface)
                .clickable { expanded = true }
                .padding(horizontal = 6.dp),
            horizontalArrangement = Arrangement.spacedBy(4.dp),
            verticalAlignment = Alignment.CenterVertically
        ) {
            Text(
                text = itemLabel(selected),
                modifier = Modifier.weight(1f),
                style = textStyle,
                maxLines = 1,
                overflow = TextOverflow.Ellipsis
            )
            Text(
                text = "v",
                style = MaterialTheme.typography.labelSmall.copy(
                    fontSize = 9.sp,
                    fontWeight = FontWeight.Medium
                )
            )
        }

        DropdownMenu(
            expanded = expanded,
            onDismissRequest = { expanded = false }
        ) {
            values.forEach { item ->
                DropdownMenuItem(
                    text = {
                        Text(
                            text = itemLabel(item),
                            style = MaterialTheme.typography.bodySmall.copy(
                                fontSize = 10.sp
                            ),
                            maxLines = 1,
                            overflow = TextOverflow.Ellipsis
                        )
                    },
                    onClick = {
                        expanded = false
                        onSelected(item)
                    }
                )
            }
        }
    }
}

@Composable
fun TextureShareScreen() {
    val context = LocalContext.current
    @Suppress("DEPRECATION")
    val lifecycleOwner = LocalLifecycleOwner.current
    val view = remember {
        SkityTextureShareSurfaceView(context)
    }

    val validationRequested = VulkanDebugSettings.validationRequested
    val msaaEnabled = RenderQualitySettings.isMsaaEnabled()
    val presentMode = VulkanPresentModeSettings.presentMode
    val minImageCount = VulkanMinImageCountSettings.minImageCount
    val framePacingMode = VulkanFramePacingSettings.mode

    DisposableEffect(view, lifecycleOwner) {
        val observer = LifecycleEventObserver { _, event ->
            when (event) {
                Lifecycle.Event.ON_RESUME -> view.onResumeRendering()
                Lifecycle.Event.ON_PAUSE -> view.onPauseRendering()
                else -> Unit
            }
        }
        lifecycleOwner.lifecycle.addObserver(observer)

        onDispose {
            lifecycleOwner.lifecycle.removeObserver(observer)
            view.onPauseRendering()
            view.release()
        }
    }

    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(ScreenPadding)
    ) {
        Headline("GL-Vulkan Texture Share")
        Body("OpenGL ES renders an animated triangle into an AHardwareBuffer, which is imported as a Vulkan texture via skity's WrapTexture API and drawn to the swapchain.")
        Spacer(Modifier.height(SectionSpacing))
        SurfaceCard(modifier = Modifier.fillMaxWidth()) {
            Column {
                CardTitle("Sharing Pipeline")
                Body("GL (offscreen FBO) → AHardwareBuffer → skity WrapTexture → Vulkan texture → Canvas DrawImageRect → Swapchain")
            }
        }
        Spacer(Modifier.height(CardSpacing))
        RenderOptionsCard(
            msaaEnabled = msaaEnabled,
            onMsaaEnabledChange = { enabled ->
                RenderQualitySettings.setMsaaEnabled(enabled)
                SharedRendererRegistry.textureShareSession.setMsaaSampleCount(
                    RenderQualitySettings.msaaSampleCount
                )
            },
            showMinImageCount = true,
            minImageCount = minImageCount,
            onMinImageCountSelected = { count ->
                VulkanMinImageCountSettings.updateMinImageCount(count)
                SharedRendererRegistry.textureShareSession.setMinImageCount(count.imageCount)
            },
            showFramePacing = true,
            framePacingMode = framePacingMode,
            onFramePacingModeSelected = { mode ->
                VulkanFramePacingSettings.updateMode(mode)
                SharedRendererRegistry.textureShareSession.setFramePacingMode(mode)
            },
            showPresentMode = true,
            presentMode = presentMode,
            onPresentModeSelected = { mode ->
                VulkanPresentModeSettings.updatePresentMode(mode)
                SharedRendererRegistry.textureShareSession.setPresentMode(mode)
            },
            showValidation = true,
            validationEnabled = validationRequested,
            onValidationEnabledChange = { enabled ->
                VulkanDebugSettings.updateValidationRequested(enabled)
                SharedRendererRegistry.textureShareSession.setValidationRequested(enabled)
            }
        )
        Spacer(Modifier.height(SectionSpacing))
        SurfaceCard(modifier = Modifier.fillMaxWidth()) {
            AndroidView(
                modifier = Modifier
                    .fillMaxWidth()
                    .height(380.dp),
                factory = { view }
            )
        }
        Spacer(Modifier.height(10.dp))
        GpuInfoOverlay(
            statsProvider = { view.getOverlayDetails() }
        )
    }
}
