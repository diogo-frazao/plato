#include "text.h"
#include "log.h"

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
			return { "Who are you?", 10 , HIGH_TENSION };
			case MARKETING_PHONE_2_B_1:
				return "Oh";
			case MARKETING_PHONE_2_B_2:
				return "My name is FREDERICO from KELIA's marketing department";
		case MARKETING_PHONE_2_C:
			return { "Shut the fuck up", 20, HIGH_TENSION };
			case MARKETING_PHONE_2_C_1:
				return { "What did you just say to me?", 10, HIGH_TENSION };
			case MARKETING_PHONE_2_C_2:
				return { "DO YOU KNOW WHO I AM", 10, HIGH_TENSION };
			case MARKETING_PHONE_2_C_3:
				return { "I BRING TO YOU THE HOLY TECHNOLOGY FROM FATHER EDWARD HIMSELF", 10, HIGH_TENSION };
			case MARKETING_PHONE_2_C_4:
				return { "AND YOU HAVE THE AUDACITY TO RECEIVE IT LIKE THIS?", 10, HIGH_TENSION };
				case MARKETING_PHONE_2_C_4_A:
					return {"YES < HANG UP >", 10, FATAL_TENSION };
				case MARKETING_PHONE_2_C_4_B:
					return { "FUCK YOU < HANG UP >", 20, FATAL_TENSION };
	case MARKETING_PHONE_3:
		return "I bring to you a deal too good to miss!";
		case MARKETING_PHONE_3_A:
			return "I'm listning";
			case MARKETING_PHONE_3_A_1:
				return "PERFECT";
		case MARKETING_PHONE_3_B:
			return { "I don't care", 10 , HIGH_TENSION};
			case MARKETING_PHONE_3_B_1:
				return "...";
			case MARKETING_PHONE_3_B_2:
				return "Mr.ROSTOV please listen, I'm sure you'll be interested";
	case MARKETING_PHONE_4:
		return "(clears throath)";
	case MARKETING_PHONE_5:
		return { "It's with great honor that I present to you sir, the newest creation from FATHER EDWARD'S Research Department team, here at KELIA", 5};
	case MARKETING_PHONE_6:
		return { "AND FOR ONLY 499D, MR.ROSTOV CAN HAVE ACCESS TO IT", 5 };
	case MARKETING_PHONE_7:
		return "And what is this creation I hear you ask?";
		case MARKETING_PHONE_7_A:
			return "I'm not interested";
			case MARKETING_PHONE_7_A_1:
				return "Wait sir now I'm sure you'll be interested";
		case MARKETING_PHONE_7_B:
			return { "I didn't ask shit", 20 , HIGH_TENSION };
		case MARKETING_PHONE_7_C_LOW_TENSION:
			return "...";
		case MARKETING_PHONE_7_C_HIGH_TENSION:
			return { "< HANG UP >", 5, FATAL_TENSION };
	case MARKETING_PHONE_8:
		return { "WITH THIS NEW APPLICATION EVERYONE CAN TRACK THE CITY TRAINS IN REAL TIME", 10 };
	case MARKETING_PHONE_9:
		return { "NO MORE WAITING", 5 };
	case MARKETING_PHONE_10:
		return { "NO MORE MISSING TRAINS", 5 };
	case MARKETING_PHONE_11:
		return "ALL OF THIS... for only 499D";
		case MARKETING_PHONE_11_A:
			return "I don't have the money";
			case MARKETING_PHONE_11_A_1:
				return { "What do you mean you don't have the money?", 10 };
		case MARKETING_PHONE_11_B_LOW_TENSION:
			return { "STOP YELLING", 10, HIGH_TENSION };
			case MARKETING_PHONE_11_B_1:
				return { "What did you just say?", 10 };
		case MARKETING_PHONE_11_B_HIGH_TENSION:
			return { "< HANG UP >", 5, FATAL_TENSION };
	case MARKETING_PHONE_12:
		return { "I bring you the holy technology from FATHER EDWARD himself", 10 };
	case MARKETING_PHONE_13:
		return { "A man so humble that still helps those that live outside of KELIA!", 10 };
	case MARKETING_PHONE_14:
		return "and you receive it like this?";
		case MARKETING_PHONE_14_A:
			return { "FUCK YOU < HANG UP >", 20, FATAL_TENSION };
		case MARKETING_PHONE_14_B:
			return { "GOODBYE < HANG UP >", 10, FATAL_TENSION };
	}

	// Starting dialogue with dad
	{
		case ONE_DAD_PHONE_1:
			return "Halo? *cough* *cough* ROSTOV?";
		case ONE_DAD_PHONE_2:
			return "Can you hear me son?";
			case ONE_DAD_PHONE_2_A:
				return { "Oh, it's you Pa", -100 };
			case ONE_DAD_PHONE_2_B:
				return { "Yes Pa", - 100 };
		case ONE_DAD_PHONE_3:
			return "I tried to call you, is everything ok?";
			case ONE_DAD_PHONE_3_A:
				return "Yes, and you?";
				case ONE_DAD_PHONE_3_A_1:
					return "*cough* *cough*";
				case ONE_DAD_PHONE_3_A_2:
					return "...";
					case ONE_DAD_PHONE_3_A_2_A:
						return "Are you there?";
					case ONE_DAD_PHONE_3_A_2_B:
						return "Did you hear me?";
					case ONE_DAD_PHONE_3_A_2_C:
						return "And you?";
				case ONE_DAD_PHONE_3_A_3:
					return "Yes ROSTOV";
			case ONE_DAD_PHONE_3_B:
				return { "A solicitor called me", 10, HIGH_TENSION };
				case ONE_DAD_PHONE_3_B_1:
					return "Oh brilliant...";
				case ONE_DAD_PHONE_3_B_2:
					return "*cough* *cough*";
				case ONE_DAD_PHONE_3_B_3:
					return "Another one from the cult";
					case ONE_DAD_PHONE_3_B_3_A:
						return "They are all the same";
					case ONE_DAD_PHONE_3_B_3_B:
						return { "I hate them", 10, HIGH_TENSION };
				case ONE_DAD_PHONE_3_B_4:
					return "I know son, I know...";
				case ONE_DAD_PHONE_3_B_5:
					return { "Let's talk about something else", -100 };
			case ONE_DAD_PHONE_4:
				return "...";
				case ONE_DAD_PHONE_4_A:
					return "How did your exam go?";
				case ONE_DAD_PHONE_4_B:
					return "Do you know the results already?";
				case ONE_DAD_PHONE_4_C:
					return { "I miss you dad", -100, LOW_TENSION };
					case ONE_DAD_PHONE_4_C_1:
						return "...";
					case ONE_DAD_PHONE_4_C_2:
						return "Cmon ROSTOV, you're going to make the old man here cry";
					case ONE_DAD_PHONE_4_C_3:
						return "You know I can't have any visits...";
			case ONE_DAD_PHONE_5:
				return "Oh...";
			case ONE_DAD_PHONE_6:
				return { "No news yet, it will still take a while", 5 };
				case ONE_DAD_PHONE_6_A:
					return "Are you sure?";
				case ONE_DAD_PHONE_6_B:
					return { "Be honest", 5 , HIGH_TENSION};
			case ONE_DAD_PHONE_7:
				return "ROSTOV, please *cough* cough*";
			case ONE_DAD_PHONE_8:
				return "With the money you send I'm getting the proper treatment";
			case ONE_DAD_PHONE_9:
				return "But the truth is that the results show th-";
	}

	}
	D_ASSERT(false, "Invalid text for type: %i", textType);
	return "Invalid Text";
}