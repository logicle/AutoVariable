#include "AutoVariableContainer.h"
#include "AutoDeltaVariable.h"
#include "SerializationHelper.h"

AutoVariableContainer::AutoVariableContainer() :
_variables()
, _deltas()
{

}

AutoVariableContainer::~AutoVariableContainer()
{
	_variables.clear();
	_deltas.clear();
}

void AutoVariableContainer::add(AutoDeltaVariableBase & variable)
{
	auto f = std::find(_variables.begin(), _variables.end(), &variable);
	if (f == _variables.end())
	{
		_variables.push_back(&variable);
	}
}

void AutoVariableContainer::remove(AutoDeltaVariableBase & variable)
{
	auto f = std::find(_variables.begin(), _variables.end(), &variable);
	if (f != _variables.end())
	{
		_variables.erase(f);
	}
}

void AutoVariableContainer::pack(std::vector<unsigned char> & target) const
{
	for (auto var : _variables)
	{
		var->pack(target);
	}
}

void AutoVariableContainer::unpack(std::vector<unsigned char>::const_iterator & source)
{
	for (auto var : _variables)
	{
		var->unpack(source);
	}
}

void AutoVariableContainer::packDeltas(std::vector<unsigned char> & target) const
{
	const unsigned short count = _deltas.size();
	target << count;
	for (auto var : _deltas)
	{
		auto f = std::find(_variables.begin(), _variables.end(), var);
		if (f != _variables.end())
		{
			unsigned short index = std::distance(_variables.begin(), f);
			target << index;
			var->packDelta(target);
		}
	}
	_deltas.clear();
}

void AutoVariableContainer::unpackDeltas(std::vector<unsigned char>::const_iterator & source)
{
	const unsigned short count(initializeFrom<unsigned short>(source));
	unsigned short i = 0;
	for (i = 0; i < count; ++i)
	{
		unsigned short index(initializeFrom<unsigned short>(source));
		AutoDeltaVariableBase * variable = _variables[i];
		variable->unpackDelta(source);
	}
}

void AutoVariableContainer::touch(const AutoDeltaVariableBase & dirtyVariable) const
{
	auto f = std::find(_variables.begin(), _variables.end(), &dirtyVariable);
	if (f != _variables.end())
	{
		auto d = std::find(_deltas.begin(), _deltas.end(), &dirtyVariable);
		if (d == _deltas.end())
		{
			_deltas.push_back(&dirtyVariable);
		}
	}
}