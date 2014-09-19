#ifndef _INCLUDED_AutoDeltaVariable_MapSpecialization_H
#define _INCLUDED_AutoDeltaVariable_MapSpecialization_H

#include "AutoDeltaVariable.h"

#if 0 // attempting to write a recursive auto delta container, failing on map insert/move semantics
template<typename KeyType, typename ValueType, typename NotificationTargetType>
class AutoDeltaVariable<std::map<KeyType, ValueType>, NotificationTargetType > : public AutoDeltaVariableBase
{
public:
	typedef AutoDeltaVariable<std::map<KeyType, ValueType>, NotificationTargetType > MapType;
	typedef AutoDeltaVariable<ValueType, MapType> ElementType;

	AutoDeltaVariable(AutoVariableContainer & container, NotificationTargetType & notificationTarget);

	virtual void pack(std::vector<unsigned char> & target) const;
	virtual void unpack(std::vector<unsigned char>::const_iterator & source);
	virtual void packDelta(std::vector<unsigned char> & target) const;
	virtual void unpackDelta(std::vector<unsigned char>::const_iterator & source);

	void onInsertElement(const AutoDeltaVariable<ValueType, AutoDeltaVariable> & newType)
	{
	}

	void onElementChanged(const ValueType & oldValue, const ValueType & ValueType)
	{
	}

	ElementType & operator[](const KeyType & key)
	{
		&MapType::onElementChanged;
		ElementType e(ValueType(), _container, *this, &MapType::onElementChanged);
		std::map<KeyType, AutoDeltaVariable<ValueType, AutoDeltaVariable> >::const_iterator f = _map.find(key);
		if (f == _map.end())
			_map[key] = ElementType(ValueType(), _container, *this, &MapType::onElementChanged);
		return _map[key];
	}

	const bool operator==(const AutoDeltaVariable & rhs) const
	{
		return _map == rhs._map;
	}
private:
	AutoVariableContainer _container;
	std::map < KeyType, ElementType> _map;
};

#else
template<typename KeyType, typename ValueType, typename NotificationTargetType>
class AutoDeltaVariable<std::map<KeyType, ValueType>, NotificationTargetType > : public AutoDeltaVariableBase
{
public:
	// the order of operations is important for
	// maps. To push only sparse deltas, each
	// operation is recorded at the source
	// then played back when unpacking deltas on
	// the target object
	enum MapOperations
	{
		NONE
		, CREATE
		, UPDATE
		, DELETE
	};

	AutoDeltaVariable(AutoVariableContainer & container, NotificationTargetType & notificationTarget);
	virtual ~AutoDeltaVariable();

	virtual void pack(std::vector<unsigned char> & target) const;
	virtual void unpack(std::vector<unsigned char>::const_iterator & source);
	virtual void packDelta(std::vector<unsigned char> & target) const;
	virtual void unpackDelta(std::vector<unsigned char>::const_iterator & source);

	operator const std::map<KeyType, ValueType> & () const
	{
		return _map;
	}

	const bool operator == (const AutoDeltaVariable & rhs) const
	{
		return _map == rhs._map;
	}

	typename std::map<KeyType, ValueType>::const_iterator end() const
	{
		return _map.end();
	}

	typename std::map<KeyType, ValueType>::const_iterator begin() const
	{
		return _map.begin();
	}

protected:
	friend class WrappedElement;
	class MapCommand
	{
	public:
		MapCommand(MapOperations operation, const KeyType & key, const ValueType & value) :
			_operation(operation)
			, _key(key)
			, _value(value)
		{

		}

		void writeTo(std::vector<unsigned char> & target) const
		{
			unsigned char op = static_cast<unsigned char>(_operation);
			target << op;
			target << _key;
			target << _value;
		}

		const MapOperations _operation;
		const KeyType _key;
		ValueType _value;
	};

public:
	class WrappedElement
	{
	public:
		WrappedElement(AutoDeltaVariable & owner, ValueType & target, MapCommand operation) :
			_owner(owner)
			, _target(target)
			, _operation(operation) {}

		~WrappedElement(){}

		operator ValueType &()
		{
			return _target;
		}

		WrappedElement & operator=(const ValueType & rhs)
		{
			if (rhs != _oldValue)
			{
				_target = rhs;
				_operation._value = _target;
				_owner.addCommand(_operation);
			}
			return *this;
		}

	private:
		AutoDeltaVariable & _owner;
		ValueType _oldValue;
		ValueType & _target;
		MapCommand _operation;
	};

	WrappedElement operator[](const KeyType & key)
	{
		// this could either be a create or a change operation when 
		// WrappedElement leaves scope, ensure the write operation
		// is encoded for packing deltas.
		MapOperations operation = NONE;
		if (_map.find(key) == _map.end())
		{
			operation = CREATE;
		}
		else
		{
			// set operation
			operation = UPDATE;
		}

		return WrappedElement(*this, _map[key], MapCommand(operation, key, _map[key]));
	}

	const ValueType & operator[](const KeyType & key) const
	{
		return _map.at(key);
	}

	size_t erase(const KeyType & key)
	{
		size_t result = 0;
		auto f = _map.find(key);
		if (f != _map.end())
		{
			ValueType value = f->second;
			MapCommand operation(DELETE, key, value);
			addCommand(operation);
			result = _map.erase(key);
		}
		return result;
	}

	typename std::map<KeyType, ValueType>::const_iterator find(const KeyType & key) const
	{
		return _map.find(key);
	}

protected:
	friend class WrappedElement;
	void addCommand(const MapCommand & newOperation)
	{
		_deltaOperations.push_back(newOperation);
		touch();
	}
private:
	NotificationTargetType & _notificationTarget;
	std::map<KeyType, ValueType> _map;
	std::vector<MapCommand> _deltaOperations;
};

template<typename KeyType, typename ValueType, typename NotificationTargetType>
AutoDeltaVariable<std::map<KeyType, ValueType>, NotificationTargetType>::AutoDeltaVariable(AutoVariableContainer & container, NotificationTargetType & notificationTarget) :
AutoDeltaVariableBase(container)
, _notificationTarget(notificationTarget)
{

}

template<typename KeyType, typename ValueType, typename NotificationTargetType>
AutoDeltaVariable<std::map<KeyType, ValueType>, NotificationTargetType>::~AutoDeltaVariable()
{

}

template<typename KeyType, typename ValueType, typename NotificationTargetType>
void AutoDeltaVariable<std::map<KeyType, ValueType>, NotificationTargetType>::pack(std::vector<unsigned char> & target) const
{
	target << _map;
}

template<typename KeyType, typename ValueType, typename NotificationTargetType>
void AutoDeltaVariable<std::map<KeyType, ValueType>, NotificationTargetType>::unpack(std::vector<unsigned char>::const_iterator & source)
{
	source >> _map;
}

template<typename KeyType, typename ValueType, typename NotificationTargetType>
void AutoDeltaVariable<std::map<KeyType, ValueType>, NotificationTargetType>::packDelta(std::vector<unsigned char> & target) const
{
	unsigned short count = _deltaOperations.size();
	target << count;
	for (MapCommand command : _deltaOperations)
	{
		command.writeTo(target);
	}
}

template<typename KeyType, typename ValueType, typename NotificationTargetType>
void AutoDeltaVariable<std::map<KeyType, ValueType>, NotificationTargetType>::unpackDelta(std::vector<unsigned char>::const_iterator & source)
{
	const unsigned short count(initializeFrom<unsigned short>(source));
	unsigned short index = 0;
	for (index = 0; index < count; ++index)
	{
		const MapOperations operation(static_cast<MapOperations>(initializeFrom<unsigned char>(source)));
		const KeyType key(initializeFrom<KeyType>(source));
		const ValueType value(initializeFrom<ValueType>(source));

		MapCommand command(operation, key, value);
		switch (command._operation)
		{
		case CREATE:
		case UPDATE:
			_map[command._key] = command._value;
			break;
		case DELETE:
		{
			auto f = _map.find(command._key);
			if (f != _map.end())
				_map.erase(f);
		}
			break;
		default:
			break;
		}
	}
}
#endif//0

#endif//_INCLUDED_AutoDeltaVariable_MapSpecialization_H
