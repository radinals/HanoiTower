#ifndef LINKEDLIST_H
#define LINKEDLIST_H

template <typename T> struct LinkedListNode {
	T data;
	LinkedListNode* next = nullptr;
};

template <typename T> class LinkedList
{
      public:
	LinkedListNode<T>*m_head = nullptr, *m_tail = nullptr;

	LinkedList(LinkedList&&) = delete;
	LinkedList() {}
	~LinkedList() { clear(); }

	bool isEmpty() { return m_head == nullptr; }

	void append(T d)
	{
		LinkedListNode<T>* new_node = new LinkedListNode<T>;
		new_node->data = d;
		if (isEmpty()) {
			m_head = new_node;
			m_tail = m_head;
		} else {
			LinkedListNode<T>* old_head = m_head;
			new_node->next = old_head;
			m_head = new_node;
		}
	}

	void clear()
	{
		LinkedListNode<T>* node = m_head;
		while (node != nullptr) {
			LinkedListNode<T>* del = node;
			node = node->next;
			delete del;
		}
		m_head = nullptr;
	}
};

#endif // LINKEDLIST_H
