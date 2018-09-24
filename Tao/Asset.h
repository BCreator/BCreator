#pragma once

class Asset : public Object
{
public:
	virtual Asset();
	virtual ~Asset();

public:
	virtual bool load(string sFile)			=NULL;
	virtual void persistence()				=NULL;
};
