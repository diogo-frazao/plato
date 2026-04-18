#include "text.h"
#include "log.h"

const char* getText(TextType textType)
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
	case ROSTOV_MARKETING_PHONE_1:
		return "hello?";
	case ROSTOV_MARKETING_PHONE_2:
		return "do i have the pleasure of speaking with mr.Rostov?";
	case ROSTOV_MARKETING_PHONE_2_1:
		return "yes...";
	case ROSTOV_MARKETING_PHONE_2_2:
		return "i don't know";
	case ROSTOV_MARKETING_PHONE_3:
		return "well fear not sir, because even if you're not mr.Rostov";
	case ROSTOV_MARKETING_PHONE_4:
		return "i call you with good news sir!";
	case ROSTOV_MARKETING_PHONE_4_1:
		return "who are you?";
	case ROSTOV_MARKETING_PHONE_4_2:
		return "i am not interested";
	case ROSTOV_MARKETING_PHONE_4_3:
		return "< say nothing >";
	case ROSTOV_MARKETING_PHONE_5:
		return "wait, one moment sir... (clears throat)";
	case ROSTOV_MARKETING_PHONE_6:
		return "mr.Rostov, i have to say, i'm a person just like you";
	case ROSTOV_MARKETING_PHONE_7:
		return "i know how frustrating it is to use the trains in this city. always overcrowded and late!";
	case ROSTOV_MARKETING_PHONE_8:
		return "but now imagine if you could track the trains' locations in real time from your very phone!!";
	case ROSTOV_MARKETING_PHONE_9:
		return "tell me sir, how perfect would that be?";
	case ROSTOV_MARKETING_PHONE_9_1:
		return "can you shut the fuck up?";
	case ROSTOV_MARKETING_PHONE_9_2:
		return "again, i am not interested";
	case ROSTOV_MARKETING_PHONE_9_3:
		return "PERFECT";
	case ROSTOV_MARKETING_PHONE_10:
		return "EXACTLY";
	case ROSTOV_MARKETING_PHONE_11:
		return "wait sir now i'm sure i'll convince you";
	case ROSTOV_MARKETING_PHONE_12:
		return "this technology comes from Father Edward's research department directly!";
	case ROSTOV_MARKETING_PHONE_13:
		return "and for the very very low price of just 499D, mr Rostov will have access to this app";
	case ROSTOV_MARKETING_PHONE_13_1:
		return "< hang up >";
	case ROSTOV_MARKETING_PHONE_13_2:
		return "fuck that cult";
	case ROSTOV_MARKETING_PHONE_13_3:
		return "< keep listening >";
	}

	D_ASSERT(false, "Invalid text for type: %i", textType);
	return "Invalid Text";
}