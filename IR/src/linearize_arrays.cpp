#include <IR.h>

/*
 * Accessing length of a dimension
 * Accessing array element (RHS vs LHS)
 * Allocating an array
 */

// creates a new variable and doesn't overwrite varToDecode with the decoded value
//std::vector<std::string> decode(Item* itemToDecode, std::string labelToAppend, int &counter) {
//    std::vector<std::string> InstructionToReturn;
//    std::string percent = itemToDecode->itemType == Atomic_Type::var ? "%" : ""; // if num don't include "%"
//    InstructionToReturn.push_back("%" + itemToDecode->labelName + labelToAppend + std::to_string(counter));
//    InstructionToReturn.push_back("<-");
//    InstructionToReturn.push_back(percent + itemToDecode->labelName);
//    InstructionToReturn.push_back(">>");
//    InstructionToReturn.push_back("1");
//
//    return InstructionToReturn;
//}
//
//// doesn't create a new variable and overwrites varToEncode with the encoded value
//std::vector<std::vector<std::string>> encode(Item* varToEncode, std::string labelToAppend, int &counter) {
//    std::vector<std::vector<std::string>> InstructionsToReturn;
//    std::vector<std::string> shiftI;
//    shiftI.push_back("%" + varToEncode->labelName);
//    shiftI.push_back("<-");
//    shiftI.push_back(varToEncode->labelName);
//    shiftI.push_back("<<");
//    shiftI.push_back("1");
//    shiftI.push_back("%" + varToEncode->labelName);
//
//    std::vector<std::string> addI;
//    addI.push_back("%" + varToEncode->labelName);
//    addI.push_back("<-");
//    addI.push_back("%" + varToEncode->labelName);
//    addI.push_back("+");
//    addI.push_back("1");
//
//    InstructionsToReturn.push_back(shiftI);
//    InstructionsToReturn.push_back(addI);
//
//    return InstructionsToReturn;
//}
//
//std::vector<std::vector<std::string>> calculateArrayLength(Instruction* ip, std::string labelToAppend) {
//    std::vector<std::vector<std::string>> iToRet;
//    int varNameCounter = 0;
//
//    for(auto &arg : ip->arguments) {
//        
//
//        
//
//
//std::vector<std::vector<std::string>> length_translation(Instruction* ip, std::string labelToGen, int labelCounter) {}
//


