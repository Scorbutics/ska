#include <iostream>
#include <doctest.h>
#include <Logging/Tokenizer.h>

TEST_CASE("[Tokenizer]") {
	
	SUBCASE("Litteral") {
		auto tokenizer = ska::loggerdetail::Tokenizer{"pure litteral string"};
		CHECK(tokenizer.size() == 1);
		auto firstToken = tokenizer.begin();
		CHECK(firstToken->type() == ska::loggerdetail::TokenType::Literal);
		CHECK(firstToken->value() == "pure litteral string");
		CHECK(firstToken->length() == std::string("pure litteral string").size());
	}
	
	SUBCASE("Color") {
		SUBCASE("With color id") {
			auto tokenizer = ska::loggerdetail::Tokenizer{"%7c"};
			CHECK(tokenizer.size() == 1);
			auto firstToken = tokenizer.begin();
			CHECK(firstToken->type() == ska::loggerdetail::TokenType::Color);
			CHECK(firstToken->value() == "");
			CHECK(firstToken->length() == 7);
		}
		
		SUBCASE("Without color id") {
			//TODO : fail ? cas qui n'a aucun sens
			auto tokenizer = ska::loggerdetail::Tokenizer{"%c"};
			CHECK(tokenizer.size() == 1);
			auto firstToken = tokenizer.begin();
			CHECK(firstToken->type() == ska::loggerdetail::TokenType::Color);
			CHECK(firstToken->value() == "");
			CHECK(firstToken->length() == 0);
		}
	}
	
	SUBCASE("Class") {
		SUBCASE("With specified class name length") {
			auto tokenizer = ska::loggerdetail::Tokenizer{"%5C"};
			CHECK(tokenizer.size() == 1);
			auto firstToken = tokenizer.begin();
			CHECK(firstToken->type() == ska::loggerdetail::TokenType::Class);
			CHECK(firstToken->value() == "");
			CHECK(firstToken->length() == 5);
		}
		
		SUBCASE("Without specified class name length") {
			auto tokenizer = ska::loggerdetail::Tokenizer{"%C"};
			CHECK(tokenizer.size() == 1);
			auto firstToken = tokenizer.begin();
			CHECK(firstToken->type() == ska::loggerdetail::TokenType::Class);
			CHECK(firstToken->value() == "");
			CHECK(firstToken->length() == 0);
		}
	}
	
	SUBCASE("File") {
		SUBCASE("With specified file name length") {
			auto tokenizer = ska::loggerdetail::Tokenizer{"%5F"};
			CHECK(tokenizer.size() == 1);
			auto firstToken = tokenizer.begin();
			CHECK(firstToken->type() == ska::loggerdetail::TokenType::File);
			CHECK(firstToken->value() == "");
			CHECK(firstToken->length() == 5);
		}
		
		SUBCASE("Without specified file name length") {
			auto tokenizer = ska::loggerdetail::Tokenizer{"%F"};
			CHECK(tokenizer.size() == 1);
			auto firstToken = tokenizer.begin();
			CHECK(firstToken->type() == ska::loggerdetail::TokenType::File);
			CHECK(firstToken->value() == "");
			CHECK(firstToken->length() == 0);
		}
	}
	
	SUBCASE("File") {
		SUBCASE("With specified file name length") {
			auto tokenizer = ska::loggerdetail::Tokenizer{"%5F"};
			CHECK(tokenizer.size() == 1);
			auto firstToken = tokenizer.begin();
			CHECK(firstToken->type() == ska::loggerdetail::TokenType::File);
			CHECK(firstToken->value() == "");
			CHECK(firstToken->length() == 5);
		}
		
		SUBCASE("Without specified file name length") {
			auto tokenizer = ska::loggerdetail::Tokenizer{"%F"};
			CHECK(tokenizer.size() == 1);
			auto firstToken = tokenizer.begin();
			CHECK(firstToken->type() == ska::loggerdetail::TokenType::File);
			CHECK(firstToken->value() == "");
			CHECK(firstToken->length() == 0);
		}
	}
	
	SUBCASE("Function") {
		SUBCASE("With specified function name length") {
			auto tokenizer = ska::loggerdetail::Tokenizer{"%5f"};
			CHECK(tokenizer.size() == 1);
			auto firstToken = tokenizer.begin();
			CHECK(firstToken->type() == ska::loggerdetail::TokenType::Function);
			CHECK(firstToken->value() == "");
			CHECK(firstToken->length() == 5);
		}
		
		SUBCASE("Without specified function name length") {
			auto tokenizer = ska::loggerdetail::Tokenizer{"%f"};
			CHECK(tokenizer.size() == 1);
			auto firstToken = tokenizer.begin();
			CHECK(firstToken->type() == ska::loggerdetail::TokenType::Function);
			CHECK(firstToken->value() == "");
			CHECK(firstToken->length() == 0);
		}
	}
	
	SUBCASE("Function") {
		SUBCASE("With specified function name length") {
			auto tokenizer = ska::loggerdetail::Tokenizer{"%5f"};
			CHECK(tokenizer.size() == 1);
			auto firstToken = tokenizer.begin();
			CHECK(firstToken->type() == ska::loggerdetail::TokenType::Function);
			CHECK(firstToken->value() == "");
			CHECK(firstToken->length() == 5);
		}
		
		SUBCASE("Without specified function name length") {
			auto tokenizer = ska::loggerdetail::Tokenizer{"%f"};
			CHECK(tokenizer.size() == 1);
			auto firstToken = tokenizer.begin();
			CHECK(firstToken->type() == ska::loggerdetail::TokenType::Function);
			CHECK(firstToken->value() == "");
			CHECK(firstToken->length() == 0);
		}
	}
	
	SUBCASE("Line") {
		SUBCASE("With specified line length") {
			auto tokenizer = ska::loggerdetail::Tokenizer{"%5l"};
			CHECK(tokenizer.size() == 1);
			auto firstToken = tokenizer.begin();
			CHECK(firstToken->type() == ska::loggerdetail::TokenType::Line);
			CHECK(firstToken->value() == "");
			CHECK(firstToken->length() == 5);
		}
		
		SUBCASE("Without specified line length") {
			auto tokenizer = ska::loggerdetail::Tokenizer{"%l"};
			CHECK(tokenizer.size() == 1);
			auto firstToken = tokenizer.begin();
			CHECK(firstToken->type() == ska::loggerdetail::TokenType::Line);
			CHECK(firstToken->value() == "");
			CHECK(firstToken->length() == 0);
		}
	}
	
	SUBCASE("Identifier") {
		SUBCASE("With specified identifier length") {
			//TODO : fail ? cas qui n'a aucun sens
			auto tokenizer = ska::loggerdetail::Tokenizer{"%5i"};
			CHECK(tokenizer.size() == 1);
			auto firstToken = tokenizer.begin();
			CHECK(firstToken->type() == ska::loggerdetail::TokenType::Identifier);
			CHECK(firstToken->value() == "");
			CHECK(firstToken->length() == 5);
		}
		
		SUBCASE("Without specified identifier length") {
			auto tokenizer = ska::loggerdetail::Tokenizer{"%i"};
			CHECK(tokenizer.size() == 1);
			auto firstToken = tokenizer.begin();
			CHECK(firstToken->type() == ska::loggerdetail::TokenType::Identifier);
			CHECK(firstToken->value() == "");
			CHECK(firstToken->length() == 0);
		}
	}
	
	SUBCASE("Value") {
		SUBCASE("With specified value length") {
			//TODO : fail ? cas qui n'a aucun sens pour le moment
			auto tokenizer = ska::loggerdetail::Tokenizer{"%5v"};
			CHECK(tokenizer.size() == 1);
			auto firstToken = tokenizer.begin();
			CHECK(firstToken->type() == ska::loggerdetail::TokenType::Value);
			CHECK(firstToken->value() == "");
			CHECK(firstToken->length() == 5);
		}
		
		SUBCASE("Without specified value length") {
			auto tokenizer = ska::loggerdetail::Tokenizer{"%v"};
			CHECK(tokenizer.size() == 1);
			auto firstToken = tokenizer.begin();
			CHECK(firstToken->type() == ska::loggerdetail::TokenType::Value);
			CHECK(firstToken->value() == "");
			CHECK(firstToken->length() == 0);
		}
	}
	
	SUBCASE("Year") {
		SUBCASE("With specified year length") {
			//TODO : fail ? cas qui n'a aucun sens pour le moment
			auto tokenizer = ska::loggerdetail::Tokenizer{"%5y"};
			CHECK(tokenizer.size() == 1);
			auto firstToken = tokenizer.begin();
			CHECK(firstToken->type() == ska::loggerdetail::TokenType::Year);
			CHECK(firstToken->value() == "");
			CHECK(firstToken->length() == 5);
		}
		
		SUBCASE("Without specified year length") {
			auto tokenizer = ska::loggerdetail::Tokenizer{"%y"};
			CHECK(tokenizer.size() == 1);
			auto firstToken = tokenizer.begin();
			CHECK(firstToken->type() == ska::loggerdetail::TokenType::Year);
			CHECK(firstToken->value() == "");
			CHECK(firstToken->length() == 0);
		}
	}
	
	SUBCASE("Month") {
		SUBCASE("With specified month length") {
			//TODO : fail ? cas qui n'a aucun sens pour le moment
			auto tokenizer = ska::loggerdetail::Tokenizer{"%5M"};
			CHECK(tokenizer.size() == 1);
			auto firstToken = tokenizer.begin();
			CHECK(firstToken->type() == ska::loggerdetail::TokenType::Month);
			CHECK(firstToken->value() == "");
			CHECK(firstToken->length() == 5);
		}
		
		SUBCASE("Without specified month length") {
			auto tokenizer = ska::loggerdetail::Tokenizer{"%M"};
			CHECK(tokenizer.size() == 1);
			auto firstToken = tokenizer.begin();
			CHECK(firstToken->type() == ska::loggerdetail::TokenType::Month);
			CHECK(firstToken->value() == "");
			CHECK(firstToken->length() == 0);
		}
	}
	
	SUBCASE("Day") {
		SUBCASE("With specified day length") {
			//TODO : fail ? cas qui n'a aucun sens pour le moment
			auto tokenizer = ska::loggerdetail::Tokenizer{"%5d"};
			CHECK(tokenizer.size() == 1);
			auto firstToken = tokenizer.begin();
			CHECK(firstToken->type() == ska::loggerdetail::TokenType::Day);
			CHECK(firstToken->value() == "");
			CHECK(firstToken->length() == 5);
		}
		
		SUBCASE("Without specified day length") {
			auto tokenizer = ska::loggerdetail::Tokenizer{"%d"};
			CHECK(tokenizer.size() == 1);
			auto firstToken = tokenizer.begin();
			CHECK(firstToken->type() == ska::loggerdetail::TokenType::Day);
			CHECK(firstToken->value() == "");
			CHECK(firstToken->length() == 0);
		}
	}
	
	SUBCASE("Hour") {
		SUBCASE("With specified hour length") {
			//TODO : fail ? cas qui n'a aucun sens pour le moment
			auto tokenizer = ska::loggerdetail::Tokenizer{"%5h"};
			CHECK(tokenizer.size() == 1);
			auto firstToken = tokenizer.begin();
			CHECK(firstToken->type() == ska::loggerdetail::TokenType::Hour);
			CHECK(firstToken->value() == "");
			CHECK(firstToken->length() == 5);
		}
		
		SUBCASE("Without specified hour length") {
			auto tokenizer = ska::loggerdetail::Tokenizer{"%h"};
			CHECK(tokenizer.size() == 1);
			auto firstToken = tokenizer.begin();
			CHECK(firstToken->type() == ska::loggerdetail::TokenType::Hour);
			CHECK(firstToken->value() == "");
			CHECK(firstToken->length() == 0);
		}
	}
	
	SUBCASE("Minute") {
		SUBCASE("With specified minute length") {
			//TODO : fail ? cas qui n'a aucun sens pour le moment
			auto tokenizer = ska::loggerdetail::Tokenizer{"%5m"};
			CHECK(tokenizer.size() == 1);
			auto firstToken = tokenizer.begin();
			CHECK(firstToken->type() == ska::loggerdetail::TokenType::Minute);
			CHECK(firstToken->value() == "");
			CHECK(firstToken->length() == 5);
		}
		
		SUBCASE("Without specified minute length") {
			auto tokenizer = ska::loggerdetail::Tokenizer{"%m"};
			CHECK(tokenizer.size() == 1);
			auto firstToken = tokenizer.begin();
			CHECK(firstToken->type() == ska::loggerdetail::TokenType::Minute);
			CHECK(firstToken->value() == "");
			CHECK(firstToken->length() == 0);
		}
	}
	
	SUBCASE("Second") {
		SUBCASE("With specified second length") {
			//TODO : fail ? cas qui n'a aucun sens pour le moment
			auto tokenizer = ska::loggerdetail::Tokenizer{"%5s"};
			CHECK(tokenizer.size() == 1);
			auto firstToken = tokenizer.begin();
			CHECK(firstToken->type() == ska::loggerdetail::TokenType::Second);
			CHECK(firstToken->value() == "");
			CHECK(firstToken->length() == 5);
		}
		
		SUBCASE("Without specified second length") {
			auto tokenizer = ska::loggerdetail::Tokenizer{"%s"};
			CHECK(tokenizer.size() == 1);
			auto firstToken = tokenizer.begin();
			CHECK(firstToken->type() == ska::loggerdetail::TokenType::Second);
			CHECK(firstToken->value() == "");
			CHECK(firstToken->length() == 0);
		}
	}
	
	SUBCASE("MilliSecond") {
		SUBCASE("With specified millisecond length") {
			//TODO : fail ? cas qui n'a aucun sens pour le moment
			auto tokenizer = ska::loggerdetail::Tokenizer{"%5T"};
			CHECK(tokenizer.size() == 1);
			auto firstToken = tokenizer.begin();
			CHECK(firstToken->type() == ska::loggerdetail::TokenType::MilliSecond);
			CHECK(firstToken->value() == "");
			CHECK(firstToken->length() == 5);
		}
		
		SUBCASE("Without specified millisecond length") {
			auto tokenizer = ska::loggerdetail::Tokenizer{"%T"};
			CHECK(tokenizer.size() == 1);
			auto firstToken = tokenizer.begin();
			CHECK(firstToken->type() == ska::loggerdetail::TokenType::MilliSecond);
			CHECK(firstToken->value() == "");
			CHECK(firstToken->length() == 0);
		}
	}
	
	SUBCASE("Unknown") {
		SUBCASE("With specified length") {
			try {
				auto tokenizer = ska::loggerdetail::Tokenizer{"%5?"};
			} catch(std::exception& e) {
				CHECK(true);
			}
		}
		
		SUBCASE("Without specified length") {
			try {
				auto tokenizer = ska::loggerdetail::Tokenizer{"%?"};
			} catch(std::exception& e) {
				CHECK(true);
			}
		}
	}
}

