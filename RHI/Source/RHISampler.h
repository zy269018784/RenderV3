#pragma once

#include <RHIDefinitions.h>


class RHISampler {
public:
	virtual ~RHISampler() {};
	virtual void create() = 0;

	virtual void setFilter(EFilter filter) = 0;
	virtual void setAddressMode(EAddressMode mode) = 0;
	virtual void setCompareOp(ECompareFunction cmp) = 0; 
};

