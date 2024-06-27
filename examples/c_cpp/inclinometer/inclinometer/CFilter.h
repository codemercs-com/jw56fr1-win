#pragma once
#include <vector>

class CFilter
{
private:
	std::vector<int32_t> m_Vector;
	double m_Alpha;
public:
	CFilter();
	~CFilter();
	void SetAlpha(double alpha);
	void Add(int32_t value);
	void Clear();
	int32_t Execute();
	size_t GetLength();
};

