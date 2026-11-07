/* SPDX-License-Identifier: GPL-3.0-only */
static void cvr_draw(void)
{
    const cf_progression_t *idea = cf_progression_get(cpg.idea);
    const char *detail = cvr.context == CV_HARMONY ? (idea ? idea->name : "No compatible suggestion") :
                         cvr.context == CV_DRUMS ? DM_RHY[dm_cur()].name :
                         cvr.context == CV_SOUND ? cf_snd[cvr.part].name : CV_LEVELS[cvr.level];
    fb_text_r(&CF_FONT_UI, 234, HEAD_LINE, CV_LEVELS[cvr.level], C_ACC(1), 1);
    fb_text_c(&CF_FONT_UI, 120, VIEW_Y + 10, CV_NAMES[cvr.context], C_ACC(0), 2);
    fb_text_c(&CF_FONT_UI, 120, VIEW_Y + 38, detail, FL_INK2, 1);
    fb_text_c(&CF_FONT_UI, 120, VIEW_Y + 58,
              cvr.pending ? "K4 cancels queued change" :
              cvr.context == CV_FX || cvr.context == CV_SOUND ? "SAVE keeps a preset" :
              cvr.context == CV_HARMONY ? "See HOME Progressions" : "Settings kept automatically", FL_INK2, 1);
}
