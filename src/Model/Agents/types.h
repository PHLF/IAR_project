#ifndef TYPES_H
#define TYPES_H

#include <bit>
#include <cstdint>
#include <string_view>

namespace sim
{
class Predator;
class Prey;

enum ETypeId : uint8_t
{
    __none           = 0,
    prey             = 1,
    predator         = prey << 1,
    __nb_agent_types = std::countr_zero(predator) + 1
};

template <typename K = void> struct TypeId
{
    static constexpr ETypeId type_idx = ETypeId::__none;
};

template <> struct TypeId<Predator>
{
    static constexpr ETypeId type_idx = ETypeId::predator;
};

template <> struct TypeId<Prey>
{
    static constexpr ETypeId type_idx = ETypeId::prey;
};

constexpr auto idx_from(std::string_view name)
{
    if (name == "prey")
        return TypeId<Prey>::type_idx;
    if (name == "predator")
        return TypeId<Predator>::type_idx;
    return TypeId<>::type_idx;
};

} // namespace sim

#endif // TYPES_H
