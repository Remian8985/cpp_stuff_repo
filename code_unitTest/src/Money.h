
class Money {

public:
	 Money (const float amount, const std::string currency)
	 		: amount_(amount)  // bug fixed, parameter was amount_
	{

	}

	float get_amount() const {
		return amount_;
	}

	std::string get_currency() const {
		return "invalid" ; // this si wwrong
	}

private: 
	float amount_;
};