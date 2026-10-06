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
		return {" Stop yelling", CELLPHONE_DIALOGUE, 10, HIGH_TENSION, 30};
	case ETC_TEXT:
		return "...";
	case MARKETING_PHONE_1:
		return "[wave]Helloooooooooo[wave]";
	case MARKETING_PHONE_2:
		return "Do I have the [pink]pleasure[pink] of speaking with [rostov]Mr ROSTOV?[rostov]";
		case MARKETING_PHONE_2_A:
			return "Yes...";
			case MARKETING_PHONE_2_A_1:
				return "[pinkwave]PERFECT![pinkwave] I call you with good news sir";
		case MARKETING_PHONE_2_B:
			return { "Who are you?", CELLPHONE_DIALOGUE, 5 , HIGH_TENSION };
			case MARKETING_PHONE_2_B_1:
				return "Oh...";
			case MARKETING_PHONE_2_B_2:
				return "I am [pinkwave]FREDERICO[pinkwave] from [yellow]KELIA's marketing department[yellow]";
	case MARKETING_PHONE_3:
		return "I bring to you a [blue]deal[blue] too good to miss!";
		case MARKETING_PHONE_3_A:
			return "A [blue]deal[blue]?";
			case MARKETING_PHONE_3_A_1:
				return "Yes sir, a [blue]deal[blue]";
		case MARKETING_PHONE_3_B:
			return { "Not interested", CELLPHONE_DIALOGUE, 5 , HIGH_TENSION};
			case MARKETING_PHONE_3_B_1:
				return "...";
			case MARKETING_PHONE_3_B_2:
				return "[rostov]Mr ROSTOV[rostov] please listen, [yellow]I'm sure[yellow] you'll be interested";
		case MARKETING_PHONE_3_C:
			return {"Ok goodbye", CELLPHONE_DIALOGUE, 5 , HIGH_TENSION };
			case MARKETING_PHONE_3_C_1:
				return { "What did [redshake]you[redshake] just say to me?", CELLPHONE_DIALOGUE, 5, HIGH_TENSION };
			case MARKETING_PHONE_3_C_2:
				return { "DO YOU KNOW [redshake]WHO.[redshake] [redshake]I.[redshake] [redshake]AM.[redshake]", CELLPHONE_DIALOGUE, 10, HIGH_TENSION };
			case MARKETING_PHONE_3_C_3:
				return { "DO YOU EVEN UNDERSTAND WHAT I'M OFFERING TO A [redshake]PEASANT[redshake] LIKE YOU?", CELLPHONE_DIALOGUE, 10, HIGH_TENSION };
				case MARKETING_PHONE_3_C_3_I:
					return { "Stop yelling", CELLPHONE_DIALOGUE, 5, HIGH_TENSION, 10 };
			case MARKETING_PHONE_3_C_4:
				return { "I BRING YOU THE [yellow]HOLY TECHNOLOGY[yellow] FROM OUR [pink]HARD WORKING AND FEARFUL LEADER[pink], [fatheredward]FATHER EDWARD[fatheredward] HIMSELF. ", CELLPHONE_DIALOGUE, 5, HIGH_TENSION };
				case MARKETING_PHONE_3_C_4_I:
					return { "< HANG UP >", CELLPHONE_DIALOGUE, 5, FATAL_TENSION, 10 };
			case MARKETING_PHONE_3_C_5:
				return { "AND YOU HAVE THE [red]AUDACITY[red] TO RECEIVE IT LIKE THIS?", CELLPHONE_DIALOGUE, 5, HIGH_TENSION };
			case MARKETING_PHONE_3_C_5_A:
				return { "YES < HANG UP >", CELLPHONE_DIALOGUE, 10, FATAL_TENSION };
			case MARKETING_PHONE_3_C_5_B:
				return { "FUCK YOU < HANG UP >", CELLPHONE_DIALOGUE, 20, FATAL_TENSION };
	case MARKETING_PHONE_4:
		return "[interjection]*Ahem*[interjection]";
	case MARKETING_PHONE_5:
		return { "It's with [yellow]great honor[yellow] that I present to you sir, [blue]THE NEWEST CREATION[blue] FROM [fatheredward]FATHER EDWARD'S[fatheredward] RESEARCH DEPARTMENT TEAM, HERE AT [yellow]KELIA[yellow]", CELLPHONE_DIALOGUE, 5};
	case MARKETING_PHONE_6:
		return { "AND FOR [yellow]ONLY 499 Dram[yellow], [rostov]MR ROSTOV[rostov] CAN HAVE ACCESS TO THIS [wave]FANTASTIC[wave] CREATION MADE BY OUR [pink]HARD WORKING AND FEARFUL LEADER[pink], [fatheredward]FATHER EDWARD[fatheredward]", CELLPHONE_DIALOGUE, 5 };
		case MARKETING_PHONE_6_I:
			return { "Stop yelling", CELLPHONE_DIALOGUE, 5, HIGH_TENSION, 30 };
			case MARKETING_PHONE_6_I_1:
				return "Oh. I'm sorry sir...";
	case MARKETING_PHONE_7:
		return "AND WHAT IS THIS CREATION I HEAR YOU ASK?";
		case MARKETING_PHONE_7_A:
			return "I'm not really interested";
			case MARKETING_PHONE_7_A_1:
				return "Wait sir now [yellow]I'm sure[yellow] you'll be interested";
		case MARKETING_PHONE_7_B:
			return "...";
		case MARKETING_PHONE_7_C:
			return { "< HANG UP >", CELLPHONE_DIALOGUE, 10, FATAL_TENSION };
	case MARKETING_PHONE_8:
		return { "WITH THIS [yellow]NEW APPLICATION[yellow] EVERYONE CAN [blue]TRACK THE CITY'S TRAINS IN REAL TIME[blue] AND SEE WHERE THEY ARE", CELLPHONE_DIALOGUE, 5 };
	case MARKETING_PHONE_8_I:
		return { "STOP YELLING", CELLPHONE_DIALOGUE, 10, HIGH_TENSION, 10 };
	case MARKETING_PHONE_9:
		return { "[yellow]I'M SURE[yellow] YOU KNOW HOW IT FEELS TO MISS ONE... [redshake]NOW[redshake], NO. MORE. WAITING. NO. MORE. MISSING. TRAINS.", CELLPHONE_DIALOGUE, 5 };
		case MARKETING_PHONE_9_I:
			return { "< HANG UP >", CELLPHONE_DIALOGUE, 10, FATAL_TENSION, 5 };
	case MARKETING_PHONE_10:
		return "ALL OF THIS... [pinkwave]FOR ONLY 499 Dram[pinkwave]";
		case MARKETING_PHONE_10_A:
			return { "< HANG UP >", CELLPHONE_DIALOGUE, 5, FATAL_TENSION };
		case MARKETING_PHONE_10_B:
			return { " FUCK YOU < HANG UP >", CELLPHONE_DIALOGUE, 20, FATAL_TENSION };
	}

	// Starting dialogue with dad
	{
	case ONE_DAD_PHONE_1: return "Halo? [interjection]*cough*[interjection] [interjection]*cough*[interjection] [rostov]ROSTOV?[rostov]";
		case ONE_DAD_PHONE_1_I: return { "STOP CALLING", CELLPHONE_DIALOGUE, 10, HIGH_TENSION, 0 };
		case ONE_DAD_PHONE_1_I_1: return "What? Sorry son, I'll call later";
			case ONE_DAD_PHONE_1_I_1_A: return { "Wait Pa", CELLPHONE_DIALOGUE, -20, LOW_TENSION };
			case ONE_DAD_PHONE_1_I_1_B: return { "Oh it's you Pa", CELLPHONE_DIALOGUE, -20, LOW_TENSION };
		case ONE_DAD_PHONE_1_I_2: return "Are you ok? What was that for?";
			case ONE_DAD_PHONE_1_I_2_A: return { "A solicitor keeps calling me", CELLPHONE_DIALOGUE, 5, HIGH_TENSION };
			case ONE_DAD_PHONE_1_I_2_B: return "Everything's fine don't worry";
		case ONE_DAD_PHONE_1_I_3: return "Oh... [interjection]*cough*[interjection] [interjection]*cough*[interjection] I see";
		case ONE_DAD_PHONE_1_I_4: return "...But are you sure you're ok?";
			case ONE_DAD_PHONE_1_I_4_A: return "Yes Papa don't worry";
			case ONE_DAD_PHONE_1_I_4_B: return "Of course Pa";
		case ONE_DAD_PHONE_1_I_5: return "Ok son, ok";
	case ONE_DAD_PHONE_2: return "Can you hear me son?";
		case ONE_DAD_PHONE_2_A: return { "Oh it's you Pa", CELLPHONE_DIALOGUE, -20, LOW_TENSION };
		case ONE_DAD_PHONE_2_B: return { "Yes Pa", CELLPHONE_DIALOGUE, -20, LOW_TENSION };
	case ONE_DAD_PHONE_3: return "You were about to ask me [red]something[red] earlier, before the doctor came in... [interjection]*cough*[interjection] [interjection]*cough*[interjection] [yellow]What was it?[yellow]";
		case ONE_DAD_PHONE_3_A: return "Any news from [red]that?[red]";
		case ONE_DAD_PHONE_3_B: return "Do you know [red]the[red] results?";
		case ONE_DAD_PHONE_3_C: return "Did he bring [red]any[red] news?";
	case ONE_DAD_PHONE_4: return "Oh...";
	case ONE_DAD_PHONE_5: return "No, nothing... No news about [red]the exam[red], if that's what you're asking";
		case ONE_DAD_PHONE_5_A: return "It's been almost 2 weeks...";
		case ONE_DAD_PHONE_5_B: return "Why is it taking so long?";
	case ONE_DAD_PHONE_6: return "[rostov]ROSTOV,[rostov] [redshake]stop it[redshake]";
	case ONE_DAD_PHONE_7: return  { "You know I'm feeling stronger every day", CELLPHONE_DIALOGUE, -20 };
	case ONE_DAD_PHONE_8: return { "And the extra money you sent, [interjection]*cough*[interjection] [interjection]*cough*[interjection] it helps with the treatment…", CELLPHONE_DIALOGUE, -20 };
	case ONE_DAD_PHONE_9: return "[red]But the truth is the doctor told me that[red]";
	case ONE_DAD_PHONE_10: return { "What was that sound? Are you ok? What about [darwin]MR.DARWIN?[darwin]" };
		case ONE_DAD_PHONE_10_I: return { "I have to go Pa < HANG UP >", CELLPHONE_DIALOGUE, 0, FATAL_TENSION, 10 };
	case ONE_DAD_PHONE_11: return "[rostov]ROSTOV[rostov] [interjection]*cough*[interjection] [interjection]*cough*[interjection] are you there?";
		case ONE_DAD_PHONE_11_A: return "I'm fine but I don't know about [darwin]MR.DARWIN[darwin]";
		case ONE_DAD_PHONE_11_B: return { "Someone broke the radio", CELLPHONE_DIALOGUE, 5, HIGH_TENSION };
	case ONE_DAD_PHONE_12: return "Go check what it is but call me later son";
	}

	// Starting confrontation with hugo and oskar
	{
	case C_1: return { "Are you deaf? Where the [redshake]fuck[redshake] is my money? I'm losing my patience [darwin]DARWIN,[darwin] and me and [hugo]HUGO[hugo] are [redshake]not[redshake] in a good mood", OSKAR_DIALOGUE };
		case C_1_I: return { "What is going on here", OSKAR_DIALOGUE, 5, HIGH_TENSION, 30 };
			case C_1_I_1: return { "[laugh]HAHahaAhHa,[laugh] look who decided to show up [hugo]BROTHER[hugo]... Don't you [redshake]dare[redshake] interrupt me again [rostov]ROSTOV[rostov], or you'll fucking regret it you [redshake]old.[redshake] [redshake]fuck.[redshake] You don't play with a member of the [yellow]24K FIRM[yellow]", OSKAR_DIALOGUE };
				case C_1_I_1_I: return { "I asked what is going on here", OSKAR_DIALOGUE, 5, HIGH_TENSION, 30 };
			case C_1_I_2: return { "Are you [redshake]fucking[redshake] deaf? Do you want to get hurt? [yellow]Leave.[yellow]", OSKAR_DIALOGUE };
	case C_2: return { "[laugh]HAHahaAhHa,[laugh] look who decided to show up [hugo]BROTHER[hugo], another [red]old fuck.[red] Just leave [rostov]ROSTOV[rostov] [red]before you get hurt,[red] there's nothing for you to see here", OSKAR_DIALOGUE, 5 };
		case C_2_A: return { "What are you doing here?", CHOICE_DIALOGUE };
			case C_2_A_1: return { "[laugh]AHahhahHAH,[laugh] since you're so [redshake]fucking[redshake] curious, I'm here to collect [yellow]my[yellow] money. Your [redshake]fucking[redshake] boss owes us and [yellow]we're here to get it.[yellow] One way... or the other", OSKAR_DIALOGUE };
		case C_2_B: return { "Did you break the radio?", CHOICE_DIALOGUE };
			case C_2_B_1: return { "[laugh]AHahhahHAH,[laugh] since you're so [redshake]fucking[redshake] curious... [redshake]YES.[redshake] [redshake]I.[redshake] [redshake]DID IT.[redshake] And I'm here to get [yellow]my[yellow] money. Your [redshake]fucking[redshake] boss owes us and we're here to collect it. One way... or the other", OSKAR_DIALOGUE };
		case C_2_C: return { "You talk too much < AIM GUN >", CHOICE_DIALOGUE, 10, FATAL_TENSION };
	case C_3: return { "We all know that's not true, this has always been and will always be an honest business", DARWIN_DIALOGUE };
	case C_3_1: return { "[laugh]HaHhAhAhah,[laugh] do you see this [hugo]BROTHER?[hugo] [redshake]FUCKING.[redhsake] [redshake]LIAR.[redhsake]", OSKAR_DIALOGUE, 5 };
		case C_3_1_A: return { "We don't owe you anything, leave" };
		case C_3_1_B: return { "You're really pushing it now", CHOICE_DIALOGUE, 5, HIGH_TENSION };
		case C_3_1_C: return { "You are going to leave now < AIM GUN >", CHOICE_DIALOGUE, 10, FATAL_TENSION };
	case C_4: return { "And [redshake]WHAT?[redshake] I do what I [redshake]FUCKING[redshake] want, [laugh]ahAHaHaHah[laugh]", OSKAR_DIALOGUE, 5 };
	case C_4_1: return { "Serve me another drink [darwin]DARWIN,[darwin] [yellow]now.[yellow] And if you keep talking [rostov]ROSTOV,[rostov] you might end up like your [lightpink]DAD[lightpink], peeing in a cup, [red]waiting to die,[red] and can barely take a step without tripping on himself. [laugh]AHahhahHAH[laugh]", OSKAR_DIALOGUE, 50};
		case C_4_1_I: return { "You've gone too far < SHOOT HIM >", CHOICE_DIALOGUE, 30, FATAL_TENSION, 100 };
	case C_4_2: return { "...Actually, [yellow]forget it.[yellow] My patience is [redshake]over.[redshake] You have 5 seconds to get me [redshake]MY[redshake] money", OSKAR_DIALOGUE, 10 };
	case C_4_3: return { "5... 4... 3... 2... 1...", OSKAR_DIALOGUE };
		case C_4_3_I: return { "< SHOOT HIM >", CHOICE_DIALOGUE, 30, FATAL_TENSION, 10 };
	case C_AIM_1: return { "Ohohoh wait wait let's all calm down...", OSKAR_DIALOGUE };
	case C_AIM_2: return { "I get it, me and [hugo]HUGO[hugo] will leave, just give us a second and we'll be out in no time", OSKAR_DIALOGUE };
	case C_ROSTOV_HURT: return { "[laugh]ahAHAaHAhah[laugh] did you see that [hugo]BROTHER?[hugo] This old man's a [redshake]fucking[redshake] cunt, even with a gun he can't face me", OSKAR_DIALOGUE };
	case C_SHOOT_1: return { "[redshake]WTF??[redshake] [hugo]OSKAR???[hugo] I'll [redshake]KILL YOU[redshake] [rostov]ROSTOV[rostov]", HUGO_DIALOGUE };
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
		s_currentDialogueEntityDTO.dialogueAlignment = DIALOGUE_CENTER_ALIGNED;
		s_currentDialogueEntityDTO.entityId = k_playerEntityId;
		break;
	case DARWIN_DIALOGUE:
		s_currentDialogueEntityDTO.dialogueBoxColor = { 25, 11, 13 };
		s_currentDialogueEntityDTO.outlineColor = { 106, 106, 106 };
		s_currentDialogueEntityDTO.textColor = { 210, 104, 104 };
		s_currentDialogueEntityDTO.dialoguePositionOffset = { 6.f, 0.f };
		s_currentDialogueEntityDTO.dialogueAlignment = DIALOGUE_CENTER_ALIGNED;
		s_currentDialogueEntityDTO.entityId = s_darwinEntityId;
		break;
	case OSKAR_DIALOGUE:
		s_currentDialogueEntityDTO.dialogueBoxColor = { 25, 11, 13 };
		s_currentDialogueEntityDTO.outlineColor = { 61, 49, 63 };
		s_currentDialogueEntityDTO.textColor = { 193, 138, 106 };
		s_currentDialogueEntityDTO.dialoguePositionOffset = { 20.f, 8.f };
		s_currentDialogueEntityDTO.dialogueAlignment = DIALOGUE_LEFT_ALIGNED;
		s_currentDialogueEntityDTO.entityId = s_oskarEntityId;
		break;
	case HUGO_DIALOGUE:
		s_currentDialogueEntityDTO.dialogueBoxColor = { 25, 11, 13 };
		s_currentDialogueEntityDTO.outlineColor = { 60, 60, 60 };
		s_currentDialogueEntityDTO.textColor = { 251, 185, 84 };
		s_currentDialogueEntityDTO.dialoguePositionOffset = { 29.f, 8.f };
		s_currentDialogueEntityDTO.dialogueAlignment = DIALOGUE_CENTER_ALIGNED;
		s_currentDialogueEntityDTO.entityId = s_hugoEntityId;
		break;
	case BIG_DISEL_DIALOGUE:
		s_currentDialogueEntityDTO.dialogueBoxColor = { 24, 11, 25 };
		s_currentDialogueEntityDTO.outlineColor = { 85, 38, 67 };
		s_currentDialogueEntityDTO.textColor = { 251, 185, 84 };
		s_currentDialogueEntityDTO.dialoguePositionOffset = { 29.f, 8.f };
		s_currentDialogueEntityDTO.dialogueAlignment = DIALOGUE_CENTER_ALIGNED;
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

	if (strcmp(effectName, "pink") == 0)
	{
		return PINK_EFFECT;
	}

	if (strcmp(effectName, "lightpink") == 0)
	{
		return LIGHT_PINK_EFFECT;
	}

	if (strcmp(effectName, "wave") == 0)
	{
		return WAVE_EFFECT;
	}

	if (strcmp(effectName, "pinkwave") == 0)
	{
		return PINK_WAVE_EFFECT;
	}

	if (strcmp(effectName, "blue") == 0)
	{
		return BLUE_EFFECT;
	}

	if (strcmp(effectName, "yellow") == 0)
	{
		return YELLOW_EFFECT;
	}

	if (strcmp(effectName, "red") == 0)
	{
		return RED_EFFECT;
	}

	if (strcmp(effectName, "fatheredward") == 0)
	{
		return FATHER_EDWARD_EFFECT;
	}

	if (strcmp(effectName, "pa") == 0)
	{
		return PA_EFFECT;
	}

	if (strcmp(effectName, "rostov") == 0)
	{
		return ROSTOV_EFFECT;
	}

	if (strcmp(effectName, "darwin") == 0)
	{
		return DARWIN_EFFECT;
	}

	if (strcmp(effectName, "hugo") == 0)
	{
		return HUGO_EFFECT;
	}

	if (strcmp(effectName, "laugh") == 0)
	{
		return LAUGH_EFFECT;
	}

	if (strcmp(effectName, "interjection") == 0)
	{
		return INTERJECTION_EFFECT;
	}

	if (strcmp(effectName, "redshake") == 0)
	{
		return RED_SHAKE_EFFECT;
	}

	if (strcmp(effectName, "nowait") == 0)
	{
		return NO_WAIT_EFFECT;
	}

	D_LOG(ERROR, "No text effect found for %s", effectName);
	return INVALID_EFFECT;
}