#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Render the built-in drum bank and create a local, offline audition gallery and ZIP."""
import argparse
import csv
import html
from pathlib import Path
import re
import subprocess
import wave
import zipfile

ROOT = Path(__file__).resolve().parents[1]
DESCRIPTIONS = {
    30: ('Lo-fi & hip-hop', 'Dusty, swung pocket with a quiet rim answer.'),
    7: ('Lo-fi & hip-hop', 'Boom-bap backbeat, kick pickups and a second-bar hat lift.'),
    61: ('Lo-fi & hip-hop', 'Neo-soul rim pocket with broken hats and space for chords.'),
    58: ('Lo-fi & hip-hop', 'Slow trip-hop backbeat with a low-tom turn.'),
    51: ('Trap & cloud', 'Half-time snare, syncopated kicks and short sixteenth-note hat bursts.'),
    52: ('Trap & cloud', 'Airy half-time clap and a second bar that leaves the downbeat open.'),
    53: ('Trap & cloud', 'Sparse plugg bounce with rim answers and a missing second-bar downbeat.'),
    54: ('Trap & cloud', 'Driven half-time drums and busier hats for a harder synth lead.'),
    55: ('Gaze & indie', 'Driving live-kit backbeat, open-hat lift and a descending tom turnaround.'),
    56: ('Gaze & indie', 'Soft drum-machine backbeat and a light open-hat lift.'),
    57: ('Gaze & indie', 'Slow, spacious live drums with a small floor-tom turnaround.'),
    59: ('House & garage', 'Warm four-on-the-floor groove, offbeat hats and shuffled toms.'),
    60: ('House & garage', 'Restrained deep-house kick and clap with an offbeat open hat.'),
    4: ('House & garage', '909 four-on-the-floor with a small second-bar kick pickup.'),
    18: ('House & garage', 'Swung two-step: broken kicks, clipped hats and rim answers.'),
    62: ('House & garage', 'Faster four-on-the-floor garage with swung hats and a kick pickup.'),
    1: ('Gaze & indie', 'Bedroom-pop backbeat with a light clap layer.'),
    2: ('Lo-fi & hip-hop', 'Tight funk kick syncopation, rim chatter and a hat opening.'),
    5: ('Club & breaks', 'Steady techno kick underneath a rotating tom and rim response.'),
    13: ('Club & breaks', 'Broken kicks, extra snare hits and a second-bar hat opening.'),
    31: ('Club & breaks', 'Fast jungle-style snare displacement and chopped hat gaps.'),
    14: ('Club & breaks', 'Two-step drum and bass with rim detail around a firm backbeat.'),
    38: ('Club & breaks', 'Dubby half-time space with sparse rim and tom answers.'),
    35: ('Trap & cloud', 'Drill-style displaced second-bar snare and a loping hat figure.'),
    36: ('Club & breaks', 'Sparse grime-style half-time beat with an angular kick and rim.'),
    32: ('Club & breaks', 'Jersey five-kick figure, clap backbeat and an extra second-bar pickup.'),
    34: ('Club & breaks', 'Driven pop backbeat with short hat bursts and an open-hat turnaround.'),
    39: ('Club & breaks', 'Busy two-bar broken snare figure with gaps in the hat stream.'),
    40: ('Club & breaks', 'Amapiano-inspired low-tom conversation; the synth tom approximates a log drum.'),
    41: ('Club & breaks', 'Baile-inspired kick, low and mid toms with a second-bar pickup.'),
    42: ('Club & breaks', 'Dembow backbeat with a light second-bar rim response.'),
    29: ('Gaze & indie', 'Live pop-rock pocket ending in a short tom fill.'),
    48: ('Other metres', 'Gentle 6/8 brush-kit pulse, rim backbeat and low-tom pickup.'),
    49: ('Other metres', '6/8 electronic groove with an offbeat bell response.'),
    50: ('Other metres', 'Dusty 6/8 pocket with a quiet rim and open-hat turnaround.'),
    47: ('Other metres', '2/4 tom-led tamborzao-inspired rhythm.'),
    45: ('Other metres', '5/4 live-kit groove with a two-bar kick and snare variation.'),
    46: ('Other metres', '7/4 live-kit groove with a late snare pickup and tom turnaround.'),
}
REFERENCES = [
    ('Noise Honey: before the colors leave…nu gaze', 'https://splice.com/sounds/packs/noise-honey/before-the-colors-leave-nu-gaze/samples'),
    ('Drumdrops: Shoegaze', 'https://splice.com/sounds/packs/drumdrops/drumdrops-shoegaze/samples'),
    ('Attack: Lo-Fi House', 'https://www.attackmagazine.com/technique/beat-dissected/lo-fi-house/'),
    ('Attack: UK Garage', 'https://www.attackmagazine.com/technique/beat-dissected/uk-garage/'),
    ('Native Instruments: How to make a trap beat', 'https://www.native-instruments.com/blogs/blog/how-to-make-a-trap-beat'),
    ('Splice: Pluggnb Drum Kit', 'https://splice.com/sounds/collections/splice/cddxc3bm-v-q-uf31elmlta9028/samples'),
]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('output', nargs='?', type=Path, default=ROOT / 'build/drum-bank')
    parser.add_argument('--reuse', action='store_true', help='Use previously rendered WAVs and loops.csv')
    args = parser.parse_args()
    out = args.output.resolve()
    if not args.reuse:
        subprocess.run(['sh', str(ROOT / 'tools/test_drum_bank.sh'), str(out)], cwd=ROOT, check=True)
    with (out / 'loops.csv').open(newline='') as f:
        rows = list(csv.DictReader(f))
    # Read the score from the same source as the real firmware renderer, for inspection in the gallery.
    source = (ROOT / 'firmware/src/cf_drum.c').read_text()
    scores = {}
    for match in re.finditer(r'\{"([^"]+)", (\d+),.*?\}\},', source, re.S):
        scores[int(match[2])] = {
            lane: ''.join(re.findall(r'"([x.]+)"', text))
            for lane, text in re.findall(r'\[DM_(\w+)\] = ((?:"[x.]+"\s*)+)', match[0])
        }
    cards = []
    for row in rows:
        uid = int(row['id'])
        group, description = DESCRIPTIONS[uid]
        name = row['name']
        metre = '6/8' if row['beats'] == '6' else row['beats'] + '/4'
        title = f'{name} {metre}' if name == 'MathRock' else name
        step = len(next(iter(scores[uid].values()))) // int(row['bars'])
        grid = '\n'.join(f'{lane:>2}  ' + ' | '.join(' '.join(text[b:b+step][j:j+4] for j in range(0, step, 4)) for b in range(0, len(text), step)) for lane, text in scores[uid].items())
        badge = '<span class="badge">NEW</span>' if uid >= 51 else ''
        cards.append(f'''<article data-group="{html.escape(group)}" data-search="{html.escape((title+' '+group+' '+description).lower())}">
<div class="eyebrow">{html.escape(group)} {badge}</div><h2>{html.escape(title)}</h2><p class="meta">{row['bpm']} BPM · {metre} · {row['kit']} · swing {row['swing']}%</p>
<p>{html.escape(description)}</p><audio controls loop preload="none" src="{html.escape(row['file'], quote=True)}"></audio>
<details><summary>Pattern &amp; levels</summary><pre>{html.escape(grid)}</pre><p>Peak {row['peak_dbfs']} dBFS · RMS {row['rms_dbfs']} dBFS · clipped samples {row['clipped_samples']}</p><a download href="{html.escape(row['file'], quote=True)}">Download WAV</a></details></article>''')
    links = ' · '.join(f'<a href="{url}">{html.escape(label)}</a>' for label, url in REFERENCES)
    groups = ''.join(f'<option>{html.escape(group)}</option>' for group in dict.fromkeys(g for g, _ in DESCRIPTIONS.values()))
    page = '''<!doctype html><html lang="en"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>Chase FM-1 · Drum bank</title>
<style>
:root{color-scheme:dark;font-family:system-ui,sans-serif;background:#111612;color:#e7eee4}body{max-width:1180px;margin:0 auto;padding:40px 24px}h1{font-size:clamp(32px,5vw,60px);margin:12px 0}header{max-width:850px}p{line-height:1.55;color:#c0cbbd}a{color:#bae79b}.eyebrow{font-size:12px;letter-spacing:.08em;color:#a4bb95}.badge{background:#334a28;color:#d0f4ad;padding:3px 6px;border-radius:4px;margin-left:8px}nav{position:sticky;top:0;z-index:1;background:#111612ee;padding:20px 0;display:flex;gap:12px;flex-wrap:wrap}input,select,button{font:inherit;background:#1b241c;color:#eff5e9;border:1px solid #495842;border-radius:8px;padding:12px}input{flex:1;min-width:180px}main{display:grid;grid-template-columns:repeat(auto-fit,minmax(min(100%,330px),1fr));gap:16px}article{padding:24px;background:#1a221b;border:1px solid #344231;border-radius:12px}article[hidden]{display:none}h2{font-size:26px;margin:10px 0}.meta{font-size:13px;color:#a4bb95}audio{width:100%;margin:12px 0}details{font-size:13px;color:#bbcfaf}summary{cursor:pointer}pre{font-size:11px;overflow:auto;line-height:1.8}footer{margin:36px 0;font-size:13px;line-height:1.9}
</style><header><div class="eyebrow">CHASE FM-1 / ORIGINAL PATTERNS</div><h1>A new pocket.</h1><p>38 grooves. Dusty lo-fi, airy trap, driving gaze and swung club drums. Every loop is played by the firmware's actual drum engine. Each preview runs its two-bar phrase twice and can loop continuously.</p><p>Dry audio, original relative levels, no normalization or effects. The suggested BPM is used here; on the FM-1, your TEMPO stays in control. Start with Lo-fi, NeoSoul, Trap, NuGaze, LoHouse and Garage.</p></header>
<nav aria-label="Audition filters"><input type="search" aria-label="Search grooves" placeholder="Search grooves…"><select aria-label="Style family"><option value="">All styles</option>GROUPS</select><button type="button">Stop audio</button></nav><p id="count" aria-live="polite">38 grooves</p><main>CARDS</main><footer><p>Patterns use the FM-1's fixed sixteenth-note grid (eighths in 5/4 and 7/4) and a level per lane. True triplet / 32nd-note rolls, per-hit dynamics and acoustic room recordings are outside this bank. Amapiano uses a synth tom approximation. Add effects and bass on the instrument to build the full style.</p><p>Listening / production references: LINKS</p><p>Original programmed grooves; no downloaded loops or third-party audio are embedded. K kick · S snare · C clap · H closed hat · O open hat · LT/MT/HT toms · R rim · B bell · X crash.</p><a download href="loops.csv">Download catalogue and level report</a></footer>
<script>
const cards=[...document.querySelectorAll('article')],audios=[...document.querySelectorAll('audio')],search=document.querySelector('input'),select=document.querySelector('select');
function filter(){let n=0;for(const c of cards){c.hidden=!(c.dataset.search.includes(search.value.toLowerCase())&&(!select.value||c.dataset.group===select.value));if(!c.hidden)n++;else c.querySelector('audio').pause()}document.querySelector('#count').textContent=n+' grooves'}search.addEventListener('input',filter);select.addEventListener('change',filter);for(const a of audios)a.addEventListener('play',()=>audios.forEach(b=>{if(b!==a)b.pause()}));document.querySelector('button').addEventListener('click',()=>audios.forEach(a=>a.pause()));
</script></html>'''.replace('GROUPS', groups).replace('CARDS', '\n'.join(cards)).replace('LINKS', links)
    (out / 'index.html').write_text(page)
    # A short sampler: one phrase each, in this order, separated by 150 ms of silence.
    with wave.open(str(out / 'sampler.wav'), 'wb') as sampler:
        sampler.setparams((2, 2, 44100, 0, 'NONE', 'not compressed'))
        for uid in [30, 61, 51, 53, 55, 56, 59, 18]:
            row = next(row for row in rows if int(row['id']) == uid)
            with wave.open(str(out / row['file']), 'rb') as audio:
                assert audio.getparams()[:3] == (2, 2, 44100)
                sampler.writeframes(audio.readframes(audio.getnframes() // 2))
            sampler.writeframes(bytes(int(44100 * .15) * 4))
    with zipfile.ZipFile(out / 'chase-fm1-drum-auditions.zip', 'w', zipfile.ZIP_DEFLATED) as archive:
        for path in [out / 'index.html', out / 'loops.csv', out / 'sampler.wav'] + [out / row['file'] for row in rows]:
            archive.write(path, path.name)
    print(f'Audition gallery: {out / "index.html"}')
    print(f'Sampler order: Lo-fi → NeoSoul → Trap → Plugg → NuGaze → DreamPop → LoHouse → Garage')
    print(f'Audition ZIP: {out / "chase-fm1-drum-auditions.zip"}')


if __name__ == '__main__':
    main()
