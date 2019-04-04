//===== Hercules Plugin ======================================
//= @adopt
//===== By ===================================================
//= Wolfie of BlackoutRO (https://blackout-ro.net)
//===== Version ==============================================
//= 1.0
//===== Compatible With ======================================
//= Hercules 2019-04-04
//===== Description ==========================================
//= Adds @adopt atcommand to your server.
//===== Topic ================================================
//= http://herc.ws/board/topic/16679-adopt-atcommand/
//===== Additional Comments ==================================
//= Usage: @adopt <char name>
//= Check this link for an explanation on the adoption system:
// https://ragnarok.gamepedia.com/Adoption_System
//============================================================

#include "common/hercules.h"
#include "map/atcommand.h"
#include "map/map.h"
#include "map/pc.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"@adopt",
	SERVER_TYPE_MAP,
	"1.0",
	HPM_VERSION,
};

// @adopt - attempt to adopt a character
ACMD(adopt)
{
	char player_name[NAME_LENGTH] = "";

	if (!*message || sscanf(message, "%23s", player_name) != 1) {
		clif->message(fd, "Usage: @adopt <char name>");
		return false;
	}

	struct map_session_data *p_sd = map->charid2sd(sd->status.partner_id), *tsd = map->nick2sd(player_name);

	if (p_sd == NULL) {
		clif->message(fd, "Partner must be online and in your party.");
		return false;
	}

	if (tsd == NULL) {
		clif->message(fd, "Adoptee must be online, in your party and their job must be Novice or First Class.");
		return false;
	}

	if (sd == tsd || p_sd == tsd)
		return false; // Cannot adopt self or partner!

	if (!pc->can_Adopt(sd, p_sd, tsd)) {
		clif->message(fd, "Conditions: Parents must be childless, have Wedding Rings equipped, be in the same party and be over level 70. Adoptees cannot be married or have a child, and must be Novice or First Class job.");
		return false;
	}

	tsd->adopt_invite = sd->status.account_id;
	clif->adopt_request(tsd, sd, p_sd->status.account_id);
	clif->messagecolor_self(fd, COLOR_CYAN, "Adoption request sent.");

	return true;
}

static bool pc_adoption_post(bool ret, struct map_session_data *p1_sd, struct map_session_data *p2_sd, struct map_session_data *b_sd)
{
	if (ret && p1_sd != NULL && p2_sd != NULL && b_sd != NULL) {
		static char output1[CHAT_SIZE_MAX], output2[CHAT_SIZE_MAX];

		sprintf(output1, "Adoption successful! You are now the proud parent of %s.", b_sd->status.name);
		sprintf(output2, "Adoption successful! You are now the proud %s of %s and %s.", (b_sd->status.sex == SEX_MALE) ? "son" : "daughter", p1_sd->status.name, p2_sd->status.name);

		clif->message(p1_sd->fd, output1);
		clif->message(p2_sd->fd, output1);
		clif->message(b_sd->fd, output2);
	}

	return ret;
}


HPExport void plugin_init (void)
{
	addAtcommand("adopt", adopt);
	addHookPost(pc, adoption, pc_adoption_post);
}