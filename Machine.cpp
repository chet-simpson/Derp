#include "Machine.h"


void Machine::reset()
{
    auto stack = decltype(stack_)();
    auto variables = decltype(variables_)();
    auto opcodes = decltype(opcodes_)();
    auto ipregister = decltype(ipregister_)(0);
    auto vpregister = decltype(vpregister_)(0);

    std::fill(variables_.begin(), variables_.end(), value_type(0));
    std::fill(opcodes_.begin(), opcodes_.end(), Opcode::Nop);

    std::swap(stack, stack_);
    std::swap(variables, variables_);
    std::swap(opcodes, opcodes_);
    std::swap(ipregister, ipregister_);
    std::swap(vpregister, vpregister_);
}




Machine::value_type Machine::push(const value_type value)
{
    stack_.emplace(value);
    return value;
}

Machine::value_type Machine::popx()
{
    ValidateStackSize(1);
    return internal_pop();
}

Machine::value_type Machine::internal_pop()
{
    value_type value = stack_.top();
    stack_.pop();
    return value;
}



Machine::value_type Machine::execute_call(address_type address)
{
    value_type value(0);

    ipregister_ = address;
    for (bool execute = true; execute;)
    {
        auto opcode = opcodes_[ipregister_];
        next();
        switch (opcode)
        {
        case Opcode::Nop:
            break;

        case Opcode::Return:
            execute = false;
            break;




        case Opcode::PushValue:
            value = push(static_cast<value_type>(opcodes_[ipregister_]));
            next(); //  Skip the value as it's not an opcode.
            break;

        case Opcode::Load:
        {
            ValidateStackSize(1);
            address_type address = internal_pop();
            if (address >= variables_.size())
            {
                throw machine_exception("LOAD: Address out of range");
            }
            value = push(variables_[address]);
            break;
        }

        case Opcode::Save:
        {
            ValidateStackSize(2);
            auto address = static_cast<address_type>(internal_pop() % MemorySize);
            if (address >= variables_.size())
            {
                throw machine_exception("SAVE: Address out of range");
            }
            value = internal_pop();
            variables_[address] = value;
            break;
        }




        case Opcode::BranchOnZero:
        {
            value = internal_pop();
            address_type address = internal_pop();
            if (address >= opcodes_.size())
            {
                throw machine_exception("POP.IP: Address out of range.");
            }
            if (value == 0)
            {
                ipregister_ = address;
            }
            break;
        }

        case Opcode::BranchOnNotZero:
        {
            value = internal_pop();
            address_type address = internal_pop();
            if (address >= opcodes_.size())
            {
                throw machine_exception("POP.IP: Address out of range.");
            }
            if (value != 0)
            {
                ipregister_ = address;
            }
            break;
        }

        case Opcode::Duplicate:
            ValidateStackSize(1);
            value = push(stack_.top());
            break;

        case Opcode::Swap:
        {
            ValidateStackSize(2);
            auto value1 = internal_pop();
            auto value2 = internal_pop();
            push(value2);
            push(value1);
            value = value_type();
            break;
        }

        case Opcode::Rotate:
        {
            // a b c -- b c a
            ValidateStackSize(3);
            value_type a = internal_pop();
            value_type b = internal_pop();
            value_type c = internal_pop();
            push(a);
            push(c);
            push(b);
            value = value_type();
            break;
        }

        case Opcode::Add:           value = execBinaryOp(std::plus<value_type>());          break;
        case Opcode::Subtract:      value = execBinaryOp(std::minus<value_type>());         break;
        case Opcode::Multiply:      value = execBinaryOp(std::multiplies<value_type>());    break;
        case Opcode::Divide:        value = execBinaryOp(std::divides<value_type>());       break;
        case Opcode::Modulo:        value = execBinaryOp(std::modulus<value_type>());       break;
        case Opcode::Negate:        value = execUnaryOp(std::negate<value_type>());         break;

        case Opcode::Equals:        value = execBinaryOp(std::equal_to<value_type>());      break;
        case Opcode::LessThan:      value = execBinaryOp(std::less<value_type>());          break;
        case Opcode::GreaterThan:   value = execBinaryOp(std::greater<value_type>());       break;

        case Opcode::And:           value = execBinaryOp(std::bit_and<value_type>());       break;
        case Opcode::Or:            value = execBinaryOp(std::bit_or<value_type>());        break;
        case Opcode::Xor:           value = execBinaryOp(std::bit_xor<value_type>());       break;
        case Opcode::Not:           value = execUnaryOp(std::bit_not<value_type>());        break;  //  C++14

        }
    }

    return value;
}




void Machine::ValidateStackSize(std::size_t minimumSize) const
{
    if (stack_.size() < minimumSize)
    {
        throw machine_exception("Stack does not contain enough values to perform the operation.");
    }
}




void Machine::next()
{
    ipregister_ = (ipregister_ + 1) % opcodes_.size();
}


Machine::value_type Machine::execBinaryOp(BinaryFunctionType opFunction)
{
    ValidateStackSize(2);
    value_type lvalue = internal_pop();
    value_type rvalue = internal_pop();
    return push(opFunction(lvalue, rvalue));
}

Machine::value_type Machine::execUnaryOp(UnaryFunctionType opFunction)
{
    ValidateStackSize(2);
    value_type value = internal_pop();
    return push(opFunction(value));
}








MachineEx::address_type MachineEx::genOp(Opcode opcode)
{
    opcodes_[genpointer_] = opcode;
    ++genpointer_;
    genpointer_ = genpointer_ % MemorySize;

    return genpointer_;
}


MachineEx::address_type MachineEx::genOp(Opcode opcode, value_type value)
{
    genOp(opcode);
    return genOp(static_cast<Opcode>(value));
}
