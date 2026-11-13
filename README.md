<!-- SPDX-License-Identifier: GPL-3.0-or-later -->
<!-- Copyright (C) 2026 Pau Aliagas <linuxnow@gmail.com> -->
# omx-plugins

The effects of the OpenMixer console, as plugins you can load
in any host: **CLAP** for Bitwig, REAPER, Carla, openmixer and every other CLAP host, and **LV2** for
Ardour, Carla, MOD and Zynthian, the MOD GUI included.

The DSP inside each one is the console's own (the [omx-dsp](https://github.com/FreeMixer/omx-dsp)
library), so a plugin sounds the same in your DAW as on the desk, sample for sample. Each plugin is
drawn from one declaration: its CLAP parameters, its LV2 ports and its MOD GUI cannot disagree.

## Install

On **Fedora**, from the OpenMixer package channel:

```sh
sudo dnf install https://www.openmixer.org/rpm/openmixer-release.noarch.rpm
sudo dnf install omx-plugins
```

On **Debian** bookworm or trixie, **Raspberry Pi OS** and **Zynthian**:

```sh
cd /tmp && curl -fsSLO https://www.openmixer.org/deb/openmixer-release.deb && sudo apt install ./openmixer-release.deb
sudo apt update && sudo apt install omx-plugins
```

`omx-plugins` installs both formats; `omx-plugins-clap` or `omx-plugins-lv2` takes only one.

## Start

Rescan plugins in your host and look for the **omx** plugins under openmixer. They land where hosts
look without configuration: `/usr/lib/clap`, and `/usr/lib64/lv2` on Fedora or `/usr/lib/lv2` on Debian.

## Documentation

<https://www.openmixer.org/projects/omx-plugins>, and the full documentation at
<https://www.openmixer.org/docs/plugins>. In this repository: every plugin and its controls, the
factory presets, the packages and the static library ([docs/catalogue.md](docs/catalogue.md)); a
page of parameters per plugin ([docs/plugins/README.md](docs/plugins/README.md)); building from
source ([BUILDING.md](BUILDING.md)).

## Licence

GPL-3.0-or-later. See [LICENSE](LICENSE).
