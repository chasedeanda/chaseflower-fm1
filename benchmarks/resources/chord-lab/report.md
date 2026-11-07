# Device resource audit

Source: `94bcf51e59cae070b4e0962ce6c53a54f0afbc7c`.
ELF SHA256: `664e98bfaab90d60d3fa25f85a56ec783e93035ca23b9c33ae4d645b679dcacb`.

| Region | Linked bytes | Capacity | Headroom |
| --- | ---: | ---: | ---: |
| ordinary_ram | 87,064 | 98,304 | 11,240 |
| pool | 315,404 | 344,064 | 28,660 |
| xip | 452,640 | 573,440 | 120,800 |
| ram_code | 3,092 | 24,576 | 21,484 |
| noinit | 216 | 15,664 | 15,448 |

Allocation groups are gross linked objects, not promised savings.

## scene_allocations_gross_not_net_savings

| Symbol | Section | Bytes |
| --- | --- | ---: |

## pattern_store_and_undo_not_reclaimable_without_replacement

| Symbol | Section | Bytes |
| --- | --- | ---: |
| `sq_st` | `.bss` | 7,656 |

## recording_cancel_and_flash_staging_pool

| Symbol | Section | Bytes |
| --- | --- | ---: |
| `sq_io` | `.pool` | 3,840 |
| `sq_cancel` | `.pool` | 2,584 |

## recording_order_and_preferences

| Symbol | Section | Bytes |
| --- | --- | ---: |
| `sq_meta` | `.bss` | 12 |
| `sq_pref` | `.bss` | 12 |
| `sq_serial` | `.bss` | 12 |
| `sq_order` | `.data` | 3 |

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
| ordinary_ram | +3,184 |
| pool | +0 |
| xip | +10,320 |
| ram_code | +0 |
| noinit | +0 |

- Static allocations are not net savings of a replacement build.
- Symbol sizes do not attribute inlined/shared code to a feature.
- Included source is not proof that its functions survived linking.
- Runtime, core activation and audio margins still require device diagnostics.
