#include "gfx.h"
#include "dpad.h"

extern uint8_t CFG_DISPLAY_DPAD;

//unknown 00 is a pointer to some vector transformation when the sound is tied to an actor. actor + 0x3E, when not tied to an actor (map), always 80104394
//unknown 01 is always 4 in my testing
//unknown 02 is a pointer to some kind of audio configuration Always 801043A0 in my testing
//unknown 03 is always a3 in my testing
//unknown 04 is always a3 + 0x08 in my testing (801043A8)
typedef void(*playsfx_t)(uint16_t sfx, z64_xyzf_t *unk_00_, int8_t unk_01_ , float *unk_02_, float *unk_03_, float *unk_04_);
typedef void(*usebutton_t)(z64_game_t *game, z64_link_t *link, uint8_t item, uint8_t button);

#define z64_playsfx   ((playsfx_t)      0x800C806C)
#define z64_usebutton ((usebutton_t)    0x8038C9A0)

#define ADULT	0
#define CHILD	1

#define KOKIRI_BOOTS	1
#define IRON_BOOTS		2
#define HOVER_BOOTS		3

#define DEKU_SHIELD		1
#define HYLIAN_SHIELD	2

int hood_equipped = 0;

void handle_dpad() {
    uint16_t pad_pressed = z64_game.common.input[0].pad_pressed;

    if (CAN_USE_DPAD && DISPLAY_DPAD){
        if(z64_file.link_age == ADULT) {
			if (pad_pressed & DPAD_U) {
				if (z64_file.equip_boots == KOKIRI_BOOTS) return;
				else z64_file.equip_boots = KOKIRI_BOOTS;
				z64_UpdateEquipment(&z64_game, &z64_link);
				z64_playsfx(0x835, (z64_xyzf_t*)0x80104394, 0x04, (float*)0x801043A0, (float*)0x801043A0, (float*)0x801043A8);
			}

            if (pad_pressed & DPAD_L && z64_file.iron_boots) {
                if (z64_file.equip_boots == IRON_BOOTS) return;
                else z64_file.equip_boots = IRON_BOOTS;
                z64_UpdateEquipment(&z64_game, &z64_link);
                z64_playsfx(0x80D, (z64_xyzf_t*)0x80104394, 0x04, (float*)0x801043A0, (float*)0x801043A0, (float*)0x801043A8);
            }

            if ((pad_pressed & DPAD_R) && z64_file.hover_boots) {
                if (z64_file.equip_boots == HOVER_BOOTS) return;
                else z64_file.equip_boots = HOVER_BOOTS;
                z64_UpdateEquipment(&z64_game, &z64_link);
                z64_playsfx(0x835, (z64_xyzf_t*)0x80104394, 0x04, (float*)0x801043A0, (float*)0x801043A0, (float*)0x801043A8);
            }
		}
		else if (z64_file.link_age == CHILD) {
			if ((pad_pressed & DPAD_U) && z64_game.pause_ctxt.state == 0) {
				hood_equipped = !hood_equipped;
			}
			z64_file.scene_index = 0xF7 + hood_equipped;

			if (pad_pressed & DPAD_L && z64_file.deku_shield) {
				if (z64_file.equip_shield == DEKU_SHIELD) return;
				else z64_file.equip_shield = DEKU_SHIELD;
				z64_UpdateEquipment(&z64_game, &z64_link);
				z64_playsfx(0x835, (z64_xyzf_t*)0x80104394, 0x04, (float*)0x801043A0, (float*)0x801043A0, (float*)0x801043A8);
			}

			if ((pad_pressed & DPAD_R) && z64_file.hylian_shield) {
				if (z64_file.equip_shield == HYLIAN_SHIELD) return;
				else z64_file.equip_shield = HYLIAN_SHIELD;
				z64_UpdateEquipment(&z64_game, &z64_link);
				z64_playsfx(0x81F, (z64_xyzf_t*)0x80104394, 0x04, (float*)0x801043A0, (float*)0x801043A0, (float*)0x801043A8);
			}
		}

        if ((pad_pressed & DPAD_D) && CAN_USE_OCARINA){
            z64_usebutton(&z64_game,&z64_link,z64_file.items[0x07], 2);
        }
    }
}
void draw_dpad() {
    z64_disp_buf_t *db = &(z64_ctxt.gfx->overlay);
    if (DISPLAY_DPAD && CFG_DISPLAY_DPAD) {
        gSPDisplayList(db->p++, &setup_db);
        gDPPipeSync(db->p++);
        gDPSetCombineMode(db->p++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        uint16_t alpha = z64_game.hud_alpha_channels.minimap;
        
        if (alpha == 0xAA) alpha = 0xFF;
        gDPSetPrimColor(db->p++, 0, 0, 0xFF, 0xFF, 0xFF, alpha);
        sprite_load(db, &dpad_sprite, 0, 1);
        sprite_draw(db, &dpad_sprite, 0, 271, 64, 16, 16);

        if (alpha == 0xFF && !CAN_USE_DPAD)
            gDPSetPrimColor(db->p++, 0, 0, 0xFF, 0xFF, 0xFF, 0x46);

		if ((z64_file.iron_boots || z64_file.hover_boots) && z64_file.link_age == ADULT) {
			sprite_load(db, &items_sprite, Z64_ITEM_KOKIRI_BOOTS, 1);
			if (z64_file.equip_boots == KOKIRI_BOOTS) {
				sprite_draw(db, &items_sprite, 0, 271, 51, 16, 16);
			}
			else {
				sprite_draw(db, &items_sprite, 0, 273, 53, 12, 12);
			}
		}

        if (z64_file.iron_boots && z64_file.link_age==ADULT) {
            sprite_load(db, &items_sprite, 69, 1);
            if (z64_file.equip_boots == 2) {
                sprite_draw(db, &items_sprite, 0, 258, 64, 16, 16);
            }
            else {
                sprite_draw(db, &items_sprite, 0, 260, 66, 12, 12);
            }
        }

        if (z64_file.hover_boots && z64_file.link_age == ADULT) {
            sprite_load(db, &items_sprite, 70, 1);
            if (z64_file.equip_boots == 3) {
                sprite_draw(db, &items_sprite, 0, 283, 64, 16, 16);
            }
            else {
                sprite_draw(db, &items_sprite, 0, 285, 66, 12, 12);
            }
        }

		if (z64_file.link_age == CHILD) {
			sprite_load(db, &items_sprite, Z64_ITEM_BUNNY_HOOD, 1);
			if (hood_equipped) {
				sprite_draw(db, &items_sprite, 0, 271, 51, 16, 16);
			}
			else {
				sprite_draw(db, &items_sprite, 0, 273, 53, 12, 12);
			}
		}

		if (z64_file.deku_shield && z64_file.link_age == CHILD) {
			sprite_load(db, &items_sprite, Z64_ITEM_DEKU_SHIELD, 1);
			if (z64_file.equip_shield == DEKU_SHIELD) {
				sprite_draw(db, &items_sprite, 0, 258, 64, 16, 16);
            }
            else {
                sprite_draw(db, &items_sprite, 0, 260, 66, 12, 12);
            }
		}

		if (z64_file.hylian_shield && z64_file.link_age == CHILD) {
			sprite_load(db, &items_sprite, Z64_ITEM_HYLIAN_SHIELD, 1);
			if (z64_file.equip_shield == HYLIAN_SHIELD) {
				sprite_draw(db, &items_sprite, 0, 283, 64, 16, 16);
			}
			else {
				sprite_draw(db, &items_sprite, 0, 285, 66, 12, 12);
			}
		}

        if (z64_file.items[0x07] != -1){
            if(alpha==0xFF && !CAN_USE_OCARINA) gDPSetPrimColor(db->p++, 0, 0, 0xFF, 0xFF, 0xFF, 0x46);
            sprite_load(db, &items_sprite, z64_file.items[0x07], 1);
            sprite_draw(db, &items_sprite, 0, 273, 77, 12,12);
        }

        gDPPipeSync(db->p++);
    }
}

