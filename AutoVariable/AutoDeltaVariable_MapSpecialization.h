#ifndef _INCLUDED_AutoDeltaVariable_MapSpecialization_H
#define _INCLUDED_AutoDeltaVariable_MapSpecialization_H

#include "AutoDeltaVariable.h"

template<typename KeyType, typename ValueType, typename NotificationTargetType>
class AutoDeltaVariable<std::map<KeyType, ValueType>, NotificationTargetType > : public AutoDeltaVariableBase
{
public:
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
protected:
	friend class WrappedElement;
	class MapOperation
	{
	public:
		MapOperation(MapOperations operation, const KeyType & key, const ValueType & value) :
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
		const ValueType & _value;
	};

public:
	class WrappedElement
	{
	public:
		WrappedElement(AutoDeltaVariable & owner, ValueType & target, MapOperation operation) :
			_owner(owner)
			, _target(target)
			, _operation(operation)
		{

		}

		~WrappedElement()
		{
			if (_oldValue != _target)
				_owner.addCommand(_operation);
		}

		operator ValueType &()
		{
			return _target;
		}

		WrappedElement & operator=(const ValueType & rhs)
		{
			if (rhs != _oldValue)
			{
				_target = rhs;
//				_owner.addCommand(_operation);
			}
			return *this;
		}

	private:
		AutoDeltaVariable & _owner;
		ValueType _oldValue;
		ValueType & _target;
		MapOperation _operation;
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

		return WrappedElement(*this, _map[key], MapOperation(operation, key, _map[key]));
	}

protected:
	friend class WrappedElement;
	void addCommand(const MapOperation & newOperation)
	{
		_deltaOperations.push_back(newOperation);
		touch();
	}
private:
	NotificationTargetType & _notificationTarget;
	std::map<KeyType, ValueType> _map;
	std::vector<MapOperation> _deltaOperations;
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
	for (MapOperation command : _deltaOperations)
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

		MapOperation command(operation, key, value);
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

#endif//_INCLUDED_AutoDeltaVariable_MapSpecialization_H
