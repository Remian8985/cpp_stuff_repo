#include <UnitTest++/UnitTest++.h>
#include "Money.h"

TEST(MoneyConstructorAmount)
{
	// SETUP
	const std::string currency = "CAD"; 
	const float amount = 42.00;

	//Processing 
	Money money(amount, currency);

	//verification
	CHECK_EQUAL(amount, money.get_currency());
}