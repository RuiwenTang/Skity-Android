#include "common/demo_scene_renderer.hpp"

#include <cstdint>
#include <string>
#include <vector>

#include <skity/effect/shader.hpp>
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

void DrawArcsScene(Canvas* canvas, int width, int height) {
  Paint fill;
  fill.SetAntiAlias(true);
  fill.SetColor(Argb(0xD8, 0xFF, 0x7F, 0x50));
  canvas->DrawArc(MakeRect(width * 0.12f, height * 0.18f, width * 0.46f,
                           height * 0.56f),
                  20.f, 280.f, true, fill);

  fill.SetColor(Argb(0xCC, 0x4D, 0xC3, 0xFF));
  canvas->DrawArc(MakeRect(width * 0.42f, height * 0.16f, width * 0.84f,
                           height * 0.52f),
                  -40.f, 220.f, false, fill);

  Paint stroke;
  stroke.SetAntiAlias(true);
  stroke.SetStyle(Paint::kStroke_Style);
  stroke.SetStrokeWidth(6.f);
  stroke.SetStrokeColor(Argb(0xFF, 0xFF, 0xE6, 0x6D));
  canvas->DrawOval(
      MakeRect(width * 0.16f, height * 0.58f, width * 0.44f, height * 0.82f),
      stroke);
  stroke.SetStrokeColor(Argb(0xFF, 0x70, 0xC1, 0xB3));
  canvas->DrawArc(MakeRect(width * 0.48f, height * 0.56f, width * 0.84f,
                           height * 0.84f),
                  35.f, 250.f, false, stroke);
}

void DrawTransformsScene(Canvas* canvas, int width, int height) {
  Paint frame;
  frame.SetAntiAlias(true);
  frame.SetStyle(Paint::kStroke_Style);
  frame.SetStrokeWidth(2.f);
  frame.SetStrokeColor(Argb(0x66, 0xFF, 0xFF, 0xFF));
  canvas->DrawRoundRect(
      MakeRect(width * 0.10f, height * 0.14f, width * 0.90f, height * 0.86f),
      20.f, 20.f, frame);

  for (int index = 0; index < 8; ++index) {
    Paint fill;
    fill.SetAntiAlias(true);
    fill.SetColor(Argb(static_cast<uint8_t>(160 - index * 10),
                       static_cast<uint8_t>(80 + index * 18),
                       static_cast<uint8_t>(210 - index * 12),
                       static_cast<uint8_t>(120 + index * 8)));
    canvas->Save();
    canvas->Translate(width * 0.50f, height * 0.50f);
    canvas->Rotate(index * 16.f);
    const float scale = 1.f - index * 0.08f;
    canvas->Scale(scale, scale);
    canvas->DrawRoundRect(
        MakeRect(-width * 0.22f, -height * 0.12f, width * 0.22f,
                 height * 0.12f),
        18.f, 18.f, fill);
    canvas->Restore();
  }
}

void DrawStrokesScene(Canvas* canvas, int width, int height) {
  const Paint::Cap caps[] = {Paint::kButt_Cap, Paint::kRound_Cap,
                             Paint::kSquare_Cap};
  const Paint::Join joins[] = {Paint::kMiter_Join, Paint::kRound_Join,
                               Paint::kBevel_Join};

  for (int row = 0; row < 3; ++row) {
    Paint line;
    line.SetAntiAlias(true);
    line.SetStyle(Paint::kStroke_Style);
    line.SetStrokeWidth(8.f + row * 4.f);
    line.SetStrokeCap(caps[row]);
    line.SetStrokeColor(Argb(0xFF, static_cast<uint8_t>(90 + row * 50), 0xD8,
                             static_cast<uint8_t>(110 + row * 30)));
    canvas->DrawLine(width * 0.14f, height * (0.24f + row * 0.15f),
                     width * 0.44f, height * (0.24f + row * 0.15f), line);

    Paint path_paint = line;
    path_paint.SetStrokeJoin(joins[row]);
    Path corner;
    corner.MoveTo(width * 0.58f, height * (0.18f + row * 0.20f));
    corner.LineTo(width * 0.72f, height * (0.08f + row * 0.20f));
    corner.LineTo(width * 0.84f, height * (0.22f + row * 0.20f));
    canvas->DrawPath(corner, path_paint);
  }
}

void DrawTilingScene(Canvas* canvas, int width, int height) {
  const float left = width * 0.10f;
  const float top = height * 0.16f;
  const float tile_width = width * 0.17f;
  const float tile_height = height * 0.16f;

  for (int row = 0; row < 4; ++row) {
    for (int col = 0; col < 4; ++col) {
      const float x = left + col * (tile_width + width * 0.02f);
      const float y = top + row * (tile_height + height * 0.03f);

      Paint tile;
      tile.SetAntiAlias(true);
      tile.SetColor(Argb(0xFF, static_cast<uint8_t>(50 + row * 35),
                         static_cast<uint8_t>(90 + col * 28),
                         static_cast<uint8_t>(160 + (row + col) * 10)));
      canvas->DrawRoundRect(MakeRect(x, y, x + tile_width, y + tile_height),
                            14.f, 14.f, tile);

      Paint accent;
      accent.SetAntiAlias(true);
      accent.SetColor(Argb(0xCC, 0xFF, 0xF4, 0xD6));
      canvas->DrawCircle(x + tile_width * 0.30f, y + tile_height * 0.34f, 12.f,
                         accent);
      accent.SetColor(Argb(0xD0, 0x15, 0x1F, 0x30));
      canvas->DrawLine(x + tile_width * 0.18f, y + tile_height * 0.72f,
                       x + tile_width * 0.82f, y + tile_height * 0.72f,
                       accent);
    }
  }
}

void DrawClipsScene(Canvas* canvas, int width, int height) {
  Paint bg;
  bg.SetAntiAlias(true);
  bg.SetColor(Argb(0xFF, 0x13, 0x20, 0x3A));
  canvas->DrawRoundRect(
      MakeRect(width * 0.08f, height * 0.12f, width * 0.92f, height * 0.88f),
      24.f, 24.f, bg);

  canvas->Save();
  canvas->ClipRect(
      MakeRect(width * 0.14f, height * 0.20f, width * 0.54f, height * 0.76f));
  for (int index = 0; index < 8; ++index) {
    Paint band;
    band.SetAntiAlias(true);
    band.SetColor(Argb(static_cast<uint8_t>(180 - index * 12),
                       static_cast<uint8_t>(50 + index * 18),
                       static_cast<uint8_t>(120 + index * 10),
                       static_cast<uint8_t>(220 - index * 16)));
    canvas->Save();
    canvas->Translate(width * 0.20f + index * 14.f, height * 0.24f);
    canvas->Rotate(7.f * index);
    canvas->DrawRoundRect(MakeRect(0.f, 0.f, width * 0.30f, height * 0.42f),
                          18.f, 18.f, band);
    canvas->Restore();
  }
  canvas->Restore();

  canvas->Save();
  Path clip_path;
  clip_path.MoveTo(width * 0.60f, height * 0.18f);
  clip_path.CubicTo(width * 0.90f, height * 0.22f, width * 0.86f,
                    height * 0.76f, width * 0.62f, height * 0.82f);
  clip_path.CubicTo(width * 0.48f, height * 0.70f, width * 0.46f,
                    height * 0.30f, width * 0.60f, height * 0.18f);
  canvas->ClipPath(clip_path);
  for (int row = 0; row < 7; ++row) {
    for (int col = 0; col < 4; ++col) {
      Paint tile;
      tile.SetAntiAlias(true);
      tile.SetColor(Argb(0xCC, static_cast<uint8_t>(90 + row * 16),
                         static_cast<uint8_t>(160 - col * 20),
                         static_cast<uint8_t>(100 + col * 24)));
      canvas->DrawCircle(width * 0.58f + col * 42.f, height * 0.24f + row * 42.f,
                         22.f + (row % 2) * 4.f, tile);
    }
  }
  canvas->Restore();
}

void DrawLayersSceneWithOptions(Canvas* canvas, int width, int height,
                                int layer_count, bool use_save_layer) {
  Paint bg;
  bg.SetAntiAlias(true);
  bg.SetColor(Argb(0xFF, 0x10, 0x17, 0x2F));
  canvas->DrawRoundRect(
      MakeRect(width * 0.08f, height * 0.12f, width * 0.92f, height * 0.88f),
      22.f, 22.f, bg);

  for (int layer = 0; layer < layer_count; ++layer) {
    Paint layer_paint;
    layer_paint.SetAntiAlias(true);
    layer_paint.SetColor(Argb(static_cast<uint8_t>(120 + layer * 18), 0xFF,
                              static_cast<uint8_t>(90 + layer * 20),
                              static_cast<uint8_t>(120 + layer * 14)));
    const Rect bounds = MakeRect(width * 0.14f + layer * 12.f,
                                 height * 0.20f + layer * 10.f,
                                 width * 0.80f - layer * 10.f,
                                 height * 0.78f - layer * 8.f);
    if (use_save_layer) {
      canvas->SaveLayer(bounds, Paint{});
    }
    canvas->DrawCircle(width * 0.34f + layer * 22.f, height * 0.36f + layer * 18.f,
                       34.f + layer * 6.f, layer_paint);
    canvas->DrawRoundRect(bounds, 18.f, 18.f, layer_paint);
    if (use_save_layer) {
      canvas->Restore();
    }
  }
}

void DrawLayersScene(Canvas* canvas, int width, int height) {
  DrawLayersSceneWithOptions(canvas, width, height, 6, true);
}

void DrawLayersLiteScene(Canvas* canvas, int width, int height) {
  DrawLayersSceneWithOptions(canvas, width, height, 3, true);
}

void DrawLayersFlatScene(Canvas* canvas, int width, int height) {
  DrawLayersSceneWithOptions(canvas, width, height, 6, false);
}

void DrawTextCloudScene(Canvas* canvas, int width, int height) {
  Paint bg;
  bg.SetAntiAlias(true);
  bg.SetColor(Argb(0xFF, 0x0E, 0x17, 0x29));
  canvas->DrawRoundRect(
      MakeRect(width * 0.08f, height * 0.12f, width * 0.92f, height * 0.88f),
      22.f, 22.f, bg);

  for (int row = 0; row < 5; ++row) {
    for (int col = 0; col < 4; ++col) {
      Paint text;
      text.SetAntiAlias(true);
      text.SetTextSize(14.f + row * 4.f + (col % 2) * 2.f);
      text.SetColor(Argb(0xFF, static_cast<uint8_t>(120 + row * 20),
                         static_cast<uint8_t>(180 - col * 18),
                         static_cast<uint8_t>(220 - row * 14)));
      canvas->Save();
      canvas->Translate(width * 0.16f + col * width * 0.18f,
                        height * 0.26f + row * height * 0.12f);
      canvas->Rotate(static_cast<float>((col - row) * 6));
      canvas->DrawSimpleText2("Skity", 0.f, 0.f, text);
      canvas->Restore();
    }
  }
}

void DrawStressPathsScene(Canvas* canvas, int width, int height) {
  Paint bg;
  bg.SetAntiAlias(true);
  bg.SetColor(Argb(0xFF, 0x0A, 0x12, 0x24));
  canvas->DrawRect(MakeRect(0.f, 0.f, static_cast<float>(width),
                            static_cast<float>(height)),
                   bg);

  for (int stripe = 0; stripe < 14; ++stripe) {
    Path path;
    const float top = height * 0.10f + stripe * (height * 0.05f);
    path.MoveTo(width * 0.08f, top + 12.f);
    for (int segment = 0; segment < 6; ++segment) {
      const float x0 = width * 0.08f + segment * width * 0.14f;
      const float x1 = x0 + width * 0.07f;
      const float x2 = x0 + width * 0.14f;
      const float y1 = top + ((segment + stripe) % 2 == 0 ? -18.f : 24.f);
      path.CubicTo(x0 + 18.f, y1, x1, top + 22.f, x2, top + 10.f);
    }

    Paint fill;
    fill.SetAntiAlias(true);
    fill.SetColor(Argb(static_cast<uint8_t>(70 + stripe * 8),
                       static_cast<uint8_t>(60 + stripe * 12),
                       static_cast<uint8_t>(110 + stripe * 8),
                       static_cast<uint8_t>(220 - stripe * 6)));
    canvas->DrawPath(path, fill);

    Paint stroke;
    stroke.SetAntiAlias(true);
    stroke.SetStyle(Paint::kStroke_Style);
    stroke.SetStrokeWidth(2.5f + (stripe % 3));
    stroke.SetColor(Argb(0xFF, static_cast<uint8_t>(120 + stripe * 8),
                         static_cast<uint8_t>(220 - stripe * 10),
                         static_cast<uint8_t>(140 + stripe * 4)));
    canvas->DrawPath(path, stroke);
  }
}

void DrawDashboardScene(Canvas* canvas, int width, int height) {
  Paint bg;
  bg.SetAntiAlias(true);
  bg.SetColor(Argb(0xFF, 0x0D, 0x15, 0x28));
  canvas->DrawRoundRect(
      MakeRect(width * 0.08f, height * 0.12f, width * 0.92f, height * 0.88f),
      24.f, 24.f, bg);

  for (int card = 0; card < 3; ++card) {
    const float left = width * 0.14f + card * width * 0.24f;
    Paint card_paint;
    card_paint.SetAntiAlias(true);
    card_paint.SetColor(Argb(0xFF, static_cast<uint8_t>(22 + card * 12),
                             static_cast<uint8_t>(34 + card * 18),
                             static_cast<uint8_t>(58 + card * 24)));
    canvas->DrawRoundRect(
        MakeRect(left, height * 0.20f, left + width * 0.18f, height * 0.42f),
        18.f, 18.f, card_paint);

    Paint accent;
    accent.SetAntiAlias(true);
    accent.SetColor(Argb(0xFF, static_cast<uint8_t>(80 + card * 50), 0xE6,
                         static_cast<uint8_t>(180 - card * 30)));
    for (int bar = 0; bar < 4; ++bar) {
      canvas->DrawRoundRect(
          MakeRect(left + 14.f + bar * 18.f, height * 0.36f - bar * 10.f,
                   left + 24.f + bar * 18.f, height * 0.38f + bar * 8.f),
          6.f, 6.f, accent);
    }
  }

  Paint panel;
  panel.SetAntiAlias(true);
  panel.SetColor(Argb(0xFF, 0x14, 0x22, 0x3D));
  canvas->DrawRoundRect(
      MakeRect(width * 0.14f, height * 0.50f, width * 0.86f, height * 0.80f),
      20.f, 20.f, panel);
  for (int segment = 0; segment < 6; ++segment) {
    Paint line;
    line.SetAntiAlias(true);
    line.SetStyle(Paint::kStroke_Style);
    line.SetStrokeWidth(4.f);
    line.SetStrokeColor(Argb(0xFF, static_cast<uint8_t>(120 + segment * 18),
                             static_cast<uint8_t>(140 + segment * 10), 0xF2));
    canvas->DrawLine(width * 0.18f + segment * 24.f,
                     height * 0.72f - (segment % 2 == 0 ? 26.f : 8.f),
                     width * 0.28f + segment * 24.f,
                     height * 0.60f + (segment % 2 == 0 ? 8.f : 20.f), line);
  }
}

void DrawIconListScene(Canvas* canvas, int width, int height) {
  Paint bg;
  bg.SetAntiAlias(true);
  bg.SetColor(Argb(0xFF, 0x10, 0x18, 0x2E));
  canvas->DrawRoundRect(
      MakeRect(width * 0.10f, height * 0.12f, width * 0.90f, height * 0.88f),
      22.f, 22.f, bg);

  for (int row = 0; row < 5; ++row) {
    const float top = height * 0.18f + row * height * 0.13f;
    Paint row_paint;
    row_paint.SetAntiAlias(true);
    row_paint.SetColor(Argb(0xFF, static_cast<uint8_t>(24 + row * 10),
                            static_cast<uint8_t>(38 + row * 12),
                            static_cast<uint8_t>(58 + row * 14)));
    canvas->DrawRoundRect(
        MakeRect(width * 0.14f, top, width * 0.86f, top + height * 0.10f), 16.f,
        16.f, row_paint);

    Paint icon;
    icon.SetAntiAlias(true);
    icon.SetColor(Argb(0xFF, static_cast<uint8_t>(90 + row * 18),
                       static_cast<uint8_t>(200 - row * 16), 0xF4));
    canvas->DrawCircle(width * 0.20f, top + height * 0.05f, 14.f, icon);

    Paint line;
    line.SetAntiAlias(true);
    line.SetColor(Argb(0xCC, 0xF8, 0xFA, 0xFC));
    canvas->DrawRoundRect(
        MakeRect(width * 0.26f, top + 16.f, width * 0.54f, top + 24.f), 4.f, 4.f,
        line);
    line.SetColor(Argb(0x88, 0xC7, 0xD2, 0xFE));
    canvas->DrawRoundRect(
        MakeRect(width * 0.26f, top + 32.f, width * 0.72f, top + 38.f), 3.f, 3.f,
        line);
  }
}

void DrawCompositeStackScene(Canvas* canvas, int width, int height) {
  DrawClipsScene(canvas, width, height);

  canvas->Save();
  canvas->Translate(width * 0.10f, height * 0.10f);
  canvas->Rotate(9.f);
  Paint stroke;
  stroke.SetAntiAlias(true);
  stroke.SetStyle(Paint::kStroke_Style);
  stroke.SetStrokeWidth(5.f);
  stroke.SetStrokeJoin(Paint::kRound_Join);
  stroke.SetStrokeCap(Paint::kRound_Cap);
  stroke.SetStrokeColor(Argb(0xFF, 0xFF, 0xC8, 0x57));
  Path outline;
  outline.MoveTo(width * 0.18f, height * 0.24f);
  outline.LineTo(width * 0.38f, height * 0.18f);
  outline.LineTo(width * 0.50f, height * 0.34f);
  outline.LineTo(width * 0.28f, height * 0.46f);
  canvas->DrawPath(outline, stroke);
  canvas->Restore();

  canvas->SaveLayer(MakeRect(width * 0.46f, height * 0.48f, width * 0.88f,
                             height * 0.84f),
                    Paint{});
  Paint fill;
  fill.SetAntiAlias(true);
  fill.SetColor(Argb(0x88, 0xFB, 0x71, 0x85));
  canvas->DrawCircle(width * 0.62f, height * 0.66f, 52.f, fill);
  fill.SetColor(Argb(0x88, 0x38, 0xBD, 0xF8));
  canvas->DrawCircle(width * 0.74f, height * 0.68f, 52.f, fill);
  canvas->Restore();
}

struct AdvancedBlendEntry {
  BlendMode mode;
  const char* name;
};

// The 15 advanced blend modes that have a fixed-function hardware equivalent
// (see skity ToNativeBlendOp; kModulate has none and stays on the shader path),
// followed by kSrcOver as a standard alpha-compositing reference so the per-mode
// difference is easy to compare. Name strings match skity's blend_mode_name().
const std::vector<AdvancedBlendEntry>& AdvancedBlendEntries() {
  static const std::vector<AdvancedBlendEntry> entries = {
      {BlendMode::kScreen, "Screen"},
      {BlendMode::kOverlay, "Overlay"},
      {BlendMode::kDarken, "Darken"},
      {BlendMode::kLighten, "Lighten"},
      {BlendMode::kColorDodge, "ColorDodge"},
      {BlendMode::kColorBurn, "ColorBurn"},
      {BlendMode::kHardLight, "HardLight"},
      {BlendMode::kSoftLight, "SoftLight"},
      {BlendMode::kDifference, "Difference"},
      {BlendMode::kExclusion, "Exclusion"},
      {BlendMode::kMultiply, "Multiply"},
      {BlendMode::kHue, "Hue"},
      {BlendMode::kSaturation, "Saturation"},
      {BlendMode::kColor, "Color"},
      {BlendMode::kLuminosity, "Luminosity"},
      {BlendMode::kSrcOver, "SrcOver"},
  };
  return entries;
}

void DrawAdvancedBlendingScene(Canvas* canvas, int width, int height) {
  Paint title;
  title.SetAntiAlias(true);
  title.SetColor(Color_WHITE);
  title.SetTextSize(width * 0.045f);
  canvas->DrawSimpleText2("Advanced Blend Equations", width * 0.04f,
                          height * 0.065f, title);

  Paint subtitle;
  subtitle.SetAntiAlias(true);
  subtitle.SetColor(Argb(0xFF, 0x9A, 0xA6, 0xC2));
  subtitle.SetTextSize(width * 0.026f);
  canvas->DrawSimpleText2(
      "GL_KHR_blend_equation_advanced  ·  VK_EXT_blend_operation_advanced",
      width * 0.04f, height * 0.105f, subtitle);

  const auto& entries = AdvancedBlendEntries();
  constexpr int kCols = 4;
  constexpr int kRows = 4;
  const float grid_left = width * 0.03f;
  const float grid_right = width * 0.97f;
  const float grid_top = height * 0.14f;
  const float grid_bottom = height * 0.99f;
  const float cell_w = (grid_right - grid_left) / kCols;
  const float cell_h = (grid_bottom - grid_top) / kRows;
  const float gap = (cell_w < cell_h ? cell_w : cell_h) * 0.04f;

  // Destination band covers bright -> saturated -> dark so both the separable
  // equations (Overlay/Dodge/...) and the HSL equations (Hue/Color/...) produce
  // visibly different results across the tile.
  const Vec4 dst_colors[] = {
      Vec4{1.00f, 0.93f, 0.35f, 1.f},
      Vec4{0.92f, 0.22f, 0.30f, 1.f},
      Vec4{0.20f, 0.40f, 0.92f, 1.f},
      Vec4{0.05f, 0.05f, 0.10f, 1.f},
  };

  for (int index = 0; index < static_cast<int>(entries.size()); ++index) {
    const int col = index % kCols;
    const int row = index / kCols;
    const float cell_left = grid_left + col * cell_w;
    const float cell_top = grid_top + row * cell_h;
    const Rect tile =
        MakeRect(cell_left + gap, cell_top + gap, cell_left + cell_w - gap,
                 cell_top + cell_h - gap);
    const float tile_left = tile.Left();
    const float tile_top = tile.Top();
    const float tile_w = tile.Width();
    const float tile_h = tile.Height();

    // Destination: horizontal multi-stop gradient filling the tile.
    Point pts[2] = {
        Point{tile_left, tile_top, 0.f, 1.f},
        Point{tile_left + tile_w, tile_top, 0.f, 1.f},
    };
    Paint dst_paint;
    dst_paint.SetAntiAlias(true);
    dst_paint.SetBlendMode(BlendMode::kSrc);
    dst_paint.SetShader(Shader::MakeLinear(pts, dst_colors, nullptr, 4));
    canvas->DrawRect(tile, dst_paint);

    // Source: translucent teal rect covering the bottom-right ~70% of the tile,
    // blended with the current advanced equation. The top-left keeps the raw
    // destination so each equation reads clearly against the source color.
    const float src_dx = tile_w * 0.30f;
    const float src_dy = tile_h * 0.30f;
    Paint src_paint;
    src_paint.SetAntiAlias(true);
    src_paint.SetColor(Argb(0xD0, 0x14, 0xB8, 0xA6));
    src_paint.SetBlendMode(entries[index].mode);
    canvas->DrawRect(
        MakeRect(tile_left + src_dx, tile_top + src_dy, tile_left + tile_w,
                 tile_top + tile_h),
        src_paint);

    // Label: translucent dark chip + white text at the top-left corner.
    const float text_size = (cell_w < cell_h ? cell_w : cell_h) * 0.075f;
    const float pad = text_size * 0.5f;
    const float chip_w = tile_w * 0.86f;
    const float chip_h = text_size * 1.9f;
    Paint chip;
    chip.SetAntiAlias(true);
    chip.SetColor(Argb(0xB0, 0x0B, 0x13, 0x2B));
    chip.SetBlendMode(BlendMode::kSrcOver);
    canvas->DrawRoundRect(
        MakeRect(tile_left + pad, tile_top + pad, tile_left + pad + chip_w,
                 tile_top + pad + chip_h),
        chip_h * 0.3f, chip_h * 0.3f, chip);

    Paint label;
    label.SetAntiAlias(true);
    label.SetColor(Color_WHITE);
    label.SetBlendMode(BlendMode::kSrcOver);
    label.SetTextSize(text_size);
    canvas->DrawSimpleText2(entries[index].name, tile_left + pad * 1.8f,
                            tile_top + pad + chip_h * 0.7f, label);
  }
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
    case DemoScene::kArcs:
      DrawArcsScene(canvas, width, height);
      break;
    case DemoScene::kTransforms:
      DrawTransformsScene(canvas, width, height);
      break;
    case DemoScene::kStrokes:
      DrawStrokesScene(canvas, width, height);
      break;
    case DemoScene::kTiling:
      DrawTilingScene(canvas, width, height);
      break;
    case DemoScene::kClips:
      DrawClipsScene(canvas, width, height);
      break;
    case DemoScene::kLayers:
      DrawLayersScene(canvas, width, height);
      break;
    case DemoScene::kLayersLite:
      DrawLayersLiteScene(canvas, width, height);
      break;
    case DemoScene::kLayersFlat:
      DrawLayersFlatScene(canvas, width, height);
      break;
    case DemoScene::kTextCloud:
      DrawTextCloudScene(canvas, width, height);
      break;
    case DemoScene::kStressPaths:
      DrawStressPathsScene(canvas, width, height);
      break;
    case DemoScene::kDashboard:
      DrawDashboardScene(canvas, width, height);
      break;
    case DemoScene::kIconList:
      DrawIconListScene(canvas, width, height);
      break;
    case DemoScene::kCompositeStack:
      DrawCompositeStackScene(canvas, width, height);
      break;
    case DemoScene::kAdvancedBlending:
      DrawAdvancedBlendingScene(canvas, width, height);
      break;
  }
}

}  // namespace skity::demo
