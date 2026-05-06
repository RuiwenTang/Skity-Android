# AGENTS.md

## Project Purpose

This repository is an Android-focused development and validation workspace for skity.

- The root project provides the Android Studio, Gradle, and app host environment.
- `externals/skity` contains the core rendering engine source code.
- `externals/skity/platform/android` integrates skity as the `:skity` Android library module.
- `app` is the host app used for integration, rendering validation, debugging, and performance testing on Android.

The main goals of this project are:

- Build skity reliably on Android.
- Validate rendering correctness on Android devices.
- Test and compare functionality and performance across both `GLES` and `Vulkan` backends.
- Provide a clean foundation for demos, regression checks, benchmarks, and issue investigation.

## Documentation Language

- `AGENTS.md` must be written in English.
- Unless a task explicitly requires another language, new documentation and documentation updates in this repository should also be written in English.
- This applies to files such as `README`, design notes, benchmark notes, troubleshooting guides, integration docs, and test instructions.

## Repository Structure

- `app`
  - Android host application.
  - Intended for skity integration, test screens, backend switching entry points, and performance probes.
- `externals/skity`
  - Main skity source tree.
  - Includes rendering, GPU backends, text, IO, codec, and test-related code.
- `externals/skity/platform/android`
  - Android library packaging project.
  - Included as the `:skity` module from the root `settings.gradle.kts`.
- `tools`
  - Good location for build helpers, benchmark scripts, and validation utilities.

## Known Build Setup

- Root project modules:
  - `:app`
  - `:skity` -> `externals/skity/platform/android`
- `:skity` currently builds native code through CMake.
- The Android Gradle configuration for `:skity` currently enables:
  - `SKITY_VK_BACKEND=ON`
  - `SKITY_CODEC_MODULE=ON`
  - `SKITY_IO_MODULE=ON`
  - `SKITY_LOG=ON`
- The `Vulkan` backend is explicitly enabled in the Android packaging project.
- `GLES` support should be validated based on the actual skity backend implementation and runtime behavior in this repository.

## Collaboration Rules

- Treat this repository primarily as an Android validation host plus a skity integration layer, not as a full product app.
- Changes related to the Android host experience should usually go into `app`.
- Changes related to Android packaging, JNI, CMake, AAR, or Prefab integration should usually go into `externals/skity/platform/android`.
- Changes related to rendering implementation, GPU backend behavior, or low-level performance should usually go into `externals/skity`.
- Do not upgrade AGP, Gradle, NDK, `compileSdk`, or `targetSdk` unless the task clearly requires it.
- Do not casually modify third-party sources or broadly reorganize `externals/skity/third_party` unless that is the task.

## Recommended Workflow

When handling tasks in this repository, use this default order:

1. Identify the layer the task belongs to:
   - Android host layer
   - Android packaging / JNI / CMake integration layer
   - skity engine layer
   - GPU backend layer (`GLES` / `Vulkan`)
2. Prefer the smallest change that makes the issue reproducible, testable, and easy to verify.
3. For rendering-related work, consider all of the following:
   - Functional correctness
   - Cross-backend consistency
   - Performance regressions
   - Differences between physical devices and emulators
4. After changes are complete, provide a concrete validation path.

## Build And Validation

Prefer running commands from the repository root:

```bash
./gradlew :app:assembleDebug
./gradlew :skity:assembleDebug
./gradlew :app:testDebugUnitTest
./gradlew :app:connectedDebugAndroidTest
```

If the task only touches Android library packaging for skity, validate with:

```bash
./gradlew :skity:assembleDebug
./gradlew :skity:assembleRelease
```

## GLES / Vulkan Expectations

Any rendering-related change should, by default, be evaluated with both backends in mind.

Suggested minimum checks:

- Both `GLES` and `Vulkan` can initialize successfully.
- The same input produces consistent rendering results across both backends.
- No black screen, flicker, crash, leak, or obvious device-specific issue appears.
- First-frame time, steady-state rendering cost, and memory usage do not regress noticeably.
- Failures are checked for device vendor, Android API level, and ABI specificity.

If a new demo screen, test page, or benchmark is added, prefer including:

- A backend switch entry point
- A scene switch entry point
- Basic timing or FPS visibility
- Log tags and reproducible steps

## Code Style And Change Preferences

- Keep changes small and focused. Avoid mixing host app work, build system work, and rendering logic changes in one patch unless necessary.
- For experimental code, optimize first for readability and removability.
- Any platform or driver workaround should include a short comment covering:
  - Trigger condition
  - Affected scope
  - Why the workaround is needed
- For performance changes, record at least the test scenario and device conditions behind the conclusion.

## Common Change Locations

- Add Android validation screens: `app`
- Add JNI / CMake / AAR / Prefab configuration: `externals/skity/platform/android`
- Fix skity rendering bugs: `externals/skity`
- Add automation or helper scripts: `tools`

## Agent Guidance

When an agent works in this repository, prefer the following approach:

- Start from the root `settings.gradle.kts` and the target module `build.gradle(.kts)` files to confirm the change boundary.
- If the task involves native build behavior, inspect these files early:
  - `externals/skity/platform/android/build.gradle`
  - `externals/skity/CMakeLists.txt`
  - `externals/skity/src/CMakeLists.txt`
- If the task involves backend behavior, search for keywords such as `vk`, `vulkan`, `gl`, `gles`, `gpu`, `surface`, and `render`.
- If the task involves performance, preserve or create a reproducible benchmark path instead of leaving only a conclusion.
- If there is no solid demo or test entry yet, it is acceptable to add a minimal validation path inside `app`.

## Do Not Assume Yet

- Do not assume `app` already contains a complete rendering demo flow.
- Do not assume there is already a unified UI for switching between `GLES` and `Vulkan`.
- Do not assume all ABIs, Android versions, GPU vendors, or drivers have been validated.
- Do not assume upstream changes in `externals/skity` are automatically safe for the Android host project.

## Maintenance Notes

Update this file when the repository later gains:

- Concrete app demo entry points
- Backend switching instructions
- Benchmark commands
- Device test matrix
- Known compatibility issues
- Recommended development environment versions
