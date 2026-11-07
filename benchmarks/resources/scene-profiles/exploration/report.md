# Device resource audit

Source: `1da70f0cbc97c20d473a10bf18d12c790ba30d21`.
ELF SHA256: `553fbbd2cdab42432f09f722177e89190ac9c8bd91fd28cf36ea0d523bca307e`.

| Region | Linked bytes | Capacity | Headroom |
| --- | ---: | ---: | ---: |
| ordinary_ram | 97,512 | 98,304 | 792 |
| pool | 333,852 | 344,064 | 10,212 |
| xip | 437,892 | 573,440 | 135,548 |
| ram_code | 3,016 | 24,576 | 21,560 |
| noinit | 216 | 15,664 | 15,448 |

Allocation groups are gross linked objects, not promised savings.

## scene_allocations_gross_not_net_savings

| Symbol | Section | Bytes |
| --- | --- | ---: |
| `flo` | `.pool` | 12,632 |
| `fl_px` | `.pool` | 8,400 |
| `scene_body_px` | `.pool` | 3,840 |

## pattern_store_and_undo_not_reclaimable_without_replacement

| Symbol | Section | Bytes |
| --- | --- | ---: |
| `sq_st` | `.bss` | 20,336 |
| `sqh` | `.bss` | 5,152 |

## display_framebuffer_and_dma_shared_with_recovery

| Symbol | Section | Bytes |
| --- | --- | ---: |
| `cv_px` | `.pool` | 57,600 |
| `fb_dma` | `.pool` | 15,360 |

## engine_part_and_voice_state

| Symbol | Section | Bytes |
| --- | --- | ---: |
| `trk` | `.bss` | 12,640 |
| `fm6_eff` | `.bss` | 980 |
| `fm6_patch` | `.bss` | 780 |
| `eng_state` | `.pool` | 27,792 |
| `va_vs` | `.pool` | 3,456 |
| `va_patch` | `.pool` | 424 |
| `va_lfo` | `.pool` | 288 |
| `va_mlast` | `.pool` | 64 |
| `va_rate_off` | `.pool` | 16 |

## fx_buffers_preserve_audible_tails

| Symbol | Section | Bytes |
| --- | --- | ---: |
| `xd_buf` | `.pool` | 131,072 |
| `cv_buf` | `.pool` | 35,598 |
| `cho_buf` | `.pool` | 4,096 |

## legacy_loop_conversion_staging

| Symbol | Section | Bytes |
| --- | --- | ---: |
| `sq_loop_buf` | `.pool` | 2,016 |

## Paired change (current minus baseline)

| Region | Delta bytes |
| --- | ---: |
| ordinary_ram | +736 |
| pool | +0 |
| xip | +9,984 |
| ram_code | +0 |
| noinit | +0 |

- Static allocations are not net savings of a replacement build.
- Symbol sizes do not attribute inlined/shared code to a feature.
- Included source is not proof that its functions survived linking.
- Runtime, core activation and audio margins still require device diagnostics.
