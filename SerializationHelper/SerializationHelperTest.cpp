#include "SerializationHelper.h"
#include "SerializationHelperContainers.h"
#include <assert.h>
#include <string>
#include <vector>
#include <map>

template<typename ValueType>
bool testType(const ValueType & a, const ValueType & b)
{
	const ValueType t1 = a;
	ValueType t2 = b;

	// basic serialization test
	std::vector<unsigned char> ar1;
	ar1 << t1;
	ar1.begin() >> t2;
	assert(t1 == t2);
	if (t1 != t2)
		return false;

	// const initialization test
	const ValueType t3(initializeFrom<ValueType>(ar1.begin()));
	assert(t3 == t1);
	if (t3 != t1)
		return false;

	// inequality
	const ValueType t4 = b;
	std::vector<unsigned char> ar2;
	ar2 << t4;
	const ValueType t5(initializeFrom<ValueType>(ar2.begin()));
	assert(t5 == t4);
	if (t5 != t4)
		return false;
	assert(t4 != t1);
	if (t4 == t1)
		return false;

	return true;
}

class A
{
public:
	A() : _i(2) {};
	~A() {}
	void mutate() { _i = _i * 2; }

	void writeTo(std::vector<unsigned char> & target) const
	{
		target << _i;
	}

	void readFrom(std::vector<unsigned char>::const_iterator & source)
	{
		source >> _i;
	}

	bool operator==(const A & rhs) const
	{
		return _i == rhs._i;
	}

	bool operator !=(const A & rhs) const
	{
		return _i != rhs._i;
	}
private:
	int _i;
};

class Base
{
public:
	Base() : _i() {};
	virtual ~Base() {}
	virtual void mutate() { _i++; }

	virtual void writeTo(std::vector<unsigned char> & target) const
	{
		target << _i;
	}

	virtual void readFrom(std::vector<unsigned char>::const_iterator & source)
	{
		source >> _i;
	}

	virtual bool operator==(const Base & rhs) const
	{
		return rhs._i == _i;
	}

	virtual bool operator != (const Base & rhs) const
	{
		return rhs._i != _i;
	}

private:
	int _i;
};

class Derived : public Base
{
public:
	Derived() : Base(), _f() {}
	virtual ~Derived() {}
	virtual void mutate() { _f = _f + 10.0f * 3.14f; }

	virtual void writeTo(std::vector<unsigned char> & target) const
	{
		Base::writeTo(target);
		target << _f;
	}

	virtual void readFrom(std::vector<unsigned char>::const_iterator & source)
	{
		Base::readFrom(source);
		source >> _f;
	}

	virtual bool operator == (const Derived & rhs) const
	{
		return (Base::operator==(rhs) && rhs._f == _f);
	}

	virtual bool operator != (const Derived & rhs) const
	{
		return (Base::operator!=(rhs) || rhs._f != _f);
	}
private:
	float _f;
};

void writeTo(std::vector<unsigned char> & target, const A & source)
{
	source.writeTo(target);
}

void readFrom(std::vector<unsigned char>::const_iterator & source, A & target)
{
	target.readFrom(source);
}

void writeTo(std::vector<unsigned char> & target, const Base & source)
{
	source.writeTo(target);
}

void readFrom(std::vector<unsigned char>::const_iterator & source, Base & target)
{
	target.readFrom(source);
}

int main(int, char **)
{
	// prove serialization helper can handle 
	// POD types
	if (!testType<bool>(true, false)) return -1;
	if (!testType<unsigned char>(0, 255)) return -1;
	if (!testType<char>(0, 127)) return -1;
	if (!testType<unsigned short>(0, 65535)) return -1;
	if (!testType<short>(0, 32767)) return -1;
	if (!testType<unsigned int>(0, 4294967295)) return -1;
	if (!testType<int>(0, 2147483647)) return -1;
	if (!testType<unsigned long long>(0, 18446744073709551615)) return -1;
	if (!testType<long long>(0, 9223372036854775807)) return -1;
	if (!testType<float>(0, 123.456f)) return -1;
	if (!testType<double>(0, 123456789.0987654321)) return -1;

	// now standard containers
	if (!testType<std::string>("The quick brown fox", "jumped over the lazy dog")) return -1;
	
	std::vector<int> v1;
	v1.push_back(0);
	v1.push_back(42);

	std::vector<int> v2;
	v2.push_back(314);
	if (!testType<std::vector<int> >(v1, v2)) return -1;

	std::pair<bool, int> p1(std::make_pair(false, 0));
	std::pair<bool, int> p2(std::make_pair(true, 42));
	if (!testType<std::pair<bool, int> >(p1, p2)) return -1;

	std::map<bool, int> m1;
	m1[false] = 0;
	m1[true] = 42;
	std::map<bool, int> m2;
	m2[true] = 89;
	m2[false] = -1;
	if (!testType<std::map<bool, int> >(m1, m2)) return -1;

	// recursion
	std::vector<std::vector<int> > v3;
	v3.push_back(v1);
	v3.push_back(v2);
	std::vector<std::vector<int> > v4;
	v3.push_back(v2);
	v3.push_back(v1);
	if (!testType<std::vector<std::vector<int> > >(v3, v4)) return -1;

	A a1;
	A a2;
	a2.mutate();
	if (!testType<A>(a1, a2)) return -1;

	Base b1;
	Base b2;
	b2.mutate();
	if (!testType<Base>(b1, b2)) return -1;

	Derived d1;
	Derived d2;
	d2.mutate();
	if (!testType<Derived>(d1, d2)) return -1;

	return 0;
}
