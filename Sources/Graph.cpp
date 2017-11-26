/*#include "Graph.hpp"

using namespace std;

template <class T>
Graph<T>::Graph() {
    this->vertices = new vector<Node<T> *>();
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
Node<T>::Node(T item, vector<Node<T> *> * edges) {
    this->data = item;
    this->edges = edges;
}

template <class T>
Node<T>::Node(T item, int n, Node<T> * edges) {
    this->data = item;
    this->edges = new vector<Node<T> *>(n);
    for (int i = 0; i < n; i++)
        this->edges[i] = edges[i];
}
*/