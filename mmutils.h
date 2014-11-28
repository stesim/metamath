#ifndef _MMUTILS_H_
#define _MMUTILS_H_

#include "metamath.h"

namespace mm
{

namespace utils
{

namespace op
{

template<typename Top>
class DiffX
{
public:
	typedef mm::op::Scale<mm::op::Sub<mm::op::Eval<Top,+1,0>,mm::op::Eval<Top,-1,0>>> type;
};

template<typename Top>
class DiffY
{
public:
	typedef mm::op::Scale<mm::op::Sub<mm::op::Eval<Top,0,+1>,mm::op::Eval<Top,0,-1>>> type;
};

template<typename Top>
class DiffX_Fw
{
public:
	typedef mm::op::Scale<mm::op::Sub<mm::op::Eval<Top,+1,0>,Top>> type;
};

template<typename Top>
class DiffY_Fw
{
public:
	typedef mm::op::Scale<mm::op::Sub<mm::op::Eval<Top,0,+1>,Top>> type;
};

template<typename Top>
class DiffX_Bw
{
public:
	typedef mm::op::Scale<mm::op::Sub<Top,mm::op::Eval<Top,-1,0>>> type;
};

template<typename Top>
class DiffY_Bw
{
public:
	typedef mm::op::Scale<mm::op::Sub<Top,mm::op::Eval<Top,0,-1>>> type;
};

template<typename Top>
class DiffXX
{
public:
	typedef mm::op::Scale<mm::op::Add<mm::op::Sub<mm::op::Eval<Top,-1,0>,mm::op::Scale<Top>>,mm::op::Eval<Top,+1,0>>> type;
};

template<typename Top>
class DiffYY
{
public:
	typedef mm::op::Scale<mm::op::Add<mm::op::Sub<mm::op::Eval<Top,0,-1>,mm::op::Scale<Top>>,mm::op::Eval<Top,0,+1>>> type;
};

template<typename Top>
class DiffXX_YY
{
public:
	typedef mm::op::Sub<mm::op::Add<
		mm::op::Scale<mm::op::Add<mm::op::Eval<Top,-1,0>,mm::op::Eval<Top,+1,0>>>,
		mm::op::Scale<mm::op::Add<mm::op::Eval<Top,0,-1>,mm::op::Eval<Top,0,+1>>>>,
		mm::op::Scale<Top>> type;
};

}

template<typename Top>
inline typename op::DiffX<Top>::type diffX( const Top& op, typename Top::DTYPE h )
{
	return ( 1 / ( 2 * h ) % ( mm::eval<+1,0>( op ) - mm::eval<-1,0>( op ) ) );
}

template<typename Top>
inline typename op::DiffY<Top>::type diffY( const Top& op, typename Top::DTYPE h )
{
	return ( 1 / ( 2 * h ) % ( mm::eval<0,+1>( op ) - mm::eval<0,-1>( op ) ) );
}

template<typename Top>
inline typename op::DiffX_Fw<Top>::type diffX_Fw( const Top& op, typename Top::DTYPE h )
{
	return ( 1 / h % ( mm::eval<+1,0>( op ) - op ) );
}

template<typename Top>
inline typename op::DiffY_Fw<Top>::type diffY_Fw( const Top& op, typename Top::DTYPE h )
{
	return ( 1 / h % ( mm::eval<0,+1>( op ) - op ) );
}

template<typename Top>
inline typename op::DiffX_Bw<Top>::type diffX_Bw( const Top& op, typename Top::DTYPE h )
{
	return ( 1 / h % ( op - mm::eval<-1,0>( op ) ) );
}

template<typename Top>
inline typename op::DiffY_Bw<Top>::type diffY_Bw( const Top& op, typename Top::DTYPE h )
{
	return ( 1 / h % ( op - mm::eval<0,-1>( op ) ) );
}

template<typename Top>
inline typename op::DiffXX<Top>::type diffXX( const Top& op, typename Top::DTYPE h )
{
	return ( 1 / ( h * h ) % ( mm::eval<-1,0>( op ) - 2 % op + mm::eval<+1,0>( op ) ) );
}

template<typename Top>
inline typename op::DiffYY<Top>::type diffYY( const Top& op, typename Top::DTYPE h )
{
	return ( 1 / ( h * h ) % ( mm::eval<0,-1>( op ) - 2 % op + mm::eval<0,+1>( op ) ) );
}

template<typename Top, typename Th>
inline typename op::DiffXX_YY<Top>::type diffXX_YY( const Top& op, const Th& h )
{
	return ( 1 / ( h[ 0 ] * h[ 0 ] ) % ( mm::eval<-1,0>( op ) + mm::eval<+1,0>( op ) )
			+ 1 / ( h[ 1 ] * h[ 1 ] ) % ( mm::eval<0,-1>( op ) + mm::eval<0,+1>( op ) )
			- ( 2 / ( h[ 0 ] * h[ 0 ] ) + 2 / ( h[ 1 ] * h[ 1 ] ) ) % op );
}

}

}

#endif
