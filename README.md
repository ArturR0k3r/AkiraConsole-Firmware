# AkiraConsole Firmware

Production firmware for the **AkiraConsole** — an ESP32-S3 N16R8 handheld gaming/dev console running [AkiraOS](https://github.com/ArturR0k3r/AkiraOS).

[![Firmware Release](https://github.com/ArturR0k3r/AkiraConsole-Firmware/actions/workflows/release.yml/badge.svg)](https://github.com/ArturR0k3r/AkiraConsole-Firmware/actions/workflows/release.yml)

---

## Hardware

| Component | Spec |
|-----------|------|
| SoC | ESP32-S3 N16R8 (dual Xtensa LX7, 240 MHz) |
| Flash | 16 MB |
| PSRAM | 8 MB OPI |
| Display | 320×240 ST7789V IPS (SPI) |
| Input | 8× GPIO buttons |
| Connectivity | WiFi 802.11 b/g/n, BLE 5.0 |
| Storage | MicroSD (SPI), Internal LittleFS |

---

## Repository Topology (West T2)

```
AkiraOS (platform)              github.com/ArturR0k3r/AkiraOS
     ↑ west dependency
AkiraConsole-Firmware (product) github.com/ArturR0k3r/AkiraConsole-Firmware  ← you are here
```

This repo is a **west application** that pins a specific `AkiraOS` tag and adds AkiraConsole-specific board definitions, configuration, and CI/CD.  
AkiraOS remains a clean platform — no board-specific code pollutes it.

### Directory layout

```
AkiraConsole-Firmware/
├── west.yml                      # Pins AkiraOS @ tag + imports Zephyr
├── CMakeLists.txt                # Product CMake entry-point
├── prj.conf                      # Product-level Kconfig overrides
├── boards/
│   ├── akiraconsole/             # Board definition (DTS, YAML, board.cmake)
│   ├── akiraconsole_esp32s3_procpu.conf     # KConfig for ESP32-S3
│   └── akiraconsole_esp32s3_procpu.overlay  # DTS overlay
├── signing/
│   └── keys/README.txt           # Key management docs (keys are never stored here)
└── .github/
    └── workflows/
        └── release.yml           # CI → build + sign + GitHub Release
```

---

## Getting Started

### Prerequisites

- [West](https://docs.zephyrproject.org/latest/develop/west/index.html) (`pip install west`)
- [Zephyr SDK 0.17.4](https://github.com/zephyrproject-rtos/sdk-ng/releases/tag/v0.17.4)
- [imgtool](https://pypi.org/project/imgtool/) for signing (`pip install imgtool`)

### Clone & initialise

```bash
# Create workspace
mkdir akira-workspace && cd akira-workspace

# Init west from THIS firmware repo (the manifest repo)
west init -m https://github.com/ArturR0k3r/AkiraConsole-Firmware --mr main
west update

# Export Zephyr CMake package
west zephyr-export

# Fetch ESP32 HAL binary blobs (WiFi/BT firmware)
west blobs fetch hal_espressif
```

### Build

```bash
cd akira-workspace

# Build MCUboot bootloader (first time only)
west build --pristine -b akiraconsole/esp32s3/procpu \
  bootloader/mcuboot/boot/zephyr \
  -d build-mcuboot \
  -- -DBOARD_ROOT=akiraconsole-firmware -DDTS_ROOT=akiraconsole-firmware

# Build AkiraConsole firmware
west build --pristine -b akiraconsole/esp32s3/procpu \
  akiraconsole-firmware \
  -d build-akiraconsole \
  -- -DMODULE_EXT_ROOT=akiraos
```

### Sign

```bash
# Generate a dev key (do not commit production keys)
imgtool keygen -k signing/keys/dev-root-ec256.pem -t ecdsa-p256

imgtool sign \
  --key signing/keys/dev-root-ec256.pem \
  --header-size 0x20 --align 4 \
  --slot-size 0x200000 \
  build-akiraconsole/zephyr/zephyr.bin \
  build-akiraconsole/zephyr/zephyr.signed.bin
```

### Flash

```bash
# First flash — bootloader + app
esptool --chip esp32s3 --port /dev/ttyUSB0 --baud 921600 \
  write_flash \
  0x0     build-mcuboot/zephyr/zephyr.bin \
  0x20000 build-akiraconsole/zephyr/zephyr.signed.bin

# App-only update (bootloader already flashed)
esptool --chip esp32s3 --port /dev/ttyUSB0 --baud 921600 \
  write_flash 0x20000 build-akiraconsole/zephyr/zephyr.signed.bin
```

> ⚠️ Never use `--erase` — it destroys NVS settings and installed WASM apps.

---

## Releases

Pre-built signed binaries are attached to every [GitHub Release](https://github.com/ArturR0k3r/AkiraConsole-Firmware/releases).

| File | Description |
|------|-------------|
| `akiraconsole-firmware-vX.X.X.signed.bin` | App image — slot 0 at `0x20000` |
| `akiraconsole-mcuboot-vX.X.X.bin` | MCUboot bootloader at `0x0` |
| `akiraconsole-firmware-vX.X.X.elf` | ELF with debug symbols |
| `SHA256SUMS.txt` | Checksums for all artifacts |

### Versioning

| Repo | Version scheme | Audience |
|------|----------------|----------|
| [AkiraOS](https://github.com/ArturR0k3r/AkiraOS) | `v0.x.x` — platform/SDK tags | Developers, contributors |
| [AkiraConsole-Firmware](https://github.com/ArturR0k3r/AkiraConsole-Firmware) | `v1.x.x` — firmware releases | End-users, OTA |

---

## Signing Key Setup (production)

1. Generate a key pair **locally** (never on CI):
   ```bash
   imgtool keygen -k prod-root-ec256.pem -t ecdsa-p256
   ```
2. Add the private key as a GitHub Actions secret:  
   `Settings → Secrets and variables → Actions → New repository secret`  
   Name: `FIRMWARE_SIGNING_KEY`, value: contents of `prod-root-ec256.pem`
3. Store the public key in the MCUboot KConfig for runtime verification:
   ```
   CONFIG_BOOT_SIGNATURE_TYPE_ECDSA_P256=y
   CONFIG_BOOT_SIGNATURE_KEY_FILE="path/to/prod-root-ec256.pem"
   ```
4. Delete your local copy of the private key after adding it to GitHub.

---

## Contributing

Platform bugs and feature requests → [AkiraOS](https://github.com/ArturR0k3r/AkiraOS/issues)  
AkiraConsole-specific issues → [this repo](https://github.com/ArturR0k3r/AkiraConsole-Firmware/issues)

---

## License

GPL-3.0-only — see [LICENSE](../akiraos/LICENSE).
