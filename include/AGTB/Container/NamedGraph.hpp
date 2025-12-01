#ifndef __AGTB_CONTAINER_NAMED_GRAPH_HPP__
#define __AGTB_CONTAINER_NAMED_GRAPH_HPP__

#include "../details/Macros.hpp"

#include <boost/graph/adjacency_list.hpp>

#include <map>
#include <generator>
#include <functional>

AGTB_CONTAINER_BEGIN

template <
    typename __VertexProperty = boost::no_property,
    typename __EdgeProperty = boost::no_property,
    typename __DirectedS = boost::directedS,
    typename __OutEdgeListS = boost::vecS,
    typename __VertexListS = boost::vecS,
    typename __GraphProperty = boost::no_property,
    typename __EdgeListS = boost::listS>
class NamedGraph
{
public:
    using OutEdgeListS = __OutEdgeListS;
    using VertexListS = __VertexListS;
    using DirectedS = __DirectedS;
    using VertexProperty = __VertexProperty;
    using EdgeProperty = __EdgeProperty;
    using GraphProperty = __GraphProperty;
    using EdgeListS = __EdgeListS;
    using Graph = boost::adjacency_list<
        OutEdgeListS,
        VertexListS,
        DirectedS,
        VertexProperty,
        EdgeProperty,
        GraphProperty,
        EdgeListS>;
    using GraphTraits = boost::graph_traits<Graph>;
    using name_type = std::string;
    using vertex_index_type = GraphTraits::vertex_descriptor;
    using edge_index_type = GraphTraits::edge_descriptor;
    using vertex_map_type = std::map<name_type, vertex_index_type>;
    using edge_map_type = std::map<name_type, edge_index_type>;

public:
    template <typename __map_ref_type>
    class Access
    {
    public:
        using map_ref_type = __map_ref_type;
        using map_type = std::remove_cvref_t<map_ref_type>;
        using value_type = typename map_type::value_type;
        using index_type = value_type;

    private:
        map_ref_type &ref;

    public:
        ~Access() = default;
        Access(map_ref_type src) : ref(src) {}

        template <typename __self>
        map_ref_type UnWrap(this __self &&self)
        {
            return self.ref;
        }

        std::generator<const name_type &> Names() const
        {
            for (const auto &[n, i] : ref)
            {
                co_yield n;
            }
        }

        std::generator<index_type> Indices() const
        {
            for (const auto &[n, i] : ref)
            {
                co_yield i;
            }
        }

        template <typename __self>
        decltype(auto) Begin(this __self &&self)
        {
            return self.ref.begin();
        }

        template <typename __self>
        decltype(auto) End(this __self &&self)
        {
            return self.ref.end();
        }

        template <typename __self>
        decltype(auto) Iterators(this __self &&self)
        {
            return std::make_tuple(self.Begin(), self.End());
        }
    };

private:
    template <typename __self, typename map_type>
    struct AccessGen
    {
        using ref_type = std::conditional_t<
            std::is_const_v<__self>,
            const map_type &,
            map_type &>;
        using access_type = Access<ref_type>;
    };

    template <typename __self, typename map_type>
    using AccessGenT = AccessGen<__self, map_type>::access_type;

private:
    Graph graph;
    vertex_map_type vertices;
    edge_map_type edges;

public:
    ~NamedGraph() = default;
    NamedGraph() = default;

    template <typename __self>
    auto UnWrap(this __self &&self) -> std::conditional_t<std::is_const_v<__self>, const Graph &, Graph &>
    {
        return self.graph;
    }

    template <typename __return>
    __return ApplyUnWrap(std::function<__return(Graph &, vertex_map_type &, edge_map_type &)> apply_to_all_wrapped)
    {
        return apply_to_all_wrapped(graph, vertices, edges);
    }

    template <typename __self>
    decltype(auto) Vertices(this __self &&self)
    {
        return AccessGenT<__self, vertex_map_type>(self.vertices);
    }

    template <typename __self>
    decltype(auto) Edges(this __self &&self)
    {
        return AccessGenT<__self, edge_map_type>(self.edges);
    }

    vertex_index_type AddVertex(const name_type &name)
    {
        if (vertices.contains(name))
        {
            return vertices.at(name);
        }

        vertex_index_type idx = boost::add_vertex(graph);
        vertices.insert_or_assign(name, idx);
        return idx;
    }

    vertex_index_type AddVertex(const name_type &name, const VertexProperty &property)
    {
        if (vertices.contains(name))
        {
            return vertices.at(name);
        }

        vertex_index_type idx = boost::add_vertex(property, graph);
        vertices.insert_or_assign(name, idx);
        return idx;
    }

    edge_index_type AddEdge(const name_type &beg, const name_type &end, const name_type &name, bool *add_status_ptr = nullptr)
    {
        if (edges.contains(name))
        {
            return edges.at(name);
        }

        if (!(vertices.contains(beg) && vertices.contains(end)))
        {
            AGTB_THROW(std::invalid_argument, std::format("No vertex `{}` or `{}`", beg, end));
        }

        vertex_index_type beg_idx = vertices.at(beg), end_idx = vertices.at(end);
        auto pair = boost::add_edge(beg_idx, end_idx, graph);

        if (add_status_ptr != nullptr)
        {
            *add_status_ptr = pair.second;
        }

        edges.insert_or_assign(name, pair.first);
        return pair.first;
    }

    edge_index_type AddEdge(const name_type &beg, const name_type &end, const name_type &name, const EdgeProperty &property, bool *add_status_ptr = nullptr)
    {
        if (edges.contains(name))
        {
            return edges.at(name);
        }

        if (!(vertices.contains(beg) && vertices.contains(end)))
        {
            AGTB_THROW(std::invalid_argument, std::format("No vertex `{}` or `{}`", beg, end));
        }

        vertex_index_type beg_idx = vertices.at(beg), end_idx = vertices.at(end);
        auto pair = boost::add_edge(beg_idx, end_idx, property, graph);

        if (add_status_ptr != nullptr)
        {
            *add_status_ptr = pair.second;
        }

        edges.insert_or_assign(name, pair.first);
        return pair.first;
    }

    template <typename __self>
    decltype(auto) Vertex(this __self &&self, const name_type &name)
    {
        if (!self.vertices.contains(name))
        {
            AGTB_THROW(std::out_of_range, std::format("No vertex named {}", name));
        }

        return self.graph[self.vertices.at(name)];
    }

    template <typename __self>
    decltype(auto) Vertex(this __self &&self, const vertex_index_type &vidx)
    {
        return self.graph[vidx];
    }

    template <typename __self>
    decltype(auto) Edge(this __self &&self, const name_type &name)
    {
        if (!self.edges.contains(name))
        {
            AGTB_THROW(std::out_of_range, std::format("No edge named {}", name));
        }

        return self.graph[self.edges.at(name)];
    }

    template <typename __self>
    decltype(auto) Edge(this __self &&self, const edge_index_type &eidx)
    {
        return self.graph[eidx];
    }

    decltype(auto) Source(const name_type &name) const
    {
        if (!edges.contains(name))
        {
            AGTB_THROW(std::out_of_range, std::format("No edge named {}", name));
        }

        return boost::source(edges.at(name), graph);
    }

    decltype(auto) Target(const name_type &name) const
    {
        return boost::target(edges.at(name), graph);
    }

    name_type SourceName(const name_type &name) const
    {
        vertex_index_type tar = Source(name);
        for (const auto &[k, v] : vertices)
        {
            if (v == tar)
            {
                return k;
            }
        }
    }

    name_type TargetName(const name_type &name) const
    {
        vertex_index_type tar = Target(name);
        for (const auto &[k, v] : vertices)
        {
            if (v == tar)
            {
                return k;
            }
        }
    }

    template <typename __self>
    decltype(auto) SourceVertex(this __self &&self, const name_type &name)
    {
        return self.Vertex(self.Source(name));
    }

    template <typename __self>
    decltype(auto) TargetVertex(this __self &&self, const name_type &name)
    {
        return self.Vertex(self.Target(name));
    }

private:
    class VertexAdder
    {
    private:
        NamedGraph &ref;

    public:
        ~VertexAdder() = default;
        VertexAdder(NamedGraph &src) : ref(src) {}

        VertexAdder &operator()(const name_type &name)
        {
            ref.AddVertex(name);
            return *this;
        }

        VertexAdder &operator()(const name_type &name, const VertexProperty &property)
        {
            ref.AddVertex(name, property);
            return *this;
        }
    };

    class EdgeAdder
    {
    private:
        NamedGraph &ref;

    public:
        ~EdgeAdder() = default;
        EdgeAdder(NamedGraph &src) : ref(src) {}

        EdgeAdder &operator()(const name_type &beg, const name_type &end, const name_type &name, bool *add_status_ptr = nullptr)
        {
            ref.AddEdge(beg, end, name, add_status_ptr);
            return *this;
        }

        EdgeAdder &operator()(const name_type &beg, const name_type &end, const name_type &name, const EdgeProperty &property, bool *add_status_ptr = nullptr)
        {
            ref.AddEdge(beg, end, name, property, add_status_ptr);
            return *this;
        }
    };

public:
    VertexAdder AddVertex()
    {
        return VertexAdder(*this);
    }

    EdgeAdder AddEdge()
    {
        return EdgeAdder(*this);
    }
};

AGTB_CONTAINER_END

AGTB_BEGIN

using Container::NamedGraph;

AGTB_END

#endif