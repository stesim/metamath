#ifndef _METAMATH_H_
#define _METAMATH_H_

#include <type_traits>
#include <random>

namespace mm
{

template<typename T>
using enable_if_compound =
	typename std::enable_if<std::is_compound<T>::value,T>::type;

template<typename T>
using op_dtype =
	typename std::remove_reference<decltype( std::declval<T>()( 0, 0 ) )>::type;

template<typename Tfunc>
class FunctionView
{
public:
	static const unsigned int DIM = Tfunc::DIM;

private:
	typedef op_dtype<Tfunc> DTYPE;

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

	DTYPE& operator[]( int i )
	{
		return ( *m_pFunc )[ i ];
	}

	DTYPE operator[]( int i ) const
	{
		return ( *m_pFunc )[ i ];
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
private:
	typedef op_dtype<Tfunc> DTYPE;

public:
	Eval( const Tfunc& func )
		: m_Func( func )
	{
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
private:
	typedef op_dtype<Tfunc> DTYPE;

public:
	Eval( const Tfunc& func )
		: m_Func( func )
	{
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
private:
	typedef op_dtype<Tfunc> DTYPE;

public:
	Eval( const Tfunc& func )
		: m_Func( func )
	{
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
private:
	typedef op_dtype<Tfunc> DTYPE;

public:
	Eval( const Tfunc& func )
		: m_Func( func )
	{
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
	Const( T constant )
		: m_Constant( constant )
	{
	}

	T operator()( int x, int y ) const
	{
		return m_Constant;
	}

private:
	T m_Constant;
};

template<typename T>
class Rand
{
public:
	Rand( T min, T max )
		: m_Min( min ), m_Max( max )
	{
	}

	T operator()( int x, int y ) const
	{
		return ( m_Min + ( rand() / (double)RAND_MAX ) * ( m_Max - m_Min ) );
	}

private:
	T m_Min;
	T m_Max;
};

template<typename Top1, typename Top2>
class Add
{
private:
	typedef decltype(std::declval<op_dtype<Top1>>()
			+ std::declval<op_dtype<Top2>>()) DTYPE;

public:
	Add( const Top1& op1, const Top2& op2 )
		: m_Op1( op1 ), m_Op2( op2 )
	{
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
private:
	typedef decltype(std::declval<op_dtype<Top1>>()
			- std::declval<op_dtype<Top2>>()) DTYPE;

public:
	Sub( const Top1& op1, const Top2& op2 )
		: m_Op1( op1 ), m_Op2( op2 )
	{
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
private:
	typedef decltype(std::declval<op_dtype<Top1>>()
			* std::declval<op_dtype<Top2>>()) DTYPE;

public:
	Mul( const Top1& op1, const Top2& op2 )
		: m_Op1( op1 ), m_Op2( op2 )
	{
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
private:
	typedef decltype(std::declval<op_dtype<Top1>>()
			/ std::declval<op_dtype<Top2>>()) DTYPE;

public:
	Div( const Top1& op1, const Top2& op2 )
		: m_Op1( op1 ), m_Op2( op2 )
	{
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
private:
	typedef op_dtype<Top> DTYPE;

public:
	Scale( const Top& op, DTYPE factor )
		: m_Op( op ), m_Factor( factor )
	{
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
private:
	typedef op_dtype<Top> DTYPE;

public:
	Abs( const Top& op )
		: m_Op( op )
	{
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
private:
	typedef op_dtype<Top> DTYPE;

public:
	Sqr( const Top& op )
		: m_Op( op )
	{
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
private:
	typedef op_dtype<Top> DTYPE;

public:
	Neg( const Top& op )
		: m_Op( op )
	{
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
private:
	typedef op_dtype<Top> DTYPE;

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

template<typename T>
inline op::Rand<T> rand( T min, T max )
{
	return op::Rand<T>( min, max );
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
inline op::Scale<Top> scale( const Top& op, op_dtype<Top> factor )
{
	return op::Scale<Top>( op, factor );
}

template<typename Top>
inline op::Scale<Top> operator*( op_dtype<Top> factor, const Top& op )
{
	return op::Scale<Top>( op, factor );
}

template<typename Top>
inline op::Scale<Top> operator*( const Top& op, op_dtype<Top> factor )
{
	return op::Scale<Top>( op, factor );
}

template<typename Top>
inline op::Scale<Top> operator/( op_dtype<Top> factor, const Top& op )
{
	return op::Scale<Top>( op, (op_dtype<Top>)1 / factor );
}

template<typename Top>
inline op::Scale<Top> operator/( const Top& op, op_dtype<Top> factor )
{
	return op::Scale<Top>( op, (op_dtype<Top>)1 / factor );
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
			func( i, j ) = op( i, j );
		}
	}
}

template<typename Tfunc, typename Top>
inline void set( Tfunc& func, int beginX, int beginY,
		int endX, int endY, const Top& op )
{
	for( int j = beginY; j < endY; ++j )
	{
		for( int i = beginX; i < endX; ++i )
		{
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
inline op_dtype<Top> max( const Top& op, int beginX, int beginY,
		int endX, int endY )
{
	typedef op_dtype<Top> DTYPE;

	DTYPE maxVal = op( beginX, beginY );
	for( int j = beginY; j < endY; ++j )
	{
		for( int i = beginX; i < endX; ++i )
		{
			DTYPE curVal = op( i, j );
			if( curVal > maxVal )
			{
				maxVal = curVal;
			}
		}
	}
	return maxVal;
}

template<typename Top, typename Tbegin, typename Tend>
inline op_dtype<Top> max( const Top& op, const Tbegin& begin,
		const Tend& end )
{
	return max( op, begin[ 0 ], begin[ 1 ], end[ 0 ], end[ 1 ] );
}

template<typename Top>
inline op_dtype<Top> min( const Top& op, int beginX, int beginY,
		int endX, int endY )
{
	typedef op_dtype<Top> DTYPE;

	DTYPE minVal = op( beginX, beginY );
	for( int j = beginY; j < endY; ++j )
	{
		for( int i = beginX; i < endX; ++i )
		{
			DTYPE curVal = op( i, j );
			if( curVal < minVal )
			{
				minVal = curVal;
			}
		}
	}
	return minVal;
}

template<typename Top, typename Tbegin, typename Tend>
inline op_dtype<Top> min( const Top& op, const Tbegin& begin,
		const Tend& end )
{
	return max( op, begin[ 0 ], begin[ 1 ], end[ 0 ], end[ 1 ] );
}

template<typename Top>
inline op_dtype<Top> sum( const Top& op, int beginX, int beginY,
		int endX, int endY )
{
	op_dtype<Top> sum = 0;
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
inline op_dtype<Top> sum( const Top& op, const Tbegin& begin,
		const Tend& end )
{
	return sum( op, begin[ 0 ], begin[ 1 ], end[ 0 ], end[ 1 ] );
}

}

#endif
