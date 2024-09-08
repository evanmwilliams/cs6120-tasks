#include <nlohmann/json.hpp>
#include <iostream> 
#include <vector>
#include <string>
#include <fstream>

using json = nlohmann::json; 
using instr = json;

struct BasicBlock {
  std::vector<instr> instrs;
  BasicBlock() = default;
};

std::ostream& operator<<(std::ostream& os, const BasicBlock& bb) {
    os << "[";
    for (int i = 0; i < bb.instrs.size() - 1; ++i) {
      os << bb.instrs[i] << ", ";
    }
    os << bb.instrs[bb.instrs.size() - 1] << "]";
    return os;
}

std::vector<BasicBlock> form_blocks(json const &func) {
  std::vector<BasicBlock> blocks;
  BasicBlock block;
  for(auto const &ins : func["instrs"]) {
    if ((ins.contains("op")) && (ins["op"] == "br" || ins["op"] == "jmp")) {
      block.instrs.push_back(ins);
      blocks.push_back(block);
      block.instrs.clear();
    }
    else if (ins.contains("label")) {
      if (block.instrs.size() > 0) {
        blocks.push_back(block);
        block.instrs.clear();
      }
      block.instrs.push_back(ins);
    }
    else {
      block.instrs.push_back(ins);
    }
  }
  if (block.instrs.size() > 0) {
    blocks.push_back(block);
  }
  return blocks;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
    return 1;
  }

  std::string filename = argv[1];

  std::ifstream ifs(filename);
  std::string json_str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

  auto j = json::parse(json_str);

  for (const auto &func : j["functions"]) {
    std::vector<BasicBlock> blocks = form_blocks(func);
    for (int i = 0; i < blocks.size(); ++i) {
      std::cout << "block_" << i << ": " << blocks[i] << std::endl;
    }
  }

  return 0;
}