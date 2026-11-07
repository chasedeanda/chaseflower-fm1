/* SPDX-License-Identifier: GPL-3.0-only
 * Presentation state only. No audio scheduling, buffers, callbacks or allocations.
 * Hidden workspaces do not advance effects. All targets replace their predecessor.
 */
/* Namespace tags for presentation identities, independent of hardware addresses. */
enum { CFN_CURSOR_HOME=1u<<16, CFN_FOCUS_LEGACY=1u<<18 };
static struct {
    uint32_t workspace, page_ms, focus_key, focus_ms, cursor_key, cursor_ms, tick, part_ms, quiet_ms;
    uint32_t audio_ms,received_ms;
    uint16_t energy;
    uint8_t valid, was_on, part_mask,parts,active;
    uint8_t have_level,fresh;
    int8_t direction;
} cfn_motion;
static void cfn_motion_frame(void)
{
    cf_visual_want=cf_motion;
    uint32_t key=cu.scr | (uint32_t)cu.page[cu.scr]<<8;
    if(cu.scr==SCR_EDIT)key|=(uint32_t)cu.edit_sec<<16 | (uint32_t)cu.edit_part<<24;
    if(cu.scr==SCR_ENV)key|=(uint32_t)cu.env_tgt<<16;
    if(cu.scr==SCR_FX)key|=(uint32_t)fxpu.mode<<16 | (uint32_t)cu.fx_pos<<24;
    if(!cf_motion) {
        cf_motion_epoch=cu.now;
        memset(&cfn_motion,0,sizeof cfn_motion);cfn_motion.workspace=key;
        cfn_motion.valid=1;cfn_motion.tick=cu.now;return;
    }
    if(!cfn_motion.was_on) {
        cfn_motion.workspace=key;cfn_motion.page_ms=0;cfn_motion.tick=cu.now;
        cfn_motion.valid=cfn_motion.was_on=1;
    } else if(cfn_motion.workspace!=key) {
        cfn_motion.workspace=key;cfn_motion.page_ms=cu.now?cu.now:1;
        cfn_motion.direction=cf_nav_dir<0?-1:1;
    }
    unsigned dt=cu.now-cfn_motion.tick;cfn_motion.tick=cu.now;if(dt>100)dt=100;
    /* The ISR publishes a post-MASTER block peak, including real effect tails. */
    /* Audio milliseconds have a different boot origin from cu.now on a device.
     * Track receipt/progress in UI time; never subtract one clock from the other. */
    if(!cfn_motion.have_level || cfn_motion.audio_ms!=cf_sn.visual_ms) {
        cfn_motion.audio_ms=cf_sn.visual_ms;cfn_motion.received_ms=cu.now;cfn_motion.have_level=1;
    }
    cfn_motion.fresh=cu.now-cfn_motion.received_ms<=150;
    unsigned raw=cf_sn.visual_peak*255u/32768u;
    if(!cfn_motion.fresh || cf_sn.visual_muted)raw=0;
    unsigned parts=0,mask=raw?cf_sn.visual_parts:0;for(unsigned bit=0;bit<4;bit++)parts+=(mask>>bit)&1;
    if(parts>=cfn_motion.parts){cfn_motion.parts=parts;cfn_motion.part_ms=cu.now;}
    else if(cu.now-cfn_motion.part_ms>=150){cfn_motion.parts=parts;cfn_motion.part_ms=cu.now;}
    int delta=(int)raw-cfn_motion.energy;
    int step=delta*(int)dt/(int)((delta>0?60:450)+dt);
    if(!step && dt && delta)step=delta>0?1:-1;
    cfn_motion.energy=(uint16_t)((int)cfn_motion.energy+step);
    if(cfn_motion.energy<3 && !raw)cfn_motion.energy=0;
    if(cfn_motion.energy>=10){cfn_motion.active=1;cfn_motion.quiet_ms=cu.now;}
    else if(cfn_motion.energy>=5)cfn_motion.quiet_ms=cu.now;
    else if(cu.now-cfn_motion.quiet_ms>=150)cfn_motion.active=0;
    if(cf_sn.visual_muted){cfn_motion.energy=cfn_motion.parts=cfn_motion.active=0;}
    cfn_motion.part_mask=raw?cf_sn.visual_parts:0;
}
static int cfn_page_dx(void)
{ return cf_tween_offset(cfn_motion.page_ms,170,8)*cfn_motion.direction; }
static unsigned cfn_focus(unsigned key)
{
    if(cfn_motion.focus_key!=key){cfn_motion.focus_key=key;cfn_motion.focus_ms=cf_motion?(cu.now?cu.now:1):0;}
    unsigned t=cu.now-cfn_motion.focus_ms;
    if(!cf_motion || !cfn_motion.focus_ms || (int32_t)(cfn_motion.focus_ms-cf_motion_epoch)<=0 || t>=100)return 0;
    return 2*(100-t)*(100-t)/10000;
}
static unsigned cfn_cursor(unsigned key)
{
    if((key&255u)==255u){cfn_motion.cursor_key=key;cfn_motion.cursor_ms=0;return 100;}
    if(cfn_motion.cursor_key!=key){cfn_motion.cursor_key=key;cfn_motion.cursor_ms=cf_motion?(cu.now?cu.now:1):0;}
    return 100u-(unsigned)cf_tween_offset(cfn_motion.cursor_ms,120,40);
}
static unsigned cfn_knob(unsigned k)
{
    if(!cf_motion || !cu.knob_ms[k] || (int32_t)(cu.knob_ms[k]-cf_motion_epoch)<=0)return 0;
    unsigned t=cu.now-cu.knob_ms[k];return t<90?((90-t)*(90-t)*20)/(90*90):0;
}
