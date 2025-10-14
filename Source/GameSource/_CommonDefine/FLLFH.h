#pragma once

class FLLFH
{
public:
	FLLFH();
	~FLLFH();

	static FLLFH &			GetInstance();

	void					Install();
};