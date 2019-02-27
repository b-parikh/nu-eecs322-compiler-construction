#pragma once

#include <IR.h>

namespace IR {
	std::vector<std::vector<std::string>> length_translation(Instruction* ip, std::string newLabel, int &labelCounter);
	std::vector<std::vector<std::string>> new_array_translation(Instruction* ip, std::string newLabel, int &varNameCounter);

	std::vector<std::vector<std::string>> new_tuple(Instruction* ip);
}
