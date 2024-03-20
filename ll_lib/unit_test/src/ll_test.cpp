//======== ======== ======== ======== ======== ======== ======== ========
///	\file
///
///	\copyright
///		
//======== ======== ======== ======== ======== ======== ======== ========

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <random>
#include <limits>
#include <list>

#include <ll_lib/ll_lib.hpp>


class ConstructionTester
{
public:
	enum class Type: uint8_t
	{
		default_ctor,
		copy_ctor,
		move_ctor,
		arg_ctor,
		arg2_ctor,
	};

public:
	inline ConstructionTester()                         : m_type{Type::default_ctor} {}
	inline ConstructionTester(ConstructionTester const&): m_type{Type::copy_ctor   } {}
	inline ConstructionTester(ConstructionTester&&)     : m_type{Type::move_ctor   } {}
	inline ConstructionTester(uint32_t)                 : m_type{Type::arg_ctor    } {}
	inline ConstructionTester(uint32_t, uint32_t)       : m_type{Type::arg2_ctor   } {}

	Type type() const { return m_type; }
public:
	Type const m_type;
};

template<typename T>
void standard_list_equivalence_test(dl_list<T> const& p_list, std::list<T> const& p_reference)
{
	{
		using dl_it = typename dl_list<T>::const_iterator;
		dl_it it = p_list.cbegin();
		for(T const& ref : p_reference)
		{
			ASSERT_EQ(ref, *it);
			++it;
		}
		ASSERT_TRUE(it == p_list.cend());
	}

	{
		using dl_it  = typename dl_list<T>::const_reverse_iterator;
		using std_it = typename std::list<T>::const_reverse_iterator;
		dl_it it = p_list.crbegin();
		for(
			std_it it_r = p_reference.crbegin(), it_end_r = p_reference.crend();
			it_r != it_end_r;
			++it_r, ++it)
		{
			ASSERT_EQ(*it_r, *it);
		}
		ASSERT_TRUE(it == p_list.crend());
	}
}

TEST(dl_list, empty)
{
	dl_list<uint32_t> list;

	ASSERT_TRUE(list.empty());

	list.push_back(1235);
	
	ASSERT_FALSE(list.empty());

	list.pop_back();
	
	ASSERT_TRUE(list.empty());
}

TEST(dl_list, clear)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<uint32_t> distrib(0, 1024);

	uintptr_t const test_size = std::clamp<uintptr_t>(static_cast<uintptr_t>(distrib(gen)), 1, 1024);

	dl_list<uint32_t> list;

	for(uintptr_t tcount = test_size; --tcount;)
	{
		const uint32_t tcase = distrib(gen);
		list.push_back(tcase);
	}

	ASSERT_FALSE(list.empty());

	list.clear();

	ASSERT_TRUE(list.empty());
}


TEST(dl_list, push_back)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<uint32_t> distrib(0, 1024);

	uintptr_t const test_size = std::clamp<uintptr_t>(static_cast<uintptr_t>(distrib(gen)), 1, 1024);

	dl_list<uint32_t> list;
	std::list<uint32_t> reference;

	for(uintptr_t tcount = test_size; --tcount;)
	{
		const uint32_t tcase = distrib(gen);
		list.push_back(tcase);
		reference.push_back(tcase);

		ASSERT_EQ(*(--list.end()), tcase);
	}

	standard_list_equivalence_test(list, reference);
}

TEST(dl_list, push_back_modes)
{
	dl_list<ConstructionTester> list;
	ConstructionTester temp;

	list.push_back(temp);

	ASSERT_EQ((--list.end())->type(), ConstructionTester::Type::copy_ctor);

	list.push_back(std::move(temp));

	ASSERT_EQ((--list.end())->type(), ConstructionTester::Type::move_ctor);
}
TEST(dl_list, push_front)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<uint32_t> distrib(0, 1024);

	uintptr_t const test_size = std::clamp<uintptr_t>(static_cast<uintptr_t>(distrib(gen)), 1, 1024);

	dl_list<uint32_t> list;
	std::list<uint32_t> reference;

	for(uintptr_t tcount = test_size; --tcount;)
	{
		const uint32_t tcase = distrib(gen);
		list.push_front(tcase);
		reference.push_front(tcase);

		ASSERT_EQ(*list.begin(), tcase);
	}

	standard_list_equivalence_test(list, reference);
}

TEST(dl_list, push_front_modes)
{
	dl_list<ConstructionTester> list;
	ConstructionTester temp;

	list.push_front(temp);

	ASSERT_EQ(list.begin()->type(), ConstructionTester::Type::copy_ctor);

	list.push_front(std::move(temp));

	ASSERT_EQ(list.begin()->type(), ConstructionTester::Type::move_ctor);
}

TEST(dl_list, pop_front)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<uint32_t> distrib(0, 1024);

	uintptr_t const add_count = std::clamp<uintptr_t>(static_cast<uintptr_t>(distrib(gen)), 2, 1024);
	uintptr_t const test_size = std::clamp<uintptr_t>(static_cast<uintptr_t>(distrib(gen)), 1, add_count - 1);

	dl_list<uint32_t> list;
	std::list<uint32_t> reference;
	for(uintptr_t tcount = add_count; --tcount;)
	{
		const uint32_t tcase = distrib(gen);
		list.push_front(tcase);
		reference.push_front(tcase);
	}

	for(uintptr_t tcount = test_size; --tcount;)
	{
		list.pop_front();
		reference.pop_front();
	}
	standard_list_equivalence_test(list, reference);
}

TEST(dl_list, pop_back)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<uint32_t> distrib(0, 1024);

	uintptr_t const add_count = std::clamp<uintptr_t>(static_cast<uintptr_t>(distrib(gen)), 2, 1024);
	uintptr_t const test_size = std::clamp<uintptr_t>(static_cast<uintptr_t>(distrib(gen)), 1, add_count - 1);

	dl_list<uint32_t> list;
	std::list<uint32_t> reference;
	for(uintptr_t tcount = add_count; --tcount;)
	{
		const uint32_t tcase = distrib(gen);
		list.push_front(tcase);
		reference.push_front(tcase);
	}

	for(uintptr_t tcount = test_size; --tcount;)
	{
		list.pop_back();
		reference.pop_back();
	}
	standard_list_equivalence_test(list, reference);
}

TEST(dl_list, emplace)
{
	using iterator = dl_list<uint32_t>::iterator;
	using std_iterator = std::list<uint32_t>::iterator;

	dl_list<uint32_t> list;

	//empty begin = end
	{
		uint32_t const test_val = 56;
		iterator temp = list.emplace(list.begin(), test_val);
		ASSERT_EQ(*temp, test_val);
		list.clear();
	}

	std::list<uint32_t> reference;

	{
		iterator     it = list.emplace(list.end(), uint32_t{37});
		std_iterator std_it = reference.emplace(reference.end(), uint32_t{37});

		list.emplace(list.begin(), uint32_t{123456});
		reference.emplace(reference.begin(), uint32_t{123456});

		list.emplace(list.end(), uint32_t{66666});
		reference.emplace(reference.end(), uint32_t{66666});

		list.emplace(it, uint32_t{111111111});
		reference.emplace(std_it, uint32_t{111111111});
	}
	standard_list_equivalence_test(list, reference);
}

TEST(dl_list, emplace_mode)
{
	using iterator = dl_list<ConstructionTester>::iterator;
	dl_list<ConstructionTester> list;
	ConstructionTester temp;

	list.emplace(list.end());
	list.emplace(list.end(), uint32_t{66666});
	list.emplace(list.end(), uint32_t{123}, uint32_t{123});
	list.emplace(list.end(), temp);
	list.emplace(list.end(), std::move(temp));

	iterator it = list.begin();
	ASSERT_EQ((it++)->type(), ConstructionTester::Type::default_ctor);
	ASSERT_EQ((it++)->type(), ConstructionTester::Type::arg_ctor);
	ASSERT_EQ((it++)->type(), ConstructionTester::Type::arg2_ctor);
	ASSERT_EQ((it++)->type(), ConstructionTester::Type::copy_ctor);
	ASSERT_EQ((it++)->type(), ConstructionTester::Type::move_ctor);
}


TEST(dl_list, insert)
{
	using iterator = dl_list<uint32_t>::iterator;
	using std_iterator = std::list<uint32_t>::iterator;

	dl_list<uint32_t> list;

	//empty begin = end
	{
		uint32_t const test_val = 56;
		iterator temp = list.insert(list.begin(), test_val);
		ASSERT_EQ(*temp, test_val);
		list.clear();
	}

	std::list<uint32_t> reference;

	{
		iterator     it = list.insert(list.end(), uint32_t{37});
		std_iterator std_it = reference.insert(reference.end(), uint32_t{37});

		list.insert(list.begin(), uint32_t{123456});
		reference.insert(reference.begin(), uint32_t{123456});

		list.insert(list.end(), uint32_t{66666});
		reference.insert(reference.end(), uint32_t{66666});

		list.insert(it, uint32_t{111111111});
		reference.insert(std_it, uint32_t{111111111});
	}
	standard_list_equivalence_test(list, reference);
}

TEST(dl_list, insert_mode)
{
	using iterator = dl_list<ConstructionTester>::iterator;
	dl_list<ConstructionTester> list;
	ConstructionTester temp;

	list.insert(list.end(), temp);
	list.insert(list.end(), std::move(temp));

	iterator it = list.begin();
	ASSERT_EQ((it++)->type(), ConstructionTester::Type::copy_ctor);
	ASSERT_EQ((it++)->type(), ConstructionTester::Type::move_ctor);
}


TEST(dl_list, erase)
{
	using iterator     = dl_list<uint32_t>::iterator;
	using std_iterator = std::list<uint32_t>::iterator;

	dl_list<uint32_t> list;
	std::list<uint32_t> reference;

	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<uint32_t> distrib(0, std::numeric_limits<uint32_t>::max());

		for(uintptr_t tcount = 1024; --tcount;)
		{
			const uint32_t tcase = distrib(gen);
			list.push_front(tcase);
			reference.push_front(tcase);
		}
	}

	iterator it = list.begin();
	std_iterator std_it = reference.begin();

	it     = list.erase(it);
	std_it = reference.erase(std_it);

	ASSERT_EQ(*it, *std_it);
	standard_list_equivalence_test(list, reference);

	for(uintptr_t tcount = 254; --tcount;)
	{
		++it;
		++std_it;
	}

	iterator it_last = it;
	std_iterator std_it_last = std_it;

	for(uintptr_t tcount = 321; --tcount;)
	{
		++it_last;
		++std_it_last;
	}

	it     = list.erase(it);
	std_it = reference.erase(std_it);

	ASSERT_EQ(*it, *std_it);
	standard_list_equivalence_test(list, reference);
}
