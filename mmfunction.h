#ifndef _MMFUNCTION_H_
#define _MMFUNCTION_H_

#include <metamath/metamath.h>
#include <cstdio>

namespace mm
{

template<typename Tfunc, typename Top>
void set( Tfunc& func, const Top& op );

template<typename T, unsigned int Dim = 2>
struct Tuple
{
public:
	Tuple()
	{
		for( unsigned int i = 0; i < Dim; ++i )
		{
			coord[ i ] = 0;
		}
	}

	template<typename U>
	Tuple( const U& values )
	{
		for( unsigned int i = 0; i < Dim; ++i )
		{
			coord[ i ] = values[ i ];
		}
	}

	Tuple<T, Dim>& operator =( const Tuple<T, Dim>& rhs )
	{
		for( unsigned int i = 0; i < Dim; ++i )
		{
			coord[ i ] = rhs.coord[ i ];
		}
		return *this;
	}

	bool operator ==( const Tuple<T, Dim>& rhs ) const
	{
		for( unsigned int i = 0; i < Dim; ++i )
		{
			if( coord[ i ] != rhs.coord[ i ] )
			{
				return false;
			}
		}
		return true;
	}

	bool operator !=( const Tuple<T, Dim>& rhs ) const
	{
		return !( *this == rhs );
	}

	T& operator []( unsigned int index )
	{
		return coord[ index ];
	}

	const T& operator []( unsigned int index ) const
	{
		return coord[ index ];
	}

	Tuple<T, Dim> operator +( const Tuple<T, Dim>& rhs ) const
	{
		Tuple<T> res( *this );
		for( unsigned int i = 0; i < Dim; ++i )
		{
			res.coord[ i ] += rhs.coord[ i ];
		}
		return res;
	}

	Tuple<T, Dim>& operator +=( const Tuple<T, Dim>& rhs )
	{
		for( unsigned int i = 0; i < Dim; ++i )
		{
			coord[ i ] += rhs.coord[ i ];
		}
		return *this;
	}

	Tuple<T, Dim> operator -( const Tuple<T, Dim>& rhs ) const
	{
		Tuple<T> res( *this );
		for( unsigned int i = 0; i < Dim; ++i )
		{
			res.coord[ i ] -= rhs.coord[ i ];
		}
		return res;
	}

	Tuple<T, Dim>& operator -=( const Tuple<T, Dim>& rhs )
	{
		for( unsigned int i = 0; i < Dim; ++i )
		{
			coord[ i ] -= rhs.coord[ i ];
		}
		return *this;
	}

public:
	T coord[ Dim ];
};

template<typename T>
struct Tuple<T, 2>
{
public:
	Tuple()
		: x( 0 ), y( 0 )
	{
	}

	Tuple( T _x, T _y )
		: x( _x ), y( _y )
	{
	}

	Tuple<T, 2>& operator =( const Tuple<T, 2>& rhs )
	{
		x = rhs.x;
		y = rhs.y;
		return *this;
	}

	bool operator ==( const Tuple<T, 2>& rhs ) const
	{
		return ( x == rhs.x && y == rhs.y );
	}

	bool operator !=( const Tuple<T, 2>& rhs ) const
	{
		return !( *this == rhs );
	}

	T& operator []( unsigned int index )
	{
		return coord[ index ];
	}

	const T& operator []( unsigned int index ) const
	{
		return coord[ index ];
	}

	Tuple<T, 2> operator +( const Tuple<T, 2>& rhs ) const
	{
		return Tuple<T, 2>( x + rhs.x, y + rhs.y );
	}

	Tuple<T, 2>& operator +=( const Tuple<T, 2>& rhs )
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	Tuple<T, 2> operator -( const Tuple<T, 2>& rhs ) const
	{
		return Tuple<T>( x - rhs.x, y - rhs.y );
	}

	Tuple<T, 2>& operator -=( const Tuple<T, 2>& rhs )
	{
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}

public:
	static const Tuple<T, 2> ZERO;
	static const Tuple<T, 2> ONE;

	union
	{
		T coord[ 2 ];
		struct
		{
			T x;
			T y;
		};
	};
};

template<typename T>
const Tuple<T, 2> Tuple<T, 2>::ZERO( 0, 0 );
template<typename T>
const Tuple<T, 2> Tuple<T, 2>::ONE( 1, 1 );

template<typename T, unsigned int Dim = 2>
class Function
{
public:
	typedef T DTYPE;
	static const unsigned int DIM = Dim;

public:
	Function()
		: m_pData( NULL ), m_bOwnsData( true )
	{
		for( unsigned int i = 0; i < Dim; ++i )
		{
			m_Size[ i ] = 0;
		}

		m_pData = NULL;
	}

	template<typename U>
	Function( const U& _size )
		: m_bOwnsData( true )
	{
		int totalSize = 1;
		for( unsigned int i = 0; i < Dim; ++i )
		{
			int dimSize = _size[ i ];
			m_Size[ i ] = dimSize;
			totalSize *= dimSize;
		}

		m_pData = new DTYPE[ totalSize ];
	}

	Function( const Function<T, Dim>& ref )
		: m_pData( ref.m_pData ), m_bOwnsData( false )
	{
		for( unsigned int i = 0; i < Dim; ++i )
		{
			m_Size[ i ] = ref.m_Size[ i ];
		}
	}

	~Function()
	{
		if( m_bOwnsData && m_pData != NULL )
		{
			delete[] m_pData;
		}
	}

	DTYPE& operator[]( int i )
	{
		return m_pData[ i ];
	}

	const DTYPE& operator[]( int i ) const
	{
		return m_pData[ i ];
	}

	DTYPE& operator()( int point )
	{
		return m_pData[ point ];
	}

	DTYPE operator()( int point ) const
	{
		return m_pData[ point ];
	}

	DTYPE& operator()( int x, int y )
	{
		return m_pData[ y * m_Size[ 0 ] + x ];
	}

	const DTYPE& operator()( int x, int y ) const
	{
		return m_pData[ y * m_Size[ 0 ] + x ];
	}

	const Tuple<int, Dim>& size() const
	{
		return m_Size;
	}

	template<typename Top>
	Function<T, Dim>& operator=( const Top& op )
	{
		set( *this, op );
		return *this;
	}

private:
	DTYPE* m_pData;
	Tuple<int, Dim> m_Size;
	bool m_bOwnsData;
};

template<typename Tfunc, typename Tbegin, typename Tend>
void printFunc( const Tfunc& func, const Tbegin& begin, const Tend& end )
{
	for( int j = begin.y; j < end.y; ++j )
	{
		for( int i = begin.x; i < end.x; ++i )
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
