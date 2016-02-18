#include "MatlabVector.h"
#include <string>


// �����, ���� ���������� �����, � �� ������, �� ������ �� �����?
// �� ���� MV ������ MV&

MatlabVector & MatlabVector::per_member_op(MatlabVector &other, num(*op)(num, num))
{
	if (this->size() != other.size()) throw new std::string("��������� �������� ��� ��������� ������� ������� �����������");
	MatlabVector &result = *(new MatlabVector(this->size())); // ����� ������� ������ ������...
	for (uint i = 0; i < this->size(); i++) result[i] = op((*this)[i], other[i]);
	return result;
}

MatlabVector & MatlabVector::per_member_op(num a, num(*op)(num, num))
{
	MatlabVector &result = *(new MatlabVector(this->size())); // ����� ������� ������ ������...
	for (uint i = 0; i < this->size(); i++) result[i] = op((*this)[i], a);
	return result;
}

MatlabVector& operator*(num n, MatlabVector& mv) { return mv*n; }