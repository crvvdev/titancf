//---------------------------------------------------------------------------------------------
// LTTransform.h
//
// Provides the definition and implementation for LTTransform. This primitive is a means of
// representing a coordinate frame outside of a matrix. This is far more efficient than a
// matrix if the orientation often needs to be read from it or changed. In addition the
// type LTRigidTransform is defined here, which is the same as LTTransform, but contains
// no scale information.
//
//---------------------------------------------------------------------------------------------

#ifndef __LTTRANSFORM_H__
#define __LTTRANSFORM_H__

#ifndef __LTVECTOR_H__
#	include "ltvector.h"
#endif

#ifndef __LTROTATION_H__
#	include "ltrotation.h"
#endif

#ifndef __LTMATRIX_H__
#	include "ltmatrix.h"
#endif

#ifndef __LTPLANE_H__
#	include "ltplane.h"
#endif

//------------------------------------------------------------------------------------------------------
//This class use a position and quaternion to represent a coordinate frame. This is the same as
// LTTransform, but includes no scale
class LTRigidTransform
{
public:

	//support for constructors of various types

	//the default constructor initializes nothing for performance reasons
	LTRigidTransform()			{}

	//allow providing of all the values
	LTRigidTransform(const LTVector& vPos, const LTRotation& rRot)
	{
		Init(vPos, rRot);
	}

	static LTRigidTransform GetIdentity()		
	{ 
		return LTRigidTransform(LTVector::GetIdentity(), LTRotation::GetIdentity());
	}

	//supports full initialization of the transform
	void Init(const LTVector& vPos, const LTRotation& rRot)
	{
		m_vPos = vPos;
		m_rRot = rRot;
	}

	//initializes the transform to the identity transform
	void Init()
	{
		m_vPos.Init();
		m_rRot.Identity();
	}

	//this will convert to a matrix. This is the same as T * R
	void ToMatrix(LTMatrix& mMatrix) const
	{
		m_rRot.ConvertToMatrix(mMatrix);
		mMatrix.SetTranslation(m_vPos);
	}

	//this will convert to a matrix. This is the same as T * R
	void ToMatrix(LTMatrix3x4& mMatrix) const
	{
		m_rRot.ConvertToMatrix(mMatrix);
		mMatrix.SetTranslation(m_vPos);
	}

	//Given a matrix, this will setup the information for the transform from the matrix. Note
	//that this matrix must have orthogonal basis vectors for this to operate properly, meaning there
	//can be no shears. This version will extract the scale from the matrix. Note that this should not
	//be used if the matrix is not scaled though since it is slower than the non-scaled version
	void FromMatrixWithScale(const LTMatrix& mMatrix)
	{
		//we need a non scaled version in order to extract the orientation
		LTMatrix mNonScale(mMatrix);
		mNonScale.Normalize();
		FromMatrixNoScale(mNonScale);
	}

	void FromMatrixWithScale(const LTMatrix3x4& mMatrix)
	{
		//we need a non scaled version in order to extract the orientation
		LTMatrix3x4 mNonScale(mMatrix);
		mNonScale.Normalize();
		FromMatrixNoScale(mNonScale);
	}

	//Given a matrix, this will setup the information for the transform from the matrix. Note
	//that this matrix must have orthogonal basis vectors for this to operate properly, meaning there
	//can be no shears. This version will assume the matrix is non-scaled making it much faster
	void FromMatrixNoScale(const LTMatrix& mMatrix)
	{
		m_rRot.ConvertFromMatrix(mMatrix);
		mMatrix.GetTranslation(m_vPos);
	}

	void FromMatrixNoScale(const LTMatrix3x4& mMatrix)
	{
		m_rRot.ConvertFromMatrix(mMatrix);
		mMatrix.GetTranslation(m_vPos);
	}

	//given a transform, this will invert the transform. Since a transform is equal to the matrices
	//T * R, it can be decomposed to the matrices ~R * ~T
	void Inverse()
	{
		m_rRot = ~m_rRot;
		m_vPos = -m_rRot.RotateVector(m_vPos);
	}

	//returned version of inverse functionality
	LTRigidTransform GetInverse() const
	{
		LTRigidTransform rv(*this);
		rv.Inverse();
		return rv;
	}

	//this will find the difference between the two passed in transforms and store the result in this transform.
	//So the resulting state of this transform is a transform that tA * this = tB
	void Difference(const LTRigidTransform& tA, const LTRigidTransform& tB)
	{
		//we have this, and transform and know that this * diff = other, so diff is equal to ~this * other
		//so this is simply the expanded version of the inverse and mulitply functions for efficiency
		LTRotation rInvARot(~tA.m_rRot);

		m_rRot = rInvARot * tB.m_rRot ;
		m_vPos = rInvARot.RotateVector(tB.m_vPos - tA.m_vPos);
	}

	//the return version of the above funciton. This will return the difference it takes to get to the
	//specified rotation. So this * rv = tOther
	LTRigidTransform GetDifference(const LTRigidTransform& tOther) const
	{
		LTRigidTransform tDiff;
		tDiff.Difference(*this, tOther);
		return tDiff;
	}


	//multiplies two transforms together. This has the same effect of translating each to a matrix and
	//multiplying the two together. This has the effect of this * matrix = result. There are operator
	//versions of this as well, but if time is critical, this can be used to avoid copy constructors
	//and other inefficiencies
	void Multiply(const LTRigidTransform& tLhs, const LTRigidTransform& tRhs)
	{
		m_rRot = tLhs.m_rRot * tRhs.m_rRot ;
		m_vPos = tLhs.m_rRot.RotateVector(tRhs.m_vPos);
		m_vPos += tLhs.m_vPos ;
	}

	//operator forms of the multiplication
	LTRigidTransform operator*(const LTRigidTransform& tRhs) const
	{
		LTRigidTransform tRV;
		tRV.Multiply(*this, tRhs);
		return tRV;
	}

	LTRigidTransform& operator*=(const LTRigidTransform& tRhs)
	{
		*this = *this * tRhs;
		return *this;
	}

	//transformation of a vector. This is the version that takes everything as a parameter, again
	//to allow for avoiding constructor costs in performance critical code.
	void Transform(const LTVector& vVec, LTVector& vResult) const
	{
		vResult = m_rRot.RotateVector(vVec) + m_vPos;
	}

	//operator versions of vector operations
	LTVector operator*(const LTVector& vRhs) const
	{
		LTVector vRV;
		Transform(vRhs, vRV);
		return vRV;
	}

	//transformation of a plane. This is the version that takes everything as a parameter, again
	//to allow for avoiding constructor costs in performance critical code.
	void Transform(const LTPlane& Src, LTPlane& Result) const
	{
		//this takes advantage of the fact that N' = R * N, and D' = N' . (R * (N * D) + O)
		Result.m_Normal = m_rRot.RotateVector(Src.Normal());
		Result.m_Dist = Src.Dist() + Result.m_Normal.Dot(m_vPos);
	}

	//operator versions of plane operations
	LTPlane operator*(const LTPlane& Rhs) const
	{
		LTPlane RV;
		Transform(Rhs, RV);
		return RV;
	}

	//called to interpolate the transform. This takes in two transforms to interpolate between, and
	//a percentage to interpolate that should be within the range of [0..1]
	void Interpolate(const LTRigidTransform& t1, const LTRigidTransform& t2, float fT)
	{
		m_rRot.Slerp(t1.m_rRot, t2.m_rRot, fT);
		m_vPos = t1.m_vPos + (t2.m_vPos - t1.m_vPos) * fT;
	}

	//represents the translation component of the transform
	LTVector	m_vPos;

	//represents the orientation of the transform around the translation offset
	LTRotation	m_rRot;
};

//------------------------------------------------------------------------------------------------------
//This class use a position and quaternion, and uniform scale to represent a coordinate frame.
//
//Please note that there are absolutely no implicit conversions to a matrix, all of them must
//be explicit as this is a fairly costly operation.

class LTTransform
{
public:
	LTVector		m_vPos;		//0000
	LTRotation		m_rRot;		//000C
	float			m_fScale;	//001C
	unsigned char	unknown_padding[0x100];
};

//------------------------------------------------------------------------------------------------------
// Free standing operators

//used to detect NaN's within the transforms
inline bool LTIsNaN(const LTRigidTransform& tTrans)		
{ 
	return LTIsNaN(tTrans.m_vPos) || LTIsNaN(tTrans.m_rRot);
}

inline bool LTIsNaN(const LTTransform& tTrans)		
{ 
	return LTIsNaN(tTrans.m_vPos) || LTIsNaN(tTrans.m_rRot) || LTIsNaN(tTrans.m_fScale);
}

//support multiplying a rigid transform by a normal transform
inline LTTransform operator*(const LTRigidTransform& tLhs, const LTTransform& tRhs)
{
	LTTransform vRV;
	vRV.m_rRot = tLhs.m_rRot * tRhs.m_rRot;
	vRV.m_vPos = tLhs.m_rRot.RotateVector(tRhs.m_vPos);
	vRV.m_vPos += tLhs.m_vPos;
	vRV.m_fScale = tRhs.m_fScale;

	return vRV;
}

//support multiplying a rigid transform by a normal transform
inline LTTransform operator*(const LTTransform& tLhs, const LTRigidTransform& tRhs)
{
	LTTransform vRV;
	vRV.m_rRot = tLhs.m_rRot * tRhs.m_rRot ;
	vRV.m_vPos = tLhs.m_rRot.RotateVector(tRhs.m_vPos) * tLhs.m_fScale;
	vRV.m_vPos += tLhs.m_vPos;
	vRV.m_fScale = tLhs.m_fScale;

	return vRV;
}

//Serialization functions
inline void LTStream_Read(ILTInStream *pStream, LTTransform &tTrans)
{
	(*pStream) >> tTrans.m_vPos;
	(*pStream) >> tTrans.m_rRot;
	(*pStream) >> tTrans.m_fScale;
}

inline void LTStream_Write(ILTOutStream *pStream, const LTTransform &tTrans)
{
    (*pStream) << tTrans.m_vPos;
	(*pStream) << tTrans.m_rRot;
	(*pStream) << tTrans.m_fScale;
}

inline void LTStream_Read(ILTInStream *pStream, LTRigidTransform &tTrans)
{
	(*pStream) >> tTrans.m_vPos;
	(*pStream) >> tTrans.m_rRot;
}

inline void LTStream_Write(ILTOutStream *pStream, const LTRigidTransform &tTrans)
{
    (*pStream) << tTrans.m_vPos;
	(*pStream) << tTrans.m_rRot;
}

#endif // __LTTRANSFORM_H__
