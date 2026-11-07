# Device resource audit

Source: `5bfd5005901b002a19cc3b955b774d1395b1460b`.
ELF SHA256: `35c4b0e4fcf3742d7a1c876b9d13648d479d0727f66e39bf28fca52deae5dc63`.

| Region | Linked bytes | Capacity | Headroom |
| --- | ---: | ---: | ---: |
| ordinary_ram | 96,776 | 98,304 | 1,528 |
| pool | 333,852 | 344,064 | 10,212 |
| xip | 427,908 | 573,440 | 145,532 |
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


- Static allocations are not net savings of a replacement build.
- Symbol sizes do not attribute inlined/shared code to a feature.
- Included source is not proof that its functions survived linking.
- Runtime, core activation and audio margins still require device diagnostics.
