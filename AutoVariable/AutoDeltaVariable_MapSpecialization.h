#ifndef _INCLUDED_AutoDeltaVariable_MapSpecialization_H
#define _INCLUDED_AutoDeltaVariable_MapSpecialization_H

#include "AutoDeltaVariable.h"

#if 1 // attempting to write a recursive auto delta container, failing on map insert/move semantics

enum MapOperations
{
	NONE
	, UPDATE
	, ERASE
	, CLEAR
};

template<typename KeyType, typename ValueType, typename NotificationTargetType>
class MapChangeNotification
{
public:
	typedef void(NotificationTargetType::*MapCommand)(const MapChangeNotification<KeyType, ValueType, NotificationTargetType> &);

	MapChangeNotification() : _container(0), _key(), _value(), _notificationTarget(0), _onChange(0), _operation(NONE) {}
	MapChangeNotification(const ValueType & source) : _container(0), _key(), _value(source), _notificationTarget(0), _onChange(0), _operation(NONE) {}
	MapChangeNotification(AutoVariableContainer & container, const KeyType & key, const ValueType & source, NotificationTargetType & notificationTarget, MapCommand onChanged, MapOperations operation) : 
		_container(&container)
		, _key(key)
		, _value(source)
		, _notificationTarget(&notificationTarget)
		, _onChange(onChange)
		, _operation(operation)
	{
	}
	
	MapChangeNotification(const MapChangeNotification & source) :
		_container(source._container)
		, _key(source._key)
		, _value(source._value)
		, _notificationTarget(source._notificationTarget)
		, _onChange(source._onChange)
		, _operation(source._operation)
	{

	}

	~MapChangeNotification() {}

	operator const ValueType &() const
	{
		return _value;
	}

	void setNotificationCallback(NotificationTargetType & notificationTarget, MapCommand onChange)
	{
		_onChange = onChange;
		_notificationTarget = &notificationTarget;
	}

	void setContainer(AutoVariableContainer & container)
	{
		_container = &container;
	}

	void setKey(const KeyType & key)
	{
		_key = key;
	}

	void setOperation(MapOperations operation)
	{
		_operation = operation;
	}

	MapChangeNotification & operator=(const ValueType & rhs)
	{
		if ((rhs != _value) && _onChange && _notificationTarget)
		{
			_value = rhs;
			((_notificationTarget)->*(_onChange))(*this);
		}
		return *this;
	}

	void readFrom(std::vector<unsigned char>::const_iterator & source)
	{
		ValueType oldValue = _value;
		source >> _value;
		if ((oldValue != _value) && _onChange)
		{
//			((_notificationTarget).*(_onChange))(oldValue, _value);
		}
	}

	void writeTo(std::vector<unsigned char> & target) const
	{
		target << _value;
	}

private:
	// this has to be a pointer to accomodate 
	// default constructors invoked from std::map
	AutoVariableContainer * _container;
	KeyType _key;
	ValueType _value;
	NotificationTargetType * _notificationTarget;
	MapCommand _onChange;
	MapOperations _operation;
};

template<typename KeyType, typename ValueType, typename NotificationTargetType>
void writeTo(std::vector<unsigned char> & target, const MapChangeNotification<KeyType, ValueType, NotificationTargetType> & source)
{
	source.writeTo(target);
}

template<typename KeyType, typename ValueType, typename NotificationTargetType>
void readFrom(std::vector<unsigned char>::const_iterator & source, MapChangeNotification<KeyType, ValueType, NotificationTargetType> & target)
{
	target.readFrom(source);
}

template<typename KeyType, typename ValueType, typename NotificationTargetType>
class AutoDeltaVariable<std::map<KeyType, ValueType>, NotificationTargetType > : public AutoDeltaVariableBase
{
public:
	typedef AutoDeltaVariable<std::map<KeyType, ValueType>, NotificationTargetType > MapType;

	AutoDeltaVariable(AutoVariableContainer & container, NotificationTargetType & notificationTarget) : 
		AutoDeltaVariableBase(container)
		, _container()
		, _map()
		, _changes()
	{
	}

	virtual void pack(std::vector<unsigned char> & target) const
	{
		target << _map;
	}

	virtual void unpack(std::vector<unsigned char>::const_iterator & source)
	{
		source >> _map;
	}

	virtual void packDelta(std::vector<unsigned char> & target) const
	{
	}

	virtual void unpackDelta(std::vector<unsigned char>::const_iterator & source)
	{
	}


	void onChanged(const MapChangeNotification<KeyType, ValueType, MapType> & source)
	{
		_changes.push_back(source);
	}

	MapChangeNotification<KeyType, ValueType, MapType> & operator[](const KeyType & key)
	{
		auto & result = _map[key];
		result.setNotificationCallback(*this, &MapType::onChanged);
		result.setKey(key);
		result.setOperation(UPDATE);
		return result;
	}

	const bool operator==(const AutoDeltaVariable & rhs) const
	{
		return _map == rhs._map;
	}
private:
	AutoVariableContainer _container;
	std::map < KeyType, MapChangeNotification<KeyType, ValueType, MapType> > _map;
	std::vector<MapChangeNotification<KeyType, ValueType, MapType> > _changes;
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
