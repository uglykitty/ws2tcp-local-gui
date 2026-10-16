# Detecting the WSL install state on Windows

The WSL menu needs to tell four states apart:

| State | Meaning |
| --- | --- |
| **A** | WSL not installed (no components) |
| **B1** | Components installed, no distribution, **restart pending** |
| **B2** | Components installed, no distribution, restarted |
| **C** | Components installed, at least one distribution |

`wsl.exe`'s text output is UTF-16LE and differs between Windows builds, so
every signal below is an exit code, a registry value or a file check, never
parsed text.

## How it was measured

One fresh Windows 11 Build 26300.9457 virtual machine, reached over SSH, taken
through the four states in order:

1. A: fresh install, nothing done.
2. B1: `wsl --install --no-distribution`, no restart.
3. (experiment in B1) `wsl --install -d Ubuntu --no-launch`, still no restart.
4. B2: restarted.
5. C: `wsl --install -d Ubuntu --no-launch`.

WSL package 3.0.3, kernel 6.18.54.1-1. Items marked **(not measured)** were not
verified; Windows 10 and older WSL builds were not tested at all.

## Results

| Signal | A | B1 | B2 | C |
| --- | --- | --- | --- | --- |
| `wsl --version` exit code | 1 | 0 | 0 | 0 |
| `wsl --status` exit code | 50 | 0 | 0 | 0 |
| `wsl -l -q` | exit 1 | exit 0, empty | exit 0, empty | exit 0, `Ubuntu` |
| `HKCU\...\Lxss` key | absent | present | present | present |
| `Lxss\InstallStatus` subkey | absent | **present**, default value `VirtualMachinePlatform` | absent | absent |
| `Lxss\DefaultDistribution` | - | not set | not set | `{guid}` |
| `Lxss\{guid}\DistributionName` | - | - | - | `Ubuntu` |
| `Win32_OptionalFeature.InstallState`: `VirtualMachinePlatform`, `Microsoft-Windows-Subsystem-Linux` | 2, 2 | 1, 2 | 1, 2 | 1, 2 |
| DISM feature state, same two | Disabled, Disabled | Enabled, Disabled | Enabled, Disabled | Enabled, Disabled |
| `HKLM\...\Component Based Servicing\RebootPending` | absent | **present** | absent | absent |
| `HKLM\...\WindowsUpdate\Auto Update\RebootRequired` | absent | absent | absent | absent |
| `WslService` service | absent | present | present | present |
| `C:\Program Files\WSL\wsl.exe` | absent | present (5377864 bytes, 3.0.3.0) | present | present |
| `where wsl.exe` | System32 only | System32, Program Files\WSL, `%LOCALAPPDATA%\Microsoft\WindowsApps` | not measured | not measured |
| `C:\Windows\System32\wsl.exe` | 274432 bytes, 10.0.26100.8737 | identical | not measured | not measured |

`wsl --status` text:

| State | Text |
| --- | --- |
| A | "The Windows Subsystem for Linux is not installed" |
| B1 | `Default Version: 2`, "WSL1 is not supported ...", **"WSL2 is unable to start since virtualization is not enabled on this machine"**, "Enable Virtual Machine Platform by running: wsl.exe --install --no-distribution" |
| B2 | `Default Version: 2`, "WSL1 is not supported ..." |
| C | `Default Distribution: Ubuntu`, `Default Version: 2`, "WSL1 is not supported ..." |

`InstallState`: 1 = enabled, 2 = disabled. The
`Microsoft-Windows-Subsystem-Linux` feature stays disabled with the Store-based
WSL package (WSL 2 only), which is why `--status` always warns about WSL1; the
warning is harmless and the feature must not be required.

## Which signal to use

| Question | Signal |
| --- | --- |
| Is the WSL package installed? (A vs the rest) | `wsl.exe --version` exits 0; equivalently `C:\Program Files\WSL\wsl.exe` exists |
| Is a restart pending? (B1 vs B2) | `Lxss\InstallStatus` subkey exists |
| Is there a usable distribution? (C vs the rest) | `Lxss\DefaultDistribution` is set and its subkey has a non-empty `DistributionName` |

`MainWindow::isWslUsable()` implements the last row, with no process involved.

### Determining the state

Conditions that identify each state (all of them must hold). "Package" means
`C:\Program Files\WSL\wsl.exe` exists, equivalently `wsl.exe --version` exits 0.

| State | Distribution registered | `Lxss\InstallStatus` | Package |
| --- | --- | --- | --- |
| **C** | yes: `DefaultDistribution` is set and `{guid}\DistributionName` is non-empty | absent | present |
| **B1** | no | **present** | present |
| **B2** | no | absent | present |
| **A** | no | absent | **missing** |

`MainWindow::detectWslState()` evaluates them in this order and stops at the
first match:

1. **C** if a distribution is registered.
2. **B1** if `Lxss\InstallStatus` exists.
3. **A** if the package is missing.
4. **B2** otherwise.

The states are decided from registry reads and one file check, so no `wsl.exe`
process is started. The distribution is checked first on purpose: it is the one condition
`isWslUsable()` (true only in C) needs, and an older WSL that ships inside
Windows and has registered distributions but no `C:\Program Files\WSL\wsl.exe`
(not tested) would otherwise be reported as A.

The order only matters for combinations that were never observed, such as a
registered distribution together with `Lxss\InstallStatus`, which this order
reports as C. That case is **(not measured)**.

Signals that do **not** work:

- **`wsl --status` exit code.** It is 0 in B1, B2 and C, so it cannot mean
  "usable".
- **The exit code of `wsl --install --no-distribution`.** It is 0 even when it
  prints "Changes will not be effective until the system is rebooted" (A to
  B1). It is not 3010, so it cannot tell whether a restart is needed.
- **The optional-feature state.** `VirtualMachinePlatform` already reads
  enabled in B1, before the restart.
- **The generic restart markers.** `CBS\RebootPending` was set in B1 and cleared
  by the restart, but `WindowsUpdate\...\RebootRequired` was not set in this run
  (it was set on an earlier, since deleted, VM in the same state). They mean
  "any pending restart on this machine" anyway, not specifically WSL.
- **The size, version or signature of `System32\wsl.exe`.** It is the same file
  before and after the install (see below).

### `System32\wsl.exe` is a stub

In A the only `wsl.exe` is `C:\Windows\System32\wsl.exe`, which handles only
`--install`, `--update`, `--status` and `--help`; any other command prints "The
Windows Subsystem for Linux is not installed" and exits 1. After the install the
real `C:\Program Files\WSL\wsl.exe` appears (and a Store alias in
`WindowsApps`), and the System32 file is unchanged. So "is this the stub?" means
"does `C:\Program Files\WSL\wsl.exe` exist?", equivalently "does
`wsl --version` exit 0?". That the System32 file forwards to the new one is an
inference; only the working behaviour was observed.

### Behaviour of the distribution step while a restart is pending (B1)

`wsl --install -d Ubuntu --no-launch` run in B1 exits **0** and prints "Changes
will not be effective until the system is rebooted", but installs nothing:
afterwards `DefaultDistribution` is still not set, `wsl -l -q` is still empty
and `Lxss\InstallStatus` is still there. So the exit code of the distribution
step does not mean a distribution was installed; check the registry afterwards.
After the restart (B2) the same command downloads and installs Ubuntu (exit 0,
state C).

Whether `Lxss\InstallStatus` can appear in other situations (for example when
only a distribution is installed) was **(not measured)**.

## Running the install

- `wsl --install`, `wsl --install --no-distribution` and
  `wsl --install -d Ubuntu --no-launch` print "not installed" and exit 1 when
  run from an SSH session. They work when started in the interactive desktop
  session (a scheduled task created with `/it` was used for the tests), which
  is also where the GUI's `ShellExecuteEx` "runas" launch runs. Running
  the install menu items themselves on a VM desktop was **(not measured)**.
- The install therefore works as two stages with a restart in between:
  1. `wsl --install --no-distribution` (A to B1). Exit code 0; a restart is
     required afterwards (B1 to B2).
  2. `wsl --install -d <Name> --no-launch` (B2 to C). Exit code 0. It was
     measured with `<Name>` = `Ubuntu` only.
- The WSL menu has one item per stage, and each one checks `detectWslState()`
  first:

  | Menu item | A | B1 | B2 | C |
  | --- | --- | --- | --- | --- |
  | **Install WSL Components** (`MainWindow::installWslComponents()`) | Runs stage 1 elevated | Asks to restart Windows (now or later) | Says the components are already installed | Says the components are already installed |
  | **Install Linux Distribution...** (`MainWindow::installWslDistribution()`) | Error: install the components first | Asks to restart Windows (now or later) | Asks which distribution, then runs stage 2 elevated | Says a distribution is already installed |

  After stage 1 the state is detected again: B1 asks to restart, B2 tells the
  user to use "Install Linux Distribution", anything else (A) is an error.
  After stage 2 only C counts as success, whatever the exit code was. The
  distribution is chosen from the predefined list below.

  The exit codes are only used to tell failure (neither 0 nor 3010) from
  "finished"; success is always decided by detecting the state again.
- Both steps print nothing while downloading, and an empty output file does not
  mean a hang. On a slow link a step can take well over 30 minutes, so do not
  put a short timeout on it.
- Step 2 downloads the distribution list from `raw.githubusercontent.com`
  before the distribution itself, so it needs that host to be reachable.

## Predefined distributions

`src/WslDistributions.h` holds the list the "Install Linux Distribution..."
dialog offers; the selected `Name` is what goes after `wsl --install -d`. Only
these predefined names are ever put on the `wsl.exe` command line.

The list is copied from the `ModernDistributions` section of
<https://raw.githubusercontent.com/microsoft/WSL/master/distributions/DistributionInfo.json>,
the file `wsl.exe` reads for `wsl --list --online`, as fetched on 2026-10-16.
That file also has an older `Distributions` section (Ubuntu, Debian, Kali,
openSUSE Tumbleweed, SUSE Linux Enterprise 15 SP6 and three Oracle Linux
versions); only the modern one is used here, and its top-level `Default` is
`Ubuntu`. Microsoft changes the file over time, so the list will go stale: to
refresh it, fetch the file again and copy the `Name` and `FriendlyName` pairs.

| `Name` | Friendly name | x64 | ARM64 |
| --- | --- | --- | --- |
| `Ubuntu` | Ubuntu | yes | yes |
| `Ubuntu-26.04` | Ubuntu 26.04 LTS | yes | yes |
| `Ubuntu-24.04` | Ubuntu 24.04 LTS | yes | yes |
| `Ubuntu-22.04` | Ubuntu 22.04 LTS | yes | yes |
| `Ubuntu-20.04` | Ubuntu 20.04 LTS | yes | yes |
| `openSUSE-Tumbleweed` | openSUSE Tumbleweed | yes | yes |
| `openSUSE-Leap-16.0` | openSUSE Leap 16.0 | yes | yes |
| `RLCPlus-9` | Rocky Linux Plus 9 (CIQ) | yes | yes |
| `RLCPlus-10` | Rocky Linux Plus 10 (CIQ) | yes | yes |
| `SUSE-Linux-Enterprise-15-SP7` | SUSE Linux Enterprise 15 SP7 | yes | no |
| `SUSE-Linux-Enterprise-16.0` | SUSE Linux Enterprise 16.0 | yes | no |
| `kali-linux` | Kali Linux Rolling | yes | yes |
| `Debian` | Debian GNU/Linux | yes | yes |
| `AlmaLinux-8` | AlmaLinux OS 8 | yes | yes |
| `AlmaLinux-9` | AlmaLinux OS 9 | yes | yes |
| `AlmaLinux-Kitten-10` | AlmaLinux OS Kitten 10 | yes | yes |
| `AlmaLinux-10` | AlmaLinux OS 10 | yes | yes |
| `archlinux` | Arch Linux | yes | no |
| `FedoraLinux-44` | Fedora Linux 44 | yes | yes |
| `FedoraLinux-43` | Fedora Linux 43 | yes | yes |
| `eLxr` | eLxr 12.12.0.0 GNU/Linux | yes | no |

Installing a distribution other than `Ubuntu` was **(not measured)**; the names
are taken from the file as published.
