#ifndef WSLDISTRIBUTIONS_H
#define WSLDISTRIBUTIONS_H

// The distributions `wsl --install -d <Name>` accepts, for the WSL menu's
// "Install Linux Distribution" choice. Copied from the "ModernDistributions"
// list of
// https://raw.githubusercontent.com/microsoft/WSL/master/distributions/DistributionInfo.json
// (the file wsl.exe itself reads) as of 2026-10-16; see
// docs/wsl-detection.md. Microsoft adds and retires entries over time, so a
// name that is missing here can still be installed from a terminal with
// `wsl --list --online` and `wsl --install -d <Name>`.
struct WslDistribution {
  const char *name;          // the value passed to `wsl --install -d`
  const char *friendlyName;  // what the menu shows; a product name, not translated
};

inline constexpr WslDistribution kWslDistributions[] = {
    {"Ubuntu",                       "Ubuntu"},
    {"Ubuntu-26.04",                 "Ubuntu 26.04 LTS"},
    {"Ubuntu-24.04",                 "Ubuntu 24.04 LTS"},
    {"Ubuntu-22.04",                 "Ubuntu 22.04 LTS"},
    {"Ubuntu-20.04",                 "Ubuntu 20.04 LTS"},
    {"openSUSE-Tumbleweed",          "openSUSE Tumbleweed"},
    {"openSUSE-Leap-16.0",           "openSUSE Leap 16.0"},
    {"RLCPlus-9",                    "Rocky Linux Plus 9 (CIQ)"},
    {"RLCPlus-10",                   "Rocky Linux Plus 10 (CIQ)"},
    {"SUSE-Linux-Enterprise-15-SP7", "SUSE Linux Enterprise 15 SP7"},
    {"SUSE-Linux-Enterprise-16.0",   "SUSE Linux Enterprise 16.0"},
    {"kali-linux",                   "Kali Linux Rolling"},
    {"Debian",                       "Debian GNU/Linux"},
    {"AlmaLinux-8",                  "AlmaLinux OS 8"},
    {"AlmaLinux-9",                  "AlmaLinux OS 9"},
    {"AlmaLinux-Kitten-10",          "AlmaLinux OS Kitten 10"},
    {"AlmaLinux-10",                 "AlmaLinux OS 10"},
    {"archlinux",                    "Arch Linux"},
    {"FedoraLinux-44",               "Fedora Linux 44"},
    {"FedoraLinux-43",               "Fedora Linux 43"},
    {"eLxr",                         "eLxr 12.12.0.0 GNU/Linux"},
};

// `wsl --install -d` without a name installs this one, and the choice starts here.
inline constexpr const char *kDefaultWslDistribution = "Ubuntu";

#endif
