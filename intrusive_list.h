#include <iostream>
#include <iterator>
#include <algorithm>

// Fwd declarations
template <typename T> class in_list;
template <typename T,
         template<class T> class Node>
class intr_iterator ;

/* 
 * Objects for which there is a need to create intrusive list
 * must inherit from this class.
 */

template <typename T>
class ListNode {
	typedef ListNode<T> self_type;
	friend class in_list<T>;
	friend class intr_iterator<T, ListNode>;
	
public:
	// Constructor.
    // Point next and prev to self
    // while initializing.
	ListNode():mp_next_(this),
               mp_prev_(this){}
	
	// Copy constructor
	// Essentially does nothing 
	ListNode(const self_type& other):mp_next_(this)
                                    ,mp_prev_(this)
	{} 

	// Assignment operator
	// Essentially does nothing
	self_type& operator=(const self_type& other) {
		return *this;
	}

	~ListNode() {
		mp_prev_ = mp_next_ = NULL;
	}

public:
	// Conversion functions from obj to link type
	static self_type* convert2link(T* obj) {
		return static_cast<self_type*>(obj);
	}

	static self_type* convert2link(T& obj) {
		return convert2link(&obj);
	}

	const T* obj() {
		return static_cast<const T*>(this);
	}

	// Add a node before myself in the list
	void add_before(self_type* node)
	{
		ensure_unchained(node);

		// Classic list insertion
		node->mp_next_ = this;
		mp_prev_->mp_next_ = node;
		node->mp_prev_ = mp_prev_;
		mp_prev_ = node;
	}

	// Add a node after myself in the list
	void add_after(self_type* node)
	{
		ensure_unchained(node);

		// Classic list insertion
		node->mp_next_ = mp_next_;
		node->mp_prev_ = this;
		mp_next_->mp_prev_ = node;
		mp_next_ = node;
	}

	bool is_linked() const {
		return mp_next_ != this;
	}

private:
	void reset_node() {
		mp_prev_ = mp_next_ = this;
	}

	// Ensure that the node is not
	// chained.
	void ensure_unchained(self_type* node) {
		node->mp_next_->mp_prev_ = node->mp_next_;
		node->mp_prev_->mp_next_ = node->mp_prev_;
	}

	// Removes self from the intrusive list
    // and returns the pointer to the next object
	self_type* remove_self()
	{
		self_type* n = mp_next_;
		mp_prev_->mp_next_ = n;
		mp_next_->mp_prev_ = mp_prev_;
		reset_node();
		return n;
	}
private:
	self_type* mp_next_;
	self_type* mp_prev_;
};


/* Implement an iterator for the intrusive list */
template <typename T, 
		 template<class T> class Node>
class intr_iterator {
private:
	typedef intr_iterator<T,ListNode> self_type;

public:
	typedef T  value_type;
	typedef T* pointer;
	typedef const T* cpointer;
	typedef T& reference;
	typedef const T& creference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef std::bidirectional_iterator_tag iterator_category;

public:
	intr_iterator():link_(NULL)
	{}
	intr_iterator(pointer obj): link_(ListNode<T>::convert2link(obj))
	{}
	intr_iterator(reference obj): link_(ListNode<T>::convert2link(obj))
	{}
	intr_iterator(const self_type& obj): link_(obj.link_)
	{}
	intr_iterator(Node<T>* node): link_(node)
	{}
public:
	/* Implement usual operators applicable on iterators */
	bool operator==(self_type& x) const {
		return link_ == x.link_;
	}

	bool operator!=(const self_type& x) const {
		return link_ != x.link_;
	}

	creference operator*() const {
		return *(operator->());
	}
	
	cpointer operator->() const {
		return link_->obj();
	}

	self_type& operator++() { // pre-increment op
		link_ = link_->mp_next_;
		return *this;
	}

	self_type operator++(int) { // post increment op
		self_type tmp = *this;
		link_ = link_->mp_next_;
		return tmp;
	}

	self_type& operator--() { // pre-decrement op
		link_ = link_->mp_prev_;
		return *this;
	}

	self_type operator--(int) { // post-decrement op
		self_type tmp = *this;
		link_ = link_->mp_prev_;
		return tmp;
	}

	Node<T>* link() const {
		return link_;
	}

private:
	ListNode<T>* link_;
};


template <typename T>
class in_list {
public:
	typedef in_list<T> self_type;
	typedef ListNode<T> link_node;
	typedef intr_iterator<T, ListNode> iterator;
	typedef intr_iterator<T, ListNode> const_iterator;
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> reverse_const_iterator;

public:
	in_list(): link_()
	{}

	iterator begin() { return iterator(link_.mp_next_); }
	iterator end() { return iterator(&link_); }
	/*	
	const_iterator begin() const
	{ return const_iterator(link_.mp_next_); }

	const_iterator end() const
	{ return const_iterator(&link_); }
	*/

	reverse_iterator rbegin() { return reverse_iterator(end()); }
	reverse_iterator rend() { return reverse_iterator(begin()); }

	/*
	reverse_const_iterator rbegin() const
	{ return reverse_const_iterator(end()); }
	reverse_const_iterator rend() const
	{ return reverse_const_iterator((begin())); }
	*/

	bool empty() const {
		return !link_.is_linked();
	}

	// TODO: Make this const
	size_t size() {
		return std::distance(begin(), end());
	}

	T* front() { return &(*begin()); }
	T* back() { return &(*(--end())); }

	const T* front() const
	{  return &(*begin()); }
	const T* back() const
	{ return &(*(--end())); }

	iterator insert(iterator pos, T* obj) {
		pos.link()->add_before(link_node::convert2link(obj));
		print_addr();
		std::cout << "This address: " << pos.link() << std::endl;
		return iterator(obj);
	}

	iterator erase(iterator pos) {
		return iterator(pos.link().remove_self());
	}

	void push_front(T* obj) { insert(begin(), obj); }
	void push_back(T* obj) { insert(end(), obj); }
	void push_front(T obj) { insert(begin(), &obj); }
	void push_back(T obj) { insert(end(), &obj); }

	void pop_front() { erase(begin()); }
	void pop_back() { erase(--end()); }

	void print_addr() {
		std::cout << "Addr: " << &link_ << std::endl;
		std::cout << "Next addr: " << link_.mp_next_ << std::endl;
		std::cout << "Data: " << (link_.mp_next_)->obj()->test_data << std::endl;
	}

private: // Make this class non-copyable and non assignable
	in_list(const in_list&);
	void operator=(const in_list);

private:
	// This node is going to be the HEAD.
	// It will not be holding any data. 
	// It will start from mp_next_
	link_node link_;	
};
