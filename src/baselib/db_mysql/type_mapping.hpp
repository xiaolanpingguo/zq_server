#pragma once


#include <stdint.h>
#include <string>

#include <mysql.h>
using namespace std::string_view_literals;

namespace zq{


#define BOOLOID			16
#define BYTEAOID		17
#define CHAROID			18
#define NAMEOID			19
#define INT8OID			20
#define INT2OID			21
#define INT2VECTOROID	22
#define INT4OID			23
#define REGPROCOID		24
#define TEXTOID			25
#define OIDOID			26
#define TIDOID		    27
#define XIDOID          28
#define CIDOID          29
#define OIDVECTOROID	30
#define POINTOID		600
#define LSEGOID			601
#define PATHOID			602
#define BOXOID			603
#define POLYGONOID		604
#define LINEOID			628
#define FLOAT4OID       700
#define FLOAT8OID       701
#define ABSTIMEOID		702
#define RELTIMEOID		703
#define TINTERVALOID	704
#define UNKNOWNOID		705
#define CIRCLEOID		718
#define CASHOID         790
#define INETOID         869
#define CIDROID         650
#define BPCHAROID		1042
#define VARCHAROID		1043
#define DATEOID			1082
#define TIMEOID			1083
#define TIMESTAMPOID	1114
#define TIMESTAMPTZOID	1184
#define INTERVALOID		1186
#define TIMETZOID		1266
#define ZPBITOID	    1560
#define VARBITOID	    1562
#define NUMERICOID		1700

template <class T>
struct identity
{
};

#define REGISTER_TYPE(Type, Index)                                              \
inline constexpr int type_to_id(identity<Type>) noexcept { return Index; }	\
inline constexpr auto id_to_type( std::integral_constant<std::size_t, Index > ) noexcept { Type res{}; return res; }


REGISTER_TYPE( char    , MYSQL_TYPE_TINY     )
REGISTER_TYPE( short   , MYSQL_TYPE_SHORT    )
REGISTER_TYPE( int     , MYSQL_TYPE_LONG     )
REGISTER_TYPE( float   , MYSQL_TYPE_FLOAT    )
REGISTER_TYPE( double  , MYSQL_TYPE_DOUBLE   )
REGISTER_TYPE( int64_t , MYSQL_TYPE_LONGLONG )

inline int type_to_id(identity<std::string>) noexcept { return MYSQL_TYPE_VAR_STRING; }
inline std::string id_to_type(std::integral_constant<std::size_t, MYSQL_TYPE_VAR_STRING > ) noexcept { std::string res{}; return res; }

inline constexpr auto type_to_name(identity<char>) noexcept { return "TINYINT"sv; }
inline constexpr auto type_to_name(identity<short>) noexcept { return "SMALLINT"sv; }
inline constexpr auto type_to_name(identity<int>) noexcept { return "INTEGER"sv; }
inline constexpr auto type_to_name(identity<float>) noexcept { return "FLOAT"sv; }
inline constexpr auto type_to_name(identity<double>) noexcept { return "DOUBLE"sv; }
inline constexpr auto type_to_name(identity<int64_t>) noexcept { return "BIGINT"sv; }
inline auto type_to_name(identity<std::string>) noexcept { return "TEXT"sv; }

template<size_t N>
inline constexpr auto type_to_name(identity<std::array<char, N>>) noexcept 
{
	std::string s = "varchar(" + std::to_string(N) + ")";
	return s;
}
}

