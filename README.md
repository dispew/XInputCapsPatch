# XInputCapsPatch
A small DashLaunch plugin that makes third-party XInput controllers report
correct capabilities to the Xbox 360, so apps that check them stop treating the
pad as "Unknown" and start accepting it.

## What works
- Makes a generic (class-2) controller report `Type = GAMEPAD`, `SubType = GAMEPAD`, and proper capability `Flags` instead of `Unknown` (0/0)
- Presents the controller to the system as a **standard Xbox 360 wireless gamepad** (Flags = `WIRELESS | VOICE | PMD`), so it's recognized identically to a first-party wireless pad
- Fixes apps that reject Unknown devices
- Restores analog stick and `LT`/`RT` trigger input in the **native Xbox 360 dashboard (NXE)** and **Aurora** — with Unknown capabilities the dashboards ignore these, so scrolling/navigation and trigger actions only worked partially; now the pad behaves like a full controller everywhere
- Device-agnostic — no vendor-ID list; it repairs the shared capability path, not a specific controller
- RAM-only: two in-place kernel patches applied at boot; nothing is written to persistent storage

## Tested with
- Xbox 360 dashboard **2.0.17559.0** (kernel build 17559) — **this fix only works on this exact version**; the patch addresses are build-specific
- **8BitDo Ultimate Bluetooth** controller in **2.4GHz mode**, connected through its 8BitDo 2.4GHz dongle (XInput mode)

## Current limitations
- Kernel build **17559** only — the patch addresses are build-specific (the technical write-up covers how to locate them for another build)
- Covers class-2 devices (the usual third-party XInput pads); other device classes aren't touched
- Only the controller above has been verified; other controllers are expected to work but are untested

## Requirements
- **UsbdSecPatch** must be installed and loaded. It's what lets a third-party controller connect and register on the console in the first place; without it the pad never reaches the capability path this fix patches, so XInputCapsPatch has nothing to act on. Both are DashLaunch plugins loaded from `launch.ini` (the order between them doesn't matter — both apply their kernel patches at boot, before any controller connects).

## How to use
1. Make sure **UsbdSecPatch** is installed (see Requirements)
2. Download `XInputCapsPatch.xex` from the [Releases](../../releases) page and copy it to `Hdd:\Plugins\`
3. Add both to your `launch.ini`:
   ```ini
   [Plugins]
   plugin1 = Hdd:\Plugins\UsbdSecPatch.xex
   plugin2 = Hdd:\Plugins\XInputCapsPatch.xex
   ```
4. Reboot. The pad is now recognized as a gamepad.

## How to build
1. Acquire the official Xbox 360 SDK (XDK) — by magic, by wishing upon a star, or by building a time machine and travelling back to get one
2. Set the `XEDK` environment variable to your SDK path
3. Run `build_plugin.bat` — it uses the SDK's PowerPC compiler/linker + `imagexex` and produces `XInputCapsPatch.xex`

*(A prebuilt `XInputCapsPatch.xex` is attached to each [release](../../releases) — you only need the SDK if you want to rebuild or modify it.)*

## How it works
The kernel routine that fills a controller's `XINPUT_CAPABILITIES` bails out for
devices that never send a "capability descriptor", leaving Type/SubType at 0/0.
The plugin makes two in-place edits to that routine so it fills the capabilities
instead of bailing. A full technical write-up — addresses, the capability
pipeline, and the dead ends along the way — is posted on the forum.

## Changelog
### v0.1
- First release.
- Class-2 XInput controllers now report as a standard Xbox 360 wireless gamepad (Type/SubType `GAMEPAD`, Flags `WIRELESS | VOICE | PMD`) instead of `Unknown`.
- Restores analog stick and `LT`/`RT` trigger input in the native dashboard (NXE) and Aurora.
- Fixes controller rejection in a few games and apps.
- Tested on dashboard 2.0.17559.0 with an 8BitDo Ultimate Bluetooth (2.4GHz mode via dongle).

## Thanks
- The **Xbox 360 modding community**, for years of homebrew and reverse-engineering that made all of this possible
- **[UsbdSecPatch](https://github.com/InvoxiPlayGames/UsbdSecPatch) (InvoxiPlayGames)**, for the third-party controller work and the in-place body-patch approach this fix follows
- **[hiddriver360](https://github.com/EinTim23/) (EinTim23)**, for the HID/controller reverse-engineering knowledge that guided the investigation

## License
MIT — see [LICENSE](LICENSE).
