# Skity Android Workspace

This repository is an Android-focused development and validation workspace for
[skity](https://github.com/lynx-family/skity).

It is used to build skity on Android, validate rendering behavior on real
devices, and compare `GLES` and `Vulkan` backends in a lightweight host app.

## What This Repository Contains

- `app`
  - Android host app used for rendering demos, backend switching, debugging,
    and performance checks.
  - Built with Kotlin and Compose.
- `externals/skity`
  - Main skity engine source tree.
- `externals/skity/platform/android`
  - Android library packaging project exposed as the `:skity` module.

## Project Goals

- Build skity reliably on Android.
- Verify rendering correctness across Android devices and ABIs.
- Compare `GLES` and `Vulkan` behavior and performance.
- Provide a practical place for demos, driver workarounds, debugging, and
  regression validation.

## Modules

The root Gradle project currently includes:

- `:app`
- `:skity` -> `externals/skity/platform/android`

`skity` is built through CMake and packaged as an Android library module.

## Current Android Setup

`externals/skity/platform/android/build.gradle` currently enables:

- `SKITY_VK_BACKEND=ON`
- `SKITY_CODEC_MODULE=ON`
- `SKITY_IO_MODULE=ON`
- `SKITY_LOG=ON`

The host app is configured with:

- `minSdk = 29`
- `targetSdk = 36`
- Compose UI
- native integration through `app/src/main/cpp`

## App Capabilities

The host app is intended for validation rather than product UI.

Current app features include:

- `Scene Gallery`
  - Multiple rendering examples for basic primitives, compositing, text, and
    heavier stress scenes.
- `Backend Compare`
  - Side-by-side `GLES` and `Vulkan` preview surfaces for quick visual checks.
- `Capability Info`
  - Runtime information for backend availability and device setup.
- `Performance Test`
  - Lightweight benchmark entry for repeatable workload-based comparisons.
- Runtime debug controls
  - Vulkan validation toggle in debug builds.
  - MSAA toggle for supported preview and benchmark flows.

## Build

Run habitat script to fetch skity and other dependencies:

```bash
./tools/hab sync # windows ./tools/hab.ps1 sync
```

Run commands from the repository root:

```bash
./gradlew :app:assembleDebug
./gradlew :skity:assembleDebug
./gradlew :skity:assembleRelease
```

Useful additional commands:

```bash
./gradlew :app:testDebugUnitTest
./gradlew :app:connectedDebugAndroidTest
```

## Notes For Vulkan Validation

- Debug builds of `app` allow Vulkan validation to be enabled at runtime.
- Release builds ignore the validation toggle.
- Validation layers can be packaged through `app/src/main/jniLibs` when needed
  for device-side debugging.

## Recommended Usage

This repository works best as an Android validation lab for skity changes:

- make rendering or backend changes in `externals/skity`
- make Android integration changes in `externals/skity/platform/android`
- make demo, debug, benchmark, and validation UI changes in `app`

For rendering-related work, prefer checking both `GLES` and `Vulkan` on at
least one physical Android device.

## Documentation

Project documentation in this repository should be written in English unless a
task explicitly requires another language.
