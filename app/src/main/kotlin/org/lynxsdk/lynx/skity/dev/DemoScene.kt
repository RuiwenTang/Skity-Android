package org.lynxsdk.lynx.skity.dev

enum class DemoScene(val title: String, val description: String) {
    CLEAR("Clear + Overlay", "Basic color fill, alpha layering, and composition."),
    SHAPES("Shapes", "Rect, round rect, line, and circle coverage."),
    PATHS("Paths", "Stroke and fill quality for custom vector paths."),
    GRADIENTS("Gradients", "Linear and radial gradient rendering behavior."),
    TEXT("Text", "Text metrics, baseline alignment, and glyph raster output."),
    IMAGE("Image", "Bitmap sampling, scaling, and clipping behavior."),
    ARCS("Arcs + Ovals", "Arc sweep, wedge fill, and oval edge behavior."),
    TRANSFORMS("Transforms", "Translate, rotate, and scale stability across repeated geometry."),
    STROKES("Stroke Detail", "Stroke width, cap, join, and miter rendering detail."),
    TILING("Tiling Grid", "Dense repeated primitives and layout consistency in tiled content."),
    CLIPS("Clip Stack", "Nested clipping, transforms, and constrained drawing regions."),
    LAYERS("Layer Blend", "Alpha overlap, compositing, and save-layer style behavior."),
    TEXT_CLOUD("Text Cloud", "Dense glyph placement, varied sizes, and repeated text draws."),
    STRESS_PATHS("Stress Paths", "Many repeated path fills and strokes for heavier vector load."),
    DASHBOARD("Dashboard Cards", "Card-style panels, mini charts, and mixed typography blocks."),
    ICON_LIST("Icon List", "Repeated icon-and-label rows for UI list composition checks."),
    COMPOSITE_STACK("Composite Stack", "Clip, stroke, transform, and layered overlap in one combined scene.")
}
