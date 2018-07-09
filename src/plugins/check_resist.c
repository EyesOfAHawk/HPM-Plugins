//===== Hercules Plugin ======================================
//= check_resist() script command
//===== By ===================================================
//= Wolfie of BlackoutRO (https://blackout-ro.net)
//===== Version ==============================================
//= 1.0
//===== Description ==========================================
//= Checks how much resistance a character has to a particular
// element.
//
//= check_resist(<type>{, <account id>});
//= E.g. check_resist(Ele_Water);
//= E.g. check_resist(Ele_Wind, getcharid(CHAR_ID_ACCOUNT));
/* Types:
	Ele_Neutral:   0
	Ele_Water:     1
	Ele_Earth:     2
	Ele_Fire:      3
	Ele_Wind:      4
	Ele_Poison:    5
	Ele_Holy:      6
	Ele_Dark:      7
	Ele_Ghost:     8
	Ele_Undead:    9
*/
//============================================================

#include "common/hercules.h" /* Should always be the first Hercules file included! (if you don't make it first, you won't be able to use interfaces) */

#include "map/map.h"
#include "map/pc.h"
#include "map/script.h"

#include "common/HPMDataCheck.h" /* should always be the last Hercules file included! (if you don't make it last, it'll intentionally break compile time) */

HPExport struct hplugin_info pinfo = {
	"BUILDIN(check_resist)",    // Plugin name
	SERVER_TYPE_MAP,// Which server types this plugin works with?
	"1.0",       // Plugin version
	HPM_VERSION, // HPM Version (don't change, macro is automatically updated)
};

BUILDIN(check_resist)
{
	int type = script_getnum(st, 2);
	struct map_session_data *sd = (script_hasdata(st, 3)) ? script->id2sd(st, script_getnum(st, 3)) : script->rid2sd(st);

	if (sd != NULL)
		script_pushint(st, sd->subele[type]);
	else
		script_pushint(st, 0);

	return true;
}

/* run when server starts */
HPExport void plugin_init (void)
{
	addScriptCommand("check_resist", "i?", check_resist);
}

/* run when server is ready (online) */
HPExport void server_online (void)
{
	ShowInfo("'%s' Plugin by Wolfie of BlackoutRO. Version '%s'\n", pinfo.name, pinfo.version);
}
