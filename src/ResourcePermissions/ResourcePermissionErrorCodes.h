/*   2log.io
 *   Copyright (C) 2021 - 2log.io | mail@2log.io,  mail@friedemann-metzger.de
 *   
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Affero General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Affero General Public License for more details.
 *
 *   You should have received a copy of the GNU Affero General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef RESOURCEPERMISSIONERRORCODES_H
#define RESOURCEPERMISSIONERRORCODES_H

#include <type_traits>

enum ResourcePermissionErrorCode
{
	PERMISSION_GRANTED			= 0,
	NO_DATA						= (1u << 0),
	USER_DISABLED				= (1u << 1),
	CARD_DISABLED				= (1u << 2),
	PERMISSION_DISABLED			= (1u << 3),
	PERMISSION_EXPIRED			= (1u << 4),
	PLUGIN_PERMISSION_DENIED	= (1u << 5),
	RESOURCE_BOOKING_DENIED		= (1u << 6),
	RESOURCE_BOOKING_BUSY		= (1u << 7),
	UNKNOWN_ERROR				= (1u << 8),
	TIMEOUT						= (1u << 9),
};

template<typename Enum>
struct EnableBitMaskOperators
{
	static const bool enable = false;
};

template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
operator |(Enum lhs, Enum rhs)
{
	using underlying = typename std::underlying_type<Enum>::type;
	return static_cast<Enum> (
		static_cast<underlying>(lhs) |
		static_cast<underlying>(rhs)
	);
}

template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
operator &(Enum lhs, Enum rhs)
{
	using underlying = typename std::underlying_type<Enum>::type;
	return static_cast<Enum> (
		static_cast<underlying>(lhs) &
		static_cast<underlying>(rhs)
	);
}

template<>
struct EnableBitMaskOperators<ResourcePermissionErrorCode>
{
	static const bool enable = true;
};

#endif // RESOURCEPERMISSIONERRORCODES_H
