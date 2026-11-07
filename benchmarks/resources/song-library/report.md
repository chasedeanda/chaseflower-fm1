# Device resource audit

Source: `a28efdedf85d46a83404774de9cf9791efbafb23`.
ELF SHA256: `ed29e086bc2337a1dcc28b237442189b547d585e63442eaaff13b78d19165385`.

| Region | Linked bytes | Capacity | Headroom |
| --- | ---: | ---: | ---: |
| ordinary_ram | 87,864 | 98,304 | 10,440 |
| pool | 316,792 | 344,064 | 27,272 |
| xip | 459,852 | 573,440 | 113,588 |
| ram_code | 3,128 | 24,576 | 21,448 |
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
| ordinary_ram | +800 |
| pool | +1,388 |
| xip | +7,212 |
| ram_code | +36 |
| noinit | +0 |

- Static allocations are not net savings of a replacement build.
- Symbol sizes do not attribute inlined/shared code to a feature.
- Included source is not proof that its functions survived linking.
- Runtime, core activation and audio margins still require device diagnostics.
