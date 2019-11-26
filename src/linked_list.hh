#pragma once

#include "../src/common.hh"

namespace r2 {

template <typename T> struct Node {
  Node *prev_p = nullptr;
  Node *next_p = nullptr;
  T val;

  explicit Node(const T &val) : val(val) {}

  Node *set_prev(Node *p) {
    this->prev_p = p;
    return this;
  }

  Node *set_next(Node *p) {
    this->next_p = p;
    return this;
  }
};

template <typename T> class LinkedList {
public:
  Node<T> *header_p = nullptr;
  Node<T> *tailer_p = nullptr;

  bool null() const {
    return header_p == nullptr;
  }

  inline Node<T> *add(Node<T> *n) {
    auto prev = tailer_p;

    if (unlikely(null())) {
      tailer_p = ( header_p = n);
      prev = n;
    }
    tailer_p->set_next(n);
    tailer_p = n;

    // reset n's position
    n->set_prev(prev)->set_next(header_p);

    return n;
  }

  /*!
    \note: the n must be in the list
    \ret: the next element
    TODO: what if there is only one coroutine ?
   */
  inline Node<T> *leave_one(Node<T> *n) {
    auto next = n->next_p;
    n->prev_p->set_next(next);
    next->set_prev(n->prev_p);

    if (tailer_p == n) {
      tailer_p = n->prev_p;
    }
    return next;
  }

  /*!
    Peek the first node from the list
   */
  inline Option<Node<T> *> peek() {
    if (unlikely(null()))
      return {};
    auto res = header_p;
    if (unlikely(header_p->next_p == header_p))
      header_p = nullptr;
    else
      header_p = header_p->next_p;

    return res;
  }

};

} // namespace r2
