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
	case ROSTOV_DAD_PHONE_1:
		return "halo?";
	case ROSTOV_DAD_PHONE_2:
		return "haloooo? rostov?";
	case ROSTOV_DAD_PHONE_2_1:
		return "yes it's indeed very alarming my friend";
	case ROSTOV_DAD_PHONE_2_2:
		return "I heard a commotion dowstairs and it's";
	case ROSTOV_DAD_PHONE_2_3:
		return "mamaco peludo";
	}

	D_ASSERT(false, "Invalid text for type: %i", textType);
	return "Invalid Text";
}