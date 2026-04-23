# HiStreamer Media Foundation - Agent Guide

## Project Overview
HiStreamer is a lightweight media engine for OpenHarmony providing pipeline-based media processing for player/recorder scenarios. It connects to `player_framework` (standard devices) or `media_lite` (mini/small devices).

## Build System
- **GN-based build** (OpenHarmony standard)
- Optional CMake support for cross-platform development outside OHOS
- Platform types: `mini`, `small`, `standard` (auto-detected in `config.gni`)

## Key Commands
```bash
# Build main targets
./build.sh --product-name <product> --build-target histreamer
./build.sh --product-name <product> --build-target media_foundation

# Build and run tests
./build.sh --product-name <product> --build-target histreamer_test
./build.sh --product-name <product> --build-target media_foundation_unit_test
```

## Architecture
Three-layer architecture:
1. **Scene layer** (`engine/scene/`): HiPlayer, HiRecorder
2. **Pipeline layer** (`engine/pipeline/`): Filter nodes (codec, demux, muxer, sink, source)
3. **Plugin layer** (`engine/plugin/`): Extensible plugins (FFmpeg adapter, HDI adapter, sources, sinks)

## Key Directories
- `engine/` - Core engine (pipeline, plugins, scene implementations)
- `src/` - Public API implementation (buffers, meta, plugins, OSAL)
- `interface/` - Public headers (`inner_api/` for internal, `kits/` for CAPI)
- `services/media_monitor/` - Media monitoring service
- `test/` - Unit tests and scene tests
- `tests/` - Additional unit tests (avbuffer, format, meta, etc.)

## Feature Flags (`config.gni`)
Features are controlled by `media_foundation_enable_*` flags. Key features:
- `media_foundation_enable_plugin_ffmpeg_adapter` - FFmpeg support
- `media_foundation_enable_recorder` - Recording support
- `media_foundation_enable_video` - Video support
- Platform-specific defaults set automatically based on `hst_is_mini_sys`, `hst_is_small_sys`, `hst_is_standard_sys`

## Important Patterns
- Plugins are dynamically loaded from `/system/lib64/media/media_plugins/` (or `/system/lib/media/media_plugins/`)
- Plugin file suffix: `.z.so`
- Code uses `-fno-rtti` and `-fexceptions`
- C++17 standard

## Testing
- Unit tests use `ohos_unittest` with gtest
- Test targets defined in `test/BUILD.gn` and `tests/BUILD.gn`
- Most tests only run on standard system (`hst_is_standard_sys`)

## Dependencies
Key external dependencies: ffmpeg, hilog, hitrace, audio_framework, graphic_surface, ipc, samgr, c_utils, bounds_checking_function