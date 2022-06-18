#include "MathTests.h"

#include "Tests/Constexpr.h"
#include "Tests/DefaultedCtor.h"
#include "Tests/FuncCommon.h"
#include "Tests/FuncExponential.h"
#include "Tests/FuncGeometric.h"
#include "Tests/FuncMatrix.h"
#include "Tests/FuncVectorRelational.h"
#include "Tests/TypeLength.h"
#include "Tests/TypeMat3.h"
#include "Tests/TypeMat4.h"
#include "Tests/TypeVec2.h"
#include "Tests/TypeVec3.h"
#include "Tests/TypeVec4.h"
#include "Tests/TypeQuaternion.h"
#include "Tests/ScalarCommon.h"
#include "Tests/ScalarConstants.h"
#include "Tests/ScalarInteger.h"
#include "Tests/ScalarRelational.h"
#include "Tests/VectorCommon.h"
#include "Tests/VectorSized.h"
#include "Tests/ColorSpace.h"

MathTests::MathTests()
	: Layer("Math")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void MathTests::OnAttach()
{
	int32_t error = 0;

	RunConstexprTests();
	error += RunDefaultedCtorTests();
	error += RunFuncCommonTests();
	error += RunFuncExponentialTests();
	error += RunFuncGeometricTests();
	error += RunFuncMatrixTests();
	error += RunFuncVectorRelationalTests();
	error += RunTypeLengthTests();
	error += RunTypeMat3Tests();
	error += RunTypeMat4Tests();
	error += RunTypeVec2Tests();
	error += RunTypeVec3Tests();
	error += RunTypeVec4Tests();
	error += RunTypeQuaternionTests();
	error += RunScalarCommonTests();
	error += RunScalarConstantsTests();
	error += RunScalarIntegerTests();
	error += RunScalarRelationalTests();
	error += RunVectorCommonTests();
	error += RunVectorSizedTests();
	error += RunColorSpaceTests();

	TRAP_ASSERT(error == 0, "MathTests: Failed");

	TRAP::Application::Shutdown();
}
