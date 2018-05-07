//===== Hercules Plugin ======================================
//= TF_BACKSLIDING Animation Fix
//===== By ===================================================
//= Wolfie of BlackoutRO (https://blackout-ro.net)
//===== Version ==============================================
//= 1.0
//===== Description ==========================================
//= Removes clif->fixpos from Back Sliding, which causes the
//	animation not to occur.
//============================================================

#include "common/hercules.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "map/clif.h"
#include "map/map.h"
#include "map/mob.h"
#include "map/pc.h"
#include "map/skill.h"
#include "map/status.h"

#include "common/nullpo.h"
#include "common/timer.h"

#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
    "TF_BACKSLIDING", // Plugin name
    SERVER_TYPE_MAP, // Which server types this plugin works with?
    "1.0", // Plugin version
    HPM_VERSION, // HPM Version (don't change, macro is automatically updated)
};

///int skill_castend_nodamage_id(struct block_list *src, struct block_list *bl, uint16 skill_id, uint16 skill_lv, int64 tick, int flag)
int skill_castend_nodamage_id_pre(struct block_list **src_, struct block_list **bl_, uint16 *skill_id_, uint16 *skill_lv_, int64 *tick_, int *flag_)
{
	struct block_list *src = *src_, *bl = *bl_;
	uint16 skill_id = *skill_id_, skill_lv = *skill_lv_;
	int64 tick = *tick_;
	int flag = *flag_;

	struct map_session_data *sd;
	struct mob_data *dstmd;

	if ((skill_id > 0 && skill_lv <= 0) || skill_id != TF_BACKSLIDING)
		return 0;

	nullpo_retr(1, src);
	nullpo_retr(1, bl);

	if (src->m != bl->m)
		return 1;

	sd = BL_CAST(BL_PC, src);
	dstmd = BL_CAST(BL_MOB, bl);

	if (bl->prev == NULL || status->isdead(src))
		return 1;
	
	map->freeblock_lock();
	clif->skill_nodamage(src, bl, skill_id, skill_lv, 1);
	skill->blown(src, bl, skill->get_blewcount(skill_id, skill_lv), unit->getdir(bl), 0);

	if (dstmd) { //Mob skill event for no damage skills (damage ones are handled in battle_calc_damage) [Skotlex]
		mob->log_damage(dstmd, src, 0); //Log interaction (counts as 'attacker' for the exp bonus)
		mob->skill_event(dstmd, src, tick, MSC_SKILLUSED | (skill_id << 16));
	}
	if (sd && !(flag & 1)) { // ensure that the skill last-cast tick is recorded
		sd->canskill_tick = timer->gettick();

		if (sd->state.arrow_atk) // consume arrow on last invocation to this skill.
			battle->consume_ammo(sd, skill_id, skill_lv);
		skill->onskillusage(sd, bl, skill_id, tick);
		// perform skill requirement consumption
		skill->consume_requirement(sd, skill_id, skill_lv, 2);
	}

	map->freeblock_unlock();
	hookStop();
	return 0;
}

HPExport void plugin_init(void)
{
	addHookPre(skill, castend_nodamage_id, skill_castend_nodamage_id_pre);
}

HPExport void server_online(void)
{
    ShowInfo("'%s' Plugin by Wolfie/blackout-ro.net. Version '%s'\n", pinfo.name, pinfo.version);
}