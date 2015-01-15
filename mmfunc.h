#ifndef _MMFUNC_H_
#define _MMFUNC_H_

#include <cstddef>
#include <type_traits>
#include <cstdio>
#include <cstring>
#include <metamath/metamath.h>

namespace mm
{

template<typename Tfunc, typename Top>
void set( Tfunc& func, const Top& op );

template<typename T, std::size_t N>
class Tuple;

template<typename T, std::size_t Dim>
class Func
{
static_assert( Dim > 0, "mm::Func dimension must be greater than 0" );

public:
	typedef T DTYPE;
	static const std::size_t DIM = Dim;

public:
	Func()
		: m_pData( nullptr ), m_bOwnsData( true )
	{
	}

	template<typename... Tvar>
	Func( Tvar... size )
		: m_bOwnsData( true )
	{
		static_assert( sizeof...( Tvar ) == DIM, "Invalid parameter count" );
		std::size_t totalSize = setSize( size... );
		m_pData = new DTYPE[ totalSize ];
	}

	Func( const Func<DTYPE, DIM>& ref )
		: m_pData( ref.m_pData ), m_bOwnsData( false )
	{
		for( std::size_t i = 0; i < DIM; ++i )
		{
			m_Size[ i ] = ref.m_Size[ i ];
		}
	}

	template<typename U, typename Enable = typename std::enable_if<
		!std::is_convertible<U, std::size_t>::value>::type>
	Func( const U& size )
		: m_bOwnsData( true )
	{
		int totalSize = 1;
		for( std::size_t i = 0; i < DIM; ++i )
		{
			int dimSize = size[ i ];
			m_Size[ i ] = dimSize;
			totalSize *= dimSize;
		}

		m_pData = new DTYPE[ totalSize ];
	}

	~Func()
	{
		if( m_bOwnsData && m_pData != nullptr )
		{
			delete[] m_pData;
		}
	}

	template<typename Top>
	Func<DTYPE, DIM>& operator =( const Top& op )
	{
		set( *this, op );
		return *this;
	}

	DTYPE& operator[]( int i )
	{
		return m_pData[ i ];
	}

	const DTYPE& operator[]( int i ) const
	{
		return m_pData[ i ];
	}

	template<typename... Tvar>
	DTYPE& operator()( Tvar... idx )
	{
		static_assert( sizeof...( Tvar ) == DIM, "Invalid parameter count" );
		return m_pData[ getLinearIndex( idx... ) ];
	}

	template<typename... Tvar>
	const DTYPE& operator()( Tvar... idx ) const
	{
		static_assert( sizeof...( Tvar ) == DIM, "Invalid parameter count" );
		return m_pData[ getLinearIndex( idx... ) ];
	}

	const Tuple<std::size_t, DIM>& size() const
	{
		return m_Size;
	}

	const std::size_t& size( std::size_t dim ) const
	{
		return m_Size[ dim ];
	}

private:
	template<typename... Tvar>
	std::size_t getLinearIndex( std::size_t idx1, Tvar... idx ) const
	{
		return ( idx1 + m_Size[ DIM - sizeof...( Tvar ) - 1 ] *
				getLinearIndex( idx... ) );
	}

	std::size_t getLinearIndex( std::size_t idx ) const
	{
		return idx;
	}

	template<typename... Tvar>
	std::size_t setSize( std::size_t size1, Tvar... size )
	{
		m_Size[ DIM - sizeof...( Tvar ) - 1 ] = size1;
		return size1 * setSize( size... );
	}

	std::size_t setSize( std::size_t size1 )
	{
		m_Size[ DIM - 1 ] = size1;
		return size1;
	}

private:
	DTYPE* m_pData;
	Tuple<std::size_t, DIM> m_Size;
	bool m_bOwnsData;
};

template<typename T, std::size_t Dim>
class Tuple
{
	template<typename U, std::size_t D>
	friend class Tuple;

public:
	typedef T DTYPE;
	static const std::size_t DIM = Dim;

public:
	Tuple()
		: x( m_pData[ 0 ] ), y( m_pData[ 1 ] )
	{
	}

	Tuple( const Tuple<DTYPE, DIM>& ref )
		: x( m_pData[ 0 ] ), y( m_pData[ 1 ] )
	{
		memcpy( m_pData, ref.m_pData, DIM * sizeof( DTYPE ) );
	}

	template<typename U>
	Tuple( const Tuple<U, DIM>& ref )
		: x( m_pData[ 0 ] ), y( m_pData[ 1 ] )
	{
		for( std::size_t i = 0; i < DIM; ++i )
		{
			m_pData[ i ] = ref.m_pData[ i ];
		}
	}

	template<typename... Tvar, typename Enable = typename std::enable_if<
		sizeof...( Tvar ) == DIM>::type>
	Tuple( Tvar... vals )
		: x( m_pData[ 0 ] ), y( m_pData[ 1 ] )
	{
		static_assert( sizeof...( Tvar ) == DIM, "Invalid paramter count" );
		setData( vals... );
	}

	Tuple<DTYPE, DIM>& operator =( const Tuple<DTYPE, DIM>& ref )
	{
		memcpy( m_pData, ref.m_pData, DIM * sizeof( DTYPE ) );
		return *this;
	}

	template<typename U>
	Tuple<DTYPE, DIM> operator =( const Tuple<U, DIM>& ref )
	{
		for( std::size_t i = 0; i < DIM; ++i )
		{
			m_pData[ i ] = ref.m_pData[ i ];
		}
		return *this;
	}

	/*
	Tuple<DTYPE, DIM>& operator =( const Tuple<DTYPE, DIM>&& ref )
	{
		memcpy( m_pData, ref.m_pData, DIM * sizeof( DTYPE ) );
		return *this;
	}
	*/

	constexpr static std::size_t size()
	{
		return DIM;
	}

	template<typename U, typename Enable = typename std::enable_if<
		std::is_convertible<DTYPE, U>::value>::type>
	operator Tuple<U, DIM>() const
	{
		Tuple<U, DIM> res;
		for( std::size_t i = 0; i < DIM; ++i )
		{
			res.m_pData[ i ] = m_pData[ i ];
		}
		return res;
	}

	DTYPE& operator[]( std::size_t idx )
	{
		return m_pData[ idx ];
	}

	const DTYPE& operator[]( std::size_t idx ) const
	{
		return m_pData[ idx ];
	}

	bool operator ==( const Tuple<DTYPE, DIM>& rhs ) const
	{
		for( std::size_t i = 0; i < DIM; ++i )
		{
			if( m_pData[ i ] != rhs.m_pData[ i ] )
			{
				return false;
			}
		}
		return true;
	}

	bool operator !=( const Tuple<DTYPE, DIM>& rhs ) const
	{
		return !( *this == rhs );
	}

	template<typename U>
	Tuple<decltype(DTYPE() + U()), DIM> operator +(
			const Tuple<U, DIM>& rhs ) const
	{
		Tuple<decltype(DTYPE() - U()), DIM> res;
		for( std::size_t i = 0; i < DIM; ++i )
		{
			res.m_pData[ i ] = m_pData[ i ] + rhs.m_pData[ i ];
		}
		return res;
	}

	template<typename U>
	Tuple<DTYPE, DIM>& operator +=( const Tuple<U, DIM>& rhs )
	{
		for( std::size_t i = 0; i < DIM; ++i )
		{
			m_pData[ i ] += rhs.m_pData[ i ];
		}
		return *this;
	}

	template<typename U>
	Tuple<decltype(DTYPE() - U()), DIM> operator -(
			const Tuple<U, DIM>& rhs ) const
	{
		Tuple<decltype(DTYPE() - U()), DIM> res;
		for( std::size_t i = 0; i < DIM; ++i )
		{
			res.m_pData[ i ] = m_pData[ i ] - rhs.m_pData[ i ];
		}
		return res;
	}

	template<typename U>
	Tuple<DTYPE, DIM>& operator -=( const Tuple<U, DIM>& rhs )
	{
		for( std::size_t i = 0; i < DIM; ++i )
		{
			m_pData[ i ] -= rhs.m_pData[ i ];
		}
		return *this;
	}

private:
	template<typename... Tvar>
	void setData( DTYPE data1, Tvar... data )
	{
		m_pData[ DIM - sizeof...( Tvar ) - 1 ] = data1;
		setData( data... );
	}

	void setData( DTYPE data1 )
	{
		m_pData[ DIM - 1 ] = data1;
	}

public:
	/* TODO: implement properly for DIM != 2 */
	static const Tuple<DTYPE, Dim> ZERO;
	static const Tuple<DTYPE, Dim> ONE;

	DTYPE& x;
	DTYPE& y;
	/* ------------------------------------- */

private:
	DTYPE m_pData[ DIM ];
};

template<typename T, std::size_t Dim>
const Tuple<T, Dim> Tuple<T, Dim>::ZERO( 0, 0 );

template<typename T, std::size_t Dim>
const Tuple<T, Dim> Tuple<T, Dim>::ONE( 1, 1 );

template<typename Tfunc, typename Tbegin, typename Tend>
void printFunc( const Tfunc& func, const Tbegin& begin, const Tend& end )
{
	for( int j = begin[ 1 ]; j < end[ 1 ]; ++j )
	{
		for( int i = begin[ 0 ]; i < end[ 0 ]; ++i )
		{
			printf( "%12.4f", func( i, j ) );
		}
		printf( "\n" );
	}
}

template<typename Tfunc>
void printFunc( const Tfunc& func )
{
	int sizeX = func.size()[ 0 ];
	int sizeY = func.size()[ 1 ];
	for( int j = 0; j < sizeY; ++j )
	{
		for( int i = 0; i < sizeX; ++i )
		{
			printf( "%12.4f", func( i, j ) );
		}
		printf( "\n" );
	}
}

}

#endif
