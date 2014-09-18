#include <map>
#include "AutoDeltaVariable.h"
#include "AutoVariableContainer.h"
#include "SerializationHelperContainers.h"

class VariableTest
{
public:
	VariableTest() :
		_container()
		, _intTest(0, _container, *this, &VariableTest::onIntTestChanged)
		, _stringTest("", _container, *this, &VariableTest::onStringTestChanged)
		, _mapTest(_container, *this)
	{

	}

	~VariableTest() {}

	void setIntTest(int newValue)
	{
		_intTest = newValue;
	}

	void setStringTest(const std::string & newValue)
	{
		_stringTest = newValue;
	}

	void onMapTestChanged(const std::map<int, int> & oldValue, const std::map<int, int> & newValue)
	{

	}

	void onIntTestChanged(const int & oldValue, const int & newValue)
	{

	}

	void onStringTestChanged(const std::string & oldValue, const std::string & newValue)
	{

	}

	const AutoVariableContainer & getContainer() const
	{
		return _container;
	}

	AutoVariableContainer & getContainer()
	{
		return _container;
	}

	void changeMap()
	{
//		_mapTest[42] = 1;
	}
	const bool operator==(const VariableTest & rhs) const
	{
		return (rhs._intTest == _intTest &&
			rhs._stringTest == _stringTest);
	}
private:
	AutoVariableContainer _container;
	AutoDeltaVariable<int, VariableTest> _intTest;
	AutoDeltaVariable<std::string, VariableTest> _stringTest;
	AutoDeltaVariable<std::map<int, int>, VariableTest> _mapTest;
};

int main(int, char **)
{
	VariableTest v1;
	v1.setIntTest(42);

	VariableTest v2;
	std::vector<unsigned char> ar1;
	v1.getContainer().packDeltas(ar1);

	v2.getContainer().unpackDeltas(ar1.begin());

	if (!(v1 == v2)) return -1;

	return 0;
}
