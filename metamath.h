#ifndef _METAMATH_H_
#define _METAMATH_H_

#include <type_traits>

#ifndef NULL
#define NULL 0
#endif

namespace mm
{

template<typename T>
using enable_if_compound =
	typename std::enable_if<std::is_compound<T>::value,T>::value;

template<typename Tfunc>
class FunctionView
{
public:
	typedef typename Tfunc::DTYPE DTYPE;
	static const unsigned int DIM = Tfunc::DIM;

public:
	FunctionView( Tfunc& func, int beginX, int beginY, int endX, int endY )
		: m_pFunc( &func )
	{
		m_pBegin[ 0 ] = beginX;
		m_pBegin[ 1 ] = beginY;
		m_pSize[ 0 ] = endX - beginX;
		m_pSize[ 1 ] = endY - beginY;
	}

	template<typename Tbegin, typename Tend>
	FunctionView( Tfunc& func, const Tbegin& begin, const Tend& end )
		: m_pFunc( &func )
	{
		m_pBegin[ 0 ] = begin[ 0 ];
		m_pBegin[ 1 ] = begin[ 1 ];
		m_pSize[ 0 ] = end[ 0 ] - begin[ 0 ];
		m_pSize[ 1 ] = end[ 1 ] - begin[ 1 ];
	}

	DTYPE& operator()( int x, int y )
	{
		return ( *m_pFunc )( m_pBegin[ 0 ] + x, m_pBegin[ 1 ] + y );
	}

	DTYPE operator()( int x, int y ) const
	{
		return ( *m_pFunc )( m_pBegin[ 0 ] + x, m_pBegin[ 1 ] + y );
	}

	const int* size() const
	{
		return m_pSize;
	}

	template<typename Top>
	FunctionView<Tfunc>& operator=( const Top& op )
	{
		set( *this, op );
		return *this;
	}

private:
	Tfunc* m_pFunc;
	int m_pBegin[ DIM ];
	int m_pSize[ DIM ];
};

namespace op
{

/* === BEGIN OPERATOR DECLARATIONS === */

template<typename Tfunc, int OffsetX = 0, int OffsetY = 0>
class Eval
{
public:
	typedef typename Tfunc::DTYPE DTYPE;

public:
	Eval( const Tfunc& func )
		: m_Func( func )
	{
	}

	DTYPE operator()( int point ) const
	{
		return m_Func( point + OffsetY * m_Func->size()[ 0 ] + OffsetX );
	}

	DTYPE operator()( int x, int y ) const
	{
		return m_Func( x + OffsetX, y + OffsetY );
	}

private:
	const Tfunc m_Func;
};

template<typename Tfunc>
class Eval<Tfunc, 0, 0>
{
public:
	typedef typename Tfunc::DTYPE DTYPE;

public:
	Eval( const Tfunc& func )
		: m_Func( func )
	{
	}

	DTYPE operator()( int point ) const
	{
		return m_Func( point );
	}

	DTYPE operator()( int x, int y ) const
	{
		return m_Func( x, y );
	}

private:
	const Tfunc m_Func;
};

template<typename Tfunc, int OffsetX>
class Eval<Tfunc, OffsetX, 0>
{
public:
	typedef typename Tfunc::DTYPE DTYPE;

public:
	Eval( const Tfunc& func )
		: m_Func( func )
	{
	}

	DTYPE operator()( int point ) const
	{
		return m_Func( point + OffsetX );
	}

	DTYPE operator()( int x, int y ) const
	{
		return m_Func( x + OffsetX, y );
	}

private:
	const Tfunc m_Func;
};

template<typename Tfunc, int OffsetY>
class Eval<Tfunc, 0, OffsetY>
{
public:
	typedef typename Tfunc::DTYPE DTYPE;

public:
	Eval( const Tfunc& func )
		: m_Func( func )
	{
	}

	DTYPE operator()( int point ) const
	{
		return m_Func( point + OffsetY * m_Func->size()[ 0 ] );
	}

	DTYPE operator()( int x, int y ) const
	{
		return m_Func( x, y + OffsetY );
	}

private:
	const Tfunc m_Func;
};

template<typename T>
class Const
{
public:
	typedef T DTYPE;

public:
	Const( DTYPE constant )
		: m_Constant( constant )
	{
	}

	DTYPE operator()( int point ) const
	{
		return m_Constant;
	}

	DTYPE operator()( int x, int y ) const
	{
		return m_Constant;
	}

private:
	DTYPE m_Constant;
};

template<typename Top1, typename Top2>
class Add
{
public:
	typedef typename Top1::DTYPE DTYPE;

public:
	Add( const Top1& op1, const Top2& op2 )
		: m_Op1( op1 ), m_Op2( op2 )
	{
	}

	DTYPE operator()( int point ) const
	{
		return m_Op1( point ) + m_Op2( point );
	}

	DTYPE operator()( int x, int y ) const
	{
		return ( m_Op1( x, y ) + m_Op2( x, y ) );
	}

private:
	const Top1 m_Op1;
	const Top2 m_Op2;
};

template<typename Top1, typename Top2>
class Sub
{
public:
	typedef typename Top1::DTYPE DTYPE;

public:
	Sub( const Top1& op1, const Top2& op2 )
		: m_Op1( op1 ), m_Op2( op2 )
	{
	}

	DTYPE operator()( int point ) const
	{
		return m_Op1( point ) - m_Op2( point );
	}

	DTYPE operator()( int x, int y ) const
	{
		return ( m_Op1( x, y ) - m_Op2( x, y ) );
	}

private:
	const Top1 m_Op1;
	const Top2 m_Op2;
};

template<typename Top1, typename Top2>
class Mul
{
public:
	typedef typename Top1::DTYPE DTYPE;

public:
	Mul( const Top1& op1, const Top2& op2 )
		: m_Op1( op1 ), m_Op2( op2 )
	{
	}

	DTYPE operator()( int point ) const
	{
		return m_Op1( point ) * m_Op2( point );
	}

	DTYPE operator()( int x, int y ) const
	{
		return ( m_Op1( x, y ) * m_Op2( x, y ) );
	}

private:
	const Top1 m_Op1;
	const Top2 m_Op2;
};

template<typename Top1, typename Top2>
class Div
{
public:
	typedef typename Top1::DTYPE DTYPE;

public:
	Div( const Top1& op1, const Top2& op2 )
		: m_Op1( op1 ), m_Op2( op2 )
	{
	}

	DTYPE operator()( int point ) const
	{
		return m_Op1( point ) / m_Op2( point );
	}

	DTYPE operator()( int x, int y ) const
	{
		return ( m_Op1( x, y ) / m_Op2( x, y ) );
	}

private:
	const Top1 m_Op1;
	const Top2 m_Op2;
};

template<typename Top>
class Scale
{
public:
	typedef typename Top::DTYPE DTYPE;

public:
	Scale( const Top& op, DTYPE factor )
		: m_Op( op ), m_Factor( factor )
	{
	}

	DTYPE operator()( int point ) const
	{
		return m_Factor * m_Op( point );
	}

	DTYPE operator()( int x, int y ) const
	{
		return ( m_Factor * m_Op( x, y ) );
	}

private:
	const Top m_Op;
	DTYPE m_Factor;
};

template<typename Top>
class Abs
{
public:
	typedef typename Top::DTYPE DTYPE;

public:
	Abs( const Top& op )
		: m_Op( op )
	{
	}

	DTYPE operator()( int point ) const
	{
		DTYPE val = m_Op( point );
		return ( val >= 0 ? val : -val );
	}

	DTYPE operator()( int x, int y ) const
	{
		DTYPE val = m_Op( x, y );
		return ( val >= 0 ? val : -val );
	}

private:
	const Top m_Op;
};

template<typename Top>
class Sqr
{
public:
	typedef typename Top::DTYPE DTYPE;

public:
	Sqr( const Top& op )
		: m_Op( op )
	{
	}

	DTYPE operator()( int point ) const
	{
		DTYPE val = m_Op( point );
		return ( val * val );
	}

	DTYPE operator()( int x, int y ) const
	{
		DTYPE val = m_Op( x, y );
		return ( val * val );
	}

private:
	const Top m_Op;
};

template<typename Top>
class Neg
{
public:
	typedef typename Top::DTYPE DTYPE;

public:
	Neg( const Top& op )
		: m_Op( op )
	{
	}

	DTYPE operator()( int point ) const
	{
		return -m_Op( point );
	}

	DTYPE operator()( int x, int y ) const
	{
		return -m_Op( x, y );
	}

private:
	const Top m_Op;
};

template<typename Top>
class Transpose
{
public:
	typedef typename Top::DTYPE DTYPE;

public:
	Transpose( const Top& op )
		: m_Op( op )
	{
	}

	DTYPE operator()( int x, int y ) const
	{
		return m_Op( y, x );
	}

private:
	const Top m_Op;
};

/* === END OPERATOR DECLARATIONS === */

}

/* === BEGIN OPERATOR PROXIES === */

template<typename Tfunc>
inline FunctionView<Tfunc> view( Tfunc& func, int beginX, int beginY,
		int endX, int endY )
{
	return FunctionView<Tfunc>( func, beginX, beginY, endX, endY );
}

template<typename Tfunc, typename Tbegin, typename Tend>
inline FunctionView<Tfunc> view( Tfunc& func, const Tbegin& begin,
		const Tend& end )
{
	return FunctionView<Tfunc>( func, begin, end );
}

template<int OffsetX, int OffsetY, typename Tfunc>
inline op::Eval<Tfunc, OffsetX, OffsetY> eval( const Tfunc& func )
{
	return op::Eval<Tfunc, OffsetX, OffsetY>( func );
}

template<typename Tfunc>
inline Tfunc eval( const Tfunc& func )
{
	return func;
}

template<typename T>
inline op::Const<T> constant( T value )
{
	return op::Const<T>( value );
}

template<typename Top1, typename Top2>
inline op::Add<Top1, Top2> add( const Top1& op1, const Top2& op2 )
{
	return op::Add<Top1, Top2>( op1, op2 );
}

template<typename Top1, typename Top2>
inline op::Add<Top1, Top2> operator+( const Top1& op1, const Top2& op2 )
{
	return op::Add<Top1, Top2>( op1, op2 );
}

template<typename Top1, typename Top2>
inline op::Sub<Top1, Top2> sub( const Top1& op1, const Top2& op2 )
{
	return op::Sub<Top1, Top2>( op1, op2 );
}

template<typename Top1, typename Top2>
inline op::Sub<Top1, Top2> operator-( const Top1& op1, const Top2& op2 )
{
	return op::Sub<Top1, Top2>( op1, op2 );
}

template<typename Top1, typename Top2>
inline op::Mul<Top1, Top2> mul( const Top1& op1, const Top2& op2 )
{
	return op::Mul<Top1, Top2>( op1, op2 );
}

template<typename Top1, typename Top2,
	typename Enable1 = enable_if_compound<Top1>,
	typename Enable2 = enable_if_compound<Top2>>
inline op::Mul<Top1, Top2> operator*( const Top1& op1, const Top2& op2 )
{
	return op::Mul<Top1, Top2>( op1, op2 );
}

template<typename Top1, typename Top2>
inline op::Div<Top1, Top2> div( const Top1& op1, const Top2& op2 )
{
	return op::Div<Top1, Top2>( op1, op2 );
}

template<typename Top1, typename Top2,
	typename Enable1 = enable_if_compound<Top1>,
	typename Enable2 = enable_if_compound<Top2>>
inline op::Div<Top1, Top2> operator/( const Top1& op1, const Top2& op2 )
{
	return op::Div<Top1, Top2>( op1, op2 );
}

template<typename Top>
inline op::Scale<Top> scale( const Top& op, typename Top::DTYPE factor )
{
	return op::Scale<Top>( op, factor );
}

template<typename Top>
inline op::Scale<Top> operator*( typename Top::DTYPE factor, const Top& op )
{
		return op::Scale<Top>( op, factor );
}

template<typename Top>
inline op::Scale<Top> operator*( const Top& op, typename Top::DTYPE factor )
{
		return op::Scale<Top>( op, factor );
}

template<typename Top>
inline op::Scale<Top> operator/( typename Top::DTYPE factor, const Top& op )
{
		return op::Scale<Top>( op, (typename Top::DTYPE)1 / factor );
}

template<typename Top>
inline op::Scale<Top> operator/( const Top& op, typename Top::DTYPE factor )
{
		return op::Scale<Top>( op, (typename Top::DTYPE)1 / factor );
}

template<typename Top>
inline op::Abs<Top> abs( const Top& op )
{
	return op::Abs<Top>( op );
}

template<typename Top>
inline op::Sqr<Top> sqr( const Top& op )
{
	return op::Sqr<Top>( op );
}

template<typename Top>
inline op::Neg<Top> neg( const Top& op )
{
	return op::Neg<Top>( op );
}

template<typename Top>
inline op::Neg<Top> operator-( const Top& op )
{
	return op::Neg<Top>( op );
}

template<typename Top>
inline op::Transpose<Top> transpose( const Top& op )
{
	return op::Transpose<Top>( op );
}

/* === END OPERATOR PROXIES === */

template<typename Tfunc, typename Top>
inline void set( Tfunc& func, const Top& op )
{
	int sizeX = func.size()[ 0 ];
	int sizeY = func.size()[ 1 ];
	for( int j = 0; j < sizeY; ++j )
	{
		for( int i = 0; i < sizeX; ++i )
		{
// 			int point = j * sizeX + i;
// 			func( point ) = op( point );
			func( i, j ) = op( i, j );
		}
	}
}

template<typename Tfunc, typename Top>
inline void set( Tfunc& func, int beginX, int beginY,
		int endX, int endY, const Top& op )
{
//	int sizeX = func.size()[ 0 ];
	for( int j = beginY; j < endY; ++j )
	{
		for( int i = beginX; i < endX; ++i )
		{
//			int point = j * sizeX + i;
//			func( point ) = op( point );
			func( i, j ) = op( i, j );
		}
	}
}

template<typename Tfunc, typename Top, typename Tbegin, typename Tend>
inline void set( Tfunc& func, const Tbegin& begin,
		const Tend& end, const Top& op )
{
	set( func, begin[ 0 ], begin[ 1 ], end[ 0 ], end[ 1 ], op );
}

template<typename Tfunc, typename Top, typename Tbegin, typename Tend, typename Tstep>
inline void set( Tfunc& func, const Tbegin& begin,
		const Tend& end, const Top& op, const Tstep& step )
{
	int beginX = begin[ 0 ];
	int beginY = begin[ 1 ];
	int endX = end[ 0 ];
	int endY = end[ 1 ];
	int stepX = step[ 0 ];
	int stepY = step[ 1 ];
	for( int j = beginY; j < endY; j += stepY )
	{
		for( int i = beginX; i < endX; i += stepX )
		{
			func( i, j ) = op( i, j );
		}
	}
}

template<typename Top>
inline typename Top::DTYPE max( const Top& op, int beginX, int beginY,
		int endX, int endY )
{
	typename Top::DTYPE maxVal = op( beginX, beginY );
	for( int j = beginY; j < endY; ++j )
	{
		for( int i = beginX; i < endX; ++i )
		{
			typename Top::DTYPE curVal = op( i, j );
			if( curVal > maxVal )
			{
				maxVal = curVal;
			}
		}
	}
	return maxVal;
}

template<typename Top, typename Tbegin, typename Tend>
inline typename Top::DTYPE max( const Top& op, const Tbegin& begin,
		const Tend& end )
{
	return max( op, begin[ 0 ], begin[ 1 ], end[ 0 ], end[ 1 ] );
}

template<typename Top>
inline typename Top::DTYPE min( const Top& op, int beginX, int beginY,
		int endX, int endY )
{
	typename Top::DTYPE minVal = op( beginX, beginY );
	for( int j = beginY; j < endY; ++j )
	{
		for( int i = beginX; i < endX; ++i )
		{
			typename Top::DTYPE curVal = op( i, j );
			if( curVal < minVal )
			{
				minVal = curVal;
			}
		}
	}
	return minVal;
}

template<typename Top, typename Tbegin, typename Tend>
inline typename Top::DTYPE min( const Top& op, const Tbegin& begin,
		const Tend& end )
{
	return max( op, begin[ 0 ], begin[ 1 ], end[ 0 ], end[ 1 ] );
}

template<typename Top>
inline typename Top::DTYPE sum( const Top& op, int beginX, int beginY,
		int endX, int endY )
{
	typename Top::DTYPE sum = 0;
	for( int j = beginY; j < endY; ++j )
	{
		for( int i = beginX; i < endX; ++i )
		{
			sum += op( i, j );
		}
	}
	return sum;
}

template<typename Top, typename Tbegin, typename Tend>
inline typename Top::DTYPE sum( const Top& op, const Tbegin& begin,
		const Tend& end )
{
	return sum( op, begin[ 0 ], begin[ 1 ], end[ 0 ], end[ 1 ] );
}

}

#endif
