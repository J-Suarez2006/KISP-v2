#ifndef VM_HPP
#define VM_HPP

#include <vector>
#include <cstdint>

/**
 * @struct virtual machine
 * RISC instruction set. 4 bytes per instruction
 */
struct VM
{
    static constexpr uint8_t BYTES_PER_INSTRUCTION = 4;

    std::vector<uint8_t> program;
    std::vector<uint8_t> stack;
    std::vector<uint8_t> heap;    

    std::uint64_t ip, sp, fp;
};
#endif