#ifndef Wall_h
#define Wall_h

// #include "Rescue.h"

class Wall
{
protected:
	double probability, unprobability;

	void normalize()
	{
		probability = probability / (probability + unprobability);
		unprobability = 1.0 - probability;
	}

public:
	Wall()
	{
		probability = 0.5;
		unprobability = 0.5;
	}

	double getProb()
	{
		return probability;
	}

	void setProb(double prob)
	{
		if(prob > 1.0)
			prob = 1.0;
		else if(prob < 0.0)
			prob = 0.0;

		probability = (double)prob;
		unprobability = 1.0 - prob;
	}

	void exist(bool exist){
		setProb(exist * 1.0);
	}

	bool exist(){
		return getProb() > 0.5;
	}

	void updateWall(double chance)
	{
		if (chance < 0 || chance > 1)
			return;

		probability *= chance;
		unprobability *= 1 - chance;
		normalize();
	}

};

#endif