#pragma once

#ifndef ASSERT_MSG
#define ASSERT_MSG( cond, msg, ... ) if ( ( cond ) == false ) { __debugbreak(); }
#endif

#include <iterator>
#include <bit>

//---------------------------------------------------------------------------------
template< typename V >
class alignas( 64 ) HashMap64ImplBase
{
public:
  HashMap64ImplBase()
  : m_Meta      ( nullptr )
  , m_Elems     ( nullptr )
  , m_Capacity  ( 0 ) 
  , m_GroupCount( 0 )
  {}

  static constexpr size_t   GetRequiredBackingSize( uint32_t capacity );

         virtual   void     InitWithBacking       ( void* backing, uint32_t capacity );
         virtual   void     Clear                 ();
         virtual   void     Insert                ( uint64_t key, const V& value ) = 0;
         virtual   V*       Insert                ( uint64_t key ) = 0;
         virtual   V*       At                    ( uint64_t key ) = 0;
         virtual   const V* At                    ( uint64_t key ) const = 0;
         virtual   void     Remove                ( uint64_t key ) = 0;
         virtual   uint32_t GetCapacity           ( ) const;
         virtual   uint32_t GetCount              ( ) const = 0;
         virtual   bool     IsFull                ( ) const = 0;
         virtual   bool     IsEmpty               ( ) const = 0;
         virtual   bool     Contains              ( uint64_t key ) const = 0;
         virtual   uint64_t GetFirstKey           ( ) = 0;
  
  class iterator
  {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = V;

    explicit iterator( HashMap64ImplBase* owner, uint32_t group_idx, uint32_t ctrl_idx ) : m_Owner( owner ), m_GroupIdx( group_idx ), m_CtrlIdx( ctrl_idx ) {}
    inline bool operator==( const iterator& other ) const { return m_Owner == other.m_Owner && m_GroupIdx == other.m_GroupIdx && m_CtrlIdx == other.m_CtrlIdx; }
    inline bool operator!=( const iterator& other ) const { return !((*this) == other ); }
    inline V& operator*()
    {
      if ( m_Owner->m_Meta[ m_GroupIdx ].m_Control[ m_CtrlIdx ] & kIsNotEmpty )
      {
        return m_Owner->m_Elems[ m_GroupIdx * kElemsPerMeta + m_CtrlIdx ].m_Value;
      }
      return m_Owner->m_Elems[ -1 ].m_Value; // crash
    }
    inline V* operator->()
    {
      if ( m_Owner->m_Meta[ m_GroupIdx ].m_Control[ m_CtrlIdx ] & kIsNotEmpty )
      {
        return &m_Owner->m_Elems[ m_GroupIdx * kElemsPerMeta + m_CtrlIdx ].m_Value;
      }
      return nullptr;
    }
    inline iterator& operator++()
    {
      if ( m_GroupIdx != (uint32_t)-1 && m_CtrlIdx != (uint32_t)-1 )
      {
        m_Owner->GetNext( &m_GroupIdx, &m_CtrlIdx );
      }
      return *this;
    }
  private:
    HashMap64ImplBase* m_Owner;
    uint32_t           m_GroupIdx;
    uint32_t           m_CtrlIdx;
  };
  class const_iterator
  {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = V;

    explicit const_iterator( const HashMap64ImplBase* owner, uint32_t group_idx, uint32_t ctrl_idx ) : m_Owner( owner ), m_GroupIdx( group_idx ), m_CtrlIdx( ctrl_idx ) {}
    inline bool operator==( const const_iterator& other ) const { return m_Owner == other.m_Owner && m_GroupIdx == other.m_GroupIdx && m_CtrlIdx == other.m_CtrlIdx; }
    inline bool operator!=( const const_iterator& other ) const { return !((*this) == other ); }
    inline const V& operator*()
    {
      if ( m_Owner->m_Meta[ m_GroupIdx ].m_Control[ m_CtrlIdx ] & kIsNotEmpty )
      {
        return m_Owner->m_Elems[ m_GroupIdx * kElemsPerMeta + m_CtrlIdx ].m_Value;
      }
      return m_Owner->m_Elems[-1].m_Value; // crash
    }
    inline const V* operator->() const
    {
      if ( m_Owner->m_Meta[ m_GroupIdx ].m_Control[ m_CtrlIdx ] & kIsNotEmpty )
      {
        return &m_Owner->m_Elems[ m_GroupIdx * kElemsPerMeta + m_CtrlIdx ].m_Value;
      }
      return nullptr;
    }
    inline const_iterator& operator++()
    {
      if ( m_GroupIdx != (uint32_t)-1 && m_CtrlIdx != (uint32_t)-1 )
      {
        m_Owner->GetNext( &m_GroupIdx, &m_CtrlIdx );
      }
      return *this;
    }
  private:
    const HashMap64ImplBase* m_Owner;
    uint32_t                 m_GroupIdx;
    uint32_t                 m_CtrlIdx;
  };

  const_iterator cbegin() const
  {
    uint32_t group_idx = (uint32_t)-1;
    uint32_t ctrl_idx  = (uint32_t)-1;

    this->GetNext( &group_idx, &ctrl_idx );
    return const_iterator( this, group_idx, ctrl_idx );
  }
  const_iterator cend() const
  {
    return const_iterator( this, (uint32_t)-1, (uint32_t)-1 );
  }
  const_iterator begin() const
  {
    return cbegin();
  }
  const_iterator end() const
  {
    return cend();
  }
  iterator begin()
  {
    uint32_t group_idx = (uint32_t)-1;
    uint32_t ctrl_idx  = (uint32_t)-1;

    this->GetNext( &group_idx, &ctrl_idx );
    return iterator( this, group_idx, ctrl_idx );
  }
  iterator end()
  { 
    return iterator( this, (uint32_t)-1, (uint32_t)-1 );
  }

protected:
  static constexpr uint32_t kElemsPerMeta = 16;
  static uint32_t HashFunction( uint64_t input );

  using ControlT = uint32_t;
  enum Control : ControlT
  {
    kIsNotEmpty = 0x8000'0000,
    kHashRem    = 0x7fff'ffff,
  };

  struct Group
  {
    ControlT m_Control [ kElemsPerMeta ];
  };
  static_assert( sizeof( Group ) == sizeof( Group::m_Control ), "Must be well-packed"             );
  static_assert( sizeof( Group ) == 64,                         "Must be the size of a cacheline" );

  struct Elem
  {
    uint64_t m_Key;
    V        m_Value;
  };
  
  Group*        m_Meta;
  Elem*         m_Elems;
  uint32_t      m_Capacity;
  uint32_t      m_GroupCount;

  static inline ControlT CtrlFor( uint64_t key );
  
  virtual void GetNext( uint32_t* inout_group_idx, uint32_t* inout_ctrl_idx ) const = 0;
};

//---------------------------------------------------------------------------------
// Murmur64
template< typename V >
uint32_t HashMap64ImplBase<V>::HashFunction( uint64_t input )
{
  input ^= input >> 33;
  input *= 0xff51afd7ed558ccdULL;
  input ^= input >> 33;
  input *= 0xc4ceb9fe1a85ec53ULL;
  input ^= input >> 33;
  return (uint32_t)input;
}

//---------------------------------------------------------------------------------
template< typename V >
void HashMap64ImplBase< V >::InitWithBacking( void* backing, uint32_t capacity )
{
  ASSERT_MSG( capacity % kElemsPerMeta == 0 , "Capacity must be a multiple of 16" );
  ASSERT_MSG( (uint64_t)backing % kElemsPerMeta == 0, "Backing must be aligned to a cacheline" );

  m_Capacity   = capacity;
  m_GroupCount = capacity / kElemsPerMeta;

  size_t meta_size = ( sizeof( *m_Meta ) * m_GroupCount );
  m_Meta = (Group*)backing;

  memset( m_Meta, 0, meta_size );

  m_Elems = (Elem*)(((uint8_t*)backing) + meta_size);
}

//---------------------------------------------------------------------------------
template< typename V >
void HashMap64ImplBase< V >::Clear()
{
  size_t meta_size = ( sizeof( *m_Meta ) * m_GroupCount );
  memset( m_Meta, 0, meta_size );
}

//---------------------------------------------------------------------------------
template< typename V >
uint32_t HashMap64ImplBase< V >::GetCapacity( ) const
{
  return m_Capacity;
}


//---------------------------------------------------------------------------------
template< typename V >
constexpr size_t HashMap64ImplBase< V >::GetRequiredBackingSize( uint32_t capacity )
{
  return capacity * sizeof( Elem ) + ( capacity * sizeof( Group ) / kElemsPerMeta );
}

//---------------------------------------------------------------------------------
template< typename V >
inline typename HashMap64ImplBase<V>::ControlT HashMap64ImplBase<V>::CtrlFor( uint64_t key )
{
  return ((uint32_t )key) & kHashRem;
}