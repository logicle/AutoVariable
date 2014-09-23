#include <map>
#include <vector>
#include "AutoDeltaVariable.h"
#include "AutoVariableContainer.h"
#include "SerializationHelperContainers.h"
#include "AutoDeltaVariable_MapSpecialization.h"
#include "AutoDeltaVariable_VectorSpecialization.h"

class VariableTest
{
public:
	VariableTest() :
		_container()
		, _intTest(0, _container, *this, &VariableTest::onIntTestChanged)
		, _stringTest("", _container, *this, &VariableTest::onStringTestChanged)
		, _mapTest(_container, *this)
		, _vectorTest(_container, *this)
		, _vectorRecursionTest(_container, *this)
	{
		_mapTest[2] = 2;
		std::vector<int> v;
		v.push_back(1);
		_vectorRecursionTest.push_back(v);
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

	void onVectorRecursionTestChanged(/* const VectorChangeNotification<> & */)
	{

	}

	void testMap() const
	{
		/*
		// conversion operator for a const map
		const std::map<int, int> & b = _mapTest;
		std::map<int, int>::const_iterator s = b.begin();

		// const accessor
		const int & i = _mapTest[2];
		std::map<int, int>::const_iterator f = _mapTest.find(2);
		bool end = (f == _mapTest.end());
		// conversion operator for a copy of the map
		*/
	}
	void changeMap()
	{
/*		_mapTest[42] = 1;
		_mapTest[10] = 20;
		_mapTest[11] = 21;
		_mapTest[12] = 22;
		_mapTest.erase(11);
*/
	}
	const bool operator==(const VariableTest & rhs) const
	{
		return (rhs._intTest == _intTest &&
			rhs._stringTest == _stringTest && 
			rhs._mapTest == _mapTest);
	}
private:
	AutoVariableContainer _container;
	AutoDeltaVariable<int, VariableTest> _intTest;
	AutoDeltaVariable<std::string, VariableTest> _stringTest;
	AutoDeltaVariable<std::map<int, int>, VariableTest> _mapTest;
	AutoDeltaVariable<std::vector<int>, VariableTest> _vectorTest;
	AutoDeltaVariable<std::vector<std::vector<int> >, VariableTest > _vectorRecursionTest;
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

	v2.testMap();
	v2.changeMap();
	ar1.clear();
	v2.getContainer().packDeltas(ar1);
	v1.getContainer().unpackDeltas(ar1.begin());
	if (!(v1 == v2)) return -1;
	return 0;
}
