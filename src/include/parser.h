#pragma once
#include "lexer.h"

namespace ccc {

enum class Type {
	FUNC,
	INT,
	FLOAT,
	POINTER
};

class Symbol {
public:
	Symbol(Type type, std::string& lexeme);
	bool operator==(Symbol& other) const
	{
		return other.lexeme == lexeme;
	}

	Type type;
	std::string lexeme;
};

class Parser{

};

class 

}
