#ifndef _INCLUDED_AutoVariableContainer_H
#define _INCLUDED_AutoVariableContainer_H

#include <vector>

class AutoDeltaVariableBase;

class AutoVariableContainer
{
public:
	AutoVariableContainer();
	~AutoVariableContainer();

	void pack(std::vector<unsigned char> & target) const;
	void unpack(std::vector<unsigned char>::const_iterator & source);

	void packDeltas(std::vector<unsigned char> & target) const;
	void unpackDeltas(std::vector<unsigned char>::const_iterator & source);

	void add(AutoDeltaVariableBase & variable);
	void remove(AutoDeltaVariableBase & variable);
	
protected:
	friend class AutoDeltaVariableBase;
	void touch(const AutoDeltaVariableBase & dirtyVariable) const;

private:
	std::vector<AutoDeltaVariableBase *> _variables;
	mutable std::vector<const AutoDeltaVariableBase *> _deltas;
};
#endif//_INCLUDED_AutoVariableContainer_H
