#include "core/identifiers.hpp"

#include <cstdint>
#include <iostream>
#include <limits>
#include <type_traits>

using closed_circuit::EntityId;
using closed_circuit::NodeId;

template <typename Id, typename Input>
concept CanCreateId = requires(Input value) { Id::from_value(value); };

static_assert(!std::is_default_constructible_v<NodeId>);
static_assert(!std::is_default_constructible_v<EntityId>);
static_assert(!std::is_constructible_v<NodeId, NodeId::Value>);
static_assert(!std::is_constructible_v<EntityId, EntityId::Value>);
static_assert(!std::is_convertible_v<NodeId, NodeId::Value>);
static_assert(!std::is_convertible_v<EntityId, EntityId::Value>);
static_assert(!std::is_constructible_v<NodeId, EntityId>);
static_assert(!std::is_constructible_v<EntityId, NodeId>);
static_assert(!CanCreateId<NodeId, EntityId> && !CanCreateId<EntityId, NodeId>);
static_assert(!CanCreateId<NodeId, double> && !CanCreateId<EntityId, double>);
static_assert(!CanCreateId<NodeId, bool> && !CanCreateId<EntityId, bool>);

int main()
{
    constexpr std::uint64_t max_entity_value = 4'294'967'295;
    constexpr auto max_input = std::numeric_limits<std::uint64_t>::max();

    const auto first_node = NodeId::from_value(0);
    const auto last_node = NodeId::from_value(48);
    if (!first_node || !last_node || first_node->value() != 0 ||
        last_node->value() != 48 || *first_node >= *last_node)
    {
        std::cerr << "NodeId did not preserve its valid bounds and order\n";
        return 1;
    }
    if (NodeId::from_value(-1) || NodeId::from_value(49) ||
        NodeId::from_value(256) || NodeId::from_value(max_input))
    {
        std::cerr << "NodeId accepted a value outside 0..48\n";
        return 1;
    }

    const auto first_entity = EntityId::from_value(0);
    const auto last_entity = EntityId::from_value(max_entity_value);
    if (!first_entity || !last_entity || first_entity->value() != 0 ||
        last_entity->value() != max_entity_value ||
        *first_entity >= *last_entity)
    {
        std::cerr << "EntityId did not preserve its valid bounds and order\n";
        return 1;
    }
    if (EntityId::from_value(-1) ||
        EntityId::from_value(max_entity_value + 1) ||
        EntityId::from_value(max_input))
    {
        std::cerr
            << "EntityId accepted a value outside the 32-bit unsigned range\n";
        return 1;
    }
    return 0;
}
