# DUSTBOX 12 – Vintage Sampler Emulator

Lo-Fi-Effekt, der den Klang alter 12-Bit-Sampler nachbildet.
Formate: **VST3** (Windows/macOS), **AU** (macOS), **Standalone**.

## Signalweg

1. **INPUT**: Eingangspegel (−24 … +24 dB)
2. **DRIVE**: Übersteuerter Wandler-Eingang (Tanh-Sättigung, 0 % = linear)
3. **ALIAS**: Anti-Alias-Filter vor dem Wandler. 0 % = sauber, 100 % = kein Filter (harte Spiegelfrequenzen)
4. **RATE**: Sample-&-Hold-Samplerate (2 – 48 kHz)
5. **BITS**: Bittiefe (4 – 16 Bit, stufenlos)
6. **FILTER / RESO**: 4-Pol-Ladder-Tiefpass als Ausgangsfilter (Rekonstruktion)
7. **NOISE**: analoges Grundrauschen
8. **MIX / OUTPUT**: Dry/Wet und Ausgangspegel

## Werkspresets

| # | Name | Charakter |
|---|------|-----------|
| 01 | Classic Sixty | 40 kHz / 12 Bit, weich, punchy |
| 02 | Twelve Hundred | 26,04 kHz / 12 Bit, ohne Eingangsfilter, gritty |
| 03 | Nine-Fifty | 32 kHz / 12 Bit, steiles Filter, dunkel und warm |
| 04 | Dusty Loop | 22 kHz / 10 Bit, Lo-Fi-Hip-Hop |
| 05 | Toy Keys | 8 kHz / 8 Bit, Spielzeug-Sampler |
| 06 | Radio Ghost | 6 kHz / 6 Bit, Transistorradio |
| 07 | Bit Destroyer | 4 kHz / 4 Bit, voll kaputt |
| 08 | Init (Clean) | neutral |

## Bauen

**Automatisch (GitHub Actions):** Projekt in ein GitHub-Repo pushen. Dann unter
*Actions → Build DUSTBOX 12* die Artefakte für Windows und macOS herunterladen.

**Lokal:**
```
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```
JUCE 8.0.4 wird automatisch geladen. Alternativ: `-DJUCE_PATH=/pfad/zu/JUCE`.

## Installation

- **Windows:** Ordner `DUSTBOX 12.vst3` nach `C:\Program Files\Common Files\VST3\` kopieren.
- **macOS:** `DUSTBOX 12.vst3` nach `~/Library/Audio/Plug-Ins/VST3/` und
  `DUSTBOX 12.component` nach `~/Library/Audio/Plug-Ins/Components/` kopieren.
  Das Plugin ist nicht signiert. Gatekeeper-Sperre einmalig aufheben:
  `xattr -cr ~/Library/Audio/Plug-Ins/VST3/"DUSTBOX 12.vst3"`

## Hinweis zu Verkauf und Lizenz

JUCE 8 ist dual lizenziert (AGPLv3 oder kommerziell). Für einen kommerziellen
Closed-Source-Verkauf brauchst du eine passende JUCE-Lizenz. Die kostenlose
„Starter“-Lizenz deckt kleine Umsätze ab; Bedingungen auf juce.com prüfen.
Preset-Namen und Branding enthalten bewusst keine Hardware-Markennamen.
