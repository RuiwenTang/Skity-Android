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
                DemoScene.TEXT -> drawTextScene()
                DemoScene.IMAGE -> drawImageScene()
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

private fun androidx.compose.ui.graphics.drawscope.DrawScope.drawTextScene() {
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

@Composable
fun SurfaceCard(
    modifier: Modifier = Modifier, content: @Composable () -> Unit
) {
    androidx.compose.material3.Surface(
        modifier = modifier,
        shape = RoundedCornerShape(24.dp),
        color = MaterialTheme.colorScheme.surface,
        tonalElevation = 0.dp,
        shadowElevation = 0.dp,
        border = androidx.compose.foundation.BorderStroke(1.dp, MaterialTheme.colorScheme.outline)
    ) {
        Box(
            modifier = Modifier
                .background(MaterialTheme.colorScheme.surface)
                .padding(16.dp)
        ) {
            content()
        }
    }
}
