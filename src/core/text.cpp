#include "text.h"
#include "log.h"

TextDTO getTextInfo(TextType textType)
{
	// TODO switch on language
	switch (textType)
	{
	case INVALID_TEXT:
		return "Invalid Text";
	case DEBUG_TEXT:
		return "yes yes of course";
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
			return { "Who are you?", 10 };
			case MARKETING_PHONE_2_B_1:
				return "Oh";
			case MARKETING_PHONE_2_B_2:
				return "My name is FREDERICO from Kelia's marketing department";
	case MARKETING_PHONE_3:
		return "I bring to you a deal too good to miss!";
		case MARKETING_PHONE_3_A:
			return "I'm listning";
			case MARKETING_PHONE_3_A_1:
				return "PERFECT";
		case MARKETING_PHONE_3_B:
			return "I don't care";
			case MARKETING_PHONE_3_B_1:
				return "...";
			case MARKETING_PHONE_3_B_2:
				return "Mr.ROSTOV please listen, I'm sure you'll be interested";
	case MARKETING_PHONE_4:
		return "(clears throath)";
	case MARKETING_PHONE_5:
		return "It's with great honor that I present to you sir, the newest creation from FATHER EDWARD'S Research Department team, here at Kelia";
	case MARKETING_PHONE_6:
		return "AND FOR ONLY 499D, MR.ROSTOV CAN HAVE ACCESS TO IT";
	case MARKETING_PHONE_7:
		return "And what is this creation I hear you ask?";
		case MARKETING_PHONE_7_A:
			return "I'm not interested";
			case MARKETING_PHONE_7_A_1:
				return "Wait sir now I'm sure you'll be interested";
		case MARKETING_PHONE_7_B:
			return "Shut the fuck up";
		case MARKETING_PHONE_7_C_LOW_TENSION:
			return "...";
		case MARKETING_PHONE_7_C_HIGH_TENSION:
			return "< Hang up >";
	case MARKETING_PHONE_8:
		return "WITH THIS NEW APPLICATION EVERYONE CAN TRACK THE CITY TRAINS IN REAL TIME";
	case MARKETING_PHONE_9:
		return "NO MORE WAITING";
	case MARKETING_PHONE_10:
		return "NO MORE MISSING TRAINS";
	case MARKETING_PHONE_11:
		return "ALL OF THIS... for only 499D";
		case MARKETING_PHONE_11_A:
			return "I don't have the money";
			case MARKETING_PHONE_11_A_1:
				return "What do you mean you don't have the money?";
		case MARKETING_PHONE_11_B_LOW_TENSION:
			return "Still not interested";
			case MARKETING_PHONE_11_B_1:
				return "What did you just say?";
		case MARKETING_PHONE_11_B_HIGH_TENSION:
			return "< Hang up >";
	case MARKETING_PHONE_12:
		return "I bring you the holy technology from FATHER EDWARD himself";
	case MARKETING_PHONE_13:
		return "A man so humble that still helps those that live outside of Kelia!";
	case MARKETING_PHONE_14:
		return "and you receive it like this?";
		case MARKETING_PHONE_14_A:
			return "Fuck you < Hang up >";
		case MARKETING_PHONE_14_B:
			return "Goodbye < Hang up >";
	}

	D_ASSERT(false, "Invalid text for type: %i", textType);
	return "Invalid Text";
}