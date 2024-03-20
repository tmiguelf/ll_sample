//======== ======== ======== ======== ======== ======== ======== ========
///	\file
///
///	\copyright
///		
//======== ======== ======== ======== ======== ======== ======== ========

#pragma once
//#define EASY_MODE

#ifdef EASY_MODE
#include <list>
//Technically meets all the requirements, but it looked too easy
template<typename T>
using dl_list = std::list<T>;

#else //EASY_MODE

#include <cstdint>
#include <initializer_list>
#include <iterator>
#include <utility>

template<typename T>
class dl_list;

namespace _p
{
	template<typename T>
	struct _Container;

	template<typename T>
	struct _ContainerHeader
	{
	public:
		_ContainerHeader();
		_Container<T>* next;
		_Container<T>* prev;
	};

	template<typename T>
	struct _Container: public _ContainerHeader<T>
	{
	public:
		template <class... Args>
		inline _Container(Args&&... args) :_ContainerHeader<T>(), obj{std::forward<Args>(args)...} {}

		T obj;
	};

	template<typename T>
	inline _ContainerHeader<T>::_ContainerHeader(): next(static_cast<_Container<T>*>(this)), prev(static_cast<_Container<T>*>(this)) {};


	template<typename T>
	class _ConstIterator
	{
		friend class dl_list<T>;
	protected:
		using _ParentList     = dl_list<T>;
		using _NodePtr        = _Container<T>*;

	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type      = typename _ParentList::value_type;
		using difference_type = intptr_t;
		using pointer         = value_type*;
		using reference       = value_type&;

	public:
		inline _ConstIterator()                      = default;
		inline _ConstIterator(_ConstIterator const&) = default;
		inline _ConstIterator(_ConstIterator&&)      = default;

	public:
		[[nodiscard]] inline bool operator == (_ConstIterator const& p_other) const noexcept { return p_other._container == _container; }

		inline _ConstIterator& operator = (_ConstIterator const& p_other) noexcept { _container = p_other._container; return *this; }
		inline _ConstIterator& operator = (_ConstIterator&& p_other) noexcept { _container = p_other._container; return *this; }

		inline _ConstIterator& operator ++()
		{
			_container = _container->next;
			return *this;
		}

		inline _ConstIterator operator ++(int)
		{
			_ConstIterator temp = *this;
			_container = _container->next;
			return temp;
		}

		inline _ConstIterator& operator --()
		{
			_container = _container->prev;
			return *this;
		}

		inline _ConstIterator operator --(int)
		{
			_ConstIterator temp = *this;
			_container = _container->prev;
			return temp;
		}

		[[nodiscard]] value_type const& operator*() const noexcept
		{
			return _container->obj;
		}

		[[nodiscard]] value_type const* operator->() const noexcept
		{
			return &(_container->obj);
		}

	protected:
		inline _ConstIterator(_Container<T>* const pos) noexcept: _container(pos) {}
		_NodePtr _container = nullptr;
	};

	template<typename T>
	class _Iterator final: public _ConstIterator<T>
	{
		friend class dl_list<T>;
	private:
		using _BaseT      = _ConstIterator<T>;
		using _ParentList = typename _BaseT::_ParentList;
	public:
		using value_type  = typename _ParentList::value_type;

	public:
		inline _Iterator()                 = default;
		inline _Iterator(_Iterator const&) = default;
		inline _Iterator(_Iterator&&)      = default;

		inline _Iterator& operator = (_Iterator const& p_other) noexcept { _ConstIterator<T>::operator = (p_other); return *this; }
		inline _Iterator& operator = (_Iterator&& p_other) noexcept { _ConstIterator<T>::operator = ( std::move(p_other)); return *this; }

		inline _Iterator& operator ++()
		{
			_BaseT::operator++();
			return *this;
		}
		inline _Iterator operator ++(int)
		{
			_Iterator temp = *this;
			_BaseT::operator++();
			return temp;
		}

		inline _Iterator& operator --()
		{
			_BaseT::operator--();
			return *this;
		}

		inline _Iterator operator --(int)
		{
			_Iterator temp = *this;
			_BaseT::operator--();
			return temp;
		}

		[[nodiscard]] value_type& operator*() const noexcept
		{
			return _BaseT::_container->obj;
		}

		[[nodiscard]] value_type* operator->() const noexcept
		{
			return &(_BaseT::_container->obj);
		}

	private:
		_Iterator(_Container<T>*const pos): _BaseT(pos) {}
	};

} //namespace _p


template<typename T>
class dl_list
{
public:
	using value_type      = T;
	using size_type       = uintptr_t;
	using reference       = value_type&;
	using const_reference = value_type const&;

	using iterator               = _p::_Iterator        <value_type>;
	using const_iterator         = _p::_ConstIterator   <value_type>;
	using reverse_iterator       = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	using _Container_t = _p::_Container<value_type>;

public:
	inline dl_list() = default;
	~dl_list()
	{
		_Container_t* pivot = __end.next;
		_Container_t* const end_p = _end_p();
		while(pivot != end_p)
		{
			_Container_t* const delete_me = pivot;
			pivot = pivot->next;
			delete delete_me;
		}
	}

	[[nodiscard]] inline iterator               begin  ()       noexcept { return iterator      {__end.next}; }
	[[nodiscard]] inline const_iterator         begin  () const noexcept { return const_iterator{__end.next}; }
	[[nodiscard]] inline const_iterator         cbegin () const noexcept { return const_iterator{__end.next}; }

	[[nodiscard]] inline iterator               end    ()       noexcept { return iterator      {_end_p()}; }
	[[nodiscard]] inline const_iterator         end    () const noexcept { return const_iterator{_end_p()}; }
	[[nodiscard]] inline const_iterator         cend   () const noexcept { return const_iterator{_end_p()}; }

	[[nodiscard]] inline reverse_iterator       rbegin ()       noexcept { return reverse_iterator(end()); }
	[[nodiscard]] inline const_reverse_iterator rbegin () const noexcept { return const_reverse_iterator(cend()); }
	[[nodiscard]] inline const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }

	[[nodiscard]] inline reverse_iterator       rend   ()       noexcept { return reverse_iterator(begin()); }
	[[nodiscard]] inline const_reverse_iterator rend   () const noexcept { return const_reverse_iterator(cbegin()); }
	[[nodiscard]] inline const_reverse_iterator crend  () const noexcept { return const_reverse_iterator(cbegin()); }

	[[nodiscard]] inline bool                   empty  () const noexcept { return __end.next == _end_p(); }

	void clear() noexcept
	{
		_Container_t* const end_p = _end_p();
		_Container_t* pivot = __end.next;
		__end.prev = end_p;
		__end.next = end_p;

		while(pivot != end_p)
		{
			_Container_t* const delete_me = pivot;
			pivot = pivot->next;
			delete delete_me;
		}
	}

	template< class... Args >
	iterator emplace(const_iterator pos, Args&&... args)
	{
		_Container_t* const next = pos._container;
		_Container_t* const prev = next->prev;
		_Container_t* const container =
			new _Container_t(std::forward<Args>(args)...);

		next     ->prev = container;
		prev     ->next = container;
		container->next = next;
		container->prev = prev;

		return iterator{container};
	}

	iterator insert(const_iterator const pos, const value_type& value)
	{
		return emplace(pos, value);
	}

	iterator insert(const_iterator const pos, value_type&& value)
	{
		return emplace(pos, std::move(value));
	}

	iterator erase(const_iterator const pos)
	{
		_Container_t* const container = pos._container;
		_Container_t* const prev      = container->prev;
		_Container_t* const next      = container->next;
		prev->next = next;
		next->prev = prev;
		delete container;

		return iterator{next};
	}

	iterator erase(const_iterator const first, const_iterator const last)
	{
		_Container_t* const prev      = first._container->prev;
		_Container_t* const last      = last._container;
		prev->next = last;
		last->prev = prev;

		_Container_t* pivot = first._container;
		while(pivot != last)
		{
			_Container_t* const delete_me = pivot;
			pivot = pivot->next;
			delete delete_me;
		}

		return iterator{last};
	}

	void push_back(const value_type& value)
	{
		emplace(end(), value);
	}

	void push_back(value_type&& value)
	{
		emplace(end(), std::move(value));
	}

	void pop_back()
	{
		_Container_t* const container = __end.prev;
		_Container_t* const prev = container->prev;
		prev->next = _end_p();
		__end.prev = prev;
		delete container;
	}

	void push_front(const value_type& value)
	{
		emplace(begin(), value);
	}

	void push_front(value_type&& value)
	{
		emplace(begin(), std::move(value));
	}

	void pop_front()
	{
		_Container_t* const container = __end.next;
		_Container_t* const next = container->next;
		next->prev = _end_p();
		__end.next = next;
		delete container;
	}

#if 0
	//could have implemented these but felt unecessary to meet the requirements
	//no need to waste time

	dl_list(size_type count, const value_type& value);
	explicit dl_list( size_type count);
	template< class InputIt >
	dl_list(InputIt first, InputIt last);
	dl_list(const dl_list& other);
	dl_list(dl_list&& other );
	dl_list(std::initializer_list<value_type> init);

	dl_list& operator=( const dl_list& other );
	dl_list& operator=( dl_list&& other );
	dl_list& operator=(std::initializer_list<value_type> ilist );

	void assign(size_type count, const value_type& value );
	template<class InputIt>
	void assign( InputIt first, InputIt last);
	void assign(std::initializer_list<value_type> ilist );

	reference front();
	const_reference front() const;

	reference back();
	const_reference back() const;

	size_type size() const noexcept;


	template<class... Args >
	reference emplace_back( Args&&... args );

	template< class... Args >
	reference emplace_front( Args&&... args );

	iterator insert(const_iterator pos, size_type count, const value_type& value );
	template< class InputIt >
	iterator insert( const_iterator pos, InputIt first, InputIt last );
	iterator insert( const_iterator pos, std::initializer_list<value_type> ilist );


	void resize( size_type count );
	void resize( size_type count, const value_type& value );

	void swap(dl_list& other) noexcept;

	void merge(dl_list& other );
	void merge(dl_list&& other );

	template< class Compare >
	void merge(dl_list& other, Compare comp );

	template< class Compare >
	void merge(dl_list&& other, Compare comp );

	void splice( const_iterator pos, dl_list& other );
	void splice( const_iterator pos, dl_list&& other );
	void splice( const_iterator pos, dl_list& other, const_iterator it );
	void splice( const_iterator pos, dl_list&& other, const_iterator it );
	void splice( const_iterator pos, dl_list& other,
		const_iterator first, const_iterator last);
	void splice( const_iterator pos, dl_list&& other,
		const_iterator first, const_iterator last );

	size_type remove( const value_type& value );
	template< class UnaryPredicate >
	size_type remove_if( UnaryPredicate p );

	void reverse();
	void reverse() noexcept;
	size_type unique();
	template< class BinaryPredicate >
	size_type unique( BinaryPredicate p );

	void sort();
	template< class Compare >
	void sort( Compare comp );
#endif

private:
	[[nodiscard]] inline _Container_t* _end_p() const
	{
		return const_cast<_Container_t* const>(static_cast<_Container_t const* const>(&__end));
	}

	_p::_ContainerHeader<value_type> __end;
};

#endif // !EASY_MODE
