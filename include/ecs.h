#pragma once
#include <vector>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <tuple>

class TypeRegistry {
    static inline int counter = 0;
public:
    template <typename T>
    static int get_id() {
        static const int id = counter++;
        return id;
    }
};

class BasePool {
public:
    virtual ~BasePool() = default;
    virtual bool contains(int entity) const = 0;
    virtual size_t size() const = 0;
};

template <typename T>
class ComponentPool : public BasePool {
    std::vector<int> sparse;
    std::vector<int> entity_map;
    std::vector<T> dense;
public:
    void assign(int entity, T comp) {
        if (entity >= (int)sparse.size()) sparse.resize(entity + 1, -1);
        sparse[entity] = static_cast<int>(dense.size());
        entity_map.push_back(entity);
        dense.push_back(std::move(comp));
    }
    T& get(int entity) { return dense[sparse[entity]]; }
    bool contains(int entity) const override { 
        return entity < (int)sparse.size() && sparse[entity] != -1; 
    }
    size_t size() const override { return dense.size(); }
    const std::vector<int>& entities() const { return entity_map; }
};

class Registry {
    std::unordered_map<int, std::unique_ptr<BasePool>> pools;
public:
    template <typename T>
    ComponentPool<T>& get_pool() {
        int id = TypeRegistry::get_id<T>();
        if (!pools.count(id)) pools[id] = std::make_unique<ComponentPool<T>>();
        return *static_cast<ComponentPool<T>*>(pools[id].get());
    }

    template <typename... Components, typename Func>
    void view(Func&& func) {
        using LeaderType = std::tuple_element_t<0, std::tuple<Components...>>;
        auto& leader_pool = get_pool<LeaderType>();

        for (int entity : leader_pool.entities()) {
            if ((get_pool<Components>().contains(entity) && ...)) {
                func(get_pool<Components>().get(entity)...);
            }
        }
    }
};