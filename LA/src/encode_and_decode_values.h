#pragma once

#include <LA.h>

namespace LA {
  /*
   * Encoding
   */

    std::vector<Item*> assign_to_encode(Instruction* ip);
    std::vector<std::vector<std::string>> encode_items(Instruction* ip, std::vector<Item*> items_to_encode, std::string newLabel, int &varNameCounter);


    /*
     * Decoding
     */

    std::vector<Item*> br_to_decode(Instruction* ip);
    std::vector<Item*> array_load_to_decode(Instruction* ip);
    std::vector<Item*> array_store_to_decode(Instruction* ip);
    std::vector<Item*> array_length_to_decode(Instruction* ip);
    std::vector<Item*> assign_to_decode(Instruction* ip);
    std::vector<std::vector<std::string>> decode_items(Instruction* ip, std::vector<Item*> items_to_decode, std::string newLabel, int &varNameCounter);
}
