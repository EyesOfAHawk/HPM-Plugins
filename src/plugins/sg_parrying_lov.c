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
int extra_skill[] = {
	// Add the skills/levels here
	// Format: skill_id, skill_lv,
	WZ_VERMILION, 10,
	LK_PARRYING, 10
};
// End Configuration

void pc_calc_skilltree_clear_post(struct map_session_data *sd)
{
	int i, skill_id, skill_lv;
	nullpo_retv(sd);

	if (sd->sc.count && sd->sc.data[SC_SOULLINK] && sd->sc.data[SC_SOULLINK]->val2 == SL_STAR) {
		for (i = 0; i < ARRAYLENGTH(extra_skill); i += 2) {
			skill_id = extra_skill[i];
			if ((skill_lv = extra_skill[i+1]) > MAX_SKILL_LEVEL)
				skill_lv = MAX_SKILL_LEVEL;

			if (skill_id > 0 && skill_lv > 0) {
				sd->status.skill[skill_id].id = skill_id;
				sd->status.skill[skill_id].lv = skill_lv;
				sd->status.skill[skill_id].flag = SKILL_FLAG_PERMANENT;
			}
		}
	}
}

// This will make it so parrying will ignore weapon requirements for SG
struct skill_condition skill_get_requirement_post(struct skill_condition req, struct map_session_data *sd, uint16 skill_id, uint16 skill_lv)
{
	if (skill_id == LK_PARRYING && sd != NULL && sd->sc.count && sd->sc.data[SC_SOULLINK] && sd->sc.data[SC_SOULLINK]->val2 == SL_STAR)
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

/* run when server is ready (online) */
HPExport void server_online (void)
{
	if (SERVER_TYPE == SERVER_TYPE_MAP) {
		if (ARRAYLENGTH(extra_skill) % 2 != 0)
			ShowWarning("%s: extra_skill[] array size is not configured correctly (must be even - skill_id, skill_lv).\n", pinfo.name);
		else
			ShowInfo("'%s' Plugin by Wolfie of BlackoutRO. Version '%s'\n", pinfo.name, pinfo.version);
	}
}