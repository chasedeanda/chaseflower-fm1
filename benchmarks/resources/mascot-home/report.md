# Device resource audit

Source: `4502c2f36ab5d27f8dd627e256f91619fc00f416`.
ELF SHA256: `c1a08e161f0bb7dc6289ae4c39d6cf75909f5e206a555e06de88f10c68b49af2`.

| Region | Linked bytes | Capacity | Headroom |
| --- | ---: | ---: | ---: |
| ordinary_ram | 88,696 | 98,304 | 9,608 |
| pool | 316,792 | 344,064 | 27,272 |
| xip | 553,624 | 573,440 | 19,816 |
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

## mascot_art_const_not_runtime_image_buffers

| Symbol | Section | Bytes |
| --- | --- | ---: |
| `CFM_DEVIL_DATA` | `.text` | 23,118 |
| `CFM_CAT_DATA` | `.text` | 22,847 |
| `CFM_CAT_ROWS` | `.text` | 896 |
| `CFM_DEVIL_ROWS` | `.text` | 896 |
| `CFM_CAT_RGB` | `.text` | 240 |
| `CFM_DEVIL_RGB` | `.text` | 240 |
| `CFM_ART` | `.text` | 24 |

## native_font_const_bitmaps_and_glyph_tables

| Symbol | Section | Bytes |
| --- | --- | ---: |
| `CFN_64_BITS` | `.text` | 37,427 |
| `CFN_32_BITS` | `.text` | 9,623 |
| `CFN_16_BITS` | `.text` | 2,504 |
| `CFN_14_BITS` | `.text` | 1,979 |
| `CFN_12_BITS` | `.text` | 1,411 |
| `CFN_11_BITS` | `.text` | 1,161 |
| `CFN_11_G` | `.text` | 1,140 |
| `CFN_12_G` | `.text` | 1,140 |
| `CFN_14_G` | `.text` | 1,140 |
| `CFN_16_G` | `.text` | 1,140 |
| `CFN_32_G` | `.text` | 1,140 |
| `CFN_64_G` | `.text` | 1,140 |
| `CFN_11` | `.text` | 8 |
| `CFN_12` | `.text` | 8 |
| `CFN_14` | `.text` | 8 |
| `CFN_16` | `.text` | 8 |
| `CFN_32` | `.text` | 8 |
| `CFN_64` | `.text` | 8 |

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
| ordinary_ram | +1,056 |
| pool | +0 |
| xip | +84,692 |
| ram_code | +0 |
| noinit | +0 |

- Static allocations are not net savings of a replacement build.
- Symbol sizes do not attribute inlined/shared code to a feature.
- Included source is not proof that its functions survived linking.
- Runtime, core activation and audio margins still require device diagnostics.
