# Default MIDI SoundFont

`default.sf2` is **GeneralUser GS v2.0.3** by S. Christian Collins
(<https://www.schristiancollins.com/generaluser.php>), mirrored from
<https://github.com/mrbumpy409/GeneralUser-GS>.

It is used by `free-api`'s `MidiMusic.cpp` (TinySoundFont + TinyMidiLoader
backend, MIT-licensed) to render the game's `.mid`-based music on platforms
that don't have a Windows GM.DLS/MCI sequencer available (Linux, including
Steam Deck; Web; Android).

See [`LICENSE.txt`](LICENSE.txt) for the full SoundFont license — free for
private and commercial use, redistribution as a local copy is explicitly
permitted.

SHA-256 of the vendored file (for verifying re-downloads):

```
9575028c7a1f589f5770fccc8cff2734566af40cd26ed836944e9a5152688cfe  default.sf2
```

To use a different SoundFont instead, set the `FREE_API_SOUNDFONT`
environment variable to its path, or overwrite this file.
