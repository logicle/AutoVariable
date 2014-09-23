#ifndef _INCLUDED_AutoDeltaVariable_VectorSpecialization_H
#define _INCLUDED_AutoDeltaVariable_VectorSpecialization_H

class NoCallback
{

};

template<typename ValueType, typename NotificationTargetType>
class AutoDeltaVariable<std::vector<ValueType>, NotificationTargetType > : public AutoDeltaVariableBase
{
public:
	typedef AutoDeltaVariable<std::vector<ValueType>, NotificationTargetType > DeltaVectorType;

	AutoDeltaVariable(AutoVariableContainer & container, NotificationTargetType & notificationTarget) :
		AutoDeltaVariableBase(container)
		, _notificationTarget()
	{

	}

	DeltaVectorType(const DeltaVectorType &&);

	virtual ~AutoDeltaVariable()
	{

	}



	virtual void pack(std::vector<unsigned char> & target) const
	{
		target << _vector;
	}

	virtual void unpack(std::vector<unsigned char>::const_iterator & source)
	{

	}

	virtual void packDelta(std::vector<unsigned char> & target) const
	{

	}

	virtual void unpackDelta(std::vector<unsigned char>::const_iterator & source)
	{

	}

	void push_back(const ValueType & source)
	{
		_vector.push_back(AutoDeltaVariable<ValueType, NoCallback>(_container, _notificationTarget));
		touch();
	}

private:
	DeltaVectorType(const DeltaVectorType &);
	std::vector<AutoDeltaVariable<ValueType, NoCallback> > _vector;
	NoCallback _notificationTarget;
	
};

template<typename ValueType, typename NotificationTargetType>
void writeTo(std::vector<unsigned char> & target, const std::vector<AutoDeltaVariable<ValueType, NotificationTargetType> > & source)
{
	const unsigned short count (static_cast<const unsigned short>(source.size()));
	
	target << count;
	std::vector<AutoDeltaVariable<ValueType, NotificationTargetType> >::const_iterator i;
	for (i = source.begin(); i != source.end(); ++i)
	{
		const AutoDeltaVariable<ValueType, NotificationTargetType> & element = *i;
		//target << *i;
	}
	/*
	for (const AutoDeltaVariable<ValueType, NotificationTargetType> & element : source)
	{
		//const ValueType & v = element;
		target << element;
	}
	*/
}

#endif//_INCLUDED_AutoDeltaVariable_VectorSpecialization_H
