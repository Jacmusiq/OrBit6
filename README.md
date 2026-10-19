# Apollo Crate

A minimal, 80s-terminal-styled sample browser for drum packs. Point it at a
folder, navigate the kit's subfolders, click a sample to hear it instantly,
and drag it straight into a track or sampler in Logic Pro X (or any other
DAW).

## Features

- **One-click instant preview** — click any sample row to hear it immediately;
  clicking a new one stops whatever was playing, so you can click through a
  kit fast.
- **8/16/24/32-bit support** — reads WAV and AIFF at any of those bit depths
  (plus FLAC, Ogg and MP3), and shows each sample's bit depth / sample rate
  next to its name.
- **Drag-and-drop export** — drag a sample row out of the window and drop it
  onto a track or a sampler plugin in Logic Pro X (or Finder, or anywhere
  else that accepts a dropped audio file).
- **Subfolder navigation** — "Load Drum Pack..." scans a folder recursively;
  the tree on the left lets you drill into nested kit subfolders (e.g.
  `808 Pack/Kicks`, `808 Pack/Snares/Acoustic`) the way most drum packs ship.
- **80s terminal look** — black background, monospace type, and a phosphor
  accent colour you can switch between green / pink / white / blue / purple
  (the 5 swatches in the top-right; your choice is remembered next launch).

## Requirements

- macOS, Apple Silicon or Intel
- Xcode command line tools (`xcode-select --install`) — full Xcode not required
- CMake 3.22+ and Ninja:
  ```bash
  pip3 install cmake ninja
  ```

## Building

```bash
cmake -G Ninja -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

The first configure fetches JUCE 8 via `FetchContent` — this takes a few
minutes. The app bundle lands at:

```
build/ApolloCrate_artefacts/Release/Apollo Crate.app
```

Launch it with:

```bash
open "build/ApolloCrate_artefacts/Release/Apollo Crate.app"
```

## Using it

1. Click **LOAD DRUM PACK...** and choose the top-level folder of a drum pack
   (it can contain any number of nested subfolders).
2. Click through the tree on the left to open a subfolder — its samples
   appear in the list on the right.
3. Click a sample to preview it. Click it again (or click another) to stop
   it / switch.
4. Drag a sample out of the list and drop it onto a track or a sampler's
   drop zone in Logic Pro X to use it in a beat.
5. Pick an accent colour from the 5 swatches top-right — it's remembered
   between launches.

## Sharing the built app

The build is ad-hoc signed, which is enough for it to launch without a
"damaged app" warning on your own Mac. If you copy `Apollo Crate.app` to
another Mac, macOS Gatekeeper will still flag it as being from an
unidentified developer (it isn't notarized) — the recipient should
right-click the app and choose **Open** the first time, or clear the
quarantine flag themselves:

```bash
xattr -cr "Apollo Crate.app"
```

## Known limitations

- Playback is a simple one-shot preview (no looping, no gain/pitch controls)
  — it's a browser, not a sampler.
- Supported formats are whatever JUCE's bundled `AudioFormatManager` reads:
  WAV, AIFF, FLAC, Ogg Vorbis and MP3 (decode only, no encoding).
