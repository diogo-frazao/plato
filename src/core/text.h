#pragma once

enum TextType
{
	INVALID_TEXT,
	DEBUG_TEXT,
	ROSTOV_DAD_PHONE_1,
	ROSTOV_DAD_PHONE_2,
	ROSTOV_DAD_PHONE_2_1,
	ROSTOV_DAD_PHONE_2_2,
	ROSTOV_DAD_PHONE_2_3
};

const char* getText(TextType textTye);