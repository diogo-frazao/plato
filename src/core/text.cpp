#include "text.h"
#include "log.h"
#include "systems.h"

TextDTO getTextInfo(TextType textType)
{
	// TODO switch on language
	switch (textType)
	{

	// Marketing dialogue
	{
	case INVALID_TEXT:
		return "Invalid Text";
	case DEBUG_TEXT:
		return "Is that SR.DARWIN?";
	case ETC_TEXT:
		return "...";
	case MARKETING_PHONE_1:
		return "Goooooood afternoon";
	case MARKETING_PHONE_2:
		return "Do I have the pleasure of speaking with Mr.ROSTOV?";
		case MARKETING_PHONE_2_A:
			return "Yes...";
			case MARKETING_PHONE_2_A_1:
				return "PERFECT! I call you with good news sir";
		case MARKETING_PHONE_2_B:
			return { "Who are you?", CELLPHONE_DIALOGUE, 10 , HIGH_TENSION };
			case MARKETING_PHONE_2_B_1:
				return "Oh";
			case MARKETING_PHONE_2_B_2:
				return "My name is FREDERICO from KELIA's marketing department";
		case MARKETING_PHONE_2_C:
			return { "Shut the fuck up", CELLPHONE_DIALOGUE, 20, HIGH_TENSION };
			case MARKETING_PHONE_2_C_1:
				return { "What did you just say to me?", CELLPHONE_DIALOGUE, 10, HIGH_TENSION };
			case MARKETING_PHONE_2_C_2:
				return { "DO YOU KNOW WHO I AM", CELLPHONE_DIALOGUE, 10, HIGH_TENSION };
			case MARKETING_PHONE_2_C_3:
				return { "I BRING TO YOU THE HOLY TECHNOLOGY FROM FATHER EDWARD HIMSELF", CELLPHONE_DIALOGUE, 10, HIGH_TENSION };
			case MARKETING_PHONE_2_C_4:
				return { "AND YOU HAVE THE AUDACITY TO RECEIVE IT LIKE THIS?", CELLPHONE_DIALOGUE, 10, HIGH_TENSION };
				case MARKETING_PHONE_2_C_4_A:
					return {"YES < HANG UP >", CELLPHONE_DIALOGUE, 10, FATAL_TENSION };
				case MARKETING_PHONE_2_C_4_B:
					return { "FUCK YOU < HANG UP >", CELLPHONE_DIALOGUE, 20, FATAL_TENSION };
	case MARKETING_PHONE_3:
		return "I bring to you a deal too good to miss!";
		case MARKETING_PHONE_3_A:
			return "I'm listning";
			case MARKETING_PHONE_3_A_1:
				return "PERFECT";
		case MARKETING_PHONE_3_B:
			return { "I don't care", CELLPHONE_DIALOGUE, 10 , HIGH_TENSION};
			case MARKETING_PHONE_3_B_1:
				return "...";
			case MARKETING_PHONE_3_B_2:
				return "Mr.ROSTOV please listen, I'm sure you'll be interested";
	case MARKETING_PHONE_4:
		return "(clears throath)";
	case MARKETING_PHONE_5:
		return { "It's with great honor that I present to you sir, the newest creation from FATHER EDWARD'S Research Department team, here at KELIA", CELLPHONE_DIALOGUE, 5};
	case MARKETING_PHONE_6:
		return { "AND FOR ONLY 499D, MR.ROSTOV CAN HAVE ACCESS TO IT", CELLPHONE_DIALOGUE, 5 };
	case MARKETING_PHONE_7:
		return "And what is this creation I hear you ask?";
		case MARKETING_PHONE_7_A:
			return "I'm not interested";
			case MARKETING_PHONE_7_A_1:
				return "Wait sir now I'm sure you'll be interested";
		case MARKETING_PHONE_7_B:
			return { "I didn't ask shit", CELLPHONE_DIALOGUE, 20 , HIGH_TENSION };
		case MARKETING_PHONE_7_C_LOW_TENSION:
			return "...";
		case MARKETING_PHONE_7_C_HIGH_TENSION:
			return { "< HANG UP >", CELLPHONE_DIALOGUE, 5, FATAL_TENSION };
	case MARKETING_PHONE_8:
		return { "WITH THIS NEW APPLICATION EVERYONE CAN TRACK THE CITY TRAINS IN REAL TIME", CELLPHONE_DIALOGUE, 10 };
	case MARKETING_PHONE_9:
		return { "NO MORE WAITING", CELLPHONE_DIALOGUE, 5 };
	case MARKETING_PHONE_10:
		return { "NO MORE MISSING TRAINS", CELLPHONE_DIALOGUE, 5 };
	case MARKETING_PHONE_11:
		return "ALL OF THIS... for only 499D";
		case MARKETING_PHONE_11_A:
			return "I don't have the money";
			case MARKETING_PHONE_11_A_1:
				return { "What do you mean you don't have the money?", CELLPHONE_DIALOGUE, 10 };
		case MARKETING_PHONE_11_B_LOW_TENSION:
			return { "STOP YELLING", CELLPHONE_DIALOGUE, 10, HIGH_TENSION };
			case MARKETING_PHONE_11_B_1:
				return { "What did you just say?", CELLPHONE_DIALOGUE, 10 };
		case MARKETING_PHONE_11_B_HIGH_TENSION:
			return { "< HANG UP >", CELLPHONE_DIALOGUE, 5, FATAL_TENSION };
	case MARKETING_PHONE_12:
		return { "I bring you the holy technology from FATHER EDWARD himself", CELLPHONE_DIALOGUE, 10 };
	case MARKETING_PHONE_13:
		return { "A man so humble that still helps those that live outside of KELIA!", CELLPHONE_DIALOGUE, 10 };
	case MARKETING_PHONE_14:
		return "and you receive it like this?";
		case MARKETING_PHONE_14_A:
			return { "FUCK YOU < HANG UP >", CELLPHONE_DIALOGUE, 20, FATAL_TENSION };
		case MARKETING_PHONE_14_B:
			return { "GOODBYE < HANG UP >", CELLPHONE_DIALOGUE, 10, FATAL_TENSION };
	}

	// Starting dialogue with dad
	{
	case ONE_DAD_PHONE_1:
		return "Halo? *cough* *cough* ROSTOV?";
	case ONE_DAD_PHONE_2:
		return "Can you hear me son?";
		case ONE_DAD_PHONE_2_A:
			return { "Oh, it's you Pa", CELLPHONE_DIALOGUE, -100 , LOW_TENSION};
		case ONE_DAD_PHONE_2_B:
			return { "Yes Pa", CELLPHONE_DIALOGUE, - 100 , LOW_TENSION};
	case ONE_DAD_PHONE_3:
		return "I was trying to call you, is everything ok?";
		case ONE_DAD_PHONE_3_A:
			return "Yes and you?";
			case ONE_DAD_PHONE_3_A_1:
				return "*cough* *cough*";
			case ONE_DAD_PHONE_3_A_2:
				return "The usual...";
				case ONE_DAD_PHONE_3_A_2_A:
					return "But any news?";
				case ONE_DAD_PHONE_3_A_2_B:
					return "What else?";
				case ONE_DAD_PHONE_3_A_2_C:
					return "Something to tell me?";
			case ONE_DAD_PHONE_3_A_3:
				return "ROSTOV, stop it";
			case ONE_DAD_PHONE_3_A_4:
				return "If there's something you want to know, *cough*, just say it";
		case ONE_DAD_PHONE_3_B:
			return { "A solicitor called me", CELLPHONE_DIALOGUE, 10, HIGH_TENSION };
			case ONE_DAD_PHONE_3_B_1:
				return "Oh brilliant...";
			case ONE_DAD_PHONE_3_B_2:
				return "*cough* *cough*";
			case ONE_DAD_PHONE_3_B_3:
				return "Another one from the cult";
				case ONE_DAD_PHONE_3_B_3_A:
					return "They are all the same";
				case ONE_DAD_PHONE_3_B_3_B:
					return { "I hate them", CELLPHONE_DIALOGUE, 10, HIGH_TENSION };
			case ONE_DAD_PHONE_3_B_4:
				return "I know son, I know...";
			case ONE_DAD_PHONE_3_B_5:
				return { "But let's talk about something else", CELLPHONE_DIALOGUE, -5};
		case ONE_DAD_PHONE_4:
			return "...";
			case ONE_DAD_PHONE_4_A:
				return "How did your exam go?";
			case ONE_DAD_PHONE_4_B:
				return "Do you know the results already?";
			case ONE_DAD_PHONE_4_C:
				return { "I miss you pa", CELLPHONE_DIALOGUE , -100, LOW_TENSION };
				case ONE_DAD_PHONE_4_C_1:
					return "...";
				case ONE_DAD_PHONE_4_C_2:
					return "Cmon ROSTOV, you're going to make your old man cry";
				case ONE_DAD_PHONE_4_C_3:
					return "You know I can't have any visits...";
		case ONE_DAD_PHONE_5:
			return "Oh...";
		case ONE_DAD_PHONE_6:
			return { "No no news yet, it will still take a while", CELLPHONE_DIALOGUE, 5 };
			case ONE_DAD_PHONE_6_A:
				return "Are you sure?";
			case ONE_DAD_PHONE_6_B:
				return { "Be honest", CELLPHONE_DIALOGUE, 5 , HIGH_TENSION};
		case ONE_DAD_PHONE_7:
			return "ROSTOV, please *cough* cough*";
		case ONE_DAD_PHONE_8:
			return "With the money you send I'm getting the proper treatment";
		case ONE_DAD_PHONE_9:
			return "But the truth is the doctor told me that";
	}

	// Starting dialogue with Darwin
	{
	case ONE_DARWIN_1:
		return { "ROSTOV your shift is o-", DARWIN_DIALOGUE };
	case ONE_DARWIN_2:
		return {"Oh. I'm sorry, I didn't know you were on the phone.", DARWIN_DIALOGUE };
		case ONE_DARWIN_2_A:
			return "No worries DARWIN";
		case ONE_DARWIN_2_B:
			return "I'm talking with Pa";
	case ONE_DARWIN_3:
		return "SR.DARWIN! *cough* *cough*";
	case ONE_DARWIN_4:
		return "Did ROSTOV do a good job today?";
	case ONE_DARWIN_5:
		return { "I hope he didn't fell asleep again", CELLPHONE_DIALOGUE, 10 };
	case ONE_DARWIN_6:
		return { "Ahahah. Don't worry MISTER", DARWIN_DIALOGUE };
	case ONE_DARWIN_7:
		return { "ROSTOV works very hard and helps me every day", DARWIN_DIALOGUE, -10 };
	case ONE_DARWIN_8:
		return { "...", DARWIN_DIALOGUE };
	case ONE_DARWIN_9:
		return { "MISTER what did the doctors say about your exam?", DARWIN_DIALOGUE };
	case ONE_DARWIN_10:
		return { "Can he hear me?", DARWIN_DIALOGUE };
		case ONE_DARWIN_10_A:
			return "Pa are you there?";
		case ONE_DARWIN_10_B:
			return "Did you hear SR.DARWIN?";
		case ONE_DARWIN_10_C:
			return "Papa?";
	case ONE_DARWIN_11:
		return "Sorry the doctor is here *cough* *cough* I need to go";
	case ONE_DARWIN_12:
		return { "Cmon ROSTOV don't worry... I'm sure he will recover", DARWIN_DIALOGUE };
	case ONE_DARWIN_13:
		return {"And you are a good man, still working after all these years to help your Pa", DARWIN_DIALOGUE };
		case ONE_DARWIN_13_A:
			return "Thank you DARWIN";
		case ONE_DARWIN_13_B:
			return { "He's all I have", CELLPHONE_DIALOGUE , -5, LOW_TENSION};
		case ONE_DARWIN_13_C:
			return { "It is what it is", CELLPHONE_DIALOGUE, 5, HIGH_TENSION};
	case ONE_DARWIN_14:
		return { "Well... your shift is over ROSTOV, you can go home when you want.", DARWIN_DIALOGUE };
	case ONE_DARWIN_15:
		return { "HEY!! STOP IT!!", DARWIN_DIALOGUE, 10 };
	case ONE_DARWIN_16:
		return { "WHAT ARE YOU DOING HERE??", DARWIN_DIALOGUE, 10 };

	}

	// Starting confrontation with hugo and oskar
	{
	case C_1:
		return { "That's right, if BIG DIESEL says you owe money, YOU DO", OSKAR_DIALOGUE };
	case C_2:
		return { "It can't be... Maybe you are mistaken?", DARWIN_DIALOGUE };
	case C_3:
		return { "ahahHAHAH", HUGO_DIALOGUE };
	case C_4:
		return { "ROSTOV, please don't get involved", DARWIN_DIALOGUE };
		case C_4_A:
			return { "What's going on?", CHOICE_DIALOGUE };
			case C_4_A_1:
				return { "Isn't it obvious you OLD PIG!?", OSKAR_DIALOGUE, 5};
			case C_4_A_2:
				return { "BIG DIESEL sent us to collect OUR money", OSKAR_DIALOGUE };
			case C_4_A_3:
				return { "Because SR.DARWIN here owes us some", OSKAR_DIALOGUE };
			case C_4_A_4:
				return { "That is not true and we all know it", DARWIN_DIALOGUE };
			case C_4_A_5:
				return { "WHAT did you just say old man?", HUGO_DIALOGUE, 5 };
			case C_4_A_6:
				return { "Are YOU calling BIG DIESEL A LIAR?", HUGO_DIALOGUE, 5 };
				case C_4_A_6_A:
					return { "Let's all calm down", CHOICE_DIALOGUE };
				case C_4_A_6_B:
					return { "I am", CHOICE_DIALOGUE, 10, HIGH_TENSION};
					case C_4_A_6_AB_1:
						return { "AHAHAHAH", HUGO_DIALOGUE };
					case C_4_A_6_AB_2:
						return { "Listen ROSTOV...", OSKAR_DIALOGUE };
					case C_4_A_6_AB_3:
						return { "I'm here to get our money, not to talk", OSKAR_DIALOGUE };
				case C_4_A_6_C:
					return { "I'VE HEARD ENOUGH < LEAVE DIALOGUE >", CHOICE_DIALOGUE, 20, FATAL_TENSION};
		case C_4_B:
			return { "I already did", CHOICE_DIALOGUE, 5, HIGH_TENSION };
		case C_4_C:
			return { "What are these kids doing here?", CHOICE_DIALOGUE , 10, HIGH_TENSION };
			case C_4_BC_1:
				return { "ahahAHAHAHah", HUGO_DIALOGUE };
			case C_4_BC_2:
				return { "OSKAR, make sure this PIG doesn't get in the way of our money", HUGO_DIALOGUE };
				case C_4_BC_2_A:
					return { "We don't have your money", CHOICE_DIALOGUE };
				case C_4_BC_2_B:
					return { "I can feel your breath", CHOICE_DIALOGUE };
					case C_4_BC_2_B_1:
						return { "ahAHAHAHA", HUGO_DIALOGUE };
					case C_4_BC_2_B_2:
						return { "SHUT THE FUCK UP ROSTOV", OSKAR_DIALOGUE };
				case C_4_BC_2_C:
					return { "YOU ASKED FOR IT < LEAVE DIALOGUE >", CHOICE_DIALOGUE };
					case C_4_BC_2_C_CUE_1:
						return { "Asking for what heh?", OSKAR_DIALOGUE };
					case C_4_BC_2_C_CUE_2:
						return { "Be careful with that golf cue ROSTOV, your arm might fall off", HUGO_DIALOGUE };
					case C_4_BC_2_C_CUE_3:
						return { "AHAHAHA", OSKAR_DIALOGUE };
					case C_4_BC_2_C_1:
						return { "Asking for what heh?", OSKAR_DIALOGUE };
					case C_4_BC_2_C_2:
						return { "ROSTOV grab that golf cue near the table", DARWIN_DIALOGUE };
					case C_4_BC_2_C_3:
						return { "But be careful ROSTOV, your arm might fall off trying to pick it up", HUGO_DIALOGUE };
					case C_4_BC_2_C_4:
						return { "AHAHAHAH", OSKAR_DIALOGUE };
					case C_4_BC_2_C_5:
						return { "That was a good one brother", OSKAR_DIALOGUE };
	}
	}
	D_ASSERT(false, "Invalid text for type: %i", textType);
	return "Invalid Text";
}

void updateDialogueColorsAndOffsetForEntity(DialogueEntityType dialogueColorsType)
{
	switch (dialogueColorsType)
	{
	case INVALID_DIALOGUE_ENTITY:
		D_ASSERT(false, "Invalid color");
		break;
	case CELLPHONE_DIALOGUE:
		s_currentDialogueEntityDTO.dialogueBoxColor = { 9, 7, 19 };
		s_currentDialogueEntityDTO.outlineColor = { 27, 52, 45 };
		s_currentDialogueEntityDTO.textColor = { 145, 210, 104 };
		s_currentDialogueEntityDTO.dialoguePositionOffset = { 22.f, 9.f };
		break;
	case DARWIN_DIALOGUE:
		s_currentDialogueEntityDTO.dialogueBoxColor = { 25, 11, 13 };
		s_currentDialogueEntityDTO.outlineColor = { 160, 63, 39 };
		s_currentDialogueEntityDTO.textColor = { 210, 104, 104 };
		s_currentDialogueEntityDTO.dialoguePositionOffset = { 6.f, 0.f };
		break;
	case OSKAR_DIALOGUE:
		s_currentDialogueEntityDTO.dialogueBoxColor = { 25, 11, 13 };
		s_currentDialogueEntityDTO.outlineColor = { 61, 49, 63 };
		s_currentDialogueEntityDTO.textColor = { 193, 138, 106 };
		s_currentDialogueEntityDTO.dialoguePositionOffset = { 6.f, 0.f };
		break;
	case HUGO_DIALOGUE:
		s_currentDialogueEntityDTO.dialogueBoxColor = { 25, 11, 13 };
		s_currentDialogueEntityDTO.outlineColor = { 79, 38, 32 };
		s_currentDialogueEntityDTO.textColor = { 251, 185, 84 };
		s_currentDialogueEntityDTO.dialoguePositionOffset = { 6.f, 0.f };
		break;
	default:
		D_ASSERT(false, "Unsupported dialogue entity type");
		break;
	}
}