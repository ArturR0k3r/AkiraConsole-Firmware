# Changelog — AkiraConsole Firmware

All notable product-level firmware changes. Platform (OS) changes are tracked in
[AkiraOS CHANGELOG](https://github.com/ArturR0k3r/AkiraOS/blob/main/CHANGELOG.md).

The format follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).
Versions follow [Semantic Versioning](https://semver.org/).

---

## [Unreleased]

## [v1.0.0] — 2026-04-05

### Added
- Initial AkiraConsole product firmware release
- Pins AkiraOS v1.4.10
- MCUboot bootloader with EC P-256 image signing
- GitHub Actions CI/CD: build → sign → GitHub Release on version tag push
- Pre-built signed `.bin` artifacts attached to every release

### Platform (AkiraOS v1.4.10)
- WASM runtime (WAMR AOT, ESP32-S3 Xtensa target)
- Console shell UI: home screen, settings, boot screen
- NES emulator: `display_raw_write` API, 64-step CPU batch, frameskip=0
- BLE HID, USB HID, WiFi, OTA, Web server
- PSRAM-aware memory allocator
- SRAM thread stack fix: 8 KB (was 16 KB) — resolves post-WiFi heap fragmentation
- Slot leak fix: runtime slot freed on thread stack alloc failure
