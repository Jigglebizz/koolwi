/*-----------------------------------------------------------------------------
| Copyright (c) 2013-2026, Nucleic Development Team.
|
| Distributed under the terms of the Modified BSD License.
|
| The full license is in the file LICENSE, distributed with this software.
|----------------------------------------------------------------------------*/
#pragma once
#include <functional>
#include <memory>
#include <utility>
#include "HashMap/HashMap.h"

namespace kiwi
{

namespace impl
{

// ----------------------------------------------------------------------------
template< typename KeyT, typename ValueT, typename HashT = std::hash<KeyT> >
class MapWrapper final : public HashMap64< std::pair<KeyT,ValueT> >
{
public:
  using BaseT = HashMap64< std::pair<KeyT, ValueT> >;

  void                   Insert   ( const KeyT& key, const ValueT& value ) { HashT hash; BaseT::Insert( (uint64_t)hash(key), std::make_pair( key, value ) );    }
  ValueT*                Insert   ( const KeyT& key );
  ValueT*                At       ( const KeyT& key );
  const ValueT*          At       ( const KeyT& key ) const;
  void                   Remove   ( const KeyT& key )                      { HashT hash; BaseT::Remove( (uint64_t)hash(key) );}
  bool                   Contains ( const KeyT& key ) const                { HashT hash; return BaseT::Contains( hash(key) ); }



//private:
//  void                          Insert   ( uint64_t key, const std::pair<KeyT,ValueT>& value );
//  std::pair<KeyT,ValueT>*       Insert   ( uint64_t key );
//  std::pair<KeyT,ValueT>*       At       ( uint64_t key );
//  const std::pair<KeyT,ValueT>* At       ( uint64_t key ) const;
//  void                          Remove   ( uint64_t key );
//  bool                          Contains ( uint64_t key ) const;
};

template < typename KeyT, typename ValueT, typename HashT >
ValueT* MapWrapper<KeyT, ValueT, HashT>::Insert( const KeyT& key )
{
  HashT hash_fn;
  if ( std::pair<KeyT,ValueT>* res = BaseT::Insert( hash_fn(key) ) )
  {
    return &res->second;
  } 
  return nullptr;
}

template < typename KeyT, typename ValueT, typename HashT >
ValueT* MapWrapper<KeyT, ValueT, HashT>::At( const KeyT& key )
{
  HashT hash_fn;
  if ( std::pair<KeyT,ValueT>* res = BaseT::At( hash_fn(key) ) )
  {
    return &res->second;
  } 
  return nullptr;
}

template < typename KeyT, typename ValueT, typename HashT >
const ValueT* MapWrapper<KeyT, ValueT, HashT>::At( const KeyT& key ) const
{
  HashT hash_fn;
  if ( const std::pair<KeyT,ValueT>* res = BaseT::At( (uint64_t)hash_fn(key) ) )
  {
    return &res->second;
  } 
  return nullptr;
}

// ----------------------------------------------------------------------------
template <
    typename K,
    typename V >
using MapType = MapWrapper< K, V >;

// template<
// 	typename K,
// 	typename V,
// 	typename C = std::less<K>,
// 	typename A = std::allocator< std::pair<const K, V> > >
// using MapType = std::map<K, V, C, A>;

} // namespace impl

} // namespace kiwi
