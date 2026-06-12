# Local Sync Notes

This GitHub repository has been productized in documentation and default configuration, but the real working project is expected to be validated locally.

## What changed in GitHub

- README now describes this as an ESP32-C5 XiaoZhi product firmware.
- Product cleanup roadmap added.
- Product build notes added.
- sdkconfig.defaults now targets ESP32-C5 and esp-sensairshuttle by default.
- esp-sensairshuttle config preset is aligned with product defaults.
- Upstream zh and ja README files were removed.
- main/main.cc no longer contains hard-coded Wi-Fi credentials.
- Product CMake and Kconfig replacement drafts were added.

## Important files

```text
README.md
PRODUCT_CLEANUP.md
PRODUCT_BUILD.md
LOCAL_SYNC_NOTES.md
main/CMakeLists.product.txt
main/Kconfig.product.projbuild
sdkconfig.defaults
main/boards/esp-sensairshuttle/config.json
```

## Local validation order

1. Pull the latest GitHub main into the local project.
2. Check git status.
3. Build current main once before applying product draft replacements.
4. Apply product CMake and Kconfig drafts locally.
5. Clean build artifacts.
6. Build again.
7. Fix any missing source or dependency references in the product CMake draft.
8. Only after product CMake builds, start deleting unused source directories.

## Product cleanup rule

Do not delete source files just because they look unused. First remove them from the product build entry, then build successfully, then delete them.
