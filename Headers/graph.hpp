#ifndef GRAPH_H_GUARD
#define GRAPH_H_GUARD
#pragma once//there were issues....

#include <vector>

template <class T>
class Node {
private:
public:
	T data;
	/* TODO: change to unordered_set for fast lookup, yet easy ---iteration--- <nope> */
	std::vector<Node<T> *> * edges;
    Node(T item, std::vector<Node<T> *> * edges);
    Node(T item, int n, Node<T> * edges);
    ~Node() {
        delete edges;
        edges = nullptr;
    };
};

template <class T>
class Graph {
private:
public:
    //std::vector<std::pair<Node<T> *, Node<T> *>> * all_edges;//hack, just for the vis
	/* TODO: change to unordered_set for fast lookup, yet easy  ---iteration--- <nope> */
	std::vector<Node<T> *> * vertices;
    Graph();
    ~Graph() {
        for (size_t i = 0; i < vertices.size(); i++)
            delete vertices[i];
        delete vertices;
    };
    void add_vertex(Node<T> * v);
    void add_directed_edge(Node<T> * v, Node<T> * u);
    void add_edge(Node<T> * v, Node<T> * u);
};

#endif // GRAPH_H_GUARD

/* it really bothers me that this all can't go in a .cpp */
template <class T>
Graph<T>::Graph() {
    this->vertices = new std::vector<Node<T> *>();
}

template <class T>
void Graph<T>::add_vertex(Node<T> * v) {
    this->vertices->push_back(v);
}

template <class T>
void Graph<T>::add_directed_edge(Node<T> * v, Node<T> * u) {
    //todo: check that u and v are in vertices
    v->edges->push_back(u);
}

template <class T>
void Graph<T>::add_edge(Node<T> * v, Node<T> * u) {
    //todo: check that u and v are in vertices
    this->add_directed_edge(v, u);
    this->add_directed_edge(u, v);
}


template <class T>
Node<T>::Node(T item, std::vector<Node<T> *> * edges) {
    this->data = item;
    this->edges = edges;
}

template <class T>
Node<T>::Node(T item, int n, Node<T> * edges) {
    this->data = item;
    this->edges = new std::vector<Node<T> *>(n);
    for (int i = 0; i < n; i++)
        this->edges[i] = edges[i];
}
