package org.lynxsdk.lynx.skity.dev.ui

import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.CornerRadius
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Rect
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.Path
import androidx.compose.ui.graphics.StrokeCap
import androidx.compose.ui.graphics.StrokeJoin
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.graphics.drawscope.rotate
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import org.lynxsdk.lynx.skity.dev.BackendType
import org.lynxsdk.lynx.skity.dev.DemoScene

@Composable
fun PreviewCanvas(
    scene: DemoScene, backend: BackendType, modifier: Modifier = Modifier
) {
    Box(modifier = modifier) {
        Canvas(modifier = Modifier.fillMaxSize()) {
            drawRoundRect(
                brush = Brush.linearGradient(
                    listOf(
                        Color(0xFF0B132B), Color(0xFF1C2541), Color(0xFF3A506B)
                    )
                ), cornerRadius = CornerRadius(28.dp.toPx(), 28.dp.toPx())
            )

            when (scene) {
                DemoScene.CLEAR -> drawClearScene()
                DemoScene.SHAPES -> drawShapesScene()
                DemoScene.PATHS -> drawPathsScene()
                DemoScene.GRADIENTS -> drawGradientsScene()
                DemoScene.TEXT -> drawTextScene(backend)
                DemoScene.IMAGE -> drawImageScene()
                DemoScene.ARCS -> drawArcsScene()
                DemoScene.TRANSFORMS -> drawTransformsScene()
                DemoScene.STROKES -> drawStrokesScene()
                DemoScene.TILING -> drawTilingScene()
                DemoScene.CLIPS -> drawClipsScene()
                DemoScene.LAYERS -> drawLayersScene()
                DemoScene.TEXT_CLOUD -> drawTextCloudScene()
                DemoScene.STRESS_PATHS -> drawStressPathsScene()
                DemoScene.DASHBOARD -> drawDashboardScene()
                DemoScene.ICON_LIST -> drawIconListScene()
                DemoScene.COMPOSITE_STACK -> drawCompositeStackScene()
            }

            drawRoundRect(
                color = Color(0xCC0F172A),
                topLeft = Offset(12.dp.toPx(), 12.dp.toPx()),
                size = Size(116.dp.toPx(), 30.dp.toPx()),
                cornerRadius = CornerRadius(14.dp.toPx(), 14.dp.toPx())
            )
        }

        Text(
            text = backend.title,
            color = Color(0xFFF8FAFC),
            fontSize = 12.sp,
            modifier = Modifier
                .align(Alignment.TopStart)
                .padding(start = 24.dp, top = 20.dp)
        )

        Text(
            text = "Preview",
            color = Color(0xFFE2E8F0),
            fontSize = 12.sp,
            modifier = Modifier
                .align(Alignment.TopEnd)
                .padding(end = 20.dp, top = 20.dp)
        )

        if (scene == DemoScene.TEXT) {
            val backendLabel = when (backend) {
                BackendType.VULKAN -> "Vulkan"
                BackendType.GLES -> "OpenGL ES"
                BackendType.AUTO -> "Auto"
            }
            val surfaceLabel = when (backend) {
                BackendType.VULKAN -> "Swapchain"
                BackendType.GLES -> "Framebuffer"
                BackendType.AUTO -> "Auto"
            }
            val validationLabel = when (backend) {
                BackendType.VULKAN -> "Requested"
                BackendType.GLES, BackendType.AUTO -> "Off"
            }

            Text(
                text = "Skity",
                color = Color.White,
                fontSize = 28.sp,
                fontWeight = FontWeight.Bold,
                modifier = Modifier
                    .align(Alignment.TopStart)
                    .padding(start = 48.dp, top = 110.dp)
            )
            Text(
                text = "Backend rendering preview",
                color = Color(0xFF94A3B8),
                fontSize = 16.sp,
                modifier = Modifier
                    .align(Alignment.TopStart)
                    .padding(start = 48.dp, top = 156.dp)
            )
            Text(
                text = "Backend: $backendLabel",
                color = Color(0xFF38BDF8),
                fontSize = 20.sp,
                fontWeight = FontWeight.SemiBold,
                modifier = Modifier
                    .align(Alignment.TopStart)
                    .padding(start = 48.dp, top = 196.dp)
            )
            Text(
                text = "Surface: $surfaceLabel",
                color = Color(0xFFC7D2FE),
                fontSize = 14.sp,
                modifier = Modifier
                    .align(Alignment.TopStart)
                    .padding(start = 48.dp, top = 228.dp)
            )
            Text(
                text = "Validation: $validationLabel",
                color = Color(0xFFC7D2FE),
                fontSize = 14.sp,
                modifier = Modifier
                    .align(Alignment.TopStart)
                    .padding(start = 48.dp, top = 250.dp)
            )
        }
    }
}

private fun androidx.compose.ui.graphics.drawscope.DrawScope.drawClearScene() {
    drawCircle(
        Color(0xFFF25F5C),
        radius = 42.dp.toPx(),
        center = Offset(size.width * 0.34f, size.height * 0.46f)
    )
    drawRect(
        color = Color(0xFFE066),
        topLeft = Offset(size.width * 0.42f, size.height * 0.26f),
        size = Size(size.width * 0.32f, size.height * 0.34f),
        alpha = 0.86f
    )
    drawRoundRect(
        color = Color(0xFF70C1B3),
        topLeft = Offset(size.width * 0.22f, size.height * 0.58f),
        size = Size(size.width * 0.46f, size.height * 0.24f),
        cornerRadius = CornerRadius(20.dp.toPx(), 20.dp.toPx())
    )
}

private fun androidx.compose.ui.graphics.drawscope.DrawScope.drawShapesScene() {
    drawRoundRect(
        color = Color(0xFF5BC0BE),
        topLeft = Offset(size.width * 0.10f, size.height * 0.22f),
        size = Size(size.width * 0.30f, size.height * 0.30f),
        cornerRadius = CornerRadius(18.dp.toPx(), 18.dp.toPx())
    )
    drawCircle(
        Color(0xFFF25F5C),
        radius = 46.dp.toPx(),
        center = Offset(size.width * 0.70f, size.height * 0.36f)
    )
    drawLine(
        color = Color(0xFFFFE066),
        start = Offset(size.width * 0.16f, size.height * 0.72f),
        end = Offset(size.width * 0.80f, size.height * 0.72f),
        strokeWidth = 4.dp.toPx(),
        cap = StrokeCap.Round
    )
    drawRect(
        color = Color.White,
        topLeft = Offset(size.width * 0.48f, size.height * 0.56f),
        size = Size(size.width * 0.36f, size.height * 0.26f)
    )
}

private fun androidx.compose.ui.graphics.drawscope.DrawScope.drawPathsScene() {
    val path = Path().apply {
        moveTo(size.width * 0.14f, size.height * 0.70f)
        cubicTo(
            size.width * 0.20f,
            size.height * 0.18f,
            size.width * 0.44f,
            size.height * 0.96f,
            size.width * 0.52f,
            size.height * 0.34f
        )
        cubicTo(
            size.width * 0.62f,
            size.height * 0.08f,
            size.width * 0.78f,
            size.height * 0.28f,
            size.width * 0.88f,
            size.height * 0.18f
        )
    }
    drawPath(path = path, color = Color(0xFF3A86FF), alpha = 0.5f)
    drawPath(
        path = path,
        color = Color(0xFFFFBE0B),
        style = Stroke(width = 6.dp.toPx(), cap = StrokeCap.Round, join = StrokeJoin.Round)
    )
}

private fun androidx.compose.ui.graphics.drawscope.DrawScope.drawGradientsScene() {
    drawRoundRect(
        brush = Brush.linearGradient(listOf(Color(0xFFF94144), Color(0xFFF9C74F))),
        topLeft = Offset(size.width * 0.10f, size.height * 0.16f),
        size = Size(size.width * 0.78f, size.height * 0.28f),
        cornerRadius = CornerRadius(24.dp.toPx(), 24.dp.toPx())
    )
    drawCircle(
        brush = Brush.radialGradient(listOf(Color(0xFF90E0EF), Color(0xFF03045E))),
        radius = 70.dp.toPx(),
        center = Offset(size.width * 0.36f, size.height * 0.70f)
    )
    drawCircle(
        color = Color.White.copy(alpha = 0.28f),
        radius = 52.dp.toPx(),
        center = Offset(size.width * 0.72f, size.height * 0.68f)
    )
}

private fun androidx.compose.ui.graphics.drawscope.DrawScope.drawTextScene(backend: BackendType) {
    drawRoundRect(
        color = Color(0xFF0F172A),
        topLeft = Offset(size.width * 0.10f, size.height * 0.16f),
        size = Size(size.width * 0.80f, size.height * 0.68f),
        cornerRadius = CornerRadius(18.dp.toPx(), 18.dp.toPx())
    )
    drawCircle(Color(0xFF38BDF8), 4.dp.toPx(), Offset(size.width * 0.18f, size.height * 0.30f))
    drawCircle(Color(0xFFF59E0B), 4.dp.toPx(), Offset(size.width * 0.22f, size.height * 0.30f))
    drawCircle(Color(0xFFFB7185), 4.dp.toPx(), Offset(size.width * 0.26f, size.height * 0.30f))
}

private fun androidx.compose.ui.graphics.drawscope.DrawScope.drawImageScene() {
    val outer = Rect(
        left = size.width * 0.12f,
        top = size.height * 0.18f,
        right = size.width * 0.88f,
        bottom = size.height * 0.82f
    )
    drawRoundRect(
        color = Color(0xFFE2E8F0),
        topLeft = outer.topLeft,
        size = outer.size,
        cornerRadius = CornerRadius(20.dp.toPx(), 20.dp.toPx())
    )

    val cellWidth = outer.width / 6f
    val cellHeight = outer.height / 4f
    for (row in 0 until 4) {
        for (col in 0 until 6) {
            if ((row + col) % 2 == 0) {
                drawRect(
                    color = Color(0xFFCBD5E1),
                    topLeft = Offset(outer.left + col * cellWidth, outer.top + row * cellHeight),
                    size = Size(cellWidth, cellHeight)
                )
            }
        }
    }

    drawCircle(
        Color(0xFFFB7185),
        radius = 32.dp.toPx(),
        center = Offset(size.width * 0.40f, size.height * 0.50f),
        alpha = 0.88f
    )
    drawRoundRect(
        color = Color(0xFF2563EB),
        topLeft = Offset(size.width * 0.52f, size.height * 0.34f),
        size = Size(size.width * 0.24f, size.height * 0.34f),
        cornerRadius = CornerRadius(14.dp.toPx(), 14.dp.toPx())
    )
}

private fun androidx.compose.ui.graphics.drawscope.DrawScope.drawArcsScene() {
    drawArc(
        color = Color(0xFFFF7F50),
        startAngle = 20f,
        sweepAngle = 280f,
        useCenter = true,
        topLeft = Offset(size.width * 0.12f, size.height * 0.18f),
        size = Size(size.width * 0.34f, size.height * 0.38f),
        alpha = 0.84f
    )
    drawArc(
        color = Color(0xFF4DC3FF),
        startAngle = -40f,
        sweepAngle = 220f,
        useCenter = false,
        topLeft = Offset(size.width * 0.42f, size.height * 0.16f),
        size = Size(size.width * 0.42f, size.height * 0.36f),
        style = Stroke(width = 10.dp.toPx())
    )
    drawArc(
        color = Color(0xFFFFE66D),
        startAngle = 35f,
        sweepAngle = 250f,
        useCenter = false,
        topLeft = Offset(size.width * 0.48f, size.height * 0.56f),
        size = Size(size.width * 0.36f, size.height * 0.28f),
        style = Stroke(width = 6.dp.toPx())
    )
}

private fun androidx.compose.ui.graphics.drawscope.DrawScope.drawTransformsScene() {
    drawRoundRect(
        color = Color.White.copy(alpha = 0.25f),
        topLeft = Offset(size.width * 0.10f, size.height * 0.14f),
        size = Size(size.width * 0.80f, size.height * 0.72f),
        cornerRadius = CornerRadius(20.dp.toPx(), 20.dp.toPx()),
        style = Stroke(width = 2.dp.toPx())
    )
    for (index in 0 until 8) {
        rotate(index * 16f, pivot = Offset(size.width * 0.50f, size.height * 0.50f)) {
            val scale = 1f - index * 0.08f
            drawRoundRect(
                color = Color(
                    red = 0.32f + index * 0.06f,
                    green = 0.82f - index * 0.05f,
                    blue = 0.52f + index * 0.03f,
                    alpha = 0.24f + index * 0.06f
                ),
                topLeft = Offset(
                    size.width * 0.50f - size.width * 0.22f * scale,
                    size.height * 0.50f - size.height * 0.12f * scale
                ),
                size = Size(size.width * 0.44f * scale, size.height * 0.24f * scale),
                cornerRadius = CornerRadius(18.dp.toPx(), 18.dp.toPx())
            )
        }
    }
}

private fun androidx.compose.ui.graphics.drawscope.DrawScope.drawStrokesScene() {
    val ys = listOf(0.24f, 0.39f, 0.54f)
    val widths = listOf(8f, 12f, 16f)
    ys.forEachIndexed { index, y ->
        drawLine(
            color = Color(
                red = 0.34f + index * 0.18f,
                green = 0.84f,
                blue = 0.42f + index * 0.12f
            ),
            start = Offset(size.width * 0.14f, size.height * y),
            end = Offset(size.width * 0.44f, size.height * y),
            strokeWidth = widths[index].dp.toPx(),
            cap = when (index) {
                0 -> StrokeCap.Butt
                1 -> StrokeCap.Round
                else -> StrokeCap.Square
            }
        )

        val path = Path().apply {
            moveTo(size.width * 0.58f, size.height * (0.18f + index * 0.20f))
            lineTo(size.width * 0.72f, size.height * (0.08f + index * 0.20f))
            lineTo(size.width * 0.84f, size.height * (0.22f + index * 0.20f))
        }
        drawPath(
            path = path,
            color = Color(
                red = 0.54f + index * 0.12f,
                green = 0.78f - index * 0.08f,
                blue = 0.30f + index * 0.16f
            ),
            style = Stroke(
                width = widths[index].dp.toPx(),
                cap = StrokeCap.Round,
                join = when (index) {
                    0 -> StrokeJoin.Miter
                    1 -> StrokeJoin.Round
                    else -> StrokeJoin.Bevel
                }
            )
        )
    }
}

private fun androidx.compose.ui.graphics.drawscope.DrawScope.drawTilingScene() {
    val left = size.width * 0.10f
    val top = size.height * 0.16f
    val tileWidth = size.width * 0.17f
    val tileHeight = size.height * 0.16f
    for (row in 0 until 4) {
        for (col in 0 until 4) {
            val x = left + col * (tileWidth + size.width * 0.02f)
            val y = top + row * (tileHeight + size.height * 0.03f)
            drawRoundRect(
                color = Color(
                    red = 0.20f + row * 0.12f,
                    green = 0.36f + col * 0.10f,
                    blue = 0.62f + (row + col) * 0.04f
                ),
                topLeft = Offset(x, y),
                size = Size(tileWidth, tileHeight),
                cornerRadius = CornerRadius(14.dp.toPx(), 14.dp.toPx())
            )
            drawCircle(
                color = Color(0xFFFFF4D6),
                radius = 12.dp.toPx(),
                center = Offset(x + tileWidth * 0.30f, y + tileHeight * 0.34f),
                alpha = 0.8f
            )
            drawLine(
                color = Color(0xFF151F30),
                start = Offset(x + tileWidth * 0.18f, y + tileHeight * 0.72f),
                end = Offset(x + tileWidth * 0.82f, y + tileHeight * 0.72f),
                strokeWidth = 3.dp.toPx()
            )
        }
    }
}

private fun androidx.compose.ui.graphics.drawscope.DrawScope.drawClipsScene() {
    drawRoundRect(
        color = Color(0xFF13203A),
        topLeft = Offset(size.width * 0.08f, size.height * 0.12f),
        size = Size(size.width * 0.84f, size.height * 0.76f),
        cornerRadius = CornerRadius(24.dp.toPx(), 24.dp.toPx())
    )
    for (index in 0 until 8) {
        rotate(
            degrees = index * 7f,
            pivot = Offset(size.width * 0.32f, size.height * 0.46f)
        ) {
            drawRoundRect(
                color = Color(
                    red = 0.22f + index * 0.06f,
                    green = 0.46f + index * 0.03f,
                    blue = 0.80f - index * 0.05f,
                    alpha = 0.42f
                ),
                topLeft = Offset(size.width * 0.14f + index * 8.dp.toPx(), size.height * 0.20f),
                size = Size(size.width * 0.26f, size.height * 0.38f),
                cornerRadius = CornerRadius(18.dp.toPx(), 18.dp.toPx())
            )
        }
    }
    for (row in 0 until 6) {
        for (col in 0 until 4) {
            drawCircle(
                color = Color(
                    red = 0.30f + row * 0.07f,
                    green = 0.70f - col * 0.08f,
                    blue = 0.42f + col * 0.10f,
                    alpha = 0.72f
                ),
                radius = 18.dp.toPx() + (row % 2) * 3.dp.toPx(),
                center = Offset(
                    size.width * 0.60f + col * 38.dp.toPx(),
                    size.height * 0.24f + row * 34.dp.toPx()
                )
            )
        }
    }
}

private fun androidx.compose.ui.graphics.drawscope.DrawScope.drawLayersScene() {
    drawRoundRect(
        color = Color(0xFF10172F),
        topLeft = Offset(size.width * 0.08f, size.height * 0.12f),
        size = Size(size.width * 0.84f, size.height * 0.76f),
        cornerRadius = CornerRadius(22.dp.toPx(), 22.dp.toPx())
    )
    for (layer in 0 until 6) {
        drawCircle(
            color = Color(
                red = 1.0f,
                green = 0.35f + layer * 0.08f,
                blue = 0.46f + layer * 0.05f,
                alpha = 0.22f + layer * 0.08f
            ),
            radius = 26.dp.toPx() + layer * 5.dp.toPx(),
            center = Offset(
                size.width * 0.30f + layer * 22.dp.toPx(),
                size.height * 0.32f + layer * 18.dp.toPx()
            )
        )
        drawRoundRect(
            color = Color(
                red = 0.40f + layer * 0.06f,
                green = 0.45f + layer * 0.05f,
                blue = 0.88f - layer * 0.06f,
                alpha = 0.18f + layer * 0.06f
            ),
            topLeft = Offset(
                size.width * 0.18f + layer * 12.dp.toPx(),
                size.height * 0.24f + layer * 10.dp.toPx()
            ),
            size = Size(size.width * 0.48f, size.height * 0.42f),
            cornerRadius = CornerRadius(18.dp.toPx(), 18.dp.toPx())
        )
    }
}

private fun androidx.compose.ui.graphics.drawscope.DrawScope.drawTextCloudScene() {
    drawRoundRect(
        color = Color(0xFF0E1729),
        topLeft = Offset(size.width * 0.08f, size.height * 0.12f),
        size = Size(size.width * 0.84f, size.height * 0.76f),
        cornerRadius = CornerRadius(22.dp.toPx(), 22.dp.toPx())
    )
    for (row in 0 until 5) {
        for (col in 0 until 4) {
            drawCircle(
                color = Color(
                    red = 0.46f + row * 0.08f,
                    green = 0.70f - col * 0.08f,
                    blue = 0.92f - row * 0.06f,
                    alpha = 0.55f
                ),
                radius = 8.dp.toPx() + row * 2.dp.toPx(),
                center = Offset(
                    size.width * 0.18f + col * size.width * 0.18f,
                    size.height * 0.26f + row * size.height * 0.12f
                )
            )
        }
    }
}

private fun androidx.compose.ui.graphics.drawscope.DrawScope.drawStressPathsScene() {
    for (stripe in 0 until 12) {
        val path = Path().apply {
            val top = size.height * 0.12f + stripe * size.height * 0.05f
            moveTo(size.width * 0.08f, top + 10.dp.toPx())
            for (segment in 0 until 6) {
                val x0 = size.width * 0.08f + segment * size.width * 0.14f
                val x2 = x0 + size.width * 0.14f
                val y1 = top + if ((segment + stripe) % 2 == 0) -18.dp.toPx() else 22.dp.toPx()
                cubicTo(x0 + 18.dp.toPx(), y1, x0 + 42.dp.toPx(), top + 18.dp.toPx(), x2, top + 8.dp.toPx())
            }
        }
        drawPath(
            path = path,
            color = Color(
                red = 0.22f + stripe * 0.04f,
                green = 0.42f + stripe * 0.03f,
                blue = 0.86f - stripe * 0.04f,
                alpha = 0.32f
            )
        )
        drawPath(
            path = path,
            color = Color(
                red = 0.48f + stripe * 0.03f,
                green = 0.90f - stripe * 0.05f,
                blue = 0.54f + stripe * 0.02f
            ),
            style = Stroke(width = 2.dp.toPx() + (stripe % 3) * 1.dp.toPx())
        )
    }
}

private fun androidx.compose.ui.graphics.drawscope.DrawScope.drawDashboardScene() {
    drawRoundRect(
        color = Color(0xFF0D1528),
        topLeft = Offset(size.width * 0.08f, size.height * 0.12f),
        size = Size(size.width * 0.84f, size.height * 0.76f),
        cornerRadius = CornerRadius(24.dp.toPx(), 24.dp.toPx())
    )
    for (card in 0 until 3) {
        val left = size.width * 0.14f + card * size.width * 0.24f
        drawRoundRect(
            color = Color(
                red = 0.08f + card * 0.08f,
                green = 0.14f + card * 0.07f,
                blue = 0.22f + card * 0.10f
            ),
            topLeft = Offset(left, size.height * 0.20f),
            size = Size(size.width * 0.18f, size.height * 0.22f),
            cornerRadius = CornerRadius(18.dp.toPx(), 18.dp.toPx())
        )
        for (bar in 0 until 4) {
            drawRoundRect(
                color = Color(
                    red = 0.32f + card * 0.18f,
                    green = 0.90f,
                    blue = 0.74f - card * 0.10f
                ),
                topLeft = Offset(left + 14.dp.toPx() + bar * 18.dp.toPx(), size.height * 0.36f - bar * 10.dp.toPx()),
                size = Size(10.dp.toPx(), 18.dp.toPx() + bar * 6.dp.toPx()),
                cornerRadius = CornerRadius(6.dp.toPx(), 6.dp.toPx())
            )
        }
    }
    drawRoundRect(
        color = Color(0xFF14223D),
        topLeft = Offset(size.width * 0.14f, size.height * 0.50f),
        size = Size(size.width * 0.72f, size.height * 0.30f),
        cornerRadius = CornerRadius(20.dp.toPx(), 20.dp.toPx())
    )
}

private fun androidx.compose.ui.graphics.drawscope.DrawScope.drawIconListScene() {
    drawRoundRect(
        color = Color(0xFF10182E),
        topLeft = Offset(size.width * 0.10f, size.height * 0.12f),
        size = Size(size.width * 0.80f, size.height * 0.76f),
        cornerRadius = CornerRadius(22.dp.toPx(), 22.dp.toPx())
    )
    for (row in 0 until 5) {
        val top = size.height * 0.18f + row * size.height * 0.13f
        drawRoundRect(
            color = Color(
                red = 0.10f + row * 0.04f,
                green = 0.16f + row * 0.05f,
                blue = 0.24f + row * 0.05f
            ),
            topLeft = Offset(size.width * 0.14f, top),
            size = Size(size.width * 0.72f, size.height * 0.10f),
            cornerRadius = CornerRadius(16.dp.toPx(), 16.dp.toPx())
        )
        drawCircle(
            color = Color(
                red = 0.36f + row * 0.08f,
                green = 0.86f - row * 0.07f,
                blue = 0.96f
            ),
            radius = 14.dp.toPx(),
            center = Offset(size.width * 0.20f, top + size.height * 0.05f)
        )
        drawRoundRect(
            color = Color(0xFFF8FAFC),
            topLeft = Offset(size.width * 0.26f, top + 16.dp.toPx()),
            size = Size(size.width * 0.28f, 8.dp.toPx()),
            cornerRadius = CornerRadius(4.dp.toPx(), 4.dp.toPx()),
            alpha = 0.82f
        )
        drawRoundRect(
            color = Color(0xFFC7D2FE),
            topLeft = Offset(size.width * 0.26f, top + 32.dp.toPx()),
            size = Size(size.width * 0.46f, 6.dp.toPx()),
            cornerRadius = CornerRadius(3.dp.toPx(), 3.dp.toPx()),
            alpha = 0.66f
        )
    }
}

private fun androidx.compose.ui.graphics.drawscope.DrawScope.drawCompositeStackScene() {
    drawClipsScene()
    rotate(9f, pivot = Offset(size.width * 0.36f, size.height * 0.34f)) {
        val outline = Path().apply {
            moveTo(size.width * 0.18f, size.height * 0.24f)
            lineTo(size.width * 0.38f, size.height * 0.18f)
            lineTo(size.width * 0.50f, size.height * 0.34f)
            lineTo(size.width * 0.28f, size.height * 0.46f)
            close()
        }
        drawPath(
            path = outline,
            color = Color(0xFFFFC857),
            style = Stroke(width = 5.dp.toPx(), cap = StrokeCap.Round, join = StrokeJoin.Round)
        )
    }
    drawCircle(
        color = Color(0xFFFB7185),
        radius = 52.dp.toPx(),
        center = Offset(size.width * 0.62f, size.height * 0.66f),
        alpha = 0.45f
    )
    drawCircle(
        color = Color(0xFF38BDF8),
        radius = 52.dp.toPx(),
        center = Offset(size.width * 0.74f, size.height * 0.68f),
        alpha = 0.45f
    )
}

@Composable
fun SurfaceCard(
    modifier: Modifier = Modifier,
    contentPadding: androidx.compose.foundation.layout.PaddingValues = androidx.compose.foundation.layout.PaddingValues(8.dp),
    content: @Composable () -> Unit
) {
    androidx.compose.material3.Surface(
        modifier = modifier,
        shape = RoundedCornerShape(20.dp),
        color = MaterialTheme.colorScheme.surface,
        tonalElevation = 0.dp,
        shadowElevation = 0.dp,
        border = androidx.compose.foundation.BorderStroke(1.dp, MaterialTheme.colorScheme.outline)
    ) {
        Box(
            modifier = Modifier
                .background(MaterialTheme.colorScheme.surface)
                .padding(contentPadding)
        ) {
            content()
        }
    }
}
