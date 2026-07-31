#pragma once

#include <stdint.h>
#include "constants.h"
#include "lib.h"
#include <SDL3/SDL_pixels.h>

enum TextType
{
	INVALID_TEXT,
	DEBUG_TEXT,
	ETC_TEXT,
	// Marketing dialogue
	MARKETING_PHONE_1,
	MARKETING_PHONE_2,
	MARKETING_PHONE_2_A,
	MARKETING_PHONE_2_A_1,
	MARKETING_PHONE_2_B,
	MARKETING_PHONE_2_B_1,
	MARKETING_PHONE_2_B_2,
	MARKETING_PHONE_2_C,
	MARKETING_PHONE_2_C_1,
	MARKETING_PHONE_2_C_2,
	MARKETING_PHONE_2_C_3,
	MARKETING_PHONE_2_C_4,
	MARKETING_PHONE_2_C_4_A,
	MARKETING_PHONE_2_C_4_B,
	MARKETING_PHONE_3,
	MARKETING_PHONE_3_A,
	MARKETING_PHONE_3_A_1,
	MARKETING_PHONE_3_B,
	MARKETING_PHONE_3_B_1,
	MARKETING_PHONE_3_B_2,
	MARKETING_PHONE_4,
	MARKETING_PHONE_5,
	MARKETING_PHONE_6,
	MARKETING_PHONE_7,
	MARKETING_PHONE_7_A,
	MARKETING_PHONE_7_A_1,
	MARKETING_PHONE_7_B,
	MARKETING_PHONE_7_C_LOW_TENSION,
	MARKETING_PHONE_7_C_HIGH_TENSION,
	MARKETING_PHONE_8,
	MARKETING_PHONE_9,
	MARKETING_PHONE_10,
	MARKETING_PHONE_11,
	MARKETING_PHONE_11_A,
	MARKETING_PHONE_11_A_1,
	MARKETING_PHONE_11_B_LOW_TENSION,
	MARKETING_PHONE_11_B_HIGH_TENSION,
	MARKETING_PHONE_11_B_1,
	MARKETING_PHONE_12,
	MARKETING_PHONE_13,
	MARKETING_PHONE_14,
	MARKETING_PHONE_14_A,
	MARKETING_PHONE_14_B,
	// Starting dialogue with dad
	ONE_DAD_PHONE_1,
	ONE_DAD_PHONE_2,
	ONE_DAD_PHONE_2_A,
	ONE_DAD_PHONE_2_B,
	ONE_DAD_PHONE_3,
	ONE_DAD_PHONE_3_A,
	ONE_DAD_PHONE_3_A_1,
	ONE_DAD_PHONE_3_A_2,
	ONE_DAD_PHONE_3_B,
	ONE_DAD_PHONE_3_B_1,
	ONE_DAD_PHONE_3_B_2,
	ONE_DAD_PHONE_3_B_3,
	ONE_DAD_PHONE_3_B_3_A,
	ONE_DAD_PHONE_3_B_3_B,
	ONE_DAD_PHONE_3_B_4,
	ONE_DAD_PHONE_4,
	ONE_DAD_PHONE_4_A,
	ONE_DAD_PHONE_4_B,
	ONE_DAD_PHONE_4_C,
	ONE_DAD_PHONE_4_C_1,
	ONE_DAD_PHONE_4_C_2,
	ONE_DAD_PHONE_5,
	ONE_DAD_PHONE_6,
	ONE_DAD_PHONE_6_A,
	ONE_DAD_PHONE_6_B,
	ONE_DAD_PHONE_7,
	ONE_DAD_PHONE_8,
	ONE_DAD_PHONE_9,
	ONE_DARWIN_1,
	ONE_DARWIN_2,
	ONE_DARWIN_2_A,
	ONE_DARWIN_2_B,
	ONE_DARWIN_3,
	ONE_DARWIN_4,
	ONE_DARWIN_5,
	ONE_DARWIN_6,
	ONE_DARWIN_7,
	ONE_DARWIN_8,
	ONE_DARWIN_9,
	ONE_DARWIN_10,
	ONE_DARWIN_10_A,
	ONE_DARWIN_10_B,
	ONE_DARWIN_10_C,
	ONE_DARWIN_11,
	ONE_DARWIN_12,
	ONE_DARWIN_13,
	ONE_DARWIN_13_A,
	ONE_DARWIN_13_B,
	ONE_DARWIN_13_C,
	ONE_DARWIN_14,
	ONE_DARWIN_15,
	ONE_DARWIN_16,
	C_1,
	C_2,
	C_3,
	C_4,
	C_4_A,
	C_4_B,
	C_4_BC_1,
	C_4_BC_2,
	C_4_BC_3,
	C_4_BC_3_A,
	C_4_BC_3_B,
	C_4_BC_2_3_1,
	C_4_BC_2_3_2,
	C_4_BC_2_3_3,
	C_4_BC_3_C,
	C_4_BC_3_C_CUE_1,
	C_4_BC_3_C_CUE_2,
	C_4_BC_3_C_CUE_3,
	C_4_BC_3_C_1,
	C_4_BC_3_C_2,
	C_4_BC_3_C_3,
	C_4_BC_3_C_4,
	C_4_BC_3_C_5,
	C_4_C,
	C_4_A_1,
	C_4_A_2,
	C_4_A_3,
	C_4_A_4,
	C_4_A_5,
	C_4_A_6,
	C_4_A_6_A,
	C_4_A_6_B,
	C_4_A_6_C,
	C_4_A_6_AB_1,
	C_4_A_6_AB_2,
	C_4_A_6_AB_3,
	D_1,
	D_2,
	D_3,
	D_4,
	D_5,
	D_6,
	D_7,
	D_8,
	D_9,
	D_10,
	D_11,
	D_12,
	D_12_A,
	D_12_A_1,
	D_12_A_2,
	D_12_A_2_A,
	D_12_A_2_B,
	D_12_B,
	D_12_B_1,
	D_12_B_2,
	D_12_B_2_A,
	D_12_B_2_B,
	D_13_LOW_TENSION,
	D_13_HIGH_TENSION,
	D_14,
	E_1,
	E_2,
	E_3,
	E_4,
	E_5,
	E_6,
	E_7,
	E_7_A,
	E_7_A_1,
	E_7_A_2,
	E_7_A_2_A,
	E_7_A_2_A_1,
	E_7_A_2_A_2,
	E_7_A_2_A_3,
	E_7_A_2_A_3_A,
	E_7_A_2_A_3_A_1,
	E_7_A_2_A_3_A_2,
	E_7_A_2_A_3_A_3,
	E_7_A_2_A_3_A_4,
	E_7_A_2_A_3_A_5,
	E_7_A_2_A_3_A_5_A,
	E_7_A_2_A_3_A_5_A_1,
	E_7_A_2_A_3_A_5_A_2,
	E_7_A_2_A_3_A_5_A_3,
	E_7_A_2_A_3_A_5_A_3_A,
	E_7_A_2_A_3_A_5_A_3_B,
	E_7_A_2_A_3_A_5_A_3_C,
	E_7_A_2_A_3_A_5_A_4,
	E_7_A_2_A_3_A_5_A_5,
	E_7_A_2_A_3_A_5_A_5_A,
	E_7_A_2_A_3_A_5_A_5_B,
	E_7_A_2_A_3_A_5_B,
	E_7_A_2_A_3_A_5_B_1,
	E_7_A_2_A_3_A_5_B_2,
	E_7_A_2_A_3_A_5_B_3,
	E_7_A_2_A_3_A_5_B_4,
	E_7_A_2_A_3_B,
	E_7_A_2_A_3_B_1,
	E_7_A_2_A_3_B_2,
	E_7_A_2_A_3_B_3,
	E_7_A_2_A_3_B_4,
	E_7_A_2_A_3_B_4_A,
	E_7_A_2_A_3_B_4_B,
	E_7_A_2_A_3_B_5,
	E_7_A_2_A_3_B_6,
	E_7_A_2_A_3_B_7,
	E_7_A_2_A_3_B_8,
	E_7_A_2_A_3_B_8_A,
	E_7_A_2_A_3_B_8_B,
	E_7_A_2_A_3_B_9,
	E_7_A_2_A_3_B_10,
	E_7_A_2_A_3_B_11,
	E_7_A_2_A_3_B_12,
	E_7_A_2_A_3_B_13,
	E_7_A_2_A_3_B_13_A,
	E_7_A_2_A_3_B_13_B,
	E_7_A_2_B,
	E_7_A_2_B_1,
	E_7_B,
	E_N_1,
	E_N_2,
	E_N_2_A,
	E_N_2_A_1,
	E_N_2_A_1_A,
	E_N_2_A_1_B,
	E_N_2_B,
	E_7_1,
	E_8,
	E_9,
	E_10,
	E_11,
	E_12,
	E_13,
	E_14,
	E_15,
	E_16,
	E_17,
	E_18,
	E_19,
	E_20,
	E_21,
	E_22,
	E_23,
	E_24,
};

enum TextTensionType
{
	NORMAL_TENSION,
	LOW_TENSION,
	HIGH_TENSION,
	FATAL_TENSION,
};

enum DialogueEntityType
{
	INVALID_DIALOGUE_ENTITY,
	CHOICE_DIALOGUE,
	CELLPHONE_DIALOGUE,
	DARWIN_DIALOGUE,
	OSKAR_DIALOGUE,
	HUGO_DIALOGUE,
	BIG_DISEL_DIALOGUE,
};

struct TextDTO
{
	char* text = nullptr;
	int8_t playerTensionDelta = 0;
	TextTensionType tensionType = NORMAL_TENSION;
	DialogueEntityType entityTalking = INVALID_DIALOGUE_ENTITY;

	TextDTO(char* text, DialogueEntityType entity = CELLPHONE_DIALOGUE, int8_t playerTensionDelta = 0, TextTensionType tensionType = NORMAL_TENSION)
	{
		this->text = text;
		this->entityTalking = entity;
		this->playerTensionDelta = playerTensionDelta;
		this->tensionType = tensionType;
	}
};

TextDTO getTextInfo(TextType textTye);

struct DialogueEntityDTO
{
	SDL_Color dialogueBoxColor;
	SDL_Color outlineColor;
	SDL_Color textColor;
	int32_t entityId = k_invalidId;

	Vec2 dialoguePositionOffset{ 0.f, 0.f };
};

inline DialogueEntityDTO s_currentDialogueEntityDTO{};
void updateDialogueColorsAndOffsetForEntity(DialogueEntityType entityTalking);

enum TextEffectType
{
	INVALID_EFFECT,
	//Colors
	PINK_EFFECT, // Love/happiness
	LIGHT_PINK_EFFECT, // Use for Rostov - Dad relationship
	BLUE_EFFECT, // Important things
	YELLOW_EFFECT, // Also important things
	RED_EFFECT, // Negative but not necessarily mad
	FATHER_EDWARD_EFFECT, // He has a custom color
	//Wave
	WAVE_EFFECT, // Playful
	PINK_WAVE_EFFECT, // Playful + love/happiness
	//Shake
	INTERJECTION_EFFECT, // *cough* *ahem*, etc. Camera shake + show all characters at the same time + small delay at the end
	RED_SHAKE_EFFECT, // Camera shake + Mad / Objections
	//No Wait
	NO_WAIT_EFFECT, // If the last word didn't end ex: "Rostov your shift is o-". The wait time to move to the next dialogue is 0.5
};

TextEffectType getTextEffectTypeFromName(char* effectName);
