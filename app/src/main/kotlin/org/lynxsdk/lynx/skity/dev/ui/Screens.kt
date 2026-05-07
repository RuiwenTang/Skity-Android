package org.lynxsdk.lynx.skity.dev.ui

import android.os.Build
import android.content.pm.PackageManager
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
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
import org.lynxsdk.lynx.skity.dev.SkityRenderSurfaceView
import org.lynxsdk.lynx.skity.dev.SkityNative
import org.lynxsdk.lynx.skity.dev.SkityVulkanSurfaceView

@Composable
fun LauncherScreen(
    onOpenSceneGallery: () -> Unit,
    onOpenBackendCompare: () -> Unit,
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
        OutlinedButton(modifier = Modifier.fillMaxWidth(), onClick = onOpenCapabilityInfo) {
            Text("Capability Info")
        }
    }
}

@Composable
fun SceneGalleryScreen() {
    var scene by remember { mutableStateOf(DemoScene.SHAPES) }
    var backend by remember { mutableStateOf(BackendType.AUTO) }

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
        SurfaceCard(modifier = Modifier.fillMaxWidth()) {
            ScenePreviewHost(
                scene = scene,
                backend = backend,
                modifier = Modifier
                    .fillMaxWidth()
                    .height(260.dp)
            )
        }
    }
}

@Composable
fun BackendCompareScreen() {
    var scene by remember { mutableStateOf(DemoScene.SHAPES) }

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
        Spacer(Modifier.height(18.dp))
        CardTitle("GLES Preview")
        Spacer(Modifier.height(8.dp))
        SurfaceCard(modifier = Modifier.fillMaxWidth()) {
            ScenePreviewHost(
                scene = scene,
                backend = BackendType.GLES,
                modifier = Modifier
                    .fillMaxWidth()
                    .height(220.dp)
            )
        }
        Spacer(Modifier.height(18.dp))
        CardTitle("Vulkan Preview")
        Spacer(Modifier.height(8.dp))
        SurfaceCard(modifier = Modifier.fillMaxWidth()) {
            ScenePreviewHost(
                scene = scene,
                backend = BackendType.VULKAN,
                modifier = Modifier
                    .fillMaxWidth()
                    .height(220.dp)
            )
        }
    }
}

@Composable
fun CapabilityInfoScreen() {
    val context = LocalContext.current
    val packageManager = context.packageManager
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
        InfoCard("Native Status", SkityNative.getStatusSummary())
    }
}

@Composable
private fun InfoCard(title: String, body: String) {
    SurfaceCard(modifier = Modifier.fillMaxWidth()) {
        Column {
            CardTitle(title)
            Spacer(Modifier.height(10.dp))
            Body(body)
        }
    }
}

@Composable
private fun Headline(text: String) {
    Text(
        text = text,
        style = MaterialTheme.typography.headlineMedium.copy(
            fontWeight = FontWeight.Bold,
            fontSize = 30.sp
        )
    )
}

@Composable
private fun CardTitle(text: String) {
    Text(
        text = text,
        style = MaterialTheme.typography.titleMedium.copy(fontWeight = FontWeight.Bold)
    )
}

@Composable
private fun Body(text: String) {
    Text(
        text = text,
        style = MaterialTheme.typography.bodyMedium,
        color = MaterialTheme.colorScheme.onSurface.copy(alpha = 0.72f)
    )
}

@Composable
private fun ScenePreviewHost(
    scene: DemoScene,
    backend: BackendType,
    modifier: Modifier = Modifier
) {
    val context = LocalContext.current
    val supportsVulkan = context.packageManager.hasSystemFeature(PackageManager.FEATURE_VULKAN_HARDWARE_LEVEL)
    when (backend) {
        BackendType.AUTO, BackendType.GLES -> RealGlesPreview(scene = scene, modifier = modifier)
        BackendType.VULKAN -> if (supportsVulkan) {
            RealVulkanPreview(scene = scene, modifier = modifier)
        } else {
            PreviewFallback(
                title = "Vulkan unavailable",
                body = "This device does not report Vulkan window-system support, so the Vulkan preview stays in fallback mode."
            )
        }
    }
}

@Composable
private fun RealGlesPreview(
    scene: DemoScene,
    modifier: Modifier = Modifier
) {
    val context = LocalContext.current
    @Suppress("DEPRECATION")
    val lifecycleOwner = LocalLifecycleOwner.current
    val view = remember {
        SkityRenderSurfaceView(context)
    }

    DisposableEffect(view, lifecycleOwner) {
        val observer = LifecycleEventObserver { _, event ->
            when (event) {
                Lifecycle.Event.ON_RESUME -> view.onResume()
                Lifecycle.Event.ON_PAUSE -> view.onPause()
                else -> Unit
            }
        }
        lifecycleOwner.lifecycle.addObserver(observer)

        onDispose {
            lifecycleOwner.lifecycle.removeObserver(observer)
            view.onPause()
            view.release()
        }
    }

    AndroidView(
        modifier = modifier,
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

@Composable
private fun RealVulkanPreview(
    scene: DemoScene,
    modifier: Modifier = Modifier
) {
    val context = LocalContext.current
    @Suppress("DEPRECATION")
    val lifecycleOwner = LocalLifecycleOwner.current
    val view = remember {
        SkityVulkanSurfaceView(context)
    }

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

    AndroidView(
        modifier = modifier,
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
private fun <T> EnumDropdown(
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
