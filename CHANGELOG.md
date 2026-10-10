# Changelog

All notable changes to this project are documented here, generated with
[git-cliff](https://git-cliff.org) from [Conventional Commits](https://www.conventionalcommits.org)
commit messages. Commits made before this file existed are grouped
best-effort under "Other".
<!-- git-cliff: end of header -->
## [0.7.0] - 2026-10-10

### 🚀 Features

- Change the HTTP/3 mode from a drop-down, also while running

### 💼 Other

- Update ws2tcp-local
- Update ws2tcp-local to 0.7.0
- Update ws2tcp-local-ffi to 0.6.0

## [0.6.0] - 2026-10-10

### 🚀 Features

- Add an HTTP/3 only option to the settings

### 💼 Other

- Update ws2tcp-local to 0.6.0

## [0.5.2] - 2026-10-10

### 💼 Other

- Update ws2tcp-local-ffi to 0.4.1
- Update ws2tcp-local to 0.5.2

## [0.5.1] - 2026-10-10

### ⚙️ Miscellaneous Tasks

- Publish latest.json into the releases directory

### 💼 Other

- Update ws2tcp-local to 0.5.1

## [0.5.0] - 2026-10-08

### 🚀 Features

- *(settings)* Add an option to use HTTP/3 (QUIC) to the gateway

### 🐛 Bug Fixes

- *(build)* Declare the Cargo-built FFI libraries as byproducts

### ⚙️ Miscellaneous Tasks

- Check out core through the CLI and FFI submodules
- Disable the macOS jobs until signing keys are available

### 💼 Other

- Vendor ws2tcp-local and ws2tcp-local-ffi as git submodules
- Update ws2tcp-local-ffi to 0.4.0

## [0.4.0] - 2026-09-21

### 🚀 Features

- *(rules)* Edit, import and export custom rules in the app

## [0.3.1] - 2026-09-21

### 🚀 Features

- *(tray)* Show whether the proxy is running on the tray icon

### 🐛 Bug Fixes

- *(settings)* Send update checks and downloads through the upstream proxy

## [0.3.0] - 2026-09-21

### 🚀 Features

- *(settings)* Connect to the gateway through an upstream proxy

## [0.2.2] - 2026-09-21

### 🐛 Bug Fixes

- *(wsl)* Prompt to restart Windows now or later after installing WSL

## [0.2.1] - 2026-09-21

### 🚀 Features

- *(update)* Check for updates on startup and offer to upgrade at once

## [0.2.0] - 2026-09-20

### 🚀 Features

- *(auth)* [**breaking**] Choose the authentication method, token is the new default

## [0.1.1] - 2026-09-20

### 💼 Other

- *(package)* Prefix installer and DMG file names with ws2tcp-local-gui

## [0.1.0] - 2026-09-20

### 💼 Other

- Initial commit

