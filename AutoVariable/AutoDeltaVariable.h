#ifndef _INCLUDED_AutoDeltaVariable_H
#define _INCLUDED_AutoDeltaVariable_H

#include <vector>
#include <map>

#include "AutoVariableContainer.h"
#include "SerializationHelper.h"

class AutoDeltaVariableBase
{
public:
	AutoDeltaVariableBase(AutoVariableContainer & container) : _container(container)
	{
		_container.add(*this);
	}

	virtual ~AutoDeltaVariableBase()
	{
		_container.remove(*this);
	}

	virtual void pack(std::vector<unsigned char> & target) const = 0;
	virtual void unpack(std::vector<unsigned char>::const_iterator & source) = 0;

	virtual void packDelta(std::vector<unsigned char> & target) const = 0;
	virtual void unpackDelta(std::vector<unsigned char>::const_iterator & source) = 0;
protected:
	void touch() { _container.touch(*this); }
protected:
	AutoVariableContainer & _container;
};

template<typename VariableType, typename NotificationTargetType>
class AutoDeltaVariable : public AutoDeltaVariableBase
{
public:
	AutoDeltaVariable(const VariableType & initialValue, AutoVariableContainer & container, NotificationTargetType & notificationTarget, void(NotificationTargetType::*onChange)(const VariableType & oldValue, const VariableType & newValue));
	virtual ~AutoDeltaVariable() {}

	void pack(std::vector<unsigned char> & target) const;
	void unpack(std::vector<unsigned char>::const_iterator & source);

	void packDelta(std::vector<unsigned char> & target) const;
	void unpackDelta(std::vector<unsigned char>::const_iterator & source);
	
	AutoDeltaVariable & operator=(const VariableType & newValue);
	const bool operator==(const AutoDeltaVariable & rhs) const
	{
		return rhs._variable == _variable;
	}

protected:
	void notify(const VariableType & oldValue);

private:
	AutoDeltaVariable(const AutoDeltaVariable &);

private:
	VariableType _variable;
	NotificationTargetType & _notificationTarget;
	void(NotificationTargetType::*_onChange)(const VariableType &, const VariableType &);
};

template<typename VariableType, typename NotificationTargetType>
AutoDeltaVariable<VariableType, NotificationTargetType>::AutoDeltaVariable(const VariableType & initialValue, AutoVariableContainer & container, NotificationTargetType & notificationTarget, void(NotificationTargetType::*onChange)(const VariableType & oldValue, const VariableType & newValue)) :
AutoDeltaVariableBase(container)
, _variable(initialValue)
, _notificationTarget(notificationTarget)
, _onChange(onChange)
{

}

template<typename VariableType, typename NotificationTargetType>
void AutoDeltaVariable<VariableType, NotificationTargetType>::pack(std::vector<unsigned char> & target) const
{
	target << _variable;
}

template<typename VariableType, typename NotificationTargetType>
void AutoDeltaVariable<VariableType, NotificationTargetType>::notify(const VariableType & oldValue)
{
	if (_variable != oldValue)
	{
		if (_onChange)
		{
			((_notificationTarget).*(_onChange))(oldValue, _variable);
		}
	}
}

template<typename VariableType, typename NotificationTargetType>
void AutoDeltaVariable<VariableType, NotificationTargetType>::unpack(std::vector<unsigned char>::const_iterator & source)
{
	const VariableType oldValue = _variable;
	source >> _variable;
	notify(oldValue);
}

template<typename VariableType, typename NotificationTargetType>
void AutoDeltaVariable<VariableType, NotificationTargetType>::packDelta(std::vector<unsigned char> & target) const
{
	pack(target);
}

template<typename VariableType, typename NotificationTargetType>
void AutoDeltaVariable<VariableType, NotificationTargetType>::unpackDelta(std::vector<unsigned char>::const_iterator & source)
{
	unpack(source);
}

template<typename VariableType, typename NotificationTargetType>
AutoDeltaVariable<VariableType, NotificationTargetType> & AutoDeltaVariable<VariableType, NotificationTargetType>::operator=(const VariableType & rhs)
{
	if (_variable != rhs)
	{
		VariableType oldValue = _variable;
		_variable = rhs;
		notify(oldValue);
		touch();
	}
	return *this;
}

#endif//_INCLUDED_AutoDeltaVariable_H
