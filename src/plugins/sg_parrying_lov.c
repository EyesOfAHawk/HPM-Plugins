//===== Hercules Plugin ======================================
//= Star Gladiator: WZ_VERMILION/LK_PARRYING while Soul Linked
//===== By ===================================================
//= Wolfie of BlackoutRO (https://blackout-ro.net)
//===== Version ==============================================
//= 1.0 - Idea from this Hercules thread:
// http://herc.ws/board/topic/16029-parrying-skill-star-gladiator-soul-link/
//===== Description ==========================================
//= When a Star Gladiator is linked, they will be able to
// use Lord of Vermilion and Parrying.
//===== Comments =============================================
//= Go to the 'Configuration' to configure which skills you
// want to be active and what level they should be available.
//= Do not claim this as your own work.
//= Do not sell this work. It is free to use.
//============================================================

#include "common/hercules.h" /* Should always be the first Hercules file included! (if you don't make it first, you won't be able to use interfaces) */

#include "common/nullpo.h"
#include "map/pc.h"

#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h" /* should always be the last Hercules file included! (if you don't make it last, it'll intentionally break compile time) */

HPExport struct hplugin_info pinfo = {
	"Star Gladiator LOV/Parrying",    // Plugin name
	SERVER_TYPE_MAP,// Which server types this plugin works with?
	"1.0",       // Plugin version
	HPM_VERSION, // HPM Version (don't change, macro is automatically updated)
};

// Configuration
bool lov = true; // Will linked Star Gladiator get Lord of Vermilion? (true/false)
int lov_lv = 10; // What level Lord of Vermilion will they get? (1-10)
bool parrying = true; // Will linked Star Gladiator get Parrying? (true/false)
int parrying_lv = 10; // What level Parrying will they get? (1-10)

void pc_calc_skilltree_clear_post(struct map_session_data *sd)
{
	nullpo_retv(sd);

	if (sd->sc.count && sd->sc.data[SC_SOULLINK] && sd->sc.data[SC_SOULLINK]->val2 == SL_STAR) {
		if (lov) {
			sd->status.skill[WZ_VERMILION].id = WZ_VERMILION;
			sd->status.skill[WZ_VERMILION].lv = lov_lv;
			sd->status.skill[WZ_VERMILION].flag = SKILL_FLAG_PERMANENT;
		}
		if (parrying) {
			sd->status.skill[LK_PARRYING].id = LK_PARRYING;
			sd->status.skill[LK_PARRYING].lv = parrying_lv;
			sd->status.skill[LK_PARRYING].flag = SKILL_FLAG_PERMANENT;
		}
	}
}

// This will make it so parrying will ignore weapon requirements for SG
struct skill_condition skill_get_requirement_post(struct skill_condition req, struct map_session_data *sd, uint16 skill_id, uint16 skill_lv)
{
	if (skill_id == LK_PARRYING && sd->sc.count && sd->sc.data[SC_SOULLINK] && sd->sc.data[SC_SOULLINK]->val2 == SL_STAR)
		req.weapon = skill->dbs->db[skill->get_index(NV_BASIC)].weapon;
	return req;
}


/* run when server starts */
HPExport void plugin_init (void)
{
	if (SERVER_TYPE == SERVER_TYPE_MAP) {
		addHookPost(pc, calc_skilltree_clear, pc_calc_skilltree_clear_post);
		addHookPost(skill, get_requirement, skill_get_requirement_post);
	}
}