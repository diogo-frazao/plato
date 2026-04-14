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
		return "do i have the pleasure of speaking with Mr.Rostov?";
	case ROSTOV_MARKETING_PHONE_2_1:
		return "yes...";
	case ROSTOV_MARKETING_PHONE_2_2:
		return "i don't know";
	case ROSTOV_MARKETING_PHONE_3:
		return "well fear not sir, because even if you're not Mr.Rostov";
	case ROSTOV_MARKETING_PHONE_4:
		return "i call you with good news sir!";
	case ROSTOV_MARKETING_PHONE_4_1:
		return "who are you?";
	case ROSTOV_MARKETING_PHONE_4_2:
		return "i am not interested";
	case ROSTOV_MARKETING_PHONE_4_3:
		return "< say nothing >";
	}

	D_ASSERT(false, "Invalid text for type: %i", textType);
	return "Invalid Text";
}