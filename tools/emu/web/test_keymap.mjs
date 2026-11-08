// SPDX-License-Identifier: GPL-3.0-only
import assert from 'node:assert/strict';
import './keymap.js';
const {KEYMAP,noteName}=globalThis.FM1;
assert.equal(new Set(KEYMAP.map(m=>m.code)).size,KEYMAP.length,'No conflicting shortcuts');
const keys=KEYMAP.filter(m=>m.kind==='key');
assert.equal(new Set(keys.map(m=>m.idx)).size,27,'Every physical key is reachable');
assert.equal(noteName(KEYMAP.find(m=>m.code==='KeyR').idx),'C4');
const notes=['G3','A3','B3','C4','D4','E4','F4','G4','A4'];
[...'QWERTYUIO'].forEach((letter,i)=>assert.equal(noteName(KEYMAP.find(m=>m.code==='Key'+letter).idx),notes[i]));
[...'1234567890'].forEach(d=>assert.ok(noteName(KEYMAP.find(m=>m.code==='Digit'+d).idx).includes('#'),'Numbers reach upper keys'));
assert.equal(KEYMAP.find(m=>m.code==='Backquote').idx,25,'Backtick reaches HOLD');
console.log('PASS: R=C4, Q–O roots, numbers as modifiers, all 27 keys, no shortcut conflicts');
