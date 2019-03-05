#pragma once

#include <LA.h>

namespace LA {
  /*
   * Encoding
   */

    std::vector<Item*> assign_encode(Instruction* ip);
    std::vector<Item*> assign_to_encode(Instruction* ip);
    std::vector<Item*> call_encode(Instruction* ip);
    std::vector<Item*> return_value_encode(Instruction* ip);
    std::vector<Item*> br_conditional_encode(Instruction* ip);
    std::vector<Item*> assign_store_array_encode(Instruction* ip);
    std::vector<Item*> assign_new_array_encode(Instruction* ip);
    std::vector<Item*> assign_new_tuple_encode(Instruction* ip);
    std::vector<Item*> print_encode(Instruction* ip);
    std::vector<std::vector<std::string>> encode_items(Instruction* ip, std::vector<Item*> items_to_encode, std::string newLabel, int &varNameCounter);
    std::vector<std::vector<std::string>> encode_all_items(Instruction* ip, std::vector<Item*> items_to_encode, std::string newLabel, int &varNameCounter);


    /*
     * Decoding
     */

    std::vector<Item*> assign_to_decode(Instruction* ip);
    std::vector<Item*> br_conditional_decode(Instruction* ip);
    std::vector<Item*> assign_load_array_decode(Instruction* ip);
    std::vector<Item*> assign_store_array_decode(Instruction* ip);
    std::vector<Item*> array_length_decode(Instruction* ip);
    std::vector<std::vector<std::string>> decode_all_items(Instruction* ip, std::vector<Item*> items_to_decode, std::string newLabel, int &varNameCounter);
}
