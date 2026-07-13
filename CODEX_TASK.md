# Current Codex task: smooth travelling music waveform

Replace the current ugly point-bobbing waveform animation with a genuinely smooth travelling sine wave.

## Required visual behavior

- A crest must continuously travel horizontally across the waveform area.
- The curve body must visibly translate through phase progression; individual vertices must not independently bob up and down.
- Use one shared phase for the whole curve.
- Use at least 31 evenly spaced samples across the visible width, preferably 41 if performance allows.
- Compute Y from a coherent waveform such as:

```c
y = baseline
    + primary_amplitude * sin(k1 * x - phase)
    + secondary_amplitude * sin(k2 * x - phase * 0.65f + offset);
```

- `phase` advances every frame. X stays fixed; Y is regenerated from the common phase.
- Primary amplitude: about 8-12 px. Secondary harmonic: about 2-4 px.
- Target 30 FPS using approximately 33 ms per update; use 40 ms only if necessary.
- Update the main line and glow from the exact same point array.
- No random values, no per-point oscillators, no triangle-wave approximation, and no sparse 13-point polygon.

## State behavior

- Playing: smooth continuous horizontal travel.
- Paused: ease amplitude toward a subtle near-flat wave over several frames; no abrupt freeze.
- Resume: continue from the existing phase without jumping.
- Screen unload: pause timer and bar animations.
- Screen reload: restore animation from saved music state.
- Screen destroy: delete timer and clear pointer.
- Timer must be created once only.

## Scope and efficiency

- Modify only the music UI source/header unless a tiny interface change is required.
- Reuse existing objects and colors.
- Add no fonts, images, SDL2 setup, simulator dependency, or unrelated cleanup.
- Do not run a clean full build first. Inspect, patch, perform targeted validation, then use an incremental build if available.

## Acceptance

A short recording must show a crest travelling from one side of the waveform region to the other with a continuous, smooth curve. Bars moving while the line merely jitters does not pass.