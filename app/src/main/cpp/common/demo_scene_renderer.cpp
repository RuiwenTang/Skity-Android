#include "common/demo_scene_renderer.hpp"

#include <cstdint>

#include <skity/graphic/color.hpp>
#include <skity/graphic/paint.hpp>
#include <skity/graphic/path.hpp>
#include <skity/render/canvas.hpp>

namespace skity::demo {
namespace {

Color Argb(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
  return ColorSetARGB(a, r, g, b);
}

Rect MakeRect(float left, float top, float right, float bottom) {
  return Rect(left, top, right, bottom);
}

const char* BackendTitle(DemoBackend backend) {
  switch (backend) {
    case DemoBackend::kVulkan:
      return "Vulkan";
    case DemoBackend::kGles:
      return "OpenGL ES";
    case DemoBackend::kAuto:
    default:
      return "Auto";
  }
}

const char* SurfaceTitle(DemoBackend backend) {
  switch (backend) {
    case DemoBackend::kVulkan:
      return "Swapchain";
    case DemoBackend::kGles:
      return "Framebuffer";
    case DemoBackend::kAuto:
    default:
      return "Auto";
  }
}

const char* ValidationTitle(bool validation_enabled) {
  return validation_enabled ? "On" : "Off";
}

void DrawClearScene(Canvas* canvas, int width, int height) {
  Paint paint;
  paint.SetAntiAlias(true);
  paint.SetColor(Argb(0xFF, 0xF2, 0x5F, 0x5C));
  canvas->DrawCircle(width * 0.34f, height * 0.46f, 42.f, paint);

  paint.SetColor(Argb(0xDC, 0xFF, 0xE0, 0x66));
  canvas->DrawRect(
      MakeRect(width * 0.42f, height * 0.26f, width * 0.74f, height * 0.60f),
      paint);

  paint.SetColor(Argb(0xFF, 0x70, 0xC1, 0xB3));
  canvas->DrawRoundRect(
      MakeRect(width * 0.22f, height * 0.58f, width * 0.68f, height * 0.82f),
      20.f, 20.f, paint);
}

void DrawShapesScene(Canvas* canvas, int width, int height) {
  Paint fill;
  fill.SetAntiAlias(true);
  fill.SetColor(Argb(0xFF, 0x5B, 0xC0, 0xBE));
  canvas->DrawRoundRect(
      MakeRect(width * 0.10f, height * 0.22f, width * 0.40f, height * 0.52f),
      18.f, 18.f, fill);

  fill.SetColor(Argb(0xFF, 0xF2, 0x5F, 0x5C));
  canvas->DrawCircle(width * 0.70f, height * 0.36f, 46.f, fill);

  Paint stroke;
  stroke.SetAntiAlias(true);
  stroke.SetStyle(Paint::kStroke_Style);
  stroke.SetStrokeWidth(4.f);
  stroke.SetColor(Argb(0xFF, 0xFF, 0xE0, 0x66));
  canvas->DrawLine(width * 0.16f, height * 0.72f, width * 0.80f,
                   height * 0.72f, stroke);

  fill.SetColor(Color_WHITE);
  canvas->DrawRect(
      MakeRect(width * 0.48f, height * 0.56f, width * 0.84f, height * 0.82f),
      fill);
}

void DrawPathsScene(Canvas* canvas, int width, int height) {
  Path path;
  path.MoveTo(width * 0.14f, height * 0.70f);
  path.CubicTo(width * 0.20f, height * 0.18f, width * 0.44f, height * 0.96f,
               width * 0.52f, height * 0.34f);
  path.CubicTo(width * 0.62f, height * 0.08f, width * 0.78f, height * 0.28f,
               width * 0.88f, height * 0.18f);

  Paint fill;
  fill.SetAntiAlias(true);
  fill.SetColor(Argb(0x90, 0x3A, 0x86, 0xFF));
  canvas->DrawPath(path, fill);

  Paint stroke;
  stroke.SetAntiAlias(true);
  stroke.SetStyle(Paint::kStroke_Style);
  stroke.SetStrokeWidth(6.f);
  stroke.SetColor(Argb(0xFF, 0xFF, 0xBE, 0x0B));
  canvas->DrawPath(path, stroke);
}

void DrawGradientsScene(Canvas* canvas, int width, int height) {
  Paint warm;
  warm.SetAntiAlias(true);
  warm.SetColor(Argb(0xFF, 0xF9, 0x41, 0x44));
  canvas->DrawRoundRect(
      MakeRect(width * 0.10f, height * 0.16f, width * 0.88f, height * 0.44f),
      24.f, 24.f, warm);

  warm.SetColor(Argb(0xB8, 0xF9, 0xC7, 0x4F));
  canvas->DrawCircle(width * 0.62f, height * 0.30f, 52.f, warm);

  Paint cool;
  cool.SetAntiAlias(true);
  cool.SetColor(Argb(0xFF, 0x90, 0xE0, 0xEF));
  canvas->DrawCircle(width * 0.36f, height * 0.70f, 70.f, cool);

  cool.SetColor(Argb(0x60, 0x03, 0x04, 0x5E));
  canvas->DrawCircle(width * 0.72f, height * 0.68f, 52.f, cool);
}

void DrawTextScene(Canvas* canvas, DemoBackend backend, bool validation_enabled,
                   int width, int height) {
  Paint panel;
  panel.SetAntiAlias(true);
  panel.SetColor(Argb(0xFF, 0x0F, 0x17, 0x2A));
  canvas->DrawRoundRect(
      MakeRect(width * 0.10f, height * 0.16f, width * 0.90f, height * 0.84f),
      18.f, 18.f, panel);

  Paint title;
  title.SetAntiAlias(true);
  title.SetColor(Color_WHITE);
  title.SetTextSize(28.f);
  canvas->DrawSimpleText2("Skity", width * 0.16f, height * 0.42f, title);

  Paint body;
  body.SetAntiAlias(true);
  body.SetColor(Argb(0xFF, 0x94, 0xA3, 0xB8));
  body.SetTextSize(16.f);
  canvas->DrawSimpleText2("Backend rendering preview", width * 0.16f,
                          height * 0.56f, body);

  Paint badge;
  badge.SetAntiAlias(true);
  badge.SetColor(Argb(0xFF, 0x38, 0xBD, 0xF8));
  badge.SetTextSize(20.f);
  canvas->DrawSimpleText2(
      (std::string("Backend: ") + BackendTitle(backend)).c_str(),
      width * 0.16f,
                          height * 0.70f, badge);

  Paint meta;
  meta.SetAntiAlias(true);
  meta.SetColor(Argb(0xFF, 0xC7, 0xD2, 0xFE));
  meta.SetTextSize(14.f);
  canvas->DrawSimpleText2(
      (std::string("Surface: ") + SurfaceTitle(backend)).c_str(),
      width * 0.16f, height * 0.80f, meta);
  canvas->DrawSimpleText2(
      (std::string("Validation: ") + ValidationTitle(validation_enabled))
          .c_str(),
      width * 0.16f, height * 0.88f, meta);
}

void DrawImageScene(Canvas* canvas, int width, int height) {
  Paint base;
  base.SetAntiAlias(true);
  base.SetColor(Argb(0xFF, 0xE2, 0xE8, 0xF0));
  canvas->DrawRoundRect(
      MakeRect(width * 0.12f, height * 0.18f, width * 0.88f, height * 0.82f),
      20.f, 20.f, base);

  Paint cell;
  cell.SetColor(Argb(0xFF, 0xCB, 0xD5, 0xE1));
  const float left = width * 0.12f;
  const float top = height * 0.18f;
  const float panel_width = width * 0.76f;
  const float panel_height = height * 0.64f;
  const float cell_width = panel_width / 6.f;
  const float cell_height = panel_height / 4.f;
  for (int row = 0; row < 4; ++row) {
    for (int col = 0; col < 6; ++col) {
      if ((row + col) % 2 == 0) {
        canvas->DrawRect(
            MakeRect(left + col * cell_width, top + row * cell_height,
                     left + (col + 1) * cell_width,
                     top + (row + 1) * cell_height),
            cell);
      }
    }
  }

  Paint accent;
  accent.SetAntiAlias(true);
  accent.SetColor(Argb(0xDD, 0xFB, 0x71, 0x85));
  canvas->DrawCircle(width * 0.40f, height * 0.50f, 32.f, accent);

  accent.SetColor(Argb(0xFF, 0x25, 0x63, 0xEB));
  canvas->DrawRoundRect(
      MakeRect(width * 0.52f, height * 0.34f, width * 0.76f, height * 0.68f),
      14.f, 14.f, accent);
}

}  // namespace

void DrawDemoScene(Canvas* canvas, DemoScene scene, DemoBackend backend,
                   bool validation_enabled, int width, int height) {
  canvas->Clear(Argb(0xFF, 0x0B, 0x13, 0x2B));

  switch (scene) {
    case DemoScene::kClear:
      DrawClearScene(canvas, width, height);
      break;
    case DemoScene::kShapes:
      DrawShapesScene(canvas, width, height);
      break;
    case DemoScene::kPaths:
      DrawPathsScene(canvas, width, height);
      break;
    case DemoScene::kGradients:
      DrawGradientsScene(canvas, width, height);
      break;
    case DemoScene::kText:
      DrawTextScene(canvas, backend, validation_enabled, width, height);
      break;
    case DemoScene::kImage:
      DrawImageScene(canvas, width, height);
      break;
  }
}

}  // namespace skity::demo
