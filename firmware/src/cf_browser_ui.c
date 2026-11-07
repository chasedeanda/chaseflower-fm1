/* SPDX-License-Identifier: GPL-3.0-only */
static const char *const CPB_ORDER_NAME[CPB_MODES]={"Catalog", "A-Z", "Shuffle", "Favorites"};
static struct { uint32_t menu_hold, save_hold; uint8_t idea, fired; } cpbu;
static unsigned cpb_target(void)
{
    unsigned idea=cpg.idea;
    fm1_irq_off();
    if(cpa.on)idea=cpa_setup.flags?cpa_setup.plan.idea:cpa.plan.idea;
    fm1_irq_on();
    return cf_progression_pick(idea,0);
}
static void cpb_changed_ui(void)
{ cpb_dirty=1; cpb_changed=cu.now; cpb_retry=0; }
static void cpb_star_ui(unsigned idea)
{
    int on=!cf_browser_favorite(&cpb,idea);
    cf_browser_star(&cpb,idea,on); cpb_changed_ui();
    cf_message(on?"Added to Favorites":"Removed from Favorites");
}
static void cf_progression_select(int d)
{
    if(!d)return;
    unsigned from=cpb_target();
    if(!cpb.n && cpb.mode==CPB_FAVORITES) {
        cf_message("No favorites yet\nHold SAVE on an idea"); return;
    }
    cf_progression_choose(cf_browser_pick(&cpb,from,d));
}
static void cpb_knob_info(int k, cf_knob_t *o)
{
    unsigned idea=cpb_target();
    if(k<2) { kfmt(o->name,12,k?"JUMP 1":"JUMP 10"); kint(o->val,idea+1,0); }
    if(k==2) { kfmt(o->name,12,"ORDER"); kfmt(o->val,14,CPB_ORDER_NAME[cpb.mode]); }
    if(k==3) { kfmt(o->name,12,"FAVORITE"); kfmt(o->val,14,cf_browser_favorite(&cpb,idea)?"Yes":"No"); }
}
static void cpb_knob_turn(int k,int d)
{
    unsigned idea=cpb_target(), count=cf_progression_count();
    if(k<2)cf_progression_choose(clampi((int)idea+d*(k?1:10),0,(int)count-1));
    if(k==2) {
        unsigned mode=wrapi(cpb.mode+d,CPB_MODES);
        cf_browser_order(&cpb,mode,cu.now ^ fm1_ms); cpb_changed_ui();
    }
    if(k==3) {
        int on=d>0;
        if(on!=cf_browser_favorite(&cpb,idea)) {
            cf_browser_star(&cpb,idea,on); cpb_changed_ui();
        }
    }
}
static int cpb_home(void)
{ return cu.scr==SCR_HOME && cf_scene()==CF_SCENE_PROGRESSIONS; }
static int cpb_page(void)
{ return cu.scr==SCR_GLO && cu.page[SCR_GLO]==CF_GLO_BROWSE; }
static void cpb_hold_poll(void)
{
    if(cpbu.menu_hold) {
        if(!(fm1_in.buttons>>panel.btn[B_GLO]&1u) || cu.scr!=SCR_GLO)cpbu.menu_hold=0;
        else if(cu.now-cpbu.menu_hold>=700u) {
            cu.page[SCR_GLO]=CF_GLO_BROWSE; cpbu.menu_hold=0; cu.pop=POP_NONE;
        }
    }
    if(!cpbu.save_hold)return;
    if(!cpb_home() && !cpb_page()) { cpbu.save_hold=0; return; }
    int held=fm1_in.buttons>>panel.btn[B_SAVE]&1u;
    if(held && !cpbu.fired && cu.now-cpbu.save_hold>=700u) {
        cpbu.fired=1; cpb_star_ui(cpbu.idea);
    } else if(!held) {
        int tap=!cpbu.fired; cpbu.save_hold=0;
        if(tap) { if(cpb_page())cpb_star_ui(cpbu.idea); else cf_save_open(); }
    }
}
