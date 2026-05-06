package org.lynxsdk.lynx.skity.dev

enum class DemoScene(val title: String, val description: String) {
    CLEAR("Clear + Overlay", "Basic color fill, alpha layering, and composition."),
    SHAPES("Shapes", "Rect, round rect, line, and circle coverage."),
    PATHS("Paths", "Stroke and fill quality for custom vector paths."),
    GRADIENTS("Gradients", "Linear and radial gradient rendering behavior."),
    TEXT("Text", "Text metrics, baseline alignment, and glyph raster output."),
    IMAGE("Image", "Bitmap sampling, scaling, and clipping behavior.")
}
