# OrBit 6

An 80s-terminal-styled drum sample browser and keyboard sampler in one app.
**ORBIT 6** (top-left) is the Browser — navigate your drum packs and
favorites with instant preview; **SAMPLER** (next to it, styled the same way)
loads any sample onto a computer-keyboard piano with editing, a vintage tape
effect, and recording.

## Features

### Browser (ORBIT 6)
- **One-click instant preview** — click any sample row to hear it immediately;
  clicking a new one stops whatever was playing, so you can click through a
  kit fast. Hit **Space** to pause and resume exactly where you left off.
- **Live waveform** — shows the waveform of whatever's playing, with a moving
  playhead.
- **8/16/24/32-bit support** — reads WAV and AIFF at any of those bit depths
  (plus FLAC, Ogg and MP3), and shows each sample's bit depth / sample rate
  next to its name.
- **Drag-and-drop anywhere** — drag a sample row straight out of the window
  and drop it onto the Desktop, into Finder, or onto a track or sampler
  plugin in Logic Pro X (or any other DAW) — a real native file drag.
- **Your own stored library** — "LOAD DRUM PACK..." imports a copy of the
  folder you choose into the app's own storage (per macOS user account), so
  it's there every time you reopen the app. Right-click a pack in the tree
  to delete it permanently.
- **Subfolder navigation** — each imported pack can contain any number of
  nested subfolders, browsable in the tree on the left.
- **Favorites, right in the tree** — right-click any sample and choose
  **Add to / Remove from Favorites**. A **★ FAVORITES** entry always sits at
  the top of the tree, collecting every starred sample from every pack in
  one flat list — same preview/double-click/drag behaviour as any pack.

### Sampler
- **Double-click (or right-click > Load into Sampler) any sample** in the
  Browser tree or Favorites to send it here.
- **Computer-keyboard piano** — play the loaded sample pitched across your
  keyboard: `A S D F G H J K L ;` are the white keys, `W E T Y U O P` are the
  black keys between them, **Z / X** shift the whole keyboard down/up an
  octave. It's polyphonic — hold several keys at once — and also playable
  with the mouse on the on-screen keys.
- **Editing** — Start/End trim, Gain, Tune (±24 semitones), Reverse, and a
  full Attack/Decay/Sustain/Release envelope.
- **Vintage tape machine** — an optional insert effect (Reel-to-Reel,
  Cassette, or Lo-Fi Boombox, with an Amount knob) adding tape saturation,
  wow & flutter pitch wobble, hiss, and high-end rolloff.
- **Recording** — hit **● REC** and play the keyboard; it captures exactly
  what comes out of the Sampler (tape effect included if it's on) to a WAV
  file. Hit **■ STOP** to save it.

### Look
- **Vintage hardware look** — a black terminal screen inset into a brushed
  chassis with corner screws, CRT grain, and a vignette, in a phosphor accent
  colour you can switch between green / pink / white / blue / purple (the 5
  swatches top-right; your choice is remembered next launch).
- **Raw pixel type** — every piece of text in the app (titles, lists, the
  keyboard, labels) is rendered as a small bitmap and scaled up with no
  smoothing, for a blocky, low-res terminal look rather than smooth system
  type.
- **ORBIT 6 / SAMPLER** are plain bold text headers, not boxed tabs — click
  either to switch; the active one is underlined.

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
build/OrBit6_artefacts/Release/OrBit 6.app
```

Launch it with:

```bash
open "build/OrBit6_artefacts/Release/OrBit 6.app"
```

## Using it

1. Click **ORBIT 6** (top-left) if it isn't already showing, click **LOAD
   DRUM PACK...**, and choose a drum pack's top-level folder. It's copied
   into the app's own library and appears in the tree on the left — this can
   take a moment for a large pack.
2. Click through the tree to open a pack's subfolders — the samples directly
   inside the selected folder appear in the list on the right. Click
   **★ FAVORITES** at the top of the tree to see everything you've starred.
3. Click a sample to preview it; click again to stop. **Space** pauses/resumes
   in place. Drag a row out to the Desktop, Finder, or a DAW.
4. **Double-click a sample** (or right-click it and choose **Load into
   Sampler**) to send it to **SAMPLER** and switch there automatically.
5. In Sampler, play it with the on-screen/computer keyboard (see the key map
   above), adjust Start/End/Gain/Tune/Reverse/ADSR, optionally flip on the
   tape machine, and hit **● REC** to print a take to a WAV file.
6. Right-click a sample anywhere to **Add to Favorites**.
7. Right-click a pack (a top-level entry in the tree) to delete it from your
   library permanently.
8. Pick an accent colour from the 5 swatches top-right — it's remembered
   between launches.

## Where things are stored

Everything lives under `~/Library/Application Support/OrBit6/`, scoped to
your macOS user account:
- `Packs/` — one folder per imported drum pack (rescanned on every launch).
- `favorites.txt` — the list of favorited sample paths.
- `Recordings/` — WAV files captured from the Sampler, named
  `Take-YYYYMMDD-HHMMSS.wav`.

Deleting a pack from the app removes its folder from disk for good.

## Sharing the built app

The build is ad-hoc signed, which is enough for it to launch without a
"damaged app" warning on your own Mac. If you copy `OrBit 6.app` to another
Mac, macOS Gatekeeper will still flag it as being from an unidentified
developer (it isn't notarized) — the recipient should right-click the app
and choose **Open** the first time, or clear the quarantine flag themselves:

```bash
xattr -cr "OrBit 6.app"
```

## Known limitations

- The Sampler is monotimbral — one loaded sample at a time, polyphonic
  across the keyboard, not a multi-sample/multi-pad instrument.
- No loop points — ADSR release lets a held note tail off, but the sample
  body itself doesn't loop.
- Supported formats are whatever JUCE's bundled `AudioFormatManager` reads:
  WAV, AIFF, FLAC, Ogg Vorbis and MP3 (decode only, no encoding).
