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
		return "[yellow]Goooooood[yellow] afternoon";
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
		return "I was trying to call you, is everything ok son?";
		case ONE_DAD_PHONE_3_A:
			return "Yes, you?";
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
					return { "YOU ASKED FOR IT < LEAVE DIALOGUE >", CHOICE_DIALOGUE, 20, FATAL_TENSION};
		case C_4_B:
			return { "I already did", CHOICE_DIALOGUE, 5, HIGH_TENSION };
		case C_4_C:
			return { "What are these kids doing here?", CHOICE_DIALOGUE , 10, HIGH_TENSION };
			case C_4_BC_1:
				return { "ahahAHAHAHah", HUGO_DIALOGUE };
			case C_4_BC_2:
				return { "OSKAR, make sure this PIG doesn't get in the way of our money", HUGO_DIALOGUE };
			case C_4_BC_3:
				return { "ahAHAHAH... So, what do you have to say now?", OSKAR_DIALOGUE };
				case C_4_BC_3_A:
					return { "We don't have your money", CHOICE_DIALOGUE };
				case C_4_BC_3_B:
					return { "I can feel your breath", CHOICE_DIALOGUE, 10, HIGH_TENSION};
					case C_4_BC_2_3_1:
						return { "ahAHAHAHA", HUGO_DIALOGUE };
					case C_4_BC_2_3_2:
						return { "HE IS MOCKING YOU OSKAR", HUGO_DIALOGUE };
					case C_4_BC_2_3_3:
						return { "SHUT THE FUCK UP ROSTOV", OSKAR_DIALOGUE };
				case C_4_BC_3_C:
					return { "YOU ASKED FOR IT < LEAVE DIALOGUE >", CHOICE_DIALOGUE, 20, FATAL_TENSION};
					case C_4_BC_3_C_CUE_1:
						return { "Asked for what heh?", OSKAR_DIALOGUE };
					case C_4_BC_3_C_CUE_2:
						return { "Be careful with that golf cue ROSTOV, your arm might fall off", HUGO_DIALOGUE };
					case C_4_BC_3_C_CUE_3:
						return { "AHAHAHA", OSKAR_DIALOGUE };
					case C_4_BC_3_C_1:
						return { "Asked for what heh?", OSKAR_DIALOGUE };
					case C_4_BC_3_C_2:
						return { "ROSTOV grab that golf cue near the table", DARWIN_DIALOGUE };
					case C_4_BC_3_C_3:
						return { "But be careful ROSTOV, your arm might fall off trying to pick it up", HUGO_DIALOGUE };
					case C_4_BC_3_C_4:
						return { "AHAHAHAH", OSKAR_DIALOGUE };
					case C_4_BC_3_C_5:
						return { "That was a good one brother", OSKAR_DIALOGUE };
	}

	// Hugo confrontation after oskar is killed
	{
	case D_1: return { "WHAT. THE. FUCK.", HUGO_DIALOGUE };
	case D_2: return { "OSKAR??", HUGO_DIALOGUE };
	case D_3: return { "OSKAR... Can you hear me? It's HUGO", HUGO_DIALOGUE };
	case D_4: return { "YOU KILLED MY BROTHER", HUGO_DIALOGUE };
	case D_5: return { "You're a FUCKING MURDERER", HUGO_DIALOGUE };
	case D_6: return { "YOU OLD PIG I WILL MAKE YOU PAY", HUGO_DIALOGUE };
	case D_7: return { "ROSTOV DON'T DO IT", DARWIN_DIALOGUE };
	case D_8: return { "Please calm down... Take a deep breath", DARWIN_DIALOGUE };
	case D_9: return { "...", DARWIN_DIALOGUE };
	case D_10: return { "I don't want you to kill him... I want to know who's behind this", DARWIN_DIALOGUE };
	case D_11: return { "So he better start talking", DARWIN_DIALOGUE };
	case D_12: return { "ahahAHAHAHAH. I have NOTHING to say", HUGO_DIALOGUE };
		case D_12_A: return { "Stop laughing", CHOICE_DIALOGUE, 0, HIGH_TENSION };
			case D_12_A_1: return { "OR WHAT?", HUGO_DIALOGUE };
			case D_12_A_2: return { "ahahAHAHAHAH.", HUGO_DIALOGUE, 5 };
				case D_12_A_2_A: return { "Or you end up like your brother", CHOICE_DIALOGUE, 0, HIGH_TENSION };
				case D_12_A_2_B: return { "Don't make this harder than it needs to be", CHOICE_DIALOGUE, 0, LOW_TENSION };
		case D_12_B: return { "I promise if you talk we'll let you go", CHOICE_DIALOGUE, 0, LOW_TENSION };
			case D_12_B_1: return { "ahahAHAHAHAH", HUGO_DIALOGUE };
			case D_12_B_2: return { "Besides being OLD and a MURDERER, you're also a LIAR", HUGO_DIALOGUE, 5 };
				case D_12_B_2_A: return { "Talk or end up like your brother", CHOICE_DIALOGUE, 0, HIGH_TENSION };
				case D_12_B_2_B: return { "Don't make this harder than it needs to be", CHOICE_DIALOGUE, 0, LOW_TENSION };
	case D_13_LOW_TENSION: return { "...", HUGO_DIALOGUE };
	case D_13_HIGH_TENSION: return { "...", HUGO_DIALOGUE };
	case D_14: return { "FUCK. YOU.", HUGO_DIALOGUE, 5 };
	}
	// Phone confrontation with big diesel
	{
	case E_1: return { "Don't look at me, it's not mine either", DARWIN_DIALOGUE };
	case E_2: return { "Relax, it's mine", HUGO_DIALOGUE };
	case E_3: return { "It's probably BIG DIESEL, he just wants to know if the job is done", HUGO_DIALOGUE };
	case E_4: return { "Don't you dare pick up the call! I know you will call for backup", DARWIN_DIALOGUE };
	case E_5: return { "ahAHAHAH", HUGO_DIALOGUE };
	case E_6: return { "Don't you want to know 'who's behind this'?", HUGO_DIALOGUE };
	case E_7: return { "This is your chance", HUGO_DIALOGUE };
		case E_7_A: return { "Pick up the call now", CHOICE_DIALOGUE };
			case E_7_A_1: return { "Yoooo", BIG_DISEL_DIALOGUE };
			case E_7_A_2: return { "Yo", HUGO_DIALOGUE };
				case E_7_A_2_A: return { "< Stay silent >", CHOICE_DIALOGUE };
					case E_7_A_2_A_1: return { "I don't even need to ask, right?", BIG_DISEL_DIALOGUE };
					case E_7_A_2_A_2: return { "A gangster like you always gets the job done", BIG_DISEL_DIALOGUE };
					case E_7_A_2_A_3: return { "Did you hear me?", BIG_DISEL_DIALOGUE };
						case E_7_A_2_A_3_A: return { "< Stay silent >", CHOICE_DIALOGUE };
							case E_7_A_2_A_3_A_1: return { "Yes, it's done", HUGO_DIALOGUE };
							case E_7_A_2_A_3_A_2: return { "THAT'S MY G!", BIG_DISEL_DIALOGUE };
							case E_7_A_2_A_3_A_3: return { "What are you waiting for then? Meet us in 2 behind the restaurant", BIG_DISEL_DIALOGUE };
							case E_7_A_2_A_3_A_4: return { "Wait!", HUGO_DIALOGUE };
							case E_7_A_2_A_3_A_5: return { "Hm, what's up?", BIG_DISEL_DIALOGUE };
								case E_7_A_2_A_3_A_5_A: return { "< Stay silent >", CHOICE_DIALOGUE };
									case E_7_A_2_A_3_A_5_A_1: return { "Triple cheeseburger", HUGO_DIALOGUE, 5 };
									case E_7_A_2_A_3_A_5_A_2: return { "Tsc. I see", BIG_DISEL_DIALOGUE, 5 };
									case E_7_A_2_A_3_A_5_A_3: return { "ahAHAHAH", HUGO_DIALOGUE };
										case E_7_A_2_A_3_A_5_A_3_A: return { "I trusted you", CHOICE_DIALOGUE };
										case E_7_A_2_A_3_A_5_A_3_B: return { "I know that's some kind of code", CHOICE_DIALOGUE, 0, HIGH_TENSION };
										case E_7_A_2_A_3_A_5_A_3_C: return { "You think I'm dumb?", CHOICE_DIALOGUE, 0, HIGH_TENSION };
									case E_7_A_2_A_3_A_5_A_4: return { "If you're curious then go meet BIG DIESEL and see", HUGO_DIALOGUE };
									case E_7_A_2_A_3_A_5_A_5: return { "ahAHAHAH", HUGO_DIALOGUE };
										case E_7_A_2_A_3_A_5_A_5_A: return { "I'll make you stop laughing for all < LEAVE DiALOGUE >", CHOICE_DIALOGUE, 0, FATAL_TENSION };
										case E_7_A_2_A_3_A_5_A_5_B: return { "I'll make you end up like your brother < LEAVE DiALOGUE >", CHOICE_DIALOGUE, 0, FATAL_TENSION };
								case E_7_A_2_A_3_A_5_B: return { "< Hang up >", CHOICE_DIALOGUE, 0, HIGH_TENSION };
									case E_7_A_2_A_3_A_5_B_1: return { "You OLD PIG", HUGO_DIALOGUE, 5 };
									case E_7_A_2_A_3_A_5_B_2: return { "You might have fooled me, but BIG DIESEL will take care of you", HUGO_DIALOGUE, 5 };
									case E_7_A_2_A_3_A_5_B_3: return { "Just shut up already", DARWIN_DIALOGUE, -5 };
									case E_7_A_2_A_3_A_5_B_4: return { "ROSTOV, go meet BIG DIESEL, but please be careful", DARWIN_DIALOGUE};
						case E_7_A_2_A_3_B: return { "For sure it's not done", CHOICE_DIALOGUE, 10, HIGH_TENSION };
							case E_7_A_2_A_3_B_1: return { "WHO IS THIS??", BIG_DISEL_DIALOGUE, 5 };
							case E_7_A_2_A_3_B_2: return { "They got me DIESEL", HUGO_DIALOGUE, 10 };
							case E_7_A_2_A_3_B_3: return { "WHAT!?", BIG_DISEL_DIALOGUE, 5 };
							case E_7_A_2_A_3_B_4: return { "They want to know why we're doing this", HUGO_DIALOGUE, 5 };
								case E_7_A_2_A_3_B_4_A: return { "You said you'd be quiet", CHOICE_DIALOGUE };
								case E_7_A_2_A_3_B_4_B: return { "You'll regret this", CHOICE_DIALOGUE, 5, HIGH_TENSION };
							case E_7_A_2_A_3_B_5: return { "Tsc.", BIG_DISEL_DIALOGUE };
							case E_7_A_2_A_3_B_6: return { "LISTEN MOTHERFUCKER THIS IS BIG DIESEL ON THE PHONE", BIG_DISEL_DIALOGUE, 5 };
							case E_7_A_2_A_3_B_7: return { "Let my people go and come meet me", BIG_DISEL_DIALOGUE, 5 };
							case E_7_A_2_A_3_B_8: return { "If I have your word that you won't hurt them, I'll tell you what you want", BIG_DISEL_DIALOGUE };
								case E_7_A_2_A_3_B_8_A: return { "Deal", CHOICE_DIALOGUE };
								case E_7_A_2_A_3_B_8_B: return { "No", CHOICE_DIALOGUE, 5, HIGH_TENSION };
							case E_7_A_2_A_3_B_9: return { "Don't listen to them DIESEL", HUGO_DIALOGUE, 5 };
							case E_7_A_2_A_3_B_10: return { "They killed OSKAR!!", HUGO_DIALOGUE, 20 };
							case E_7_A_2_A_3_B_11: return { "Tsc.", BIG_DISEL_DIALOGUE, 0 };
							case E_7_A_2_A_3_B_12: return { "ROSTOV, meet me behind the restaurant in 2 minutes... And come alone", BIG_DISEL_DIALOGUE, 0 };
							case E_7_A_2_A_3_B_13: return { "ahAHAHAH", HUGO_DIALOGUE, 0 };
								case E_7_A_2_A_3_B_13_A: return { "I'll make you stop laughing for all < LEAVE DiALOGUE >", CHOICE_DIALOGUE, 0, FATAL_TENSION };
								case E_7_A_2_A_3_B_13_B: return { "I'll make you end up like your brother < LEAVE DiALOGUE >", CHOICE_DIALOGUE, 0, FATAL_TENSION };
				case E_7_A_2_B: return { "What's up BIG DIESEL", CHOICE_DIALOGUE, 10, HIGH_TENSION };
					case E_7_A_2_B_1: return { "HUGO? Is this you? Did you get the job done my G?", BIG_DISEL_DIALOGUE };
		case E_7_B: return { "Don't you dare move", CHOICE_DIALOGUE, 10, HIGH_TENSION };
			case E_N_1: return { "Well... Too bad", HUGO_DIALOGUE };
			case E_N_2: return { "I guess you'll never know", HUGO_DIALOGUE };
				case E_N_2_A: return { "This is your last chance", CHOICE_DIALOGUE, 10, HIGH_TENSION };
					case E_N_2_A_1: return { "ahAHAHAHAH", HUGO_DIALOGUE };
						case E_N_2_A_1_A: return { "I'll make you stop laughing < LEAVE DIALOGUE >", CHOICE_DIALOGUE, 10, FATAL_TENSION };
						case E_N_2_A_1_B: return { "You will end up like your brother < LEAVE DIALOGUE >", CHOICE_DIALOGUE, 10, FATAL_TENSION };
				case E_N_2_B: return { "Then you know what happens next < LEAVE DIALOGUE >", CHOICE_DIALOGUE, 10, FATAL_TENSION };
	case E_7_1: return { "Oh, and forget these two... I'll call DAISY and get this mess sorted", DARWIN_DIALOGUE };
	case E_8: return { "GOD DAMMIT ROSTOV!", DARWIN_DIALOGUE, 5 };
	case E_9: return { "I don't want to see another dead body in here", DARWIN_DIALOGUE, 5 };
	case E_10: return { "Please leave for today, I'll call DAISY and get this mess sorted", DARWIN_DIALOGUE };
	case E_11: return { "We open tomorrow at the usual time", DARWIN_DIALOGUE };
	case E_12: return { "Good boy, do as you're told", HUGO_DIALOGUE };
	case E_13: return { "Just ignore him ROSTOV", DARWIN_DIALOGUE };
	case E_14: return { "ahahAHAAH. What happened ROSTOV? Are you RUNNING AWAY?", HUGO_DIALOGUE };
	case E_15: return { "Just like you ran from your sick DAD", HUGO_DIALOGUE };
	case E_16: return { "SHUT UP", DARWIN_DIALOGUE };
	case E_17: return { "The old fuck's a talking vegetable at this point", HUGO_DIALOGUE };
	case E_18: return { "A dead man with a cellphone ", HUGO_DIALOGUE };
	case E_19: return { "ahaAHAHAhah", HUGO_DIALOGUE };
	case E_20: return { "ROSTOV, please go", DARWIN_DIALOGUE };
	case E_21: return { "ahaAHAHAhah", HUGO_DIALOGUE };
	case E_22: return { "FUCKING HELL ROSTOV", DARWIN_DIALOGUE };
	case E_23: return { "I can't believe this...", DARWIN_DIALOGUE };
	case E_24: return { "Please, just leave...", DARWIN_DIALOGUE };
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
		s_currentDialogueEntityDTO.entityId = k_playerEntityId;
		break;
	case DARWIN_DIALOGUE:
		s_currentDialogueEntityDTO.dialogueBoxColor = { 25, 11, 13 };
		s_currentDialogueEntityDTO.outlineColor = { 160, 63, 39 };
		s_currentDialogueEntityDTO.textColor = { 210, 104, 104 };
		s_currentDialogueEntityDTO.dialoguePositionOffset = { 6.f, 0.f };
		s_currentDialogueEntityDTO.entityId = s_darwinEntityId;
		break;
	case OSKAR_DIALOGUE:
		s_currentDialogueEntityDTO.dialogueBoxColor = { 25, 11, 13 };
		s_currentDialogueEntityDTO.outlineColor = { 61, 49, 63 };
		s_currentDialogueEntityDTO.textColor = { 193, 138, 106 };
		s_currentDialogueEntityDTO.dialoguePositionOffset = { 29.f, 8.f };
		s_currentDialogueEntityDTO.entityId = s_oskarEntityId;
		break;
	case HUGO_DIALOGUE:
		s_currentDialogueEntityDTO.dialogueBoxColor = { 25, 11, 13 };
		s_currentDialogueEntityDTO.outlineColor = { 60, 60, 60 };
		s_currentDialogueEntityDTO.textColor = { 251, 185, 84 };
		s_currentDialogueEntityDTO.dialoguePositionOffset = { 29.f, 8.f };
		s_currentDialogueEntityDTO.entityId = s_hugoEntityId;
		break;
	case BIG_DISEL_DIALOGUE:
		s_currentDialogueEntityDTO.dialogueBoxColor = { 24, 11, 25 };
		s_currentDialogueEntityDTO.outlineColor = { 85, 38, 67 };
		s_currentDialogueEntityDTO.textColor = { 251, 185, 84 };
		s_currentDialogueEntityDTO.dialoguePositionOffset = { 29.f, 8.f };
		s_currentDialogueEntityDTO.entityId = s_hugoEntityId;
		break;
	default:
		D_ASSERT(false, "Unsupported dialogue entity type");
		break;
	}
}

TextEffectType getTextEffectTypeFromName(char* effectName)
{
	D_LOG(MINI, "Trying to apply text effect for %s", effectName);

	if (strcmp(effectName, "yellow") == 0)
	{
		return YELLOW_EFFECT;
	}

	D_LOG(ERROR, "No text effect found for %s", effectName);
	return INVALID_EFFECT;
}