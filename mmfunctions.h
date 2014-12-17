#ifndef _MMFUNCTIONS_H_
#define _MMFUNCTIONS_H_

#include <cmath>

namespace mm
{

namespace fun
{

#define MM_OP_VAL m_Op( x, y )

#define MM_DECL_SCALAR_FUN(clsName,funName,functionality) \
template<typename Top> \
class clsName \
{ \
public: \
	typedef typename Top::DTYPE DTYPE; \
\
public: \
	clsName( const Top& op ) \
		: m_Op( op ) \
	{ \
	} \
\
	DTYPE operator()( int x, int y ) const \
	{ \
		functionality\
	} \
\
private: \
	const Top m_Op; \
}; \
\
template<typename Top> \
inline clsName<Top> funName( const Top& op ) \
{ \
	return clsName<Top>( op ); \
}

#define MM_WRAP_STD_FUN(clsName,funName,stdName) MM_DECL_SCALAR_FUN(clsName,funName,return std::stdName( MM_OP_VAL );)

MM_WRAP_STD_FUN( Sin, sin, sin )
MM_WRAP_STD_FUN( Cos, cos, cos )
MM_WRAP_STD_FUN( Tan, tan, tan )
MM_WRAP_STD_FUN( Sqrt, sqrt, sqrt )
MM_WRAP_STD_FUN( Exp, exp, exp )
MM_WRAP_STD_FUN( Log, log, log )

#undef MM_WRAP_STD_FUN

#undef MM_DECL_SCALAR_FUN

#undef MM_OP_VAL

}

}

#endif
