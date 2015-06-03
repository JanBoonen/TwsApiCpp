//----------------------------------------------------------------------------
// Enumerations
// By Jan Boonen
//----------------------------------------------------------------------------
#include "string.h"

#undef ENUM_N
#undef ENUM_V
#undef ENUM_S
#undef ENUMVS
#undef ENUMValues
#undef ENUMFunctions

#ifndef ENUMImplementation
	#define ENUMValues( Name ) \
	struct Name \
	{ \
		enum ENUMS {

	#define ENUM_N( N       )   N      , // enumerator id is Name and compilers assigns value (default)
	#define ENUM_V( N, V    )   N = V  , // idem but user assigns own value V
	#define ENUM_S( N, S    )   N      , // idem as ENUM_N
	#define ENUMVS( N, V, S )   N = V  , // idem as ENUM_V

	#define ENUMFunctions( Name ) _INVALID_ };\
		private: typedef struct { ENUMS first; const char* second; } STRINGS; \
		public:  typedef STRINGS* iterator; \
		private: static iterator get_iterator( bool GetBeginIterator ); \
		public: \
		static inline iterator begin(void) { return get_iterator( true  ); } \
		static inline iterator end  (void) { return get_iterator( false ); } \
		static inline iterator find (ENUMS       e) { iterator it; for( it = begin(); it != end(); ++it ){if( it->first == e      ) return it;}  return it; } \
		static inline iterator find (const char* s) { iterator it; for( it = begin(); it != end(); ++it ){if(!strcmp(it->second,s)) return it;}  return it; } \
		static inline bool IsValid( iterator it ) { return (it >= begin() && it < end()); } \
		\
		inline Name( void          ) :m_it( end() ) {} \
		inline Name( const ENUMS e ) :m_it(find(e)) {} \
		inline Name( const char* s ) :m_it(find(s)) {} \
		inline ENUMS operator* ( void ) const { return m_it->first;  } \
		inline operator ENUMS  ( void ) const { return m_it->first;  } \
		inline const char* str ( void ) const { return m_it->second; } \
		private: const iterator m_it; \
	}; \
	\
	inline static const char* operator *  (                 const Name::ENUMS e ) { return (    Name::find(e)->second); } \
	inline static const char* operator *= ( const char *&s, const Name::ENUMS e ) { return (s = Name::find(e)->second); } \
	inline static bool        operator *= ( Name::ENUMS& e, const char*       s ) { return (Name::_INVALID_ != (e = Name::find(s)->first)); }

#else
	#define ENUMValues( Name ) \
	Name::iterator Name::get_iterator( bool GetBeginIterator ) \
	{ \
		static STRINGS S[] = {

	#define ENUM_N( N       ) { N, #N } , // string value equals name (default)
	#define ENUM_V( N, V    ) { N, #N } , // idem as ENUM_N
	#define ENUM_S( N, S    ) { N, S  } , // string value equals parameter S (exception)
	#define ENUMVS( N, V, S ) { N, S  } , // idem as ENUM_S

	#define ENUMFunctions( Name ) { _INVALID_, "_INVALID_" } };\
		static iterator begin = &S[0]; \
		static iterator end   = &S[ (sizeof(S)/sizeof(STRINGS)) - 1 ]; \
		return GetBeginIterator ?begin :end; \
	} 
#endif



#if 0
// The following is an example of a definition of an enum and the generated
// code by this set of macro's.
ENUMValues( Player )                        // enum Player
	ENUM_V( First    , 13             )     // First , 13, "First"
	ENUM_N( Second                    )     // Second, 14, "Second"
	ENUM_V( Third    , 30             )     // Third , 30, "Third"
	ENUM_S( Fourth   , "!Surprice!"   )     // Fourth, 31, "!Surprice!"
	ENUM_N( Fifth                     )     // Fifth , 32, "Fifth"
	ENUM_VS(Sixth    , 60 , "LastOne" )     // Sixth , 60, "LastOne"
ENUMFunctions( Player )

// When including the definitions the first time
struct Player
{
	enum ENUMS
	{ First = 13
	, Second
	, Third = 30
	, Fourth
	, Fifth
	, Sixth = 60
	, _INVALID_
	};

	typedef struct {
		ENUMS       first;
		const char* second;
	} STRINGS, * iterator;

	static iterator get_iterator( bool GetBeginIterator );	// implemented where ENUMImplementation is defined

	static inline iterator begin(void)
	{
		return get_iterator( true  );
	}

	static inline iterator end(void)
	{
		return get_iterator( false );
	}

	static inline iterator find (ENUMS e)
	{
		iterator it;
		for( it = begin(); it != end(); ++it )
		{
			if( it->first == e      )
				return it;
		}
		return it;
	}

	static inline iterator find (const char* s)
	{
		iterator it;
		for( it = begin(); it != end(); ++it )
		{
			if(!strcmp(it->second,s))
				return it;
		}
		return it;
	}

	static inline bool IsValid( iterator it )
	{
		return (it >= begin() && it < end());
	}

	inline Name( const ENUMS e ) :m_it(find(e)) {}
	inline Name( const char* s ) :m_it(find(s)) {}

	inline ENUMS operator* ( void ) const { return m_it->first;  }
	inline operator ENUMS  ( void ) const { return m_it->first;  }
	inline const char* str ( void ) const { return m_it->second; }

private:
	const iterator m_it;
};


inline static const char* operator *  ( const Name::ENUMS e )
{
	return Name::find(e)->second;
}

inline static const char* operator *= ( const char *&s, const Name::ENUMS e )
{
	return s = Name::find(e)->second;
}

inline static bool operator *= ( Name::ENUMS& e, const char* s )
{
	return Name::_INVALID_ != ( e = Name::find(s)->first );
}


// When including the definitions the second time (only once in the project)

Player::iterator Player::get_iterator( bool GetBeginIterator )
{
	static STRINGS S[] = 
	{ { First     , "First"      }
	, { Second    , "Second"     }
	, { Third     , "Thisrd"     }
	, { Fourth    , "!Surprice!" }
	, { Fifth     , "Fifth"      }
	, { Sixth     , "LastOne"    }
	, { _INVALID_ , "_INVALID_"  }
	};
	static iterator begin = &S[0];
	static iterator end   = &S[ (sizeof S)/(sizeof STRINGS) - 1 ];	// points to _INVALID_

	return GetBeginIterator ?begin :end;
}

#endif
