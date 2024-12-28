#include <tuple>
#include <vector>
#include <variant>
#include <cstdint>
#include <iostream>
#include <type_traits>
#include <utility>

template<typename ...Ts>
class variant_vec
{
public:
    template<typename T, typename ...Args>
    void emplace_back(Args&& ...args)
    {
        static_assert((std::is_same_v<T, Ts> || ...), "Type T is not part of Ts...");

        auto &data_vec = get_vector<T>();
        data_vec.emplace_back(std::forward<Args>(args)...);

        _indices.push_back({get_type_index<T>(), static_cast<uint32_t>(data_vec.size() - 1)});
    }

    void push_back(const std::variant<Ts...>& value)
    {
        std::visit([this](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            static_assert((std::is_same_v<T, Ts> || ...), "Type T is not part of Ts...");

            auto &data_vec = get_vector<T>();
            data_vec.push_back(std::forward<decltype(val)>(val));

            _indices.push_back({get_type_index<T>(), static_cast<uint32_t>(data_vec.size() - 1)});
        }, value);
    }

    auto at(size_t index) const -> std::variant<Ts...>
    {
        if (index >= _indices.size())
            throw std::out_of_range("Index out of range");

        return get_variant(_indices[index]);
    }

    auto operator[](size_t index) const noexcept -> std::variant<Ts...>
    {
        return get_variant(_indices[index]);
    }

    auto size() const noexcept { return _indices.size(); }
    auto empty() const noexcept { return _indices.empty(); }

    class iterator
    {
    public:
        using value_type = std::variant<Ts...>;
        using reference = const value_type&;
        using pointer = const value_type*;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;

        iterator(const variant_vec& vec, size_t idx) : _vec(vec), _index(idx) {}

        value_type operator*() const { return _vec.at(_index); }

        iterator& operator++()
        {
            ++_index;
            return *this;
        }

        iterator operator++(int)
        {
            iterator temp = *this;
            ++(*this);
            return temp;
        }

        bool operator==(const iterator& other) const { return _index == other._index; }
        bool operator!=(const iterator& other) const { return !(*this == other); }

    private:
        const variant_vec& _vec;
        size_t _index;
    };

    iterator begin() const { return iterator(*this, 0); }
    iterator end() const { return iterator(*this, _indices.size()); }

private:
    struct VariantIndex
    {
        uint32_t type_index : 8;  // Type index 
        uint32_t data_index : 24; // Index in the specific vector
    };

    std::tuple<std::vector<Ts>...> _vectors; // Storage for each type's elements
    std::vector<VariantIndex> _indices;      // Flattened storage indices

    template<typename T>
    std::vector<T>& get_vector()
    {
        return std::get<std::vector<T>>(_vectors);
    }

    template<typename T>
    const std::vector<T>& get_vector() const
    {
        return std::get<std::vector<T>>(_vectors);
    }

    template<typename T>
    constexpr uint32_t get_type_index() const
    {
        return static_cast<uint32_t>(type_index_impl<T>(std::index_sequence_for<Ts...>{}));
    }

    template<typename T, std::size_t... Is>
    constexpr uint32_t type_index_impl(std::index_sequence<Is...>) const
    {
        return ((std::is_same_v<T, Ts> ? Is : 0) + ...);
    }

    std::variant<Ts...> get_variant(VariantIndex vi) const
    {
        return visit_index(vi.type_index, vi.data_index, std::index_sequence_for<Ts...>{});
    }

    template<std::size_t... Is>
    std::variant<Ts...> visit_index(uint32_t type_index, uint32_t data_index, std::index_sequence<Is...>) const
    {
        std::variant<Ts...> result;
        ((type_index == Is? void(result = get_value_at<Is>(data_index)) : void()), ...);

        return result;
    }

    template<std::size_t I>
    auto get_value_at(uint32_t data_index) const -> std::variant<Ts...>
    {
        using T = std::tuple_element_t<I, std::tuple<Ts...>>;
        const auto& data_vec = get_vector<T>();

        if (data_index >= data_vec.size())
            throw std::out_of_range("Vector index out of range");

        return data_vec[data_index];
    }
};

