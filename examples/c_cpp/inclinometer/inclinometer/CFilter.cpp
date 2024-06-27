#include "pch.h"
#include "CFilter.h"

CFilter::CFilter()
{
	m_Vector.clear();
	m_Alpha = 1.0;
}

CFilter::~CFilter()
{
}

size_t CFilter::GetLength()
{
	return m_Vector.size(); //Return actual vector size
}

void CFilter::SetAlpha(double alpha)
{
	m_Alpha = alpha;
}

void CFilter::Add(int32_t value)
{
	m_Vector.push_back(value); //Add value at the end and increase size +1
}

void CFilter::Clear()
{
	m_Vector.clear(); //Remove all items from vector and set size to 0
}

int32_t CFilter::Execute()
{
	uint32_t size = m_Vector.size();
	int32_t result = 0;

	if (size > 0)
	{
		result = m_Vector.at(0);  // Init with first item of vector

		for (size_t i = 1; i < size; ++i)
		{
			// EMA-Calculatopm: alpha * actual data point + (1 - alpha) * last EMA
			result = m_Alpha * m_Vector.at(i) + (1 - m_Alpha) * result;
		}
	}

	return result;
}