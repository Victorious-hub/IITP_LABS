#include <iostream>
#include <string>
#include <memory>
#include <type_traits>
#include <algorithm>
#include <queue>
#include <vector>

template<typename T>
class sharedPtr {
public:
	using element_type = T;

	sharedPtr() : ptr_(nullptr), ref_count(nullptr) {}

	explicit sharedPtr(T* p) : ptr_(p), ref_count(new int(1)) {}

	sharedPtr(const sharedPtr<T>& other) : ptr_(other.ptr_), ref_count(other.ref_count) {
		if (ref_count) {
			++(*ref_count);
		}
	}

	sharedPtr(sharedPtr<T>&& other) : ptr_(nullptr), ref_count(nullptr) {
		swap(other);
	}

	sharedPtr& operator=(const sharedPtr& sp) noexcept {
		if (ptr_ != sp.ptr_) {
			release();
			ptr_ = sp.ptr_;
			ref_count = sp.ref_count;
			if (ref_count) {
				++(*ref_count);
			}
		}
		return *this;
	}

	sharedPtr& operator=(std::nullptr_t)
	{
		reset();
		return *this;
	}

	sharedPtr& operator=(sharedPtr&& sp) noexcept {
		if (ptr_ != sp.ptr_) {
			release();
			std::swap(ptr_, sp.ptr_);
			std::swap(ref_count, sp.ref_count);
		}
		return *this;
	}

	template <class Other>
	sharedPtr& operator=(const sharedPtr<Other>& sp) noexcept {
		release();
		ptr_ = sp.ptr_;
		ref_count = sp.ref_count;
		if (ref_count) {
			++(*ref_count);
		}
		return *this;
	}

	template <class Other>
	sharedPtr& operator=(sharedPtr<Other>&& sp) noexcept {
		release();
		std::swap(ptr_, sp.ptr_);
		std::swap(ref_count, sp.ref_count);
		return *this;
	}

	~sharedPtr() {
		release();
	}

	T* get() const noexcept {
		return ptr_;
	}

	void reset(T* p = nullptr) {
		release();
		ptr_ = p;
		ref_count = new int(1);
	}

	void swap(sharedPtr<T>& other) {
		std::swap(ptr_, other.ptr_);
		std::swap(ref_count, other.ref_count);
	}


	explicit operator bool() const noexcept {
		return ptr_ != nullptr;
	}



	T* operator->() const {
		return ptr_;
	}

	bool operator==(std::nullptr_t) const noexcept {
		return ptr_ == nullptr;
	}

	bool operator==(const sharedPtr& ptr) const noexcept {
		return ptr_ == ptr.ptr_;
	}

	bool operator!=(const sharedPtr& ptr) const noexcept {
		return ptr_ != ptr.ptr_;
	}

	friend std::ostream& operator<<(std::ostream& os, const sharedPtr<T>& p) {
		os << p.ptr_;
		return os;
	}

	void release() {
		if (ref_count) {
			--(*ref_count);
			if (*ref_count == 0) {
				delete ptr_;
				delete ref_count;
			}
			ptr_ = nullptr;
			ref_count = nullptr;
		}
	}

private:
	T* ptr_;
	int* ref_count;
};

template<typename T, typename... Args>
sharedPtr<T> MakeShared(Args&&... args) {
	return sharedPtr<T>(new T(std::forward<Args>(args)...));
}


typedef bool color_type;
template<class KeyType, class ValueType>
struct Node {

	std::pair<KeyType, ValueType> container;

	sharedPtr<Node<KeyType, ValueType>> parent;

	sharedPtr<Node<KeyType, ValueType>> left;

	sharedPtr<Node<KeyType, ValueType>> right;

	color_type color; // 1 -> Red, 0 -> Black
	Node(KeyType key, ValueType value)
	{
		container.first = key;
		container.second = value;
		left = nullptr;
		right = nullptr;
		parent = nullptr;
		color = true;
	}
	Node()
	{
		container.first = KeyType();
		container.second = ValueType();
		left = nullptr;
		right = nullptr;
		parent = nullptr;
		color = true;
	}

};

template<class KeyType, class ValueType>
class Map {
private:
	sharedPtr<Node<KeyType, ValueType>> root;

	sharedPtr<Node<KeyType, ValueType>> nullptr_node;

	ValueType searchTreeHelper(sharedPtr<Node<KeyType, ValueType>> node, KeyType key) {
		if (node == nullptr_node || key == node->container.first) {
			return  node->container.second;
		}

		if (key < node->container.first) {
			return searchTreeHelper(node->left, key);
		}
		return searchTreeHelper(node->right, key);
	}


	void getRotationColorChange(sharedPtr<Node<KeyType, ValueType>> node) {
		sharedPtr<Node<KeyType, ValueType>> uncle_node;
		while (node->parent->color == true) {
			if (node->parent == node->parent->parent->right) {
				uncle_node = node->parent->parent->left; // uncle
				if (uncle_node->color == true) {
					// case 3.1
					uncle_node->color = false;
					node->parent->color = false;
					node->parent->parent->color = true;
					node = node->parent->parent;
				}
				else {
					if (node == node->parent->left) {
						// case 3.2.2
						node = node->parent;
						rightRotate(node);
					}
					// case 3.2.1
					node->parent->color = false;
					node->parent->parent->color = true;
					leftRotate(node->parent->parent);
				}
			}
			else {
				uncle_node = node->parent->parent->right; // uncle

				if (uncle_node->color == true) {
					// mirror case 3.1
					uncle_node->color = false;
					node->parent->color = false;
					node->parent->parent->color = true;
					node = node->parent->parent;
				}
				else {
					if (node == node->parent->right) {
						// mirror case 3.2.2
						node = node->parent;
						leftRotate(node);
					}
					// mirror case 3.2.1
					node->parent->color = false;
					node->parent->parent->color = true;
					rightRotate(node->parent->parent);
				}
			}
			if (node == root) {
				break;
			}
		}
		root->color = false;
	}

	void deleteFix(sharedPtr<Node<KeyType, ValueType>> x) {
		sharedPtr<Node<KeyType, ValueType>> s;
		while (x != root && x->color == 0) {
			if (x == x->parent->left) {
				s = x->parent->right;
				if (s->color == 1) {
					s->color = 0;
					x->parent->color = 1;
					leftRotate(x->parent);
					s = x->parent->right;
				}

				if (s->left->color == 0 && s->right->color == 0) {
					s->color = 1;
					x = x->parent;
				}
				else {
					if (s->right->color == 0) {
						s->left->color = 0;
						s->color = 1;
						rightRotate(s);
						s = x->parent->right;
					}

					s->color = x->parent->color;
					x->parent->color = 0;
					s->right->color = 0;
					leftRotate(x->parent);
					x = root;
				}
			}
			else {
				s = x->parent->left;
				if (s->color == 1) {
					s->color = 0;
					x->parent->color = 1;
					rightRotate(x->parent);
					s = x->parent->left;
				}

				if (s->right->color == 0 && s->right->color == 0) {
					s->color = 1;
					x = x->parent;
				}
				else {
					if (s->left->color == 0) {
						s->right->color = 0;
						s->color = 1;
						leftRotate(s);
						s = x->parent->left;
					}

					s->color = x->parent->color;
					x->parent->color = 0;
					s->left->color = 0;
					rightRotate(x->parent);
					x = root;
				}
			}
		}
		x->color = 0;
	}

	void rbTransplant(sharedPtr<Node<KeyType, ValueType>> u, sharedPtr<Node<KeyType, ValueType>> v) {
		if (u->parent == nullptr) {
			root = v;
		}
		else if (u == u->parent->left) {
			u->parent->left = v;
		}
		else {
			u->parent->right = v;
		}
		v->parent = u->parent;
	}

	void deleteNodeHelper(sharedPtr<Node<KeyType, ValueType>> node, KeyType key) {
		sharedPtr<Node<KeyType, ValueType>>  z = nullptr_node;
		sharedPtr<Node<KeyType, ValueType>>  x, y;
		while (node != nullptr_node) {
			if (node->container.first == key) {
				z = node;
			}

			if (node->container.first <= key) {
				node = node->right;
			}
			else {
				node = node->left;
			}
		}

		if (z == nullptr_node) {
			std::cout << "Key not found in the tree" << std::endl;
			return;
		}

		y = z;
		int y_original_color = y->color;
		if (z->left == nullptr_node) {
			x = z->right;
			rbTransplant(z, z->right);
		}
		else if (z->right == nullptr_node) {
			x = z->left;
			rbTransplant(z, z->left);
		}
		else {
			y = minimum(z->right);
			y_original_color = y->color;
			x = y->right;
			if (y->parent == z) {
				x->parent = y;
			}
			else {
				rbTransplant(y, y->right);
				y->right = z->right;
				y->right->parent = y;
			}

			rbTransplant(z, y);
			y->left = z->left;
			y->left->parent = y;
			y->color = z->color;
		}
		z.reset();
		if (y_original_color == 0) {
			deleteFix(x);
		}
	}

	void inOrderHelper(sharedPtr<Node<KeyType, ValueType>> node) {
		if (node != nullptr_node) {
			inOrderHelper(node->left);
			std::cout << node->container.second << " ";
			inOrderHelper(node->right);
		}
	}


public:
	Map() {
		nullptr_node = sharedPtr<Node<KeyType, ValueType>>(new Node<KeyType, ValueType>());
		nullptr_node->color = false;
		nullptr_node->left = nullptr;
		nullptr_node->right = nullptr;
		root = nullptr_node;
	}

	void inorder() {
		inOrderHelper(this->root);
	}

	sharedPtr<Node<KeyType, ValueType>> minimum(sharedPtr<Node<KeyType, ValueType>> node) {
		while (node->left != nullptr_node) {
			node = node->left;
		}
		return node;
	}

	sharedPtr<Node<KeyType, ValueType>> maximum(sharedPtr<Node<KeyType, ValueType>> node) {
		while (node->right != nullptr_node) {
			node = node->right;
		}
		return node;
	}

	sharedPtr<Node<KeyType, ValueType>> successor(sharedPtr<Node<KeyType, ValueType>> x) {
		if (x->right != nullptr_node) {
			return minimum(x->right);
		}

		sharedPtr<Node<KeyType, ValueType>> y = x->parent;
		while (y != nullptr_node && x == y->right) {
			x = y;
			y = y->parent;
		}
		return y;
	}

	sharedPtr<Node<KeyType, ValueType>> predecessor(sharedPtr<Node<KeyType, ValueType>> x) {
		if (x->left != nullptr_node) {
			return maximum(x->left);
		}

		sharedPtr<Node<KeyType, ValueType>> y = x->parent;
		while (y != nullptr_node && x == y->left) {
			x = y;
			y = y->parent;
		}

		return y;
	}


	void leftRotate(sharedPtr<Node<KeyType, ValueType>> x) {
		sharedPtr<Node<KeyType, ValueType>> y = x->right;
		x->right = y->left;
		if (y->left != nullptr_node) {
			y->left->parent = x;
		}
		y->parent = x->parent;
		if (x->parent == nullptr) {
			this->root = y;
		}
		else if (x == x->parent->left) {
			x->parent->left = y;
		}
		else {
			x->parent->right = y;
		}
		y->left = x;
		x->parent = y;
	}

	void rightRotate(sharedPtr<Node<KeyType, ValueType>> x) {
		sharedPtr<Node<KeyType, ValueType>> y = x->left;
		x->left = y->right;
		if (y->right != nullptr_node) {
			y->right->parent = x;
		}
		y->parent = x->parent;
		if (x->parent == nullptr) {
			this->root = y;
		}
		else if (x == x->parent->right) {
			x->parent->right = y;
		}
		else {
			x->parent->left = y;
		}
		y->right = x;
		x->parent = y;
	}

	void insert(KeyType key, ValueType value) {

		sharedPtr<Node<KeyType, ValueType>> node = sharedPtr<Node<KeyType, ValueType>>(new Node<KeyType, ValueType>(key, value));
		node->container.first = key;
		node->container.second = value;
		node->left = nullptr_node;
		node->right = nullptr_node;

		sharedPtr<Node<KeyType, ValueType>> y;
		y = nullptr;
		sharedPtr<Node<KeyType, ValueType>> x = this->root;

		while (x != nullptr_node) {
			y = x;
			if (node->container.first < x->container.first) {
				x = x->left;
			}
			else {
				x = x->right;
			}
		}


		node->parent = y;
		if (y == nullptr) {
			root = node;
		}
		else if (node->container.first < y->container.first) {
			y->left = node;
		}
		else {
			y->right = node;
		}

		if (node->parent == nullptr) {
			node->color = false;
			return;
		}


		if (node->parent->parent == nullptr) {
			return;
		}

		getRotationColorChange(node);
	}


	void deleteNode(KeyType data) {
		deleteNodeHelper(this->root, data);
	}

	ValueType searchTree(KeyType k) {
		return searchTreeHelper(this->root, k);
	}

	ValueType operator[](const KeyType& key)
	{
		return searchTree(key);
	}

	sharedPtr<Node<KeyType, ValueType>> getRoot() {
		return this->root;
	}

	sharedPtr<Node<KeyType, ValueType>> next(sharedPtr<Node<KeyType, ValueType>> node)
	{
		sharedPtr<Node<KeyType, ValueType>> tmp = node->right;

		if (tmp) {
			while (tmp->left) tmp = tmp->left;
			return tmp;
		}
		tmp = node;
		while (tmp->parent && tmp.equals(tmp->parent->right))
			tmp = tmp->parent;

		return tmp->parent;
	}

	class iterator
	{
	private:
		sharedPtr<Node<KeyType, ValueType>> iter;
		sharedPtr<Node<KeyType, ValueType>> root_iter;
	public:
		std::pair<KeyType, ValueType> pair;
		iterator() {}
		iterator(sharedPtr<Node<KeyType, ValueType>> root) :iter(root) {
			pair.first = root->container.first;
			pair.second = root->container.second;
		}

		bool operator==(const iterator& wp)
		{
			return wp.iter == iter;
		}
		bool operator!=(const iterator& wp)
		{
			return wp.iter != iter;
		}
	};


	iterator begin()
	{
		sharedPtr<Node<KeyType, ValueType>> current = this->root;

		return iterator(minimum(current));
	}
	iterator end()
	{
		sharedPtr<Node<KeyType, ValueType>> current = this->root;
		return iterator(maximum(current));
	}

};
