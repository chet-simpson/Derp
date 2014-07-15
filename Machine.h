#pragma once
#include <stack>
#include <functional>
#include <array>
#include <cstdint>


struct machine_exception : std::runtime_error
{
    machine_exception(const char *what) : std::runtime_error(what) {};
};

struct Machine
{
    typedef std::int16_t value_type;
    typedef std::uint16_t address_type;

    enum class Opcode : value_type
    {
        Nop,
        Return,

        PushValue, Load, Save,
        BranchOnZero, BranchOnNotZero,
        Duplicate, Swap, Rotate,
        Add, Subtract, Multiply, Divide, Modulo, Negate,
        Equals, LessThan, GreaterThan,
        //  bitwise not - operator~
        Or, And, Xor, Not,    
    };


public:

    Machine() {}
    virtual ~Machine() {}

    void reset();

    value_type push(const value_type value);
    value_type popx();
    value_type internal_pop();

    value_type execute_call(address_type address);


protected:

    typedef std::function<value_type(value_type, value_type)> BinaryFunctionType;
    typedef std::function<value_type(value_type)> UnaryFunctionType;
    
    virtual void ValidateStackSize(std::size_t minimumSize) const;

    virtual void next();    //  increase instruction pointer
    virtual value_type execUnaryOp(UnaryFunctionType opFunction);
    virtual value_type execBinaryOp(BinaryFunctionType opFunction);


protected:

    static const std::size_t MemorySize = 0x4000;

    std::array<value_type, MemorySize>  variables_;
    std::array<Opcode, MemorySize>      opcodes_;
    std::stack<value_type>              stack_;

private:

    address_type    ipregister_;    //  Instruction pointer register
    address_type    vpregister_;    //  Variable pointer register
};


struct MachineEx : Machine
{
    MachineEx() : genpointer_(0) {}

    virtual void resetGen()
    {
        genpointer_ = 0;
    }

    virtual void reset()
    {
        Machine::reset();
        resetGen();
    }

    virtual address_type getGenOpPointer() const
    {
        return genpointer_;
    }

    virtual MachineEx::address_type genOp(Opcode opcode);
    virtual MachineEx::address_type genOp(Opcode opcode, value_type value);


private:

    address_type    genpointer_;    //  Instruction pointer register
};