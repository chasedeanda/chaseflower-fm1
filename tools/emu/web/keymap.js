// SPDX-License-Identifier: GPL-3.0-only
// CoralFlower's browser emulator: the computer keyboard, by KeyboardEvent.code (physical US-layout positions).
// Lower roots run across Q–O with R = C4. Numbers select upper chord modifiers.
// A reaches F3; P and Z–B extend the lower row. Backtick is HOLD.
globalThis.FM1 = {
  KEYMAP: [
    ["KeyA", "key", 0, "A"],
    ["KeyQ", "key", 2, "Q"],
    ["KeyW", "key", 4, "W"],
    ["KeyE", "key", 6, "E"],
    ["KeyR", "key", 7, "R"],
    ["KeyT", "key", 9, "T"],
    ["KeyY", "key", 11, "Y"],
    ["KeyU", "key", 12, "U"],
    ["KeyI", "key", 14, "I"],
    ["KeyO", "key", 16, "O"],
    ["KeyP", "key", 18, "P"],
    ["KeyZ", "key", 19, "Z"],
    ["KeyX", "key", 21, "X"],
    ["KeyC", "key", 23, "C"],
    ["KeyV", "key", 24, "V"],
    ["KeyB", "key", 26, "B"],
    ["Digit1", "key", 1, "1"],
    ["Digit2", "key", 3, "2"],
    ["Digit3", "key", 5, "3"],
    ["Digit4", "key", 8, "4"],
    ["Digit5", "key", 10, "5"],
    ["Digit6", "key", 13, "6"],
    ["Digit7", "key", 15, "7"],
    ["Digit8", "key", 17, "8"],
    ["Digit9", "key", 20, "9"],
    ["Digit0", "key", 22, "0"],
    ["Backquote", "key", 25, "`"],
    ["F1", "btn", 0, "F1"], ["F2", "btn", 1, "F2"], ["F3", "btn", 2, "F3"], ["F4", "btn", 3, "F4"],
    ["F5", "btn", 4, "F5"], ["F6", "btn", 5, "F6"], ["Home", "btn", 6, "HOME"], ["F7", "btn", 6, "F7"],
    ["F8", "btn", 7, "F8"], ["F9", "btn", 8, "F9"], ["F10", "btn", 9, "F10"], ["Space", "btn", 10, "SPACE"],
    ["Enter", "btn", 11, "ENTER"], ["Minus", "btn", 12, "-"], ["Equal", "btn", 13, "="],
    ["BracketLeft", "turn", [2, -1], "["], ["BracketRight", "turn", [2, 1], "]"],
    ["Semicolon", "turn", [0, -1], ";"], ["Quote", "turn", [0, 1], "'"],
    ["Comma", "turn", [1, -1], ","], ["Period", "turn", [1, 1], "."],
    ["Tab", "cycle", 1, "TAB"], ["ArrowUp", "arrow", 1, "UP"], ["ArrowDown", "arrow", -1, "DOWN"],
    ["ArrowRight", "arrow", 1, "RIGHT"], ["ArrowLeft", "arrow", -1, "LEFT"],
  ].map(([code, kind, idx, cap]) => ({ code, kind, idx, cap })),
  // the printed labels (EMU_B_*) and what CoralFlower makes of them
  BTN: ["FX", "SEL", "ENV", "LFO", "EDIT", "GLO", "HOME", "SAVE", "ARP", "SEQ", "PLAY", "REC", "OCT-", "OCT+"],
  BTN_ROLE: ["EFFECTS", "CHORD", "ENVELOPE", "BASS", "SOUND", "GLOBAL", "HOME", "SAVE", "ARP", "SEQUENCER", "PLAY", "REC", "OCT−", "OCT+"],
  // EMU_E_* knobs and their roles
  ENC: ["SELECT", "ALGORITHM", "PRESETS", "KNOB 1", "KNOB 2", "KNOB 3", "KNOB 4", "MASTER"],
  ENC_ROLE: ["IDEAS / SELECT", "PAGES", "CHORD SOUND", "K1", "K2", "K3", "K4", "VOLUME"],
  KEY_LABEL: ["6", "7", "9", "SUS2", "SUS4", "ADD2", "11", "ADD9", "MAJOR", "MINOR", "HOLD"],
  noteName(k) {
    const N = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"], m = 53 + k;
    return N[m % 12] + (Math.floor(m / 12) - 1);
  },
};
